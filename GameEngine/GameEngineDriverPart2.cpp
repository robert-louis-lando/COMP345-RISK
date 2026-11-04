#include "GameEngine.h"
#include <exception>
#include <iostream>

int main() {
    try {
        GameEngine& gameEngine = GameEngine::getInstance();

        std::cout << "COMP 345 Risk - Part 2 startup driver\n";
        std::cout << "Initial engine state: " << gameEngine << '\n';
        gameEngine.startupPhase();
        std::cout << "Engine state after startup: " << gameEngine << '\n';
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "Startup driver failed: " << error.what() << '\n';
        return 1;
    }
}
