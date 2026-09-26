#include <iostream>
#include <string>
#include "GameEngine.h"
using namespace std;


int main(){
    GameEngine* gameEngine = new GameEngine();
    cout << "The game has booted" << endl;
    char* command = new char[100];
    while(true){
        cout << "Enter a command" << endl;
        cin >> command;
        try{
            gameEngine->executeCommand(command);
        }
        catch(invalid_argument e){
            cout << "Invalid command, please try a different one"<<endl;
        }

    }
    
    delete[] command;
    return 0;
};