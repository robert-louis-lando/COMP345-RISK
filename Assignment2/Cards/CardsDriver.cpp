#include <iostream>
#include "Cards.h"

using namespace std;

// Demonstrate the Card, Hand and Deck

int main()
{
    cout << "Warzonr Card Driver" << endl;
    cout << "----------------------------------------" << endl;

    // Create Deck

    Deck deck = Deck(5, 5, 5, 5, 5);

    cout << "Deck created" << endl;

    cout << deck << endl;

    // Create Hand

    Hand hand = Hand();

    cout << "Hand created" << endl;

    cout << hand << endl;

    // Draw five times
    cout << "Drawing five cards" << endl;
    int i = 5;
    while (i > 0)
    {
        hand.addCard(deck.draw());
        i--;
    }

    cout << hand << endl;

    cout << endl;
    cout << "Warzone Card Driver completed successfully." << endl;

    return 0;
}