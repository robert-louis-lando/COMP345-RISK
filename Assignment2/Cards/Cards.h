#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <string>

using std::string;
// Card continaining the play() method
class Card
{
private:
    string *type;

public:
    // Default Constructor
    Card();

    // Constructor
    Card(string type);

    // Copy Constructor
    Card(const Card &c);

    // Assignment Operator
    Card &operator=(const Card &c);

    // Stream Insertion
    friend std::ostream &operator<<(std::ostream &out, const Card &c);

    // Destructor
    ~Card();

    // Play
    void play();

    // Get type
    string getType() const;
};

// Deck class
class Deck
{
private:
    // Pointer to array of cards
    Card *cards = nullptr;

    // Size of deck
    int *size = nullptr;

    // Current Card index
    int *index = nullptr;

    // Shuffle
    void shuffle();

public:
    // Default Constructor
    Deck();

    // Constructor
    Deck(int nbomb, int nreinforcement, int nblockade, int nairlift, int ndiplomacy);

    // Copy Constructor
    Deck(const Deck &d);

    // Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &out, const Deck &d);

    // Assignment Operator
    Deck &operator=(const Deck &d);

    // Destructor
    ~Deck();

    // Draw a card
    Card &draw();

    // Return a card
    void returnCard(const Card &c);
};

// Hand class
class Hand
{
private:
    // List of cards
    Card *cards = nullptr;

    // Size of list
    int *size = nullptr;

    // Current index of list
    int *index = nullptr;

public:
    // Default Constructor
    Hand();

    // Copy Constructor
    Hand(const Hand &h);

    // Assignment Operator
    Hand &operator=(const Hand &h);

    // Stream Insertion Operator
    friend std::ostream &operator<<(std::ostream &out, const Hand &h);

    // Destructor
    ~Hand();

    // Return a list of cards
    Card *getCards() const;

    // Add card to list
    void addCard(const Card &c);
};
#endif