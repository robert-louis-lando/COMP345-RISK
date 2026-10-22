
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
#include <vector>

class Territory;
class Card;

//dummy class just for testing, we will replace it wth the real order.h
class Order {
private:
    std::string* description;

public:
//constructor to create order
    Order(const std::string& description);
    //Copy constructor, create a deep copy of an order with a copy of a description
    Order(const Order& other);


    Order& operator=(const Order& other);
    //destructor, release memory
    ~Order();

    //output description to an output stream
    friend std::ostream& operator<<(std::ostream& out, const Order& order);
};

//Class for players in the game, each players 
class Player {
private:
    std::string* name;
    std::vector<Territory*>* territories;
    std::vector<Territory*>* territoriesToAttack;
    std::vector<Card*>* hand;
    std::vector<Order*>* orders;

public:
//constructor, creates the player
    Player(const std::string& name);
    // copy constructor deep copy of another player
    Player(const Player& other);

     // Copy player data into 2nd player
    Player& operator=(const Player& other);
    //release memory
    ~Player();

        //add territory to player's owned territories
    void addTerritory(Territory* territory);
    //add territory to  player's territories to attack
    void addTerritoryToAttack(Territory* territory);

    //add a card to player
    void addCard(Card* card);

    //return player list of territories to defend
    std::vector<Territory*> toDefend() const;

    //return player's list of territories to attack
    std::vector<Territory*> toAttack() const;

    //create an order and add to player
    void issueOrder(const std::string& description);
    //output player information and orders
    friend std::ostream& operator<<(std::ostream& out, const Player& player);
};

#endif