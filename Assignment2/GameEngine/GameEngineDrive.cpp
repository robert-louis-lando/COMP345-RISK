#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include "GameEngine.h"
using namespace std;

//Select one input source; all commands still pass through the same processor API.
int main(int argc, char* argv[]){
    try{
        unique_ptr<CommandProcessor> processor;
        if(argc == 1 || (argc == 2 && string(argv[1]) == "-console")){
            processor.reset(new CommandProcessor());
        }
        else if(argc == 3 && string(argv[1]) == "-file" && string(argv[2]).size() > 0
                && string(argv[2]) != "-console" && string(argv[2]) != "-file"){
            processor.reset(new FileCommandProcessorAdapter(argv[2]));
        }
        else{
            cerr << "Usage: " << argv[0] << " [-console | -file <filename>]\n";
            return 2;
        }

        GameEngine& gameEngine = GameEngine::getInstance();
        cout << "The game has booted" << endl;
        while(gameEngine.isRunning()){
            Command* command = processor->getCommand(); //borrowed; history owns it
            if(command == nullptr){
                break; //file EOF, including empty files
            }
            if(command->getEffect()->empty()){
                try{
                    gameEngine.executeCommand(*command);
                }
                catch(const std::exception&){
                    //executeCommand has already saved the execution error.
                }
            }
            cout << *command;
        }
        return 0;
    }
    catch(const std::exception& error){
        cerr << error.what() << '\n';
        return 1;
    }
}
