#include "PlayerStrategies.h"

#include "../Orders/Orders.h"
#include "../Player/Player.h"
#include "../map/Map.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>

namespace
{
// Check whether a territory belongs to the specified player.
bool ownedBy(Player *player, Territory *territory)
{
    return player != nullptr && territory != nullptr &&
           territory->getOwner() == player;
}

// Build a unique list of enemy territories bordering any owned territory.
std::vector<Territory *> adjacentEnemies(Player *player)
{
    std::vector<Territory *> result;
    std::unordered_set<Territory *> seen;
    if (player == nullptr) return result;

    for (Territory *owned : player->getOwnedTerritories())
    {
        if (owned == nullptr) continue;
        for (Territory *neighbor : owned->getAdjacentTerritories())
        {
            if (neighbor != nullptr && !ownedBy(player, neighbor) &&
                seen.insert(neighbor).second)
                result.push_back(neighbor);
        }
    }
    return result;
}

// Sort territories by army count with a deterministic name tie-breaker.
void sortByArmy(std::vector<Territory *> &territories, bool strongestFirst)
{
    std::stable_sort(
        territories.begin(), territories.end(),
        // Compare territories according to the requested priority direction.
        [strongestFirst](Territory *left, Territory *right)
        {
            if (left->getArmyCount() == right->getArmyCount())
                return left->getName() < right->getName();
            return strongestFirst
                       ? left->getArmyCount() > right->getArmyCount()
                       : left->getArmyCount() < right->getArmyCount();
        });
}
}

// Delegate human order selection to the normal interactive player workflow.
void HumanPlayerStrategy::issueOrder(Player *player, Deck *deck,
                                      const std::vector<Player *> &players)
{
    // Keep all prompts and card handling in the original Player workflow.
    if (player != nullptr)
    {
        std::cout << "[Human] " << *player->getName()
                  << " is choosing orders interactively.\n";
        player->issueHumanOrder(deck, players);
    }
}

// Return enemy territories adjacent to the human player's borders.
std::vector<Territory *> HumanPlayerStrategy::toAttack(Player *player)
{
    return adjacentEnemies(player);
}

// Return all territories currently owned by the human player.
std::vector<Territory *> HumanPlayerStrategy::toDefend(Player *player)
{
    return player == nullptr ? std::vector<Territory *>()
                             : player->getOwnedTerritories();
}

// Create an independent copy of the human player strategy.
PlayerStrategy *HumanPlayerStrategy::clone() const
{
    return new HumanPlayerStrategy(*this);
}

// Reinforce the strongest territory and attack from available borders.
void AggressivePlayerStrategy::issueOrder(
    Player *player, Deck *, const std::vector<Player *> &)
{
    if (player == nullptr) return;
    std::cout << "[Aggressive] " << *player->getName()
              << " is reinforcing and attacking.\n";
    std::vector<Territory *> defend = toDefend(player);
    if (defend.empty())
    {
        std::cout << "[Aggressive] " << *player->getName()
                  << " has no territories to defend.\n";
        return;
    }

    sortByArmy(defend, true);
    const int reinforcements = *player->getReinforcementPool();
    // Send the entire reinforcement pool to the strongest owned territory.
    if (reinforcements > 0)
    {
        std::cout << "[Aggressive] " << *player->getName()
                  << " deploys " << reinforcements << " armies to "
                  << defend.front()->getName() << ".\n";
        player->issueOrder(new Deploy(player, defend.front()->getName(),
                                      reinforcements));
    }

    for (Territory *source : defend)
    {
        if (source == nullptr || source->getArmyCount() <= 0) continue;
        for (Territory *target : source->getAdjacentTerritories())
        {
            if (target != nullptr && !ownedBy(player, target))
            {
                // Queue one legal attack from each available border source.
                std::cout << "[Aggressive] " << *player->getName()
                          << " advances " << source->getArmyCount()
                          << " armies from " << source->getName()
                          << " to " << target->getName() << ".\n";
                player->issueOrder(new Advance(
                    player, source->getName(), target->getName(),
                    source->getArmyCount()));
                break;
            }
        }
    }
}

// Return adjacent enemy territories, weakest first.
std::vector<Territory *> AggressivePlayerStrategy::toAttack(Player *player)
{
    std::vector<Territory *> result = adjacentEnemies(player);
    sortByArmy(result, false);
    return result;
}

// Return owned territories, strongest first.
std::vector<Territory *> AggressivePlayerStrategy::toDefend(Player *player)
{
    std::vector<Territory *> result =
        player == nullptr ? std::vector<Territory *>()
                          : player->getOwnedTerritories();
    sortByArmy(result, true);
    return result;
}

// Create an independent copy of the aggressive player strategy.
PlayerStrategy *AggressivePlayerStrategy::clone() const
{
    return new AggressivePlayerStrategy(*this);
}

// Reinforce the weakest territory without issuing attack orders.
void BenevolentPlayerStrategy::issueOrder(
    Player *player, Deck *, const std::vector<Player *> &)
{
    if (player == nullptr) return;
    std::cout << "[Benevolent] " << *player->getName()
              << " is reinforcing without attacking.\n";
    std::vector<Territory *> defend = toDefend(player);
    // All reinforcements go to the weakest territory; no attack is queued.
    if (!defend.empty() && *player->getReinforcementPool() > 0)
    {
        std::cout << "[Benevolent] " << *player->getName()
                  << " deploys " << *player->getReinforcementPool()
                  << " armies to weakest territory "
                  << defend.front()->getName() << ".\n";
        player->issueOrder(new Deploy(player, defend.front()->getName(),
                                      *player->getReinforcementPool()));
    }
}

// Return no attack targets because benevolent players avoid combat.
std::vector<Territory *> BenevolentPlayerStrategy::toAttack(Player *)
{
    return {};
}

// Return owned territories, weakest first.
std::vector<Territory *> BenevolentPlayerStrategy::toDefend(Player *player)
{
    std::vector<Territory *> result =
        player == nullptr ? std::vector<Territory *>()
                          : player->getOwnedTerritories();
    sortByArmy(result, false);
    return result;
}

// Create an independent copy of the benevolent player strategy.
PlayerStrategy *BenevolentPlayerStrategy::clone() const
{
    return new BenevolentPlayerStrategy(*this);
}

// Skip the issue-orders phase for neutral players.
void NeutralPlayerStrategy::issueOrder(
    Player *player, Deck *, const std::vector<Player *> &)
{
    // Neutral players intentionally skip the issue-orders phase.
    if (player != nullptr)
    {
        std::cout << "[Neutral] " << *player->getName()
                  << " takes no action this turn.\n";
    }
}

// Return no attack targets for a neutral player.
std::vector<Territory *> NeutralPlayerStrategy::toAttack(Player *)
{
    return {};
}

// Return no defensive priorities for a neutral player.
std::vector<Territory *> NeutralPlayerStrategy::toDefend(Player *)
{
    return {};
}

// Create an independent copy of the neutral player strategy.
PlayerStrategy *NeutralPlayerStrategy::clone() const
{
    return new NeutralPlayerStrategy(*this);
}

// Capture adjacent enemy territories without using normal combat.
void CheaterPlayerStrategy::issueOrder(
    Player *player, Deck *, const std::vector<Player *> &)
{
    if (player == nullptr) return;
    std::cout << "[Cheater] " << *player->getName()
              << " is automatically conquering adjacent territories.\n";
    std::unordered_set<Territory *> processed;
    for (Territory *source : player->getOwnedTerritories())
    {
        if (source == nullptr || source->getArmyCount() <= 0) continue;
        for (Territory *target : source->getAdjacentTerritories())
        {
            if (target != nullptr && !ownedBy(player, target) &&
                processed.insert(target).second)
            {
                // Cheaters bypass combat but still update both owners through
                // the normal territory transfer methods.
                Player *oldOwner = target->getOwner();
                if (oldOwner != nullptr) oldOwner->removeTerritory(target);
                player->addTerritory(target);
                player->markTerritoryConquered();
                std::cout << "[Cheater] " << *player->getName()
                          << " conquers " << target->getName()
                          << " without combat.\n";
            }
        }
    }
}

// Return enemy territories adjacent to the cheater's borders.
std::vector<Territory *> CheaterPlayerStrategy::toAttack(Player *player)
{
    return adjacentEnemies(player);
}

// Return all territories currently owned by the cheater.
std::vector<Territory *> CheaterPlayerStrategy::toDefend(Player *player)
{
    return player == nullptr ? std::vector<Territory *>()
                             : player->getOwnedTerritories();
}

// Create an independent copy of the cheater player strategy.
PlayerStrategy *CheaterPlayerStrategy::clone() const
{
    return new CheaterPlayerStrategy(*this);
}
