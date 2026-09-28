#include "GameEngine.h"
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <string>
using namespace std;

//GameEngine function to return a singleton instance always
GameEngine& GameEngine::getInstance(){
    static GameEngine instance;
    return instance;
}
//default constructor sets phase to startup and state to start since this is the entry node in the state machine
GameEngine :: GameEngine(){
    this->currentPhase = new Startup();
    this->currentState = new Start();
}
//releases both state pointer and phase pointer
GameEngine :: ~GameEngine(){
    delete currentPhase;
    delete currentState;
}
//custom output stream to display information a certain way of the game engine
std::ostream& operator <<(std::ostream& os, const GameEngine& gameEngine){
    const std::string* phase=gameEngine.getCurrentPhase();
    const std::string* state=gameEngine.getCurrentState();
    os << "CurrentState: "<< *state <<", CurrentPhase: "<< *phase;
    return os;
}
//execute command calls the transition function in its current state which then will decide if we are doing a state transition or not based on the command
void GameEngine :: executeCommand(const string* command){
    //keep old state, just to print the before and after if a state transition occurs
    std::string oldState = *currentState->getName();
    std::string oldPhase = *currentPhase->getName();
    this->currentState->transition(command, this);
    //values before
    cout << "Was in state: "<< oldState<<", Was in Phase: "<<oldPhase<<endl;
    //values after
    cout << "Transition to state/phase: "<< *this << endl;
}
//getters
const std::string* GameEngine :: getCurrentPhase() const{
    return currentPhase->getName();
}
const std::string* GameEngine :: getCurrentState() const{
    return currentState->getName();
}
//setters
void GameEngine :: setPhase(Phase* phase){
    if(this->currentPhase!=phase){
        delete this->currentPhase;
        this->currentPhase=phase;
    }
}
void GameEngine :: setState(State* state){
    if(this->currentState!=state){
        delete this->currentState;
        this->currentState=state;
    }
}

//default constructor
Phase::Phase(){}
//destructor for releasing the memory of the string pointer 
Phase::~Phase(){
    delete name;
}
//custom constructor
Phase::Phase(const string* name) {
    //checking if the string pointer passed isn't a null pointer and if not then making a new string pointer with the same value
    this->name = (name != nullptr) ? new std::string(*name) : nullptr;
}
//copy constructor
Phase::Phase(const Phase& phase) {
    //checking if the phase pointer passed isn't a null pointer and if not then making a new string pointer with the same value of phase.name
    this->name = (phase.name != nullptr) ? new std::string(*phase.name) : nullptr;
}
//assignment operator
Phase& Phase::operator=(const Phase& phase){
    //checking if the phase pointer passed isn't a null pointer
    if(this != &phase){
        //releasing the memory address of the old string pointer
        delete this->name;
        //checking if the phase pointer passed isn't a null pointer and if not then making a new string pointer with the same value of phase.name
        this->name = (phase.name != nullptr) ? new std::string(*phase.name) : nullptr;
    }
    return *this;
}
//output stream to simply print the phase name
std::ostream& operator <<(std::ostream& os, const Phase& phase){
    os << *phase.getName();
    return os;
}
//getter
const string* Phase::getName() const{
    return this->name;
}

//Startup default constructor calls the parent constructor with its class name as a passed parameter which will end up being a new string *
Startup :: Startup(): Phase(new std::string("Startup")){}
//copy constructor will simply call the parent constructor which in turn will assign a new string pointer with the passed name value to the new Startup object
Startup::Startup(const Startup& startup):Phase(startup) {}
//assignment operator simply calls the parent's assignment operator since the object members are managed by the parents logic
Startup& Startup::operator=(const Startup& startup){
    if(this != &startup){
        Phase::operator=(startup);
    }
    return *this;
}
//output stream operator simply prints the phase name
std::ostream& operator <<(std::ostream& os, const Startup& startup){
    os << *startup.getName();
    return os;
}

//same as startup class
Play :: Play(): Phase(new std::string("Play")){}
Play::Play(const Play& play):Phase(play) {}
Play& Play::operator=(const Play& play){
    if(this != &play){
        Phase::operator=(play);
    }
    return *this;
}
std::ostream& operator <<(std::ostream& os, const Play& play){
    os << *play.getName();
    return os;
}

//State default constructor 
State::State(){}
//destructor deallocates the string pointer of name
State::~State(){
    delete name;
}
//copy constructor
//checking if the phase pointer passed isn't a null pointer and if not then making a new string pointer with the same value of phase.name
State::State(const State& state) {
    this->name = (state.name != nullptr) ? new std::string(*state.name) : nullptr;
}
//custom constructor when passed a string pointer assign the passed pointers value to a new string pointer and assign that to the name string pointer
State :: State(const std::string* name): name(name != nullptr ? new std::string(*name) : nullptr){}
//assignment operator checks if the pointer passed is nullptr if not then release the memory for the previous string* and then assign the passed state name pointer to a newly created string pointer
State& State::operator=(const State& state){
        if(this != &state){
            delete name;
            this->name = (state.name != nullptr) ? new std::string(*state.name) : nullptr;
        }
        return *this;
}   
//output stream prints the states name
std::ostream& operator <<(std::ostream& os, const State& state){
    os << *state.getName();
    return os;
}
//getter
const string* State ::getName() const{
    return this->name;
}

//Start inherits from state and therefore in the default constructor calls the parent constructor with their class name as string value and passes its pointer
Start :: Start(): State(new std::string("Start")){}
//copy constructor passes a pointer to a Start class, we then call the parent's copy constructor to sets its name value to our newly created Start class
Start::Start(const Start& start):State(start){}
//calls parents assignment operator since it handles the assignment of the class members
Start& Start::operator=(const Start& start){
    if(this != &start){
        State::operator=(start);
    }
    return *this;
}
//output stream prints the name of the state
std::ostream& operator << (std::ostream& os, const Start& start){
    os << *start.getName();
    return os;
}
//transition checks if the passed command is correct for a transition or else throws invalid argument exception
void Start :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("loadmap") == 0){
        gameEngine->setState(new MapLoaded());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}
//from here on out these are all implementations of a state pattern
//therefore their implementations are identical to the Start class and are all children of State
//MapLoaded
MapLoaded :: MapLoaded(): State(new std::string("MapLoaded")){}
MapLoaded::MapLoaded(const MapLoaded& mapLoaded):State(mapLoaded){}
MapLoaded& MapLoaded::operator=(const MapLoaded& mapLoaded){
    if(this != &mapLoaded){
        State::operator=(mapLoaded);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const MapLoaded& mapLoaded){
    os << *mapLoaded.getName();
    return os;
}
void MapLoaded :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("loadmap") == 0){
        gameEngine->setState(new MapLoaded());
    }
    else if(command->compare("validatemap") == 0){
        gameEngine->setState(new MapValidated());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

//MapValidated
MapValidated :: MapValidated(): State(new std::string("MapValidated")){}
MapValidated::MapValidated(const MapValidated& mapValidated):State(mapValidated){}
MapValidated& MapValidated::operator=(const MapValidated& mapValidated){
    if(this != &mapValidated){
        State::operator=(mapValidated);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const MapValidated& mapValidated){
    os << *mapValidated.getName();
    return os;
}
void MapValidated :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("addplayer") == 0){
        gameEngine->setState(new PlayersAdded());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

//PlayersAdded
PlayersAdded :: PlayersAdded(): State(new std::string("PlayersAdded")){}
PlayersAdded::PlayersAdded(const PlayersAdded& playersAdded):State(playersAdded){}
PlayersAdded& PlayersAdded::operator=(const PlayersAdded& playersAdded){
    if(this != &playersAdded){
        State::operator=(playersAdded);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const PlayersAdded& playersAdded){
    os << *playersAdded.getName();
    return os;
}
void PlayersAdded :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("addplayer") == 0){
        gameEngine->setState(new PlayersAdded());
    }
    else if(command->compare("assigncountries") == 0){
        gameEngine->setState(new AssignReinforcement());
        gameEngine->setPhase(new Play());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

//AssignReinforcement
AssignReinforcement :: AssignReinforcement(): State(new std::string("AssignReinforcement")){}
AssignReinforcement::AssignReinforcement(const AssignReinforcement& assignReinforcement):State(assignReinforcement){}
AssignReinforcement& AssignReinforcement::operator=(const AssignReinforcement& assignReinforcement){
    if(this != &assignReinforcement){
        State::operator=(assignReinforcement);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const AssignReinforcement& assignReinforcement){
    os << *assignReinforcement.getName();
    return os;
}
void AssignReinforcement :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("issueorder") == 0){
        gameEngine->setState(new IssueOrders());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

//IssueOrders
IssueOrders :: IssueOrders(): State(new std::string("IssueOrders")){}
IssueOrders::IssueOrders(const IssueOrders& issueOrders):State(issueOrders){}
IssueOrders& IssueOrders::operator=(const IssueOrders& issueOrders){
    if(this != &issueOrders){
        State::operator=(issueOrders);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const IssueOrders& issueOrders){
    os << *issueOrders.getName();
    return os;
}
void IssueOrders :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("issueorder") == 0){
        gameEngine->setState(new IssueOrders());
    }
    else if(command->compare("endissueorders") == 0){
        gameEngine->setState(new ExecuteOrders());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

//ExecuteOrders
ExecuteOrders :: ExecuteOrders(): State(new std::string("ExecuteOrders")){}
ExecuteOrders::ExecuteOrders(const ExecuteOrders& executeOrders):State(executeOrders){}
ExecuteOrders& ExecuteOrders::operator=(const ExecuteOrders& executeOrders){
    if(this != &executeOrders){
        State::operator=(executeOrders);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const ExecuteOrders& executeOrders){
    os << *executeOrders.getName();
    return os;
}
void ExecuteOrders :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("execorder") == 0){
        gameEngine->setState(new ExecuteOrders());
    }
    else if(command->compare("endexecorders") == 0){
        gameEngine->setState(new AssignReinforcement());
    }
    else if(command->compare("win") == 0){
        gameEngine->setState(new Win());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

//Win
Win :: Win(): State(new std::string("Win")){}
Win::Win(const Win& win):State(win){}
Win& Win::operator=(const Win& win){
    if(this != &win){
        State::operator=(win);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const Win& win){
    os << *win.getName();
    return os;
}
void Win :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("end") == 0){
        cout << "Ending the game" << endl; 
        exit(0);
    }
    else if(command->compare("play") == 0){
        gameEngine->setState(new Start());
        gameEngine->setPhase(new Startup());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}






