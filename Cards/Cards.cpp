#include "Cards.h"
#include <stdexcept>
#include <cstdlib>

// ============================================================
// Card
// ============================================================

// Default Constructor
Card::Card() : type(new string()) {}

// Constructor
Card::Card(string type) : type(new string(type)) {}

// Copy Constructor
Card::Card(const Card &c) : type(new string(*c.type)) {}

// Assignment Operator
Card &Card::operator=(const Card &c)
{
    if (this != &c)
    {
        *type = *c.type;
    }

    return *this;
}

// Stream Insertion
std::ostream &operator<<(std::ostream &out, const Card &c)
{
    out << "This is a card of type: " << *c.type << '\n';
    return out;
}

// Destructor
Card::~Card()
{
    delete type;
}

// Play
void Card::play()
{
    if (*type == "bomb")
    {
        std::cout << "Playing card of type: bomb" << '\n';
    }
    else if (*type == "reinforcement")
    {
        std::cout << "Playing card of type: reinforcement" << '\n';
    }
    else if (*type == "blockade")
    {
        std::cout << "Playing card of type: blockade" << '\n';
    }
    else if (*type == "airlift")
    {
        std::cout << "Playing card of type: airlift" << '\n';
    }
    else if (*type == "diplomacy")
    {
        std::cout << "Playing card of type: diplomacy" << '\n';
    }
    else
    {
        std::cout << "Invalid card type" << '\n';
    }
}

// Return type
string Card::getType() const
{
    return *type;
}

// ============================================================
// Deck
// ============================================================

// Default constructor
Deck::Deck()
{
    cards = new Card[5];

    cards[0] = Card("bomb");
    cards[1] = Card("reinforcement");
    cards[2] = Card("blockade");
    cards[3] = Card("airlift");
    cards[4] = Card("diplomacy");

    size = new int(5);

    index = new int(*size - 1);

    shuffle();
}

// Constructor
Deck::Deck(int nbomb, int nreinforcement, int nblockade, int nairlift, int ndiplomacy)
{
    if (nbomb < 0 || nreinforcement < 0 || nblockade < 0 ||
        nairlift < 0 || ndiplomacy < 0)
    {
        throw std::invalid_argument("Card counts cannot be negative");
    }

    int ncards = nbomb + nreinforcement + nblockade + nairlift + ndiplomacy;

    cards = new Card[ncards];

    int count = 0;

    while (count < ncards)
    {
        if (nbomb > 0)
        {
            cards[count++] = Card("bomb");
            nbomb--;
        }

        if (nreinforcement > 0)
        {
            cards[count++] = Card("reinforcement");
            nreinforcement--;
        }

        if (nblockade > 0)
        {
            cards[count++] = Card("blockade");
            nblockade--;
        }

        if (nairlift > 0)
        {
            cards[count++] = Card("airlift");
            nairlift--;
        }

        if (ndiplomacy > 0)
        {
            cards[count++] = Card("diplomacy");
            ndiplomacy--;
        }
    }

    size = new int(ncards);
    index = new int(*size - 1);

    shuffle();
}

// Copy Constructor
Deck::Deck(const Deck &d)
    : cards(new Card[*d.size]),
      size(new int(*d.size)),
      index(new int(*d.index))
{
    for (int i = 0; i < *size; ++i)
    {
        cards[i] = d.cards[i];
    }
}

// Stream Insertion
std::ostream &operator<<(std::ostream &out, const Deck &d)
{
    out << "Cards remaining: " << *d.index + 1 << '\n';

    for (int i = 0; i <= *d.index; i++)
    {
        out << d.cards[i] << '\n';
    }

    return out;
}

// Assignment
Deck &Deck::operator=(const Deck &d)
{
    if (this != &d)
    {

        int *newSize = new int(*d.size);

        Card *newCards = new Card[*newSize];

        int *newIndex = new int(*d.index);

        for (int i = 0; i < *newSize; i++)
        {
            newCards[i] = d.cards[i];
        }

        delete size;
        delete[] cards;
        delete index;

        size = newSize;
        cards = newCards;
        index = newIndex;
    }

    return *this;
}

// Destructor
Deck::~Deck()
{
    delete[] cards;
    delete size;
    delete index;
}

// Draw a card
Card &Deck::draw()
{
    if (*index < 0)
    {
        throw std::out_of_range("Cannot draw from an empty deck");
    }
    return cards[(*index)--];
}

// Add back to deck
void Deck::returnCard(const Card &c)
{
    if (*index + 1 == *size)
    {
        throw std::out_of_range("Deck is full");
    }

    cards[++(*index)] = Card(c.getType());
}

// Shuffle in-place
void Deck::shuffle()
{
    int iterations = 50;

    Card temp;

    while (iterations > 0)
    {
        int index1 = rand() % *size;
        int index2 = rand() % *size;

        temp = cards[index1];
        cards[index1] = cards[index2];
        cards[index2] = temp;

        iterations--;
    }
}

// ============================================================
// Hand
// ============================================================

// Constructor
Hand::Hand()
{
    cards = new Card[5];

    size = new int(5);

    index = new int(-1);
}

// Copy
Hand::Hand(const Hand &h)
{
    size = new int(*h.size);

    index = new int(*h.index);

    cards = new Card[*size];

    for (int i = 0; i < *size; i++)
    {
        cards[i] = h.cards[i];
    }
}

// Assignment
Hand &Hand::operator=(const Hand &h)
{
    if (this != &h)
    {
        int *newSize = new int(*h.size);

        int *newIndex = new int(*h.index);

        Card *newCards = new Card[*h.size];

        for (int i = 0; i < *newSize; i++)
        {
            newCards[i] = h.cards[i];
        }

        delete[] cards;
        delete index;
        delete size;

        cards = newCards;
        index = newIndex;
        size = newSize;
    }
    return *this;
}

// Output stream
std::ostream &operator<<(std::ostream &out, const Hand &h)
{
    out << "Number of cards in hand: " << (*h.index) + 1 << '\n';
    out << "Hand contains the following cards: " << '\n';
    for (int i = 0; i <= *h.index; i++)
    {
        out << h.cards[i];
    }

    return out;
}

// Destructor
Hand::~Hand()
{
    delete[] cards;
    delete index;
    delete size;
}

// Return a list of the remaining cards in the player's hand
Card *Hand::getCards() const
{
    int handSize = *index + 1;

    Card *hand = new Card[handSize];

    for (int i = 0; i < handSize; i++)
    {
        hand[i] = cards[i];
    }

    return hand;
}

// Add card to hand
void Hand::addCard(const Card &c)
{
    if ((*index) + 1 == *size)
    {
        throw std::invalid_argument("Hand is full");
    }

    cards[++(*index)] = c;
}