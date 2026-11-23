#include "GameEngine.h"
#include <exception>
#include <iostream>

// Run the GameEngine with the Part 1 player strategies.
int main() {
    try {
        GameEngine& gameEngine = GameEngine::getInstance();

        std::cout << "COMP 345 Risk - Player Strategy driver\n";
        std::cout << "Initial engine state: " << gameEngine << '\n';
        std::cout << "Use these commands to start a strategy game:\n"
                  << "  loadmap maps/solar/solar.map\n"
                  << "  validatemap\n"
                  << "  addplayer robert Human\n"
                  << "  addplayer alex Aggressive\n"
                  << "  addplayer sam Benevolent\n"
                  << "  addplayer jules Neutral\n"
                  << "  addplayer casey Cheater\n"
                  << "  gamestart\n"
                  << "Strategies: Human, Aggressive, Benevolent, Neutral, Cheater.\n";
        gameEngine.startupPhase();
        std::cout << "Engine state after startup: " << gameEngine << '\n';
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "Startup driver failed: " << error.what() << '\n';
        return 1;
    }
}
