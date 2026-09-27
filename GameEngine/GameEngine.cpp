#include "GameEngine.h"
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <string>
using namespace std;

//GameEngine
GameEngine& GameEngine::getInstance(){
    static GameEngine instance;
    return instance;
}
GameEngine :: GameEngine(){
    this->currentPhase = new Startup();
    this->currentState = new Start();
}

GameEngine :: ~GameEngine(){
    delete currentPhase;
    delete currentState;
}
std::ostream& operator <<(std::ostream& os, const GameEngine& gameEngine){
    os << "Game Engine-> CurrentPhase: "<< gameEngine.getCurrentPhase() <<", CurrentState: "<< gameEngine.getCurrentState();
    return os;
}
void GameEngine :: executeCommand(const string* command){
    std::string oldState = *currentState->getName();
    std::string oldPhase = *currentPhase->getName();
    this->currentState->transition(command, this);
    cout << "Was in state: "<< oldState<<", Was in Phase: "<<oldPhase<<endl;
    cout << "Transition to state: "<< *currentState->getName()<<", Transition to phase: "<<*currentPhase->getName()<<endl;
}
const string* GameEngine :: getCurrentPhase() const{
    return currentPhase->getName();
}
const string* GameEngine :: getCurrentState() const{
    return currentState->getName();
}
void GameEngine :: setPhase(Phase* phase){
    this->currentPhase = phase;
}
void GameEngine :: setState(State* state){
    this->currentState = state;
}

//Phase
Phase::Phase(){}
Phase::~Phase(){
    delete name;
}
Phase::Phase(const string* name) {
    this->name = (name != nullptr) ? new std::string(*name) : nullptr;
}
Phase::Phase(const Phase& phase) {
    this->name = (phase.name != nullptr) ? new std::string(*phase.name) : nullptr;
}
Phase& Phase::operator=(const Phase& phase){
    if(this != &phase){
        delete this->name;
        this->name = (phase.name != nullptr) ? new std::string(*phase.name) : nullptr;
    }
    return *this;
}
std::ostream& operator <<(std::ostream& os, const Phase& phase){
    os << *phase.getName();
    return os;
}
const string* Phase::getName() const{
    return this->name;
}

//Startup
Startup :: Startup(): Phase(new std::string("Startup")){}
Startup::Startup(const Startup& startup):Phase(startup) {}
Startup& Startup::operator=(const Startup& startup){
    if(this != &startup){
        Phase::operator=(startup);
    }
    return *this;
}
std::ostream& operator <<(std::ostream& os, const Startup& startup){
    os << *startup.getName();
    return os;
}

//Play
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

//State
State::State(){}
State::~State(){
    delete name;
}
State::State(const State& state) {
    this->name = (state.name != nullptr) ? new std::string(*state.name) : nullptr;
}
State :: State(const string* name): name(name){}
State& State::operator=(const State& state){
        if(this != &state){
            delete name;
            this->name = (state.name != nullptr) ? new std::string(*state.name) : nullptr;
        }
        return *this;
}   
std::ostream& operator <<(std::ostream& os, const State& state){
    os << *state.getName();
    return os;
}
const string* State ::getName() const{
    return this->name;
}

//Start
Start :: Start(): State(new std::string("Start")){}
Start::Start(const Start& start):State(start){}
Start& Start::operator=(const Start& start){
    if(this != &start){
        State::operator=(start);
    }
    return *this;
}
std::ostream& operator << (std::ostream& os, const Start& start){
    os << *start.getName();
    return os;
}
void Start :: transition(const string* command, GameEngine* gameEngine){
    if(command->compare("loadmap") == 0){
        gameEngine->setState(new MapLoaded());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

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






