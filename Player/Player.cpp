#include "Player.h"

//constructor, create an order
Order::Order(const std::string& description) {
    this->description = new std::string(description);
}

//copy oncstructor for a deep copy
Order::Order(const Order& other) {
    description = new std::string(*other.description);
}

// copy another order's description
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        *description = *other.description;
    }
    return *this;
}

//destructor
Order::~Order() {
    delete description;
}

//output order's description to a stream
std::ostream& operator<<(std::ostream& out, const Order& order) {
    out << *order.description;
    return out;
}

//constructor for player
Player::Player(const std::string& name) {
    this->name = new std::string(name);
    territories = new std::vector<Territory*>();
    territoriesToAttack = new std::vector<Territory*>();
    hand = new std::vector<Card*>();
    orders = new std::vector<Order*>();
}

//copy constructor, copy of a player
Player::Player(const Player& other) {
    name = new std::string(*other.name);
    territories = new std::vector<Territory*>(*other.territories);
    territoriesToAttack =
        new std::vector<Territory*>(*other.territoriesToAttack);
    hand = new std::vector<Card*>(*other.hand);
    orders = new std::vector<Order*>();

    for (Order* order : *other.orders) {
        orders->push_back(new Order(*order));
    }
}

//replace player with other player data
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        *name = *other.name;
        *territories = *other.territories;
        *territoriesToAttack = *other.territoriesToAttack;
        *hand = *other.hand;

        //delete existing orders
        for (Order* order : *orders) {
            delete order;
        }

        orders->clear();

        //create copies of other player's orders
        for (Order* order : *other.orders) {
            orders->push_back(new Order(*order));
        }
    }

    return *this;
}

//destructor to release the memory for the players' data and orders
Player::~Player() {
    delete name;
    delete territories;
    delete territoriesToAttack;
    delete hand;

    for (Order* order : *orders) {
        delete order;
    }

    delete orders;
}

//add territory to owned territories
void Player::addTerritory(Territory* territory) {
    territories->push_back(territory);
}

//add territory to the list of territories they can attack
void Player::addTerritoryToAttack(Territory* territory) {
    territoriesToAttack->push_back(territory);
}

//add card to player hand
void Player::addCard(Card* card) {
    hand->push_back(card);
}

//return player list of territories to defend
std::vector<Territory*> Player::toDefend() const {
    return *territories;
}

//return player's list of territories to attack
std::vector<Territory*> Player::toAttack() const {
    return *territoriesToAttack;
}

//create an order and add to player's list
void Player::issueOrder(const std::string& description) {
    orders->push_back(new Order(description));
}

//output player info and orders
std::ostream& operator<<(std::ostream& out, const Player& player) {
    out << "Player: " << *player.name << '\n';
    out << "Territories to defend: " << player.territories->size() << '\n';
    out << "Territories to attack: "
        << player.territoriesToAttack->size() << '\n';
    out << "Cards: " << player.hand->size() << '\n';
    out << "Orders:\n";

    for (Order* order : *player.orders) {
        out << "  " << *order << '\n';
    }

    return out;
}