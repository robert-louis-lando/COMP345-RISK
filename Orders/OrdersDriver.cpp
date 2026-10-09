#include <iostream>
#include "Orders.h"

using namespace std;

// This driver demonstrates the creation and manipulation of all six Warzone order types

int main(){

    cout << "Warzone Orders Driver" << endl;
    cout << "----------------------------------------" << endl;

    // Creates an OrdersList
    OrdersList ordersList;

    // Create one order of every type

    Order* deploy = new Deploy("Player1", "Montreal", 5);

    Order* advance = new Advance("Player1", "Montreal", "Toronto", 3);

    Order* bomb = new Bomb("Player1", "Vancouver");

    Order* blockade = new Blockade("Player1", "Montreal");

    Order* airlift = new Airlift("Player1", "Toronto", "Vancouver", 4);

    Order* negotiate = new Negotiate("Player1", "Player2");

    // Add all orders to the list
    // Orders are added sequentially

    ordersList.add(deploy);
    ordersList.add(advance);
    ordersList.add(bomb);
    ordersList.add(blockade);
    ordersList.add(airlift);
    ordersList.add(negotiate);

    cout << endl;
    cout << "1. OrdersList after adding all orders:" << endl;
    cout << "----------------------------------------" << endl;
    cout << ordersList << endl;

    // Display the number of orders
    cout << "Number of orders: " << ordersList.size() << endl;

    // Validate every order
    cout << endl;
    cout << "2. Validating all orders:" << endl;
    cout << "----------------------------------------" << endl;

    for (int i = 0; i < ordersList.size(); i++) {
        Order* order = ordersList.get(i);
        
        cout << order->getType() << ": ";

        if(order->validate()) {
            cout << "VALID" << endl;
        } else {
            cout << "INVALID" << endl;
        }
    }

    // Execute all orders
    cout << endl;
    cout << "3. Executing all orders:" << endl;
    cout << "----------------------------------------" << endl;

    for (int i = 0; i < ordersList.size(); i++) {
        ordersList.get(i)->execute();
    }

    cout << ordersList << endl;

    // Demonstrate move()
    // Move the last order to the first position
    cout << "4. Testing move():" << endl;
    cout << "----------------------------------------" << endl;

    cout << "Before move:" << endl;
    cout << ordersList << endl;

    ordersList.move(5, 0);

    cout << "After moving order 5 to position 0:" << endl;
    cout << ordersList << endl;

    // Demonstrate remove()
    // Remove the order currently at position 2
    cout << "5. Testing remove():" << endl;
    cout << "----------------------------------------" << endl;

    cout << "Before remove:" << endl;
    cout << ordersList << endl;

    ordersList.remove(2);

    cout << "After removing order at position 2:" << endl;
    cout << ordersList << endl;

    // Demonstrate copy constructor
    cout << "6. Testing copy constructor:" << endl;
    cout << "----------------------------------------" << endl;

    OrdersList copiedOrders(ordersList);

    cout << "Original OrdersList:" << endl;
    cout << ordersList << endl;
    
    cout << "Copied OrdersList:" << endl;
    cout << copiedOrders << endl;

    // Demonstrate assignment operator
    cout << "7. Testing assignment operator:" << endl;
    cout << "----------------------------------------" << endl;

    OrdersList assignedOrders;

    assignedOrders = ordersList;

    cout << "Assigned OrdersList:" << endl;
    cout << assignedOrders << endl;

    // Demonstrate an invalid order
    cout << "8. Testing an invalid order:" << endl;
    cout << "----------------------------------------" << endl;

    Order* invalidDeploy = new Deploy("Player1", "", -5);

    cout << *invalidDeploy << endl;

    cout << "Validation result: ";

    if (invalidDeploy->validate()) {
        cout << "VALID" << endl;
    } else {
        cout << "INVALID" << endl;
    }

    cout << "Executing invalid order..." << endl;

    invalidDeploy->execute();

    cout << *invalidDeploy << endl;

    delete invalidDeploy;


    // Finish
    cout << endl;
    cout << "Warzone Orders Driver completed successfully." << endl;
    
    return 0;


}