
/*
Rquirements to fulfill for players:
-own a collection of territories:
-own a hand of cards:
-Own list of orders:
-Todefend() (return a lsit of territories that are to be defended
-ToAttackIO (return a list of terr. that are to beattacked
- issueOrder() (creates order object and puts it in players list of orders)*/

#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Orders/Orders.h"

class Territory;
class Card;
class Deck;

// Class for players in the game, each players
class Player
{
private:
    std::string *name;
    std::vector<Territory *> *territories;
    std::unordered_set<Territory *> *ownedTerritories;
    std::vector<Territory *> *territoriesToAttack;
    std::vector<Card *> *hand;
    OrdersList *orders;
    int *reinforcementPool;
    std::vector<Player *> *negotiatedPlayers;
    bool *conqueredTerritoryThisTurn;

    Card *removeCard(const std::string &cardType);

public:
    // constructor, creates the player
    Player(const std::string &name);
    // copy constructor deep copy of another player
    Player(const Player &other);

    // Copy player data into 2nd player
    Player &operator=(const Player &other);
    // release memory
    ~Player();

    // add territory to player's owned territories
    void addTerritory(Territory *territory);
    // remove a territory from the player's owned territories
    void removeTerritory(Territory *territory);
    // Track temporary non-aggression agreements for the current turn.
    void negotiateWith(Player *player);
    bool isNegotiatedWith(const Player *player) const;
    void clearNegotiations();
    void markTerritoryConquered();
    bool hasConqueredTerritoryThisTurn() const;
    void clearConquestFlag();
    // add territory to  player's territories to attack
    void addTerritoryToAttack(Territory *territory);

    // add a card to player
    void addCard(Card *card);
    // Return the number of cards currently held by this player.
    int getHandSize() const;
    // getter and setter
    const std::string *getName() const;
    int *getReinforcementPool() const;

    void setReinforcementPool(int troops);

    // return player list of territories to defend
    std::vector<Territory *> toDefend() const;

    // return player's list of territories to attack
    std::vector<Territory *> toAttack() const;

    // Issue one order and return whether another order may be issued.
    void issueOrder(Deck *deck, std::vector<Player *> players);
    // Add an already constructed order to this player's execution queue.
    void issueOrder(Order *order);
    // returns orders
    OrdersList *getOrders() const;
    // output player information and orders
    friend std::ostream &operator<<(std::ostream &out, const Player &player);
};

#endif
