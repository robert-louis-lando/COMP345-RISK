#include "Player.h"

//dummy class for territory to test player
class Territory {
private:
    std::string* name;

public:
//constructor
    Territory(const std::string& name) {
        this->name = new std::string(name);
    }

    //copy constructor
    Territory(const Territory& other) {
        name = new std::string(*other.name);
    }

    //copy other territory
    Territory& operator=(const Territory& other) {
        if (this != &other) {
            *name = *other.name;
        }
        return *this;
    }

    //destructor
    ~Territory() {
        delete name;
    }

    //output in stream
    friend std::ostream& operator<<(std::ostream& out,
                                    const Territory& territory) {
        out << *territory.name;
        return out;
    }
};


//dummy class for Cards to test 
class Card {
private:
    std::string* type;

public:
//constructor
    Card(const std::string& type) {
        this->type = new std::string(type);
    }
//copy constructor
    Card(const Card& other) {
        type = new std::string(*other.type);
    }
//copy a card
    Card& operator=(const Card& other) {
        if (this != &other) {
            *type = *other.type;
        }
        return *this;
    }

    //destructor
    ~Card() {
        delete type;
    }
 //output to stream
    friend std::ostream& operator<<(std::ostream& out, const Card& card) {
        out << *card.type;
        return out;
    }
};


//main function to test functionalities in requirements 
int main() {

    //create territories
    Territory canada("Canada");
    Territory france("France");
    Territory germany("Germany");

    //create cards
    Card bomb("Bomb");
    Card reinforcement("Reinforcement");

    //create a player
    Player player("Player 1");

    //add territories (defense, attack) , cards to players
    player.addTerritory(&canada);
    player.addTerritory(&france);

    player.addCard(&bomb);
    player.addCard(&reinforcement);

    player.addTerritoryToAttack(&germany);

    //Display territories to defend
    std::cout << "Territories to defend:\n";

    for (Territory* territory : player.toDefend()) {
        std::cout << "  " << *territory << '\n';
    }

    //Dsiplay territories to attack
    std::cout << "\nTerritories to attack:\n";

    for (Territory* territory : player.toAttack()) {
        std::cout << "  " << *territory << '\n';
    }

    //create and store orders for the player
    player.issueOrder("Deploy 5 armies to Canada");
    player.issueOrder("Advance 2 armies from Canada to France");

    //test copy constructor for players
    std::cout << "\nPlayer information:\n";
    std::cout << player;

    Player player2(player);

    std::cout << "\nCopied player:\n";
    std::cout << player2;

    Player player3("Player 3");
    player3 = player;

    std::cout << "\nAssigned player:\n";
    std::cout << player3;

    return 0;
}