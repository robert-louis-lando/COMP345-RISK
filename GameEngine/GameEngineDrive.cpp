#include <iostream>
#include <string>
#include <stdexcept>
#include "GameEngine.h"
using namespace std;


int main(){
    GameEngine& gameEngine = GameEngine::getInstance();
    cout << "The game has booted" << endl;
    string* command;
    while(true){
        cout << "Enter a command" << endl;
        if (!(std::cin >> command)) { 
            break;
        }
        try{
            gameEngine->executeCommand(&command);
        }
        catch(const invalid_argument e){
            cout << "Invalid command, please try a different one"<<endl;
        }

    }
    
    return 0;
};