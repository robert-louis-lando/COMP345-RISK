#include "PlayerStrategies.h"

#include "Player.h"
#include "../Cards/Cards.h"
#include "../map/Map.h"
#include <algorithm>
#include <iostream>
#include <unordered_set>

PlayerStrategy::~PlayerStrategy() = default;

namespace
{
//Collect adjacent enemy territories once, even when several owned territories touch them.
std::vector<Territory *> adjacentEnemies(const Player &player)
{
    std::vector<Territory *> result;
    std::unordered_set<Territory *> seen;
    for (Territory *owned : player.getTerritories())
        for (Territory *neighbor : owned->getAdjacentTerritories())
            if (neighbor != nullptr && neighbor->getOwner() != &player && seen.insert(neighbor).second)
                result.push_back(neighbor);
    return result;
}

// Return the owned territory with the largest army, or nullptr if there are none.
Territory *strongest(const std::vector<Territory *> &territories)
{
    if (territories.empty()) return nullptr;
    return *std::max_element(territories.begin(), territories.end(),
        [](const Territory *a, const Territory *b) { return a->getArmyCount() < b->getArmyCount(); });
}

// Return the owned territory with the fewest armies, or nullptr if there are none.
Territory *weakest(const std::vector<Territory *> &territories)
{
    if (territories.empty()) return nullptr;
    return *std::min_element(territories.begin(), territories.end(),
        [](const Territory *a, const Territory *b) { return a->getArmyCount() < b->getArmyCount(); });
}

void deploy(Player &player, Territory *target)
{
    //Use the full reinforcement pool on the strategy's chosen territory.
    const int armies = player.getReinforcementPool() == nullptr ? 0 : *player.getReinforcementPool();
    if (target != nullptr && armies > 0)
        player.issueOrder(new Deploy(&player, target->getName(), armies));
}
}

// Make an independent copy for Player's copy operations.
PlayerStrategy *HumanPlayerStrategy::clone() const { return new HumanPlayerStrategy(*this); }
// Let the existing interactive Player routine collect human orders.
void HumanPlayerStrategy::issueOrder(Player &p, Deck *d, const std::vector<Player *> &players) { p.issueHumanOrders(d, players); }
// Human players may attack any adjacent enemy territory.
std::vector<Territory *> HumanPlayerStrategy::toAttack(const Player &p) const { return adjacentEnemies(p); }
//Human players may defend all of their owned territories.
std::vector<Territory *> HumanPlayerStrategy::toDefend(const Player &p) const { return p.getTerritories(); }
// Return the display name used for this strategy.
const char *HumanPlayerStrategy::name() const { return "Human"; }

// Make an independent copy of the aggressive strategy.
PlayerStrategy *AggressivePlayerStrategy::clone() const { return new AggressivePlayerStrategy(*this); }
//Reinforce the strongest territory, then use its armies to attack neighbors.
void AggressivePlayerStrategy::issueOrder(Player &p, Deck *, const std::vector<Player *> &)
{
    auto owned = p.getTerritories();
    Territory *source = strongest(owned);
    deploy(p, source);
    if (source == nullptr) return;
    std::cout << "  Aggressive: reinforcing " << source->getName()
              << " and attacking from it.\n";
    int available = source->getArmyCount() + (p.getReinforcementPool() ? *p.getReinforcementPool() : 0) - 1;
    for (Territory *target : toAttack(p))
    {
        if (available <= 0) break;
        int commitment = std::min(available, std::max(1, target->getArmyCount() * 2 + 1));
        std::cout << "  Aggressive: advance " << commitment << " from "
                  << source->getName() << " to " << target->getName() << ".\n";
        p.issueOrder(new Advance(&p, source->getName(), target->getName(), commitment));
        available -= commitment;
    }
}
// List adjacent enemy territories as attack targets
std::vector<Territory *> AggressivePlayerStrategy::toAttack(const Player &p) const { return adjacentEnemies(p); }
// Prioritize stronger territories when returning defense targets
std::vector<Territory *> AggressivePlayerStrategy::toDefend(const Player &p) const
{
    auto result = p.getTerritories();
    std::stable_sort(result.begin(), result.end(), [](Territory *a, Territory *b) { return a->getArmyCount() > b->getArmyCount(); });
    return result;
}
//Return the display name used for this strategy
const char *AggressivePlayerStrategy::name() const { return "Aggressive"; }

// Make an independent copy of the benevolent strategy.
PlayerStrategy *BenevolentPlayerStrategy::clone() const { return new BenevolentPlayerStrategy(*this); }
// Reinforce the weakest territory and move surplus armies there when possible.
void BenevolentPlayerStrategy::issueOrder(Player &p, Deck *, const std::vector<Player *> &)
{
    auto owned = p.getTerritories();
    Territory *weak = weakest(owned);
    deploy(p, weak);
    if (weak != nullptr)
        std::cout << "  Benevolent: reinforcing weakest territory "
                  << weak->getName() << ".\n";
    // Transfer surplus armies toward the weakest territory without attacking.
    Territory *source = nullptr;
    if (weak != nullptr)
        for (Territory *candidate : owned)
            if (candidate != weak && candidate->getArmyCount() > 1 &&
                std::find(candidate->getAdjacentTerritories().begin(), candidate->getAdjacentTerritories().end(), weak) != candidate->getAdjacentTerritories().end() &&
                (source == nullptr || candidate->getArmyCount() > source->getArmyCount()))
                source = candidate;
    if (source != nullptr)
    {
        std::cout << "  Benevolent: moving armies from " << source->getName()
                  << " to " << weak->getName() << ".\n";
        p.issueOrder(new Advance(&p, source->getName(), weak->getName(), source->getArmyCount() - 1));
    }
}
// Benevolent players never select enemy territories to attack.
std::vector<Territory *> BenevolentPlayerStrategy::toAttack(const Player &) const { return {}; }
//Prioritize weaker territories when returning defense targets.
std::vector<Territory *> BenevolentPlayerStrategy::toDefend(const Player &p) const
{
    auto result = p.getTerritories();
    std::stable_sort(result.begin(), result.end(), [](Territory *a, Territory *b) { return a->getArmyCount() < b->getArmyCount(); });
    return result;
}
// Return the display name used for this strategy.
const char *BenevolentPlayerStrategy::name() const { return "Benevolent"; }

// Make an independent copy of the neutral strategy.
PlayerStrategy *NeutralPlayerStrategy::clone() const { return new NeutralPlayerStrategy(*this); }
//Neutral players wait without issuing orders.
void NeutralPlayerStrategy::issueOrder(Player &p, Deck *, const std::vector<Player *> &)
{
    std::cout << "  Neutral: " << *p.getName() << " issues no orders.\n";
}
// Neutral players do not initiate attacks.
std::vector<Territory *> NeutralPlayerStrategy::toAttack(const Player &) const { return {}; }
// Return all owned territories as defense targets.
std::vector<Territory *> NeutralPlayerStrategy::toDefend(const Player &p) const { return p.getTerritories(); }
// Return the display name used for this strategy.
const char *NeutralPlayerStrategy::name() const { return "Neutral"; }

// Make an independent copy of the cheater strategy.
PlayerStrategy *CheaterPlayerStrategy::clone() const { return new CheaterPlayerStrategy(*this); }
//Capture the enemy territories adjacent at the start of this issuing step.
void CheaterPlayerStrategy::issueOrder(Player &p, Deck *, const std::vector<Player *> &)
{
    // Snapshot first so newly captured neighbors do not chain-capture this turn.
    const auto targets = toAttack(p);
    std::cout << "  Cheater: conquering " << targets.size()
              << " adjacent enemy territories.\n";
    for (Territory *target : targets)
    {
        Player *oldOwner = target->getOwner();
        std::cout << "    Captured " << target->getName();
        if (oldOwner != nullptr) std::cout << " from " << *oldOwner->getName();
        std::cout << ".\n";
        if (oldOwner) oldOwner->removeTerritory(target);
        p.addTerritory(target);
        p.markTerritoryConquered();
    }
}
// List adjacent enemy territories as capture targets.
std::vector<Territory *> CheaterPlayerStrategy::toAttack(const Player &p) const { return adjacentEnemies(p); }
// Return all owned territories as defense targets.
std::vector<Territory *> CheaterPlayerStrategy::toDefend(const Player &p) const { return p.getTerritories(); }
// Return the display name used for this strategy.
const char *CheaterPlayerStrategy::name() const { return "Cheater"; }
