#include "GameEngine.h"
#include <cstring>
#include <stdexcept>
#include <iostream>
using namespace std;
GameEngine :: GameEngine(): currentPhase(new Startup()), currentState(new Start()){}

//custom constructor to assign a phase's name
Phase :: Phase(const char* name): name(name){}
//getter to retrieve the phase name
const char* Phase ::getName() const{
    return name;
}

//default constructor calling the parent constructor with its name
Startup :: Startup(): Phase("Startup"){}
Play :: Play(): Phase("Startup"){}

//custom constructor to assign a state's name
State :: State(const char* name): name(name){}
//getter to retrieve the state name
const char* State ::getName() const{
    return name;
}

Start :: Start(): State("Start"){}
void Start :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "loadmap") == 0){
        gameEngine->setState(new MapLoaded());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

MapLoaded :: MapLoaded(): State("MapLoaded"){}
void MapLoaded :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "loadmap") == 0){
        gameEngine->setState(new MapLoaded());
    }
    else if(strcmp(command, "validatemap") == 0){
        gameEngine->setState(new MapValidated());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

MapValidated :: MapValidated(): State("MapValidated"){}
void MapValidated :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "addplayer") == 0){
        gameEngine->setState(new PlayersAdded());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

PlayersAdded :: PlayersAdded(): State("PlayersAdded"){}
void PlayersAdded :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "addplayer") == 0){
        gameEngine->setState(new PlayersAdded());
    }
    else if(strcmp(command, "assigncountries") == 0){
        gameEngine->setState(new AssignReinforcement());
        gameEngine->setPhase(new Play());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

AssignReinforcement :: AssignReinforcement(): State("AssignReinforcement"){}
void AssignReinforcement :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "issueorder") == 0){
        gameEngine->setState(new IssueOrders());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

IssueOrders :: IssueOrders(): State("IssueOrders"){}
void IssueOrders :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "issueorder") == 0){
        gameEngine->setState(new IssueOrders());
    }
    else if(strcmp(command, "endissueorders") == 0){
        gameEngine->setState(new ExecuteOrders());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

ExecuteOrders :: ExecuteOrders(): State("ExecuteOrders"){}
void ExecuteOrders :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "exeorder") == 0){
        gameEngine->setState(new ExecuteOrders());
    }
    else if(strcmp(command, "endexecorders") == 0){
        gameEngine->setState(new AssignReinforcement());
    }
    else if(strcmp(command, "win") == 0){
        gameEngine->setState(new Win());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}

Win :: Win(): State("Win"){}
void Win :: transition(const char* command, GameEngine* gameEngine){
    if(strcmp(command, "end") == 0){
        cout << "Ending the game" << endl; 
        exit(0);
    }
    else if(std::strcmp(command, "play") == 0){
        gameEngine->setState(new Start());
        gameEngine->setPhase(new Startup());
    }
    else{
        throw invalid_argument("Wrong command for this state");
    }
}






