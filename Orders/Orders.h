#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Order is the base class for all orders.

// An order contains the name of the player who created it,
// a description of its effect, and information about whether 
// the order has been executed.

class Order{
    protected:
        string playerName;
        string effect;
        bool executed;

    public:
        // Default constructor
        Order();

        // Constructor with a player name
        Order(const string& playerName);

        // Copy constructor
        Order(const Order& other);

        // Assignment operator
        Order& operator=(const Order& other);

        // Virtual destructor
        virtual ~Order();

        // Validates whether the order can be executed
        virtual bool validate() const = 0;

        // Executes the order if it is valid
        virtual void execute() = 0;

        // Creates a copy of the order
        virtual Order* clone() const = 0;

        // Returns the type of the order
        virtual string getType() const = 0;

        // Stream insertion operator
        friend ostream& operator<<(ostream& out, const Order& order);
};

// A deploy order represents placing armies on a territory.
class Deploy : public Order {
    private:
        string targetTerritory;
        int numberOfArmies;

    public:
        // Default constructor
        Deploy();

        // Constructor
        Deploy(const string& playerName, const string& targetTerritory, int numberOfArmies);

        // Copy constructor
        Deploy(const Deploy& other);

        // Assignment operator
        Deploy& operator=(const Deploy& other);

        // Destructor
        ~Deploy();

        // Validates the deploy order
        bool validate() const override;

        // Executes the deploy order
        void execute() override;

        // Creates a copy of this order
        Order* clone() const override;

        // Returns the order type
        string getType() const override;

        // Stream insertion operator
        friend ostream& operator<<(ostream& out, const Deploy& order);
};

// An advance order represents moving armies from one territory to another

class Advance : public Order{
    private:
        string sourceTerritory;
        string targetTerritory;
        int numberOfArmies;
    
    public:
        // Default constructor
        Advance();

        // Constructor
        Advance(const string& playerName, const string& sourceTerritory, const string& targetTerritory, int numberOfArmies);

        // Copy constructor
        Advance(const Advance& other);

        // Assignment operator
        Advance& operator=(const Advance& other);

        // Destructor
        ~Advance();

        // Validates the advance order
        bool validate() const override;

        // Executes the advance order
        void execute() override;

        // Creates a copy of this order
        Order* clone() const override;

        // Returns the order type
        string getType() const override;

        // Stream insertion operator
        friend ostream& operator<<(ostream& out, const Advance& order);
};

// A bomb order represents attacking a target territory with a bomb

class Bomb : public Order{
    private:
        string targetTerritory;

    public:
        // Default constructor
        Bomb();

        // Constructor
        Bomb(const string& playerName, const string& targetTerritory);

        // Copy constructor
        Bomb(const Bomb& other);

        // Assignment operator
        Bomb& operator=(const Bomb& other);

        // Destructor
        ~Bomb();

        // Validates the bomb order
        bool validate() const override;

        // Executes the bomb order
        void execute() override;

        // Creates a copy of this order
        Order* clone() const override;

        // Returns the order type
        string getType() const override;

        // Stream insertion operator
        friend ostream& operator<<(ostream& out, const Bomb& order);
};

// A blockade order represents turning a territory into a blockade
class Blockade : public Order{
    private:
        string targetTerritory;

    public:
        // Default constructor
        Blockade();

        // Constructor
        Blockade(const string& playerName, const string& targetTerritory);

        // Copy constructor
        Blockade(const Blockade& other);

        // Assignment operator
        Blockade& operator=(const Blockade& other);

        // Destructor
        ~Blockade();

        // Validates the blockade order
        bool validate() const override;

        // Executes the blockade order
        void execute() override;

        // Creates a copy of this order
        Order* clone() const override;

        // Returns the order type
        string getType() const override;

        // Stream insertion operator
        friend ostream& operator<<(ostream& out, const Blockade& order);
};

// An airlift order represents moving armies between two territories without requiring them to be adjacent
class Airlift : public Order{
    private:
        string sourceTerritory;
        string targetTerritory;
        int numberOfArmies;

    public:
        // Default constructor
        Airlift();

        // Constructor
        Airlift(const string& playerName, const string& sourceTerritory, const string& targetTerritory, int numberOfArmies);

        // Copy constructor
        Airlift(const Airlift& other);

        // Assignment operator
        Airlift& operator=(const Airlift& other);

        // Destructor
        ~Airlift();

        // Validates the airlift order
        bool validate() const override;

        // Executes the airlift order
        void execute() override;

        // Creates a copy of this order
        Order* clone() const override;

        // Returns the order type
        string getType() const override;

        // Stream insertion operator
        friend ostream& operator<<(ostream& out, const Airlift& order);
};

// A negotiate order represents negotiating with another player
class Negotiate : public Order{
    private:
        string targetPlayer;

    public:
        // Default constructor
        Negotiate();

        // Constructor
        Negotiate(const string& playerName, const string& targetPlayer);

        // Copy constructor
        Negotiate(const Negotiate& other);

        // Assignment operator
        Negotiate& operator=(const Negotiate& other);

        // Destructor
        ~Negotiate();

        // Validates the negotiate order
        bool validate() const override;

        // Executes the negotiate order
        void execute() override;

        // Creates a copy of this order
        Order* clone() const override;

        // Returns the order type
        string getType() const override;

        // Stream insertion operator
        friend ostream& operator<<(ostream& out, const Negotiate& order);
};

// OrdersList contains the orders created by a player

// Orders are stored as pointers so that polymorphism can be used
// The OrdersList is responsible for deleting the orders it owns

class OrdersList{
    private:
        vector<Order*> orders;

    public:
        // Default constructor
        OrdersList();

        // Copy constructor
        OrdersList(const OrdersList& other);

        // Assignment operator
        OrdersList& operator=(const OrdersList& other);

        // Destructor
        ~OrdersList();

        // Adds an order to the end of the list
        void add(Order* order);

        // Removes an order at a specified position
        bool remove(int index);

        // Moves an order from one position to another 
        bool move(int fromIndex, int toIndex);

        // Returns the number of orders 
        int size() const;

        // Returns an order at a specified position
        Order* get(int index) const;

        // Displays all orders in the list
        friend ostream& operator<<(ostream& out, const OrdersList& ordersList);
};

#endif