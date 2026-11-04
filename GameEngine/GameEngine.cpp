#include "GameEngine.h"
#include "CommandProcessing.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

namespace {
    bool hasArgument(const string& argument){
        return argument.find_first_not_of(" \t\n\r\f\v") != string::npos;
    }

    //Validation supplies rejection reasons only; execution will supply successful effects.
    bool setCheckEffect(const string& command, const string& argument,
                        const string& state, bool allowed, string* effect){
        static const char* knownCommands[] = {
            "loadmap", "validatemap", "addplayer", "gamestart", "replay", "quit",
            "issueorder", "endissueorders", "execorder", "endexecorders", "win"
        };
        bool known = false;
        for(const char* name : knownCommands){
            if(command == name){
                known = true;
                break;
            }
        }

        string error;
        bool needsArgument = command == "loadmap" || command == "addplayer";
        if(command.empty()){
            error = "No command provided";
        }
        else if(!known){
            error = "Unknown command: " + command;
        }
        else if(needsArgument && !hasArgument(argument)){
            error = command == "loadmap" ? "Missing map filename" : "Missing player name";
        }
        else if(!needsArgument && hasArgument(argument)){
            error = "Unexpected argument for command: " + command;
        }
        else if(!allowed){
            error = "Command '" + command + "' is not allowed in state '" + state + "'";
        }
        if(effect != nullptr){
            *effect = error;
        }
        return error.empty();
    }
}

//GameEngine function to return a singleton instance always
GameEngine& GameEngine::getInstance(){
    static GameEngine instance;
    return instance;
}
//default constructor sets phase to startup and state to start since this is the entry node in the state machine
GameEngine :: GameEngine(){
    this->currentPhase = new Startup();
    this->currentState = new Start();
    this->commandProcessor = new CommandProcessor();
    this->running = true;
}
//releases both state pointer and phase pointer
GameEngine :: ~GameEngine(){
    delete currentPhase;
    delete currentState;
    delete commandProcessor;
}
//custom output stream to display information a certain way of the game engine
std::ostream& operator <<(std::ostream& os, const GameEngine& gameEngine){
    const std::string* phase=gameEngine.getCurrentPhase();
    const std::string* state=gameEngine.getCurrentState();
    os << "CurrentState: "<< *state <<", CurrentPhase: "<< *phase;
    return os;
}
//Keep validation errors and execution outcomes on the Command supplied by the caller.
void GameEngine::executeCommand(Command& command){
    string name;
    try{
        istringstream stream(*command.getCommand());
        string argument;
        string extra;
        stream >> name >> argument;
        if(stream >> extra){
            throw invalid_argument("Too many arguments for command: " + name);
        }

        string error;
        if(!canExecute(name, argument, &error)){
            throw invalid_argument(error);
        }

        string oldState = *currentState->getName();
        string oldPhase = *currentPhase->getName();
        currentState->transition(&name, this, argument);

        //Only claim the state/phase change that was actually performed.
        string effect = name == "quit"
            ? "Game ended"
            : "State: " + oldState + " -> " + *getCurrentState()
                + "; Phase: " + oldPhase + " -> " + *getCurrentPhase();
        command.saveEffect(effect);
    }
    catch(const std::exception& error){
        string effect = "Failed to execute command: " + string(error.what());
        command.saveEffect(effect);
        throw;
    }

    //Return to the driver so it can print the saved effect and release the input source.
    if(name == "quit"){
        running = false;
    }
}

bool GameEngine::isRunning() const{
    return running;
}

bool GameEngine::canExecute(const string& command, const string& argument, string* error) const{
    if(!running){
        if(error != nullptr){
            *error = "Game has ended";
        }
        return false;
    }
    return currentState->canExecute(command, argument, error);
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
    if(phase == nullptr){
        throw invalid_argument("Phase cannot be null");
    }
    if(this->currentPhase!=phase){
        delete this->currentPhase;
        this->currentPhase=phase;
    }
}
void GameEngine :: setState(State* state){
    if(state == nullptr){
        throw invalid_argument("State cannot be null");
    }
    if(this->currentState!=state){
        delete this->currentState;
        this->currentState=state;
    }
}

//default constructor
Phase::Phase(): name(new string()) {}
//destructor for releasing the memory of the string pointer 
Phase::~Phase(){
    delete name;
}
//custom constructor
Phase::Phase(const string& name): name(new string(name)) {}
//copy constructor
Phase::Phase(const Phase& phase) {
    //checking if the phase pointer passed isn't a null pointer and if not then making a new string pointer with the same value of phase.name
    this->name = (phase.name != nullptr) ? new std::string(*phase.name) : nullptr;
}
//assignment operator
Phase& Phase::operator=(const Phase& phase){
    //checking if the phase pointer passed isn't a null pointer
    if(this != &phase){
        //Allocate before deleting so a failed allocation leaves this object intact.
        const string* replacement = phase.name != nullptr ? new string(*phase.name) : nullptr;
        delete this->name;
        this->name = replacement;
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
Startup :: Startup(): Phase("Startup"){}
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
Play :: Play(): Phase("Play"){}
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
State::State(): name(new string()) {}
//destructor deallocates the string pointer of name
State::~State(){
    delete name;
}
//copy constructor
//checking if the phase pointer passed isn't a null pointer and if not then making a new string pointer with the same value of phase.name
State::State(const State& state) {
    this->name = (state.name != nullptr) ? new std::string(*state.name) : nullptr;
}
//Copy the state's name into owned storage without allocating a temporary string.
State :: State(const std::string& name): name(new string(name)){}
//assignment operator checks if the pointer passed is nullptr if not then release the memory for the previous string* and then assign the passed state name pointer to a newly created string pointer
State& State::operator=(const State& state){
        if(this != &state){
            const string* replacement = state.name != nullptr ? new string(*state.name) : nullptr;
            delete name;
            this->name = replacement;
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

//The base class allocates and owns the state name.
Start :: Start(): State("Start"){}
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
bool Start::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = command == "loadmap" && hasArgument(argument);
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void Start :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    gameEngine->setState(new MapLoaded());
}
//from here on out these are all implementations of a state pattern
//therefore their implementations are identical to the Start class and are all children of State
//MapLoaded
MapLoaded :: MapLoaded(): State("MapLoaded"){}
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
bool MapLoaded::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = (command == "loadmap" && hasArgument(argument)) || command == "validatemap";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void MapLoaded :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    if(command->compare("loadmap") == 0){
        gameEngine->setState(new MapLoaded());
    }
    else{
        gameEngine->setState(new MapValidated());
    }
}

//MapValidated
MapValidated :: MapValidated(): State("MapValidated"){}
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
bool MapValidated::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = command == "addplayer" && hasArgument(argument);
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void MapValidated :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    gameEngine->setState(new PlayersAdded());
}

//PlayersAdded
PlayersAdded :: PlayersAdded(): State("PlayersAdded"){}
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
bool PlayersAdded::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = (command == "addplayer" && hasArgument(argument)) || command == "gamestart";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void PlayersAdded :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    if(command->compare("addplayer") == 0){
        gameEngine->setState(new PlayersAdded());
    }
    else{
        gameEngine->setState(new AssignReinforcement());
        gameEngine->setPhase(new Play());
    }
}

//AssignReinforcement
AssignReinforcement :: AssignReinforcement(): State("AssignReinforcement"){}
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
bool AssignReinforcement::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = command == "issueorder";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void AssignReinforcement :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    gameEngine->setState(new IssueOrders());
}

//IssueOrders
IssueOrders :: IssueOrders(): State("IssueOrders"){}
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
bool IssueOrders::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = command == "issueorder" || command == "endissueorders";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void IssueOrders :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    if(command->compare("issueorder") == 0){
        gameEngine->setState(new IssueOrders());
    }
    else{
        gameEngine->setState(new ExecuteOrders());
    }
}

//ExecuteOrders
ExecuteOrders :: ExecuteOrders(): State("ExecuteOrders"){}
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
bool ExecuteOrders::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = command == "execorder" || command == "endexecorders" || command == "win";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void ExecuteOrders :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    if(command->compare("execorder") == 0){
        gameEngine->setState(new ExecuteOrders());
    }
    else if(command->compare("endexecorders") == 0){
        gameEngine->setState(new AssignReinforcement());
    }
    else{
        gameEngine->setState(new Win());
    }
}

//Win
Win :: Win(): State("Win"){}
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
bool Win::canExecute(const string& command, const string& argument, string* effect) const{
    bool valid = command == "quit" || command == "replay";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void Win :: transition(const string* command, GameEngine* gameEngine, const string& argument){
    if(command == nullptr || gameEngine == nullptr || !canExecute(*command, argument)){
        throw invalid_argument("Wrong command for this state");
    }
    if(command->compare("quit") == 0){
        //executeCommand saves the effect and tells the driver to stop.
        return;
    }
    else{
        gameEngine->setState(new Start());
        gameEngine->setPhase(new Startup());
    }
}
