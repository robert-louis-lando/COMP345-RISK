#include <iostream>
#include <string>
#include <stdexcept>
#include "GameEngine.h"
using namespace std;


int main(){
    GameEngine& gameEngine = GameEngine::getInstance();
    cout << "The game has booted" << endl;
    while(true){
        string* command = new string();
        cout << "Enter a command" << endl;
        if (!(std::cin >> *command)) { 
            delete command;
            break;
        }
        try{
            gameEngine.executeCommand(command);
        }
        catch(const invalid_argument& e){
            cout << "Invalid command for current state: "<< *gameEngine.getCurrentState()<<", please try a different one"<<endl;
        }
        catch (const std::exception& e) {
            cout << e.what() << endl; 
}
         delete command;

    }
    
    return 0;
};