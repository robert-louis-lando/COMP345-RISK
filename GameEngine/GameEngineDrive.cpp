#include <iostream>
#include <string>
#include <stdexcept>
#include "GameEngine.h"
using namespace std;


int main(){
    //get the instance of game engine
    GameEngine& gameEngine = GameEngine::getInstance();
    cout << "The game has booted" << endl;
    //infinite loop until we end the game
    while(true){
        //makes a new string pointer for the command
        string* command = new string();
        cout << "Enter a command" << endl;
        

        try{
            //read the command entered in the console
            std::cin >> *command;
            //send the command to be executed by the game engine
            gameEngine.executeCommand(command);
        }
        //catches invalid commands
        catch(const invalid_argument& e){
            cout << "Invalid command for current state: "<< *gameEngine.getCurrentState()<<", please try a different one"<<endl;
        }
        //catches all other exceptions
        catch (const std::exception& e) {
            cout << e.what() << endl; 
}       //releases the string pointer memory block since every iteration of the loop makes a new string pointer
         delete command;

    }
    
    return 0;
};