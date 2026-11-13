#include "Player.h"
#include "Cards.h"
#include "../map/Map.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <unordered_map>

// constructor for player
Player::Player(const std::string &name)
{
    this->name = new std::string(name);
    reinforcementPool = new int(0);
    territories = new std::vector<Territory *>();
    ownedTerritories = new std::unordered_set<Territory *>();
    territoriesToAttack = new std::vector<Territory *>();
    hand = new std::vector<Card *>();
    orders = new OrdersList();
    negotiatedPlayers = new std::vector<Player *>();
    conqueredTerritoryThisTurn = new bool(false);
}

// copy constructor, copy of a player
Player::Player(const Player &other)
{
    name = new std::string(*other.name);
    reinforcementPool = new int(*other.reinforcementPool);
    territories = new std::vector<Territory *>(*other.territories);
    ownedTerritories = new std::unordered_set<Territory *>(*other.ownedTerritories);
    territoriesToAttack =
        new std::vector<Territory *>(*other.territoriesToAttack);
    hand = new std::vector<Card *>();
    for (const Card *card : *other.hand) hand->push_back(card == nullptr ? nullptr : new Card(*card));
    orders = new OrdersList(*other.orders);
    negotiatedPlayers = new std::vector<Player *>(*other.negotiatedPlayers);
    conqueredTerritoryThisTurn = new bool(*other.conqueredTerritoryThisTurn);
}

// replace player with other player data
Player &Player::operator=(const Player &other)
{
    if (this != &other)
    {
        *name = *other.name;
        *reinforcementPool = *other.reinforcementPool;
        *territories = *other.territories;
        *ownedTerritories = *other.ownedTerritories;
        *territoriesToAttack = *other.territoriesToAttack;
        for (Card *card : *hand) delete card;
        hand->clear();
        for (const Card *card : *other.hand) hand->push_back(card == nullptr ? nullptr : new Card(*card));
        *orders = *other.orders;
        *negotiatedPlayers = *other.negotiatedPlayers;
        *conqueredTerritoryThisTurn = *other.conqueredTerritoryThisTurn;
    }

    return *this;
}

// destructor to release the memory for the players' data and orders
Player::~Player()
{
    delete name;
    delete territories;
    delete ownedTerritories;
    delete territoriesToAttack;
    for (Card *card : *hand) delete card;
    delete hand;
    delete reinforcementPool;
    delete orders;
    delete negotiatedPlayers;
    delete conqueredTerritoryThisTurn;
}

// add territory to owned territories
void Player::addTerritory(Territory *territory)
{
    if (territory == nullptr || ownedTerritories->count(territory) != 0)
    {
        return;
    }
    territories->push_back(territory);
    ownedTerritories->insert(territory);
    territory->setOwner(this);
}

// Remove a territory from both the owned set and ordered list.
void Player::removeTerritory(Territory *territory)
{
    if (territory == nullptr || ownedTerritories->erase(territory) == 0)
    {
        return;
    }

    territories->erase(
        std::remove(territories->begin(), territories->end(), territory),
        territories->end());
}

// Add a unique temporary non-aggression partner.
void Player::negotiateWith(Player *player)
{
    if (player != nullptr && player != this && !isNegotiatedWith(player))
        negotiatedPlayers->push_back(player);
}

// Check whether this player has a non-aggression agreement with another.
bool Player::isNegotiatedWith(const Player *player) const
{
    return std::find(negotiatedPlayers->begin(), negotiatedPlayers->end(), player) != negotiatedPlayers->end();
}

// Clear temporary agreements at the beginning of the next execution phase.
void Player::clearNegotiations() { negotiatedPlayers->clear(); }

// Record a successful territory capture during this turn.
void Player::markTerritoryConquered() { *conqueredTerritoryThisTurn = true; }
// Report whether a territory was captured this turn.
bool Player::hasConqueredTerritoryThisTurn() const { return *conqueredTerritoryThisTurn; }
// Reset the conquest marker for the next turn.
void Player::clearConquestFlag() { *conqueredTerritoryThisTurn = false; }

// add territory to the list of territories they can attack
void Player::addTerritoryToAttack(Territory *territory)
{
    territoriesToAttack->push_back(territory);
}

// add card to player hand
void Player::addCard(Card *card)
{
    hand->push_back(card);
}

// Report the current hand size for game logic and demonstration drivers.
int Player::getHandSize() const { return static_cast<int>(hand->size()); }

// Remove and return the first card matching the requested type.
Card *Player::removeCard(const std::string &cardType)
{
    const auto card = std::find_if(
        hand->begin(),
        hand->end(),
        [&cardType](const Card *candidate)
        {
            return candidate != nullptr && candidate->getType() == cardType;
        });

    if (card == hand->end())
    {
        return nullptr;
    }

    Card *removedCard = *card;
    hand->erase(card);
    return removedCard;
}
// getter for name
const std::string *Player::getName() const
{
    return name;
}
// getter for reinforcementPool
int *Player::getReinforcementPool() const
{
    return reinforcementPool;
}
// setter for reinforcementPool
void Player::setReinforcementPool(int troops)
{
    // no less than 0 troops
    if (troops < 0)
    {
        throw std::invalid_argument("Reinforcement pool cannot be negative");
    }
    *reinforcementPool = troops;
}

// return player list of territories to defend
std::vector<Territory *> Player::toDefend() const
{
    return *territories;
}

// return player's list of territories to attack
std::vector<Territory *> Player::toAttack() const
{
    std::vector<Territory *> neighboringTerritories;
    std::unordered_set<Territory *> alreadyAdded;

    for (Territory *territory : *territories)
    {
        for (Territory *neighbor : territory->getAdjacentTerritories())
        {
            if (ownedTerritories->count(neighbor) == 0 &&
                alreadyAdded.insert(neighbor).second)
            {
                neighboringTerritories.push_back(neighbor);
            }
        }
    }

    return neighboringTerritories;
}

// Issue order loop
void Player::issueOrder(Deck *deck, std::vector<Player *> players)
{
    std::cout << "\nPlayer: " << *name << '\n';
    std::cout << "\n"
              << "Deploy all your reinforcements:"
              << "\n";
    std::unordered_map<std::string, int> reinforcementsByTerritory;
    int pendingDeployments = 0;
    std::cout << "Territories to defend: ";
    const vector<Territory *> defend = toDefend();
    const auto printDefend = [&reinforcementsByTerritory, &defend]()
    {
        for (size_t i = 0; i < defend.size(); ++i)
        {
            if (i > 0)
            {
                std::cout << ", ";
            }

            const std::string territoryName = defend[i]->getName();
            const auto reinforcement =
                reinforcementsByTerritory.find(territoryName);
            const int pendingReinforcements =
                reinforcement == reinforcementsByTerritory.end()
                    ? 0
                    : reinforcement->second;

            std::cout << territoryName
                      << " (" << defend[i]->getArmyCount() + pendingReinforcements
                      << ")";
        }
    };

    printDefend();
    std::cout << '\n';
    while (*reinforcementPool - pendingDeployments > 0)
    {
        std::cout << "Reinforcement pool: "
                  << (*reinforcementPool - pendingDeployments) << '\n'
                  << "Enter the territory name and number of reinforcements "
                  << "to deploy (for example, Canada 3): ";

        std::string input;
        if (!std::getline(std::cin >> std::ws, input))
        {
            throw invalid_argument("Unable to read reinforcement deployment");
        }

        std::stringstream command(input);
        std::string territoryName;
        int nArmies;
        std::string extraInput;
        if (!(command >> territoryName >> nArmies) ||
            (command >> extraInput) ||
            nArmies <= 0 ||
            nArmies > *reinforcementPool - pendingDeployments)
        {
            std::cout << "Invalid deployment. Enter a territory and a positive "
                      << "number no greater than the reinforcement pool.\n";
            continue;
        }
        orders->add(
            new Deploy(this, territoryName, nArmies));
        reinforcementsByTerritory[territoryName] += nArmies;
        pendingDeployments += nArmies;
    }

    std::cout << "\n"
              << "Issue your advance orders:"
              << "\n";

    std::cout << "Your territories: \n";
    printDefend();

    std::cout << "\nTerritories to attack: \n";
    const vector<Territory *> attack = toAttack();
    for (size_t i = 0; i < attack.size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ", ";
        }
        std::cout << attack[i]->getName()
                  << " (" << attack[i]->getArmyCount() << ")";
    }
    std::cout << '\n';

    bool notDone = true;

    while (notDone)
    {
        std::cout << "Enter an advance order "
                  << "(fromTerritory toTerritory nArmies), or 'done': ";

        std::string input;
        if (!std::getline(std::cin >> std::ws, input))
        {
            throw invalid_argument("Unable to read advance order");
        }

        if (input == "done")
        {
            notDone = false;
            continue;
        }

        std::stringstream command(input);
        std::string fromTerritory;
        std::string toTerritory;
        std::string extraInput;
        int nArmies;
        if (!(command >> fromTerritory >> toTerritory >> nArmies) ||
            (command >> extraInput) ||
            nArmies <= 0 ||
            fromTerritory == toTerritory)
        {
            std::cout << "Invalid advance order. Enter two different "
                      << "territories and a positive number of armies.\n";
            continue;
        }

        orders->add(
            new Advance(this, fromTerritory, toTerritory, nArmies));
        reinforcementsByTerritory[fromTerritory] -= nArmies;
    }

    std::cout << "\n"
              << "Use a card:"
              << "\n";

    std::cout << "Your cards:"
              << "\n";

    for (Card *card : *hand)
    {
        std::cout << card->getType() << '\n';
    }

    if (!hand->empty())
    {
        std::cout << "Enter the card type to use, or 'none': ";
        std::string cardType;
        if (!std::getline(std::cin >> std::ws, cardType))
        {
            throw invalid_argument("Unable to read card selection");
        }

        if (cardType != "none")
        {
            Card *card = removeCard(cardType);
            if (card == nullptr)
            {
                std::cout << "Invalid card selection.\n";
            }
            else
            {
                deck->returnCard(*card);
                delete card;
                if (cardType == "bomb")
                {
                    std::cout << "\nTerritory to bomb: \n";
                    for (size_t i = 0; i < attack.size(); ++i)
                    {
                        if (i > 0)
                        {
                            std::cout << ", ";
                        }
                        std::cout << attack[i]->getName()
                                  << " (" << attack[i]->getArmyCount() << ")";
                    }
                    std::cout << '\n';

                    std::string targetTerritory;
                    std::cout << "Enter the territory name: ";
                    if (!std::getline(std::cin >> std::ws, targetTerritory) ||
                        targetTerritory.empty())
                    {
                        throw invalid_argument("Unable to read bomb target");
                    }

                    orders->add(new Bomb(this, targetTerritory));
                }
                else if (cardType == "blockade")
                {
                    std::cout << "Select territory to blockade: \n";
                    printDefend();
                    std::cout << '\n';

                    std::string targetTerritory;
                    std::cout << "Enter the territory name: ";
                    if (!std::getline(std::cin >> std::ws, targetTerritory) ||
                        targetTerritory.empty())
                    {
                        throw invalid_argument(
                            "Unable to read blockade target");
                    }

                    orders->add(new Blockade(this, targetTerritory));
                }
                else if (cardType == "airlift")
                {
                    std::cout << "Select source and target territory to airlift: \n";
                    printDefend();
                    std::cout << '\n';

                    std::string input;
                    std::string sourceTerritory;
                    std::string targetTerritory;
                    int nArmies;
                    std::cout << "Enter source territory, target territory, "
                                 "and number of armies: ";
                    if (!std::getline(std::cin >> std::ws, input))
                    {
                        throw invalid_argument(
                            "Unable to read airlift command");
                    }

                    std::stringstream command(input);
                    if (!(command >> sourceTerritory >> targetTerritory >>
                          nArmies) ||
                        (command >> input) ||
                        nArmies <= 0 ||
                        sourceTerritory == targetTerritory)
                    {
                        throw invalid_argument(
                            "Invalid airlift command");
                    }

                    orders->add(new Airlift(
                        this,
                        sourceTerritory,
                        targetTerritory,
                        nArmies));
                }
                else if (cardType == "diplomacy")
                {
                    std::cout << "\nPlayers to negotiate: \n";
                    for (size_t i = 0; i < players.size(); ++i)
                    {
                        if (i > 0)
                        {
                            std::cout << ", ";
                        }
                        std::cout << players[i]->getName();
                    }
                    std::cout << '\n';

                    std::string targetPlayer;
                    std::cout << "Enter the player name: ";
                    if (!std::getline(std::cin >> std::ws, targetPlayer) ||
                        targetPlayer.empty())
                    {
                        throw invalid_argument(
                            "Unable to read negotiation target");
                    }

                    Player *target = nullptr;
                    for (Player *candidate : players)
                        if (candidate != nullptr && *candidate->getName() == targetPlayer) target = candidate;
                    orders->add(new Negotiate(this, target));
                }
                else
                {
                    std::cout << "This card does not have a playable order.\n";
                }
            }
        }
    }
}

// Queue an order created by this player for the game engine to execute.
void Player::issueOrder(Order *order)
{
    orders->add(order);
}

// Return order list
OrdersList *Player::getOrders() const
{
    return orders;
}

// output player info and orders
std::ostream &operator<<(std::ostream &out, const Player &player)
{
    out << "Player: " << *player.name << '\n';
    out << "Territories to defend: " << player.territories->size() << '\n';
    out << "Territories to attack: "
        << player.territoriesToAttack->size() << '\n';
    out << "Cards: " << player.hand->size() << '\n';
    out << "Orders:\n";
    // prints all the card types in the hand
    out << *player.orders << '\n';

    return out;
}
