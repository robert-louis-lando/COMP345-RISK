#include "Orders.h"

// ============================================================
// Order
// ============================================================

// Default constructor for Order
Order::Order() : playerName(""), effect(""), executed(false) {}

// Constructor for Order
Order::Order(const string& playerName) {
    this->playerName = playerName;
    effect = "";
    executed = false;
}

// Copy constructor for Order
Order::Order(const Order& other) {
    playerName = other.playerName;
    effect = other.effect;
    executed = other.executed;
}

// Assignment operator for Order
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        playerName = other.playerName;
        effect = other.effect;
        executed = other.executed;
    }
    return *this;
}

// Destructor for Order
Order::~Order() {}

// Stream insertion operator for Order
ostream& operator<<(ostream& out, const Order& order) {

    out << order.getType() << " order created by " << order.playerName;

    if (order.executed) {
        out << " [Executed]";
        out << " Effect: " << order.effect;
    } else {
        out << " [Not executed]";
    }
    return out;
}

// ============================================================
// Deploy
// ============================================================

// Default constructor for Deploy
Deploy::Deploy() : Order(){
    targetTerritory = "";
    numberOfArmies = 0;
}

// Constructor for Deploy
Deploy::Deploy(const string& playerName, const string& targetTerritory, int numberOfArmies) : Order(playerName) {
    this->targetTerritory = targetTerritory;
    this->numberOfArmies = numberOfArmies;
}

// Copy constructor for Deploy
Deploy::Deploy(const Deploy& other) : Order(other) {
    targetTerritory = other.targetTerritory;
    numberOfArmies = other.numberOfArmies;
}

// Assignment operator for Deploy
Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);
        targetTerritory = other.targetTerritory;
        numberOfArmies = other.numberOfArmies;
    }
    return *this;
}

// Destructor for Deploy
Deploy::~Deploy() {}

// Validates a Deploy order

// The complete game validation will be implemented later.
// For now, the order must contain a player, a territory, and a positive number of armies. 

bool Deploy::validate() const{
    return !playerName.empty() && !targetTerritory.empty() && numberOfArmies > 0;
}

// Executes the Deploy order
void Deploy::execute() {
    if (validate()) {
        executed = true;
        effect = "Deployed " + to_string(numberOfArmies) + " armies to " + targetTerritory;
    } else {
        executed = false;
        effect = "Deploy order is invalid.";
    }
}

// Creates a copy of the Deploy order
Order* Deploy::clone() const {
    return new Deploy(*this);
}

// Returns the order type
string Deploy::getType() const {
    return "Deploy";
}

// Stream insertion operator for Deploy
ostream& operator<<(ostream& out, const Deploy& order) {
    out << static_cast<const Order&>(order);
    out << " | Target: " << order.targetTerritory;
    out << " | Armies: " << order.numberOfArmies;
    return out;
}

// ============================================================
// Advance
// ============================================================

// Default constructor for Advance
Advance::Advance() : Order() {
    sourceTerritory = "";
    targetTerritory = "";
    numberOfArmies = 0;
}

// Constructor for Advance
Advance::Advance(const string& playerName, const string& sourceTerritory, const string& targetTerritory, int numberOfArmies) : Order(playerName) {
    this->sourceTerritory = sourceTerritory;
    this->targetTerritory = targetTerritory;
    this->numberOfArmies = numberOfArmies;
}

// Copy constructor for Advance
Advance::Advance(const Advance& other) : Order(other) {
    sourceTerritory = other.sourceTerritory;
    targetTerritory = other.targetTerritory;
    numberOfArmies = other.numberOfArmies;
}

// Assignment operator for Advance
Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);
        sourceTerritory = other.sourceTerritory;
        targetTerritory = other.targetTerritory;
        numberOfArmies = other.numberOfArmies;
    }
    return *this;
}

// Destructor for Advance
Advance::~Advance() {}

// Validates an Advance order
bool Advance::validate() const {
    return !playerName.empty() && !sourceTerritory.empty() && !targetTerritory.empty() && numberOfArmies > 0 && sourceTerritory != targetTerritory;
}

// Executes the Advance order
void Advance::execute() {
    if (validate()) {
        executed = true;
        effect = "Advanced " + to_string(numberOfArmies) + " armies from " + sourceTerritory + " to " + targetTerritory;
    } else {
        executed = false;
        effect = "Advance order is invalid.";
    }
}

// Creates a copy of the Advance order
Order* Advance::clone() const {
    return new Advance(*this);
}

// Returns the order type
string Advance::getType() const {
    return "Advance";
}

// Stream insertion operator for Advance
ostream& operator<<(ostream& out, const Advance& order) {
    out << static_cast<const Order&>(order);
    out << " | From: " << order.sourceTerritory;
    out << " | To: " << order.targetTerritory;
    out << " | Armies: " << order.numberOfArmies;
    return out;
}

// ============================================================
// Bomb
// ============================================================

// Default constructor for Bomb
Bomb::Bomb() : Order() {
    targetTerritory = "";
}

// Constructor for Bomb
Bomb::Bomb(const string& playerName, const string& targetTerritory) : Order(playerName) {
    this->targetTerritory = targetTerritory;
}

// Copy constructor for Bomb
Bomb::Bomb(const Bomb& other) : Order(other) {
    targetTerritory = other.targetTerritory;
}

// Assignment operator for Bomb
Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        targetTerritory = other.targetTerritory;
    }
    return *this;
}

// Destructor for Bomb
Bomb::~Bomb() {}

// Validates a Bomb order
bool Bomb::validate() const {
    return !playerName.empty() && !targetTerritory.empty();
}

// Executes the Bomb order
void Bomb::execute() {
    if (validate()) {
        executed = true;
        effect = "Bombed territory " + targetTerritory;
    } else {
        executed = false;
        effect = "Bomb order is invalid.";
    }
}

// Creates a copy of the Bomb order
Order* Bomb::clone() const {
    return new Bomb(*this);
}

// Returns the order type
string Bomb::getType() const {
    return "Bomb";
}

// Stream insertion operator for Bomb
ostream& operator<<(ostream& out, const Bomb& order) {
    out << static_cast<const Order&>(order);
    out << " | Target: " << order.targetTerritory;
    return out;
}

// ============================================================
// Blockade
// ============================================================

// Default constructor for Blockade
Blockade::Blockade() : Order() {
    targetTerritory = "";
}

// Constructor for Blockade
Blockade::Blockade(const string& playerName, const string& targetTerritory) : Order(playerName) {
    this->targetTerritory = targetTerritory;
}

// Copy constructor for Blockade
Blockade::Blockade(const Blockade& other) : Order(other) {
    targetTerritory = other.targetTerritory;
}

// Assignment operator for Blockade
Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        targetTerritory = other.targetTerritory;
    }
    return *this;
}

// Destructor for Blockade
Blockade::~Blockade() {}

// Validates a Blockade order
bool Blockade::validate() const {
    return !playerName.empty() && !targetTerritory.empty();
}

// Executes the Blockade order
void Blockade::execute() {
    if (validate()) {
        executed = true;
        effect = "Blockaded territory " + targetTerritory;
    } else {
        executed = false;
        effect = "Blockade order is invalid.";
    }
}

// Creates a copy of the Blockade order
Order* Blockade::clone() const {
    return new Blockade(*this);
}

// Returns the order type
string Blockade::getType() const {
    return "Blockade";
}

// Stream insertion operator for Blockade
ostream& operator<<(ostream& out, const Blockade& order) {
    out << static_cast<const Order&>(order);
    out << " | Target: " << order.targetTerritory;
    return out;
}

// ============================================================
// Airlift
// ============================================================

// Default constructor for Airlift
Airlift::Airlift() : Order() {
    sourceTerritory = "";
    targetTerritory = "";
    numberOfArmies = 0;
}

// Constructor for Airlift
Airlift::Airlift(const string& playerName, const string& sourceTerritory, const string& targetTerritory, int numberOfArmies) : Order(playerName) {
    this->sourceTerritory = sourceTerritory;
    this->targetTerritory = targetTerritory;
    this->numberOfArmies = numberOfArmies;
}

// Copy constructor for Airlift
Airlift::Airlift(const Airlift& other) : Order(other) {
    sourceTerritory = other.sourceTerritory;
    targetTerritory = other.targetTerritory;
    numberOfArmies = other.numberOfArmies;
}

// Assignment operator for Airlift
Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);
        sourceTerritory = other.sourceTerritory;
        targetTerritory = other.targetTerritory;
        numberOfArmies = other.numberOfArmies;
    }
    return *this;
}

// Destructor for Airlift
Airlift::~Airlift() {}

// Validates an Airlift order
bool Airlift::validate() const {
    return !playerName.empty() && !sourceTerritory.empty() && !targetTerritory.empty() && numberOfArmies > 0 && sourceTerritory != targetTerritory;
}

// Executes the Airlift order
void Airlift::execute() {
    if (validate()) {
        executed = true;
        effect = "Airlifted " + to_string(numberOfArmies) + " armies from " + sourceTerritory + " to " + targetTerritory;
    } else {
        executed = false;
        effect = "Airlift order is invalid.";
    }
}

// Creates a copy of the Airlift order
Order* Airlift::clone() const {
    return new Airlift(*this);
}

// Returns the order type
string Airlift::getType() const {
    return "Airlift";
}

// Stream insertion operator for Airlift
ostream& operator<<(ostream& out, const Airlift& order) {
    out << static_cast<const Order&>(order);
    out << " | From: " << order.sourceTerritory;
    out << " | To: " << order.targetTerritory;
    out << " | Armies: " << order.numberOfArmies;
    return out;
}

// ============================================================
// Negotiate
// ============================================================

// Default constructor for Negotiate
Negotiate::Negotiate() : Order() {
    targetPlayer = "";
}

// Constructor for Negotiate
Negotiate::Negotiate(const string& playerName, const string& targetPlayer) : Order(playerName) {
    this->targetPlayer = targetPlayer;
}

// Copy constructor for Negotiate
Negotiate::Negotiate(const Negotiate& other) : Order(other) {
    targetPlayer = other.targetPlayer;
}

// Assignment operator for Negotiate
Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        targetPlayer = other.targetPlayer;
    }
    return *this;
}

// Destructor for Negotiate
Negotiate::~Negotiate() {}

// Validates a Negotiate order
bool Negotiate::validate() const {
    return !playerName.empty() && !targetPlayer.empty() && playerName != targetPlayer;
}

// Executes the Negotiate order
void Negotiate::execute() {
    if (validate()) {
        executed = true;
        effect = playerName + " negotiated with " + targetPlayer;
    } else {
        executed = false;
        effect = "Negotiate order is invalid.";
    }
}

// Creates a copy of the Negotiate order
Order* Negotiate::clone() const {
    return new Negotiate(*this);
}

// Returns the order type
string Negotiate::getType() const {
    return "Negotiate";
}

// Stream insertion operator for Negotiate
ostream& operator<<(ostream& out, const Negotiate& order) {
    out << static_cast<const Order&>(order);
    out << " | Target Player: " << order.targetPlayer;
    return out;
}

// ============================================================
// OrdersList
// ============================================================

// Default constructor for OrdersList
OrdersList::OrdersList() {}

// Copy constructor for OrdersList
// Each order is cloned so that the new OrdersList owns its own copies.
// This prevents two lists from deleting the same Order objet. 
OrdersList::OrdersList(const OrdersList& other) {
    for (unsigned int i = 0; i < other.orders.size(); i++) {
        orders.push_back(other.orders[i]->clone());
    }
}

// Assignment operator for OrdersList
// Existing orders are deleted before making deep copies of the orders
// in the other list.
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        // Delete existing orders.
        for (unsigned int i = 0; i < orders.size(); i++) {
            delete orders[i];
        }
        orders.clear();

        // Make deep copies
        for (unsigned int i = 0; i < other.orders.size(); i++) {
            orders.push_back(other.orders[i]->clone());
        }
    }
    return *this;
}

// Destructor for OrdersList
// Deletes all Order objects owned by the list
OrdersList::~OrdersList() {
    for (unsigned int i = 0; i < orders.size(); i++) {
        delete orders[i];
    }
    orders.clear();
}

// Adds an order to the end of the list
void OrdersList::add(Order* order) {

    if (order != nullptr) {
        orders.push_back(order);
    }

}

// Removes an order from the list
// Returns true if the order was succesfully removed
bool OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(orders.size())) {
        return false;
    }
    delete orders[index];
    orders.erase(orders.begin() + index);
    return true;
}

// Moves an order from one position to another
// Returns true if the move was successful
bool OrdersList::move(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= static_cast<int>(orders.size()) || toIndex < 0 || toIndex >= static_cast<int>(orders.size())) {
        return false;
    }

    if (fromIndex == toIndex) {
        return true; 
    }

    Order* orderToMove = orders[fromIndex];
    orders.erase(orders.begin() + fromIndex);
    orders.insert(orders.begin() + toIndex, orderToMove);
    return true;
}

// Returns the number of orders in the list
int OrdersList::size() const {
    return static_cast<int>(orders.size());
}

// Returns the order at the specified position
// Returns nullptr if the index is invalid
Order* OrdersList::get(int index) const {
    if (index < 0 || index >= static_cast<int>(orders.size())) {
        return nullptr;
    }
    return orders[index];
}

// Stream insertion operator for OrdersList
ostream& operator<<(ostream& out, const OrdersList& ordersList) {
    if (ordersList.orders.empty()) {
        out << "Orders List is empty.";
        return out;
    } 

    for (unsigned int i = 0; i < ordersList.orders.size(); i++) {
        out << "[" << i << "]" << *ordersList.orders[i] << endl;
    }

    return out;
}