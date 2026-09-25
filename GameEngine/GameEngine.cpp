#include "GameEngine.h"
#include <cstring>
#include <stdexcept>

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
    if(std::strcmp(command, "loadmap") == 0){
        gameEngine->setState(new MapLoaded());
    }
    else{
        throw std::invalid_argument("Wrong command for this state");
    }
}

MapLoaded :: MapLoaded(): State("MapLoaded"){}






