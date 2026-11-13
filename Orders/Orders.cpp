#include "Orders.h"
#include "../Player/Player.h"
#include "../map/Map.h"
#include <algorithm>
#include <random>


#include <sstream> 


// Locate a territory visible to the player, including adjacent enemy targets.
static Territory *findTerritory(Player *player, const string &name)
{
    if (player == nullptr) return nullptr;
    for (Territory *territory : player->toDefend())
        if (territory != nullptr && territory->getName() == name) return territory;
    for (Territory *territory : player->toAttack())
        if (territory != nullptr && territory->getName() == name) return territory;
    return nullptr;
}

// Determine whether two territories share a border.
static bool adjacent(const Territory *from, const Territory *to)
{
    if (from == nullptr || to == nullptr) return false;
    const vector<Territory *> &neighbors = from->getAdjacentTerritories();
    return std::find(neighbors.begin(), neighbors.end(), to) != neighbors.end();
}

// ============================================================
// Order
// ============================================================

// Default constructor for Order
Order::Order() : playerName(new string()), effect(new string()), executed(new bool(false)), issuer(nullptr) {}

// Constructor for Order
Order::Order(const string& playerName)
    : playerName(new string(playerName)), effect(new string()), executed(new bool(false)), issuer(nullptr) {}

// Connect the order to the player whose state it will change.
Order::Order(Player *player) : playerName(new string(player ? *player->getName() : string())), effect(new string()), executed(new bool(false)), issuer(player) {}

// Copy constructor for Order
Order::Order(const Order& other)
    : Subject(other), ILoggable(other),
      playerName(new string(*other.playerName)),
      effect(new string(*other.effect)),
      executed(new bool(*other.executed)), issuer(other.issuer) {}

// Assignment operator for Order
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        Subject::operator=(other);
        ILoggable::operator=(other); 
        *playerName = *other.playerName;
        *effect = *other.effect;
        *executed = *other.executed;
        issuer = other.issuer;
    }
    return *this;
}

// Destructor for Order
Order::~Order() {
    delete playerName;
    delete effect;
    delete executed;
}

string Order::stringToLog() const { 
    return "Order Effect (" + getType() + "): " + *effect;   
} 

// Stream insertion operator for Order
ostream& operator<<(ostream& out, const Order& order) {

    out << order.getType() << " order created by " << *order.playerName;

    if (*order.executed) {
        out << " [Executed]";
        out << " Effect: " << *order.effect;
    } else {
        out << " [Not executed]";
    }
    return out;
}

// ============================================================
// Deploy
// ============================================================

// Default constructor for Deploy
Deploy::Deploy() : Order(), targetTerritory(new string()), numberOfArmies(new int(0)) {}

// Constructor for Deploy
Deploy::Deploy(const string& playerName, const string& targetTerritory, int numberOfArmies)
    : Order(playerName),
      targetTerritory(new string(targetTerritory)),
      numberOfArmies(new int(numberOfArmies)) {}

// Construct a deploy linked to the issuing player's territory state.
Deploy::Deploy(Player *player, const string& targetTerritory, int numberOfArmies)
    : Order(player), targetTerritory(new string(targetTerritory)), numberOfArmies(new int(numberOfArmies)) {}

// Copy constructor for Deploy
Deploy::Deploy(const Deploy& other)
    : Order(other),
      targetTerritory(new string(*other.targetTerritory)),
      numberOfArmies(new int(*other.numberOfArmies)) {}

// Assignment operator for Deploy
Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Order::operator=(other);
        *targetTerritory = *other.targetTerritory;
        *numberOfArmies = *other.numberOfArmies;
    }
    return *this;
}

// Destructor for Deploy
Deploy::~Deploy() {
    delete targetTerritory;
    delete numberOfArmies;
}

// Validates a Deploy order

// The complete game validation will be implemented later.
// For now, the order must contain a player, a territory, and a positive number of armies. 

bool Deploy::validate() const{
    if (playerName->empty() || targetTerritory->empty() || *numberOfArmies <= 0) return false;
    if (issuer == nullptr) return true; // Supports the standalone Orders driver.
    Territory *target = findTerritory(issuer, *targetTerritory);
    return target != nullptr && target->getOwner() == issuer &&
        *issuer->getReinforcementPool() >= *numberOfArmies;
}

// Executes the Deploy order
void Deploy::execute() {
    if (validate()) {
        if (issuer != nullptr) {
            Territory *target = findTerritory(issuer, *targetTerritory);
            issuer->setReinforcementPool(*issuer->getReinforcementPool() - *numberOfArmies);
            target->setArmyCount(target->getArmyCount() + *numberOfArmies);
        }
        *executed = true;
        *effect = "Deployed " + to_string(*numberOfArmies) + " armies to " + *targetTerritory;
    } else {
        *executed = false;
        *effect = "Deploy order is invalid.";
    }


    Notify(this);
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
    out << " | Target: " << *order.targetTerritory;
    out << " | Armies: " << *order.numberOfArmies;
    return out;
}

// ============================================================
// Advance
// ============================================================

// Default constructor for Advance
Advance::Advance()
    : Order(),
      sourceTerritory(new string()),
      targetTerritory(new string()),
      numberOfArmies(new int(0)) {}

// Constructor for Advance
Advance::Advance(const string& playerName, const string& sourceTerritory, const string& targetTerritory, int numberOfArmies)
    : Order(playerName),
      sourceTerritory(new string(sourceTerritory)),
      targetTerritory(new string(targetTerritory)),
      numberOfArmies(new int(numberOfArmies)) {}

// Construct an advance linked to the issuing player's game state.
Advance::Advance(Player *player, const string& source, const string& target, int armies)
    : Order(player), sourceTerritory(new string(source)), targetTerritory(new string(target)), numberOfArmies(new int(armies)) {}

// Copy constructor for Advance
Advance::Advance(const Advance& other)
    : Order(other),
      sourceTerritory(new string(*other.sourceTerritory)),
      targetTerritory(new string(*other.targetTerritory)),
      numberOfArmies(new int(*other.numberOfArmies)) {}

// Assignment operator for Advance
Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Order::operator=(other);
        *sourceTerritory = *other.sourceTerritory;
        *targetTerritory = *other.targetTerritory;
        *numberOfArmies = *other.numberOfArmies;
    }
    return *this;
}

// Destructor for Advance
Advance::~Advance() {
    delete sourceTerritory;
    delete targetTerritory;
    delete numberOfArmies;
}

// Validates an Advance order
bool Advance::validate() const {
    if (playerName->empty() || sourceTerritory->empty() || targetTerritory->empty() || *numberOfArmies <= 0 || *sourceTerritory == *targetTerritory) return false;
    if (issuer == nullptr) return true;
    Territory *source = findTerritory(issuer, *sourceTerritory);
    Territory *target = findTerritory(issuer, *targetTerritory);
    return source && target && source->getOwner() == issuer && adjacent(source, target) &&
        source->getArmyCount() >= *numberOfArmies &&
        !(target->getOwner() != issuer && issuer->isNegotiatedWith(target->getOwner()));
}

// Executes the Advance order
void Advance::execute() {
    if (validate()) {
        if (issuer != nullptr) {
            Territory *source = findTerritory(issuer, *sourceTerritory);
            Territory *target = findTerritory(issuer, *targetTerritory);
            source->setArmyCount(source->getArmyCount() - *numberOfArmies);
            if (target->getOwner() == issuer) {
                target->setArmyCount(target->getArmyCount() + *numberOfArmies);
            } else if (target->getOwner() != nullptr && issuer->isNegotiatedWith(target->getOwner())) {
                source->setArmyCount(source->getArmyCount() + *numberOfArmies);
                *executed = false; *effect = "Advance blocked by negotiation."; Notify(this); return;
            } else {
                int attackers = *numberOfArmies;
                int defenders = target->getArmyCount();
                static std::mt19937 rng(std::random_device{}());
                std::bernoulli_distribution attackHit(0.60), defenseHit(0.70);
                while (attackers > 0 && defenders > 0) {
                    int lostAttackers = 0, lostDefenders = 0;
                    for (int i=0;i<attackers;++i) if (attackHit(rng)) ++lostDefenders;
                    for (int i=0;i<defenders;++i) if (defenseHit(rng)) ++lostAttackers;
                    defenders = std::max(0, defenders-lostDefenders);
                    attackers = std::max(0, attackers-lostAttackers);
                }
                target->setArmyCount(defenders);
                if (defenders == 0 && attackers > 0) {
                    Player *oldOwner = target->getOwner();
                    if (oldOwner) oldOwner->removeTerritory(target);
                    target->setOwner(issuer); issuer->addTerritory(target);
                    target->setArmyCount(attackers);
                    issuer->markTerritoryConquered();
                }
            }
        }
        *executed = true;
        *effect = "Advanced " + to_string(*numberOfArmies) + " armies from " + *sourceTerritory + " to " + *targetTerritory;
    } else {
        *executed = false;
        *effect = "Advance order is invalid.";
    }

    Notify(this);
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
    out << " | From: " << *order.sourceTerritory;
    out << " | To: " << *order.targetTerritory;
    out << " | Armies: " << *order.numberOfArmies;
    return out;
}

// ============================================================
// Bomb
// ============================================================

// Default constructor for Bomb
Bomb::Bomb() : Order(), targetTerritory(new string()) {}

// Constructor for Bomb
Bomb::Bomb(const string& playerName, const string& targetTerritory)
    : Order(playerName), targetTerritory(new string(targetTerritory)) {}

// Construct a bomb order tied to its issuing player.
Bomb::Bomb(Player *player, const string& target)
    : Order(player), targetTerritory(new string(target)) {}

// Copy constructor for Bomb
Bomb::Bomb(const Bomb& other)
    : Order(other), targetTerritory(new string(*other.targetTerritory)) {}

// Assignment operator for Bomb
Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Order::operator=(other);
        *targetTerritory = *other.targetTerritory;
    }
    return *this;
}

// Destructor for Bomb
Bomb::~Bomb() {
    delete targetTerritory;
}

// Validates a Bomb order
bool Bomb::validate() const {
    if (playerName->empty() || targetTerritory->empty()) return false;
    if (issuer == nullptr) return true;
    Territory *target = findTerritory(issuer, *targetTerritory);
    if (target == nullptr || target->getOwner() == issuer || issuer->isNegotiatedWith(target->getOwner())) return false;
    for (Territory *owned : issuer->toDefend()) if (adjacent(owned, target)) return true;
    return false;
}

// Executes the Bomb order
void Bomb::execute() {
    if (validate()) {
        if (issuer != nullptr) {
            Territory *target = findTerritory(issuer, *targetTerritory);
            target->setArmyCount(target->getArmyCount() / 2);
        }
        *executed = true;
        *effect = "Bombed territory " + *targetTerritory;
    } else {
        *executed = false;
        *effect = "Bomb order is invalid.";
    }

     Notify(this); 
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
    out << " | Target: " << *order.targetTerritory;
    return out;
}

// ============================================================
// Blockade
// ============================================================

// Default constructor for Blockade
Blockade::Blockade() : Order(), targetTerritory(new string()) {}

// Constructor for Blockade
Blockade::Blockade(const string& playerName, const string& targetTerritory)
    : Order(playerName), targetTerritory(new string(targetTerritory)) {}

// Construct a blockade linked to the issuing player's state.
Blockade::Blockade(Player *player, const string& target)
    : Order(player), targetTerritory(new string(target)) {}

// Copy constructor for Blockade
Blockade::Blockade(const Blockade& other)
    : Order(other), targetTerritory(new string(*other.targetTerritory)) {}

// Assignment operator for Blockade
Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Order::operator=(other);
        *targetTerritory = *other.targetTerritory;
    }
    return *this;
}

// Destructor for Blockade
Blockade::~Blockade() {
    delete targetTerritory;
}

// Validates a Blockade order
bool Blockade::validate() const {
    if (playerName->empty() || targetTerritory->empty()) return false;
    if (issuer == nullptr) return true;
    Territory *target = findTerritory(issuer, *targetTerritory);
    return target != nullptr && target->getOwner() == issuer;
}

// Executes the Blockade order
void Blockade::execute() {
    if (validate()) {
        if (issuer != nullptr) {
            static Player neutral("Neutral");
            Territory *target = findTerritory(issuer, *targetTerritory);
            target->setArmyCount(target->getArmyCount() * 2);
            issuer->removeTerritory(target);
            target->setOwner(&neutral);
            neutral.addTerritory(target);
        }
        *executed = true;
        *effect = "Blockaded territory " + *targetTerritory;
    } else {
        *executed = false;
        *effect = "Blockade order is invalid.";
    }

    Notify(this);
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
    out << " | Target: " << *order.targetTerritory;
    return out;
}

// ============================================================
// Airlift
// ============================================================

// Default constructor for Airlift
Airlift::Airlift()
    : Order(),
      sourceTerritory(new string()),
      targetTerritory(new string()),
      numberOfArmies(new int(0)) {}

// Constructor for Airlift
Airlift::Airlift(const string& playerName, const string& sourceTerritory, const string& targetTerritory, int numberOfArmies)
    : Order(playerName),
      sourceTerritory(new string(sourceTerritory)),
      targetTerritory(new string(targetTerritory)),
      numberOfArmies(new int(numberOfArmies)) {}

// Construct an airlift tied to the issuing player's territories.
Airlift::Airlift(Player *player, const string& source, const string& target, int armies)
    : Order(player), sourceTerritory(new string(source)), targetTerritory(new string(target)), numberOfArmies(new int(armies)) {}

// Copy constructor for Airlift
Airlift::Airlift(const Airlift& other)
    : Order(other),
      sourceTerritory(new string(*other.sourceTerritory)),
      targetTerritory(new string(*other.targetTerritory)),
      numberOfArmies(new int(*other.numberOfArmies)) {}

// Assignment operator for Airlift
Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Order::operator=(other);
        *sourceTerritory = *other.sourceTerritory;
        *targetTerritory = *other.targetTerritory;
        *numberOfArmies = *other.numberOfArmies;
    }
    return *this;
}

// Destructor for Airlift
Airlift::~Airlift() {
    delete sourceTerritory;
    delete targetTerritory;
    delete numberOfArmies;
}

// Validates an Airlift order
bool Airlift::validate() const {
    if (playerName->empty() || sourceTerritory->empty() || targetTerritory->empty() || *numberOfArmies <= 0 || *sourceTerritory == *targetTerritory) return false;
    if (issuer == nullptr) return true;
    Territory *source = findTerritory(issuer, *sourceTerritory), *target = findTerritory(issuer, *targetTerritory);
    return source && target && source->getOwner() == issuer && target->getOwner() == issuer && source->getArmyCount() >= *numberOfArmies;
}

// Executes the Airlift order
void Airlift::execute() {
    if (validate()) {
        if (issuer != nullptr) {
            Territory *source = findTerritory(issuer, *sourceTerritory), *target = findTerritory(issuer, *targetTerritory);
            source->setArmyCount(source->getArmyCount() - *numberOfArmies);
            target->setArmyCount(target->getArmyCount() + *numberOfArmies);
        }
        *executed = true;
        *effect = "Airlifted " + to_string(*numberOfArmies) + " armies from " + *sourceTerritory + " to " + *targetTerritory;
    } else {
        *executed = false;
        *effect = "Airlift order is invalid.";
    }

     Notify(this); 
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
    out << " | From: " << *order.sourceTerritory;
    out << " | To: " << *order.targetTerritory;
    out << " | Armies: " << *order.numberOfArmies;
    return out;
}

// ============================================================
// Negotiate
// ============================================================

// Default constructor for Negotiate
Negotiate::Negotiate() : Order(), targetPlayer(new string()), negotiatedTarget(nullptr) {}

// Constructor for Negotiate
Negotiate::Negotiate(const string& playerName, const string& targetPlayer)
    : Order(playerName), targetPlayer(new string(targetPlayer)), negotiatedTarget(nullptr) {}

// Construct a negotiation order with a direct pointer to the other player.
Negotiate::Negotiate(Player *player, Player *target)
    : Order(player), targetPlayer(new string(target ? *target->getName() : string())), negotiatedTarget(target) {}

// Keep compatibility with name-only orders when no player object is available.
Negotiate::Negotiate(Player *player, const string& target)
    : Order(player), targetPlayer(new string(target)), negotiatedTarget(nullptr) {}

// Copy constructor for Negotiate
Negotiate::Negotiate(const Negotiate& other)
    : Order(other), targetPlayer(new string(*other.targetPlayer)), negotiatedTarget(other.negotiatedTarget) {}

// Assignment operator for Negotiate
Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Order::operator=(other);
        *targetPlayer = *other.targetPlayer;
        negotiatedTarget = other.negotiatedTarget;
    }
    return *this;
}

// Destructor for Negotiate
Negotiate::~Negotiate() {
    delete targetPlayer;
}

// Validates a Negotiate order
bool Negotiate::validate() const {
    return !playerName->empty() && !targetPlayer->empty() && *playerName != *targetPlayer &&
        (issuer == nullptr || (negotiatedTarget != nullptr && negotiatedTarget != issuer));
}

// Executes the Negotiate order
void Negotiate::execute() {
    if (validate()) {
        if (issuer != nullptr) {
            issuer->negotiateWith(negotiatedTarget);
            negotiatedTarget->negotiateWith(issuer);
        }
        *executed = true;
        *effect = *playerName + " negotiated with " + *targetPlayer;
    } else {
        *executed = false;
        *effect = "Negotiate order is invalid.";
    }

    Notify(this);
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
    out << " | Target Player: " << *order.targetPlayer;
    return out;
}

// ============================================================
// OrdersList
// ============================================================

// Default constructor for OrdersList
OrdersList::OrdersList() : orders(new vector<Order*>()) {}

// Copy constructor for OrdersList
// Each order is cloned so that the new OrdersList owns its own copies.
// This prevents two lists from deleting the same Order objet. 
OrdersList::OrdersList(const OrdersList& other) : Subject(other), ILoggable(other), orders(new vector<Order*>()) {
    for (unsigned int i = 0; i < other.orders->size(); i++) {
        orders->push_back((*other.orders)[i]->clone());
    }
}

// Assignment operator for OrdersList
// Existing orders are deleted before making deep copies of the orders
// in the other list.
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        Subject::operator=(other);
        ILoggable::operator=(other);
        // Delete existing orders.
        for (unsigned int i = 0; i < orders->size(); i++) {
            delete (*orders)[i];
        }
        orders->clear();

        // Make deep copies
        for (unsigned int i = 0; i < other.orders->size(); i++) {
            orders->push_back((*other.orders)[i]->clone());
        }
    }
    return *this;
}

// Destructor for OrdersList
// Deletes all Order objects owned by the list
OrdersList::~OrdersList() {
    for (unsigned int i = 0; i < orders->size(); i++) {
        delete (*orders)[i];
    }
    orders->clear();
    delete orders;
}

// Adds an order to the end of the list
void OrdersList::add(Order* order) {

    if (order != nullptr) {
        orders->push_back(order);
        passObserversTo(*order);
        Notify(this);
    }

}

// Removes an order from the list
// Returns true if the order was succesfully removed
bool OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(orders->size())) {
        return false;
    }
    delete (*orders)[index];
    orders->erase(orders->begin() + index);
    return true;
}

// Moves an order from one position to another
// Returns true if the move was successful
bool OrdersList::move(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= static_cast<int>(orders->size()) || toIndex < 0 || toIndex >= static_cast<int>(orders->size())) {
        return false;
    }

    if (fromIndex == toIndex) {
        return true; 
    }

    Order* orderToMove = (*orders)[fromIndex];
    orders->erase(orders->begin() + fromIndex);
    orders->insert(orders->begin() + toIndex, orderToMove);
    return true;
}

// Returns the number of orders in the list
int OrdersList::size() const {
    return static_cast<int>(orders->size());
}

// Returns the order at the specified position
// Returns nullptr if the index is invalid
Order* OrdersList::get(int index) const {
    if (index < 0 || index >= static_cast<int>(orders->size())) {
        return nullptr;
    }
    return (*orders)[index];
}

string OrdersList::stringToLog() const {   
    if (orders->empty()) {  
        return "Order list is empty.";   
    }   
    ostringstream description;   
    description << *orders->back(); 
    return "Order issued: " + description.str();  
} 

// Stream insertion operator for OrdersList
ostream& operator<<(ostream& out, const OrdersList& ordersList) {
    if (ordersList.orders->empty()) {
        out << "Orders List is empty.";
        return out;
    } 

    for (unsigned int i = 0; i < ordersList.orders->size(); i++) {
        out << "[" << i << "]" << *(*ordersList.orders)[i] << endl;
    }

    return out;
}
