#include "GameEngine.h"
#include "CommandProcessing.h"
#include "Map.h"
#include "../Player/PlayerStrategies.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <unordered_set>
#include <cctype>
#include <iomanip>
using namespace std;

namespace
{
    // Check whether a command argument contains non-whitespace text.
    bool hasArgument(const string &argument)
    {
        return argument.find_first_not_of(" \t\n\r\f\v") != string::npos;
    }

    string lowercase(string value)
    {
        transform(value.begin(), value.end(), value.begin(),
                  [](unsigned char c) { return static_cast<char>(tolower(c)); });
        return value;
    }

    bool parsePlayerSpecification(const string &argument, string &playerName, string &strategy)
    {
        istringstream stream(argument);
        string extra;
        // Player names are single tokens, followed by exactly one strategy.
        return static_cast<bool>(stream >> playerName >> strategy) && !(stream >> extra);
    }

    //helper just to check if the strategy is a valid one and also no case sensitivity
    bool isSupportedStrategy(const string &strategy)
    {
        const string normalized = lowercase(strategy);
        return normalized == "human" || normalized == "aggressive" ||
               normalized == "benevolent" || normalized == "neutral" ||
               normalized == "cheater";
    }

    // Validation supplies rejection reasons only; execution will supply successful effects.
    bool setCheckEffect(const string &command, const string &argument,
                        const string &state, bool allowed, string *effect)
    {
        static const char *knownCommands[] = {
            "loadmap", "validatemap", "addplayer", "gamestart", "replay", "quit",
            "issueorder", "endissueorders", "execorder", "endexecorders", "win", "tournament"};
        bool known = false;
        for (const char *name : knownCommands)
        {
            if (command == name)
            {
                known = true;
                break;
            }
        }

        string error;
        bool needsArgument = command == "loadmap" || command == "addplayer" || command == "tournament";
        if (command.empty())
        {
            error = "No command provided";
        }
        else if (!known)
        {
            error = "Unknown command: " + command;
        }
        else if (needsArgument && !hasArgument(argument))
        {
              error = command == "loadmap"      ? "Missing map filename"
                    : command == "addplayer" ? "Use: addplayer <name> <strategy>"
                    : "Use: tournament -M <maps> -P <strategies> -G <1-5> -D <10-50>";
        }

        else if (command == "tournament")
        {
            // Syntax and limits are checked by the command processing module
            vector<string> maps;
            vector<string> strategies;
            int games = 0;
            int maxTurns = 0;
            parseTournamentCommand(argument, maps, strategies, games, maxTurns, error);
        }

        else if (command == "addplayer")
        {
            string playerName;
            string strategy;
            if (!parsePlayerSpecification(argument, playerName, strategy))
                error = "Use: addplayer <name> <strategy>";
            else if (!isSupportedStrategy(strategy))
                error = "Unknown player strategy: " + strategy;
        }
        else if (!needsArgument && hasArgument(argument))
        {
            error = "Unexpected argument for command: " + command;
        }
         if (error.empty() && !allowed)
        {
            error = "Command '" + command + "' is not allowed in state '" + state + "'";
        }
        if (effect != nullptr)
        {
            *effect = error;
        }
        return error.empty();
    }
}

// GameEngine function to return a singleton instance always
GameEngine &GameEngine::getInstance()
{
    static GameEngine instance;
    return instance;
}
// default constructor sets phase to startup and state to start since this is the entry node in the state machine
GameEngine::GameEngine()
{
    this->currentPhase = new Startup();
    this->currentState = new Start();
    this->deck = new Deck(10, 0, 10, 10, 10);
    this->map = new Map();
    this->commandProcessor = new CommandProcessor();
    this->running = true;
    this->tournamentReport = new string();
}
// releases both state pointer and phase pointer
GameEngine::~GameEngine()
{
    delete currentPhase;
    delete currentState;
    delete deck;
    delete map;
    for (Player *player : players)
    {
        delete player;
    }
    delete commandProcessor;
    delete tournamentReport; 
}
// custom output stream to display information a certain way of the game engine
std::ostream &operator<<(std::ostream &os, const GameEngine &gameEngine)
{
    const std::string *phase = gameEngine.getCurrentPhase();
    const std::string *state = gameEngine.getCurrentState();
    os << "CurrentState: " << *state << ", CurrentPhase: " << *phase;
    return os;
}
// Keep validation errors and execution outcomes on the Command supplied by the caller.
void GameEngine::executeCommand(Command &command)
{
    string name;
    try
    {
        istringstream stream(*command.getCommand());
        string argument;
        string extra;
        stream >> name;
         if (name == "addplayer" || name == "tournament")
        {
            stream >> ws;
            getline(stream, argument);
        }
        else
        {
            stream >> argument;
            if (stream >> extra)
            {
                throw invalid_argument("Too many arguments for command: " + name);
            }
        }

        string error;
        if (!canExecute(name, argument, &error))
        {
            throw invalid_argument(error);
        }

        string oldState = *currentState->getName();
        string oldPhase = *currentPhase->getName();
        currentState->transition(&name, this, argument);

        // Only claim the state/phase change that was actually performed.
        string effect;
                if (name == "quit")
                    effect = "Game ended";
                else if (name == "tournament")
                    effect = *tournamentReport;
                else
                    effect = "State: " + oldState + " -> " + *getCurrentState() + "; Phase: " + oldPhase + " -> " + *getCurrentPhase();
                command.saveEffect(effect);
            }
            catch (const std::exception &error)
            {
                string effect = "Failed to execute command: " + string(error.what());
                command.saveEffect(effect);
                throw;
            }

    // Return to the driver so it can print the saved effect and release the input source.
    if (name == "quit")
    {
        running = false;
    }
}

// Report whether the command-processing loop should continue.
bool GameEngine::isRunning() const
{
    return running;
}

// Validate a command against the current engine state without changing it.
bool GameEngine::canExecute(const string &command, const string &argument, string *error) const
{
    if (!running)
    {
        if (error != nullptr)
        {
            *error = "Game has ended";
        }
        return false;
    }
    return currentState->canExecute(command, argument, error);
}
// getters
// Return the current phase name.
const std::string *GameEngine::getCurrentPhase() const
{
    return currentPhase->getName();
}
// Return the current state name.
const std::string *GameEngine::getCurrentState() const
{
    return currentState->getName();
}
// boolean function to check and delegate if we load the map properly
bool GameEngine::loadMap(const string &path, string &error)
{
    Map loadedMap;
    MapLoader loader;
    if (!loader.loadMap(path, loadedMap, error))
    {
        return false;
    }
    // if we can load the map then we assign it to the game engine's pointer to a Map
    *map = loadedMap;
    return true;
}
// another boolean method which checks if the map pointer isn't null and delegate the validation to the map->validate()
bool GameEngine::validateMap() const
{
    return map != nullptr && map->validate();
}

// add a player to the gameengine vector
void GameEngine::addPlayer(const string &name, const string &strategy)
{
    // check if the name is empty
    if (name.find_first_not_of(" \t\n\r\f\v") == string::npos)
    {
        throw invalid_argument("Player name cannot be empty");
    }
    // max 6 players
    if (players.size() >= 6)
    {
        throw invalid_argument("A game can have at most 6 players");
    }
    // add player to vector
    const string strategyName = lowercase(strategy);
    Player *player = new Player(name);
    if (strategyName == "aggressive") player->setStrategy(new AggressivePlayerStrategy());
    else if (strategyName == "benevolent") player->setStrategy(new BenevolentPlayerStrategy());
    else if (strategyName == "neutral") player->setStrategy(new NeutralPlayerStrategy());
    else if (strategyName == "cheater") player->setStrategy(new CheaterPlayerStrategy());
    else if (strategyName == "human") player->setStrategy(new HumanPlayerStrategy());
    else
    {
        delete player;
        throw invalid_argument("Unknown player strategy: " + strategy);
    }
    players.push_back(player);


     passObserversTo(*players.back()->getOrders());
}

// gamestart is called before transitioning to the Play phase
void GameEngine::gameStart()
{
    // should have at least 2 players to start the game
    if (players.size() < 2)
    {
        throw invalid_argument("At least 2 players are required to start");
    }
    // setting a random generator to mix the player list and decide the order of the turns.
    std::mt19937 randomGenerator(std::random_device{}());
    std::shuffle(players.begin(), players.end(), randomGenerator);
    std::cout << "Randomized player order selected.\n";

    // distributes the territories to the players ensuring that there is at most 1 difference in their numbers.
    if (map == nullptr || !map->distributeTerritories(players))
    {
        throw invalid_argument("Could not distribute territories to players");
    }
    std::cout << "Territories fairly distributed (player counts differ by at most one).\n";
    std::cout << "Player order and strategies:\n";
    for (const Player *player : players)
        std::cout << "  " << *player->getName() << " - "
                  << player->getStrategy()->name() << '\n';
    // goes through all players to assign their 50 troops and to make each player draw 2 cards
    for (Player *player : players)
    {
        player->setReinforcementPool(50);
        player->addCard(new Card(deck->draw()));
        player->addCard(new Card(deck->draw()));
        std::cout << "Each player received 50 armies and draw 2 initial cards.\n";
        std::cout << *player;
        std::cout << "Reinforcement pool: " << *player->getReinforcementPool() << '\n';
    }
}

// Calculate and assign reinforcements for every player.
void GameEngine::reinforcementPhase()
{
    if (map == nullptr)
    {
        throw invalid_argument("Cannot calculate reinforcements without a map");
    }

    std::cout << "\n--- Reinforcement phase ---\n";
    for (Player *player : players)
    {
        if (player == nullptr)
        {
            throw invalid_argument("Cannot calculate reinforcements for a null player");
        }

        const vector<Territory *> ownedTerritories = player->toDefend();
        int reinforcements =
            max(3, static_cast<int>(ownedTerritories.size()) / 3);

        unordered_set<const Territory *> owned(ownedTerritories.begin(),
                                               ownedTerritories.end());

        for (const Continent *continent : map->getContinents())
        {
            const vector<Territory *> &continentTerritories =
                continent->getTerritories();

            bool ownsEntireContinent = !continentTerritories.empty();
            for (const Territory *territory : continentTerritories)
            {
                if (owned.count(territory) == 0)
                {
                    ownsEntireContinent = false;
                    break;
                }
            }

            if (ownsEntireContinent)
            {
                reinforcements += continent->getBonusArmyCount();
            }
        }

        const int previousPool = *player->getReinforcementPool();

        player->setReinforcementPool(
            previousPool + reinforcements);
        std::cout << *player->getName() << " receives " << reinforcements
                  << " armies (pool: " << *player->getReinforcementPool() << ").\n";
    }
}

// Issue order loop for each player
// Ask each player to create orders for the current turn.
void GameEngine::issueOrdersPhase()
{
    std::cout << "\n--- Issue orders phase ---\n";
    for (Player *player : players)
    {
        if (player == nullptr)
        {
            throw invalid_argument("Cannot issue orders for a null player");
        }
        std::cout << *player->getName() << " [" << player->getStrategy()->name()
                  << "] owns " << player->getTerritories().size() << " territories.\n";
        player->issueOrder(deck, players);
        std::cout << "  Orders queued: " << player->getOrders()->size() << "\n";
    }
}

// Execute orders
// Execute and remove deploy orders, then execute remaining orders.
void GameEngine::executeOrdersPhase()
{
    std::cout << "\n--- Execute orders phase ---\n";
    for (Player *player : players)
        if (player != nullptr) {
            player->clearNegotiations();
        }
    for (Player *player : players)
    {
        if (player == nullptr)
        {
            throw invalid_argument("Cannot execute orders for a null player");
        }

        OrdersList *orders = player->getOrders();
        if (orders == nullptr)
        {
            continue;
        }

        while (orders->size() > 0)
        {
            Order *order = orders->get(0);
            if (order == nullptr || order->getType() != "Deploy")
            {
                break;
            }

            if (order->validate())
            {
                std::cout << "Executing for " << *player->getName() << ": "
                          << *order << '\n';
                order->execute();
                std::cout << "  " << order->stringToLog() << '\n';
            }
            else
            {
                std::cout << "Invalid deploy order for player "
                          << *player->getName() << ".\n";
            }

            orders->remove(0);
        }
    }

    bool ordersRemaining = true;
    while (ordersRemaining)
    {
        ordersRemaining = false;
        for (Player *player : players)
        {
            if (player == nullptr)
            {
                throw invalid_argument("Cannot execute orders for a null player");
            }

            OrdersList *orders = player->getOrders();
            if (orders == nullptr || orders->size() == 0)
            {
                continue;
            }

            Order *order = orders->get(0);
            if (order == nullptr)
            {
                orders->remove(0);
                ordersRemaining = true;
                continue;
            }

            if (order->validate())
            {
                std::cout << "Executing for " << *player->getName() << ": "
                          << *order << '\n';
                order->execute();
                std::cout << "  " << order->stringToLog() << '\n';
            }
            else
            {
                std::cout << "Invalid order for player "
                          << *player->getName() << ".\n";
            }

            orders->remove(0);
            ordersRemaining = true;
        }
    }

    // A player receives one card after conquering at least one territory this turn.
    for (Player *player : players) {
        if (player == nullptr || !player->hasConqueredTerritoryThisTurn()) continue;
        try {
            player->addCard(new Card(deck->draw()));
            std::cout << *player->getName()
                      << " conquered territory this turn and receives a card.\n";
        } catch (const std::out_of_range &) {
            // An exhausted deck cannot provide a conquest reward.
        }
    }
}

// Return the engine-owned map without transferring ownership.
Map *GameEngine::getMap() const { return map; }

// Return the engine-owned player list without transferring ownership.
const std::vector<Player *> &GameEngine::getPlayers() const { return players; }

// Remove eliminated players and detect a winner.
bool GameEngine::checkPlayPhaseResult()
{
    for (auto player = players.begin(); player != players.end();)
    {
        if (*player == nullptr || (*player)->toDefend().empty())
        {
            if (*player != nullptr)
            {
                std::cout << "Player " << *(*player)->getName()
                          << " has been eliminated.\n";
                delete *player;
            }
            player = players.erase(player);
        }
        else
        {
            ++player;
        }
    }

    if (players.size() == 1 && map != nullptr &&
        players.front()->toDefend().size() == map->getTerritories().size())
    {
        std::cout << "Player " << *players.front()->getName()
                  << " owns all territories and wins the game.\n";
        setState(new Win());
        return true;
    }

    return players.empty();
}

// Play phase
void GameEngine::playPhase(int maxTurns)
{
    int turn = 0;
    while (running && currentPhase != nullptr &&
           *currentPhase->getName() == "Play")
    {
         if (maxTurns > 0 && turn >= maxTurns)
        {
            std::cout << "Turn limit of " << maxTurns << " reached: the game is a draw.\n";
            return;
        }

        ++turn;
        std::cout << "\n========================================\n"
                  << "TURN " << turn << '\n'
                  << "========================================\n";
        for (Player *player : players)
            if (player != nullptr) player->clearConquestFlag();
        setState(new AssignReinforcement());
        reinforcementPhase();

        setState(new IssueOrders());
        issueOrdersPhase();

        setState(new ExecuteOrders());
        executeOrdersPhase();

        if (checkPlayPhaseResult())
        {
            return;
        }
    }
}

const std::string *GameEngine::getTournamentReport() const
{
    return tournamentReport;
}

// TOURNAMENT: Clear the previous game so a new tournament game starts from the Start state.
void GameEngine::resetGame()
{
    for (Player *player : players)
    {
        delete player;
    }
    players.clear();
    delete deck;
    deck = new Deck(10, 0, 10, 10, 10);
    setPhase(new Startup());
    setState(new Start());
}

// TOURNAMENT: Play one game of the tournament automatically, going through the same states as a normal game.
std::string GameEngine::playTournamentGame(const std::string &mapFile, const std::vector<std::string> &strategies,
                                           int maxTurns)
{
    resetGame();

    std::string error;
    if (!loadMap(mapFile, error))
    {
        return "Invalid map";
    }
    setState(new MapLoaded());

    if (!validateMap())
    {
        return "Invalid map";
    }
    setState(new MapValidated());

    for (const std::string &strategy : strategies)
    {
        addPlayer(strategy, strategy);
        setState(new PlayersAdded());
    }

    gameStart();
    setState(new AssignReinforcement());
    setPhase(new Play());
    playPhase(maxTurns);

    if (*currentState->getName() == "Win" && players.size() == 1)
    {
        return *players.front()->getName();
    }
    return "Draw";
}

// TOURNAMENT: Play G games on each of the M maps with the chosen strategies, then build and log the results table.
void GameEngine::runTournament(const std::vector<std::string> &maps, const std::vector<std::string> &strategies,
                               int games, int maxTurns)
{
    std::vector<std::vector<std::string>> results;
    std::cout << "\n=== Tournament started: " << maps.size() << " map(s), " << games
              << " game(s) per map, max " << maxTurns << " turns ===\n";

    for (const std::string &mapFile : maps)
    {
        std::vector<std::string> row;
        for (int game = 1; game <= games; ++game)
        {
            std::streambuf *console = std::cout.rdbuf(nullptr);
            std::string result;
            try
            {
                result = playTournamentGame(mapFile, strategies, maxTurns);
            }
            catch (const std::exception &error)
            {
                result = std::string("Error: ") + error.what();
            }
            std::cout.rdbuf(console);
            std::cout.clear();

            std::cout << "  " << mapFile << " - game " << game << ": " << result
                      << (result == "Draw" ? " (no winner after " + std::to_string(maxTurns) + " turns)" : "") << '\n';
            row.push_back(result);

            if (result == "Invalid map")
            {
                while (static_cast<int>(row.size()) < games)
                {
                    row.push_back(result);
                }
                break;
            }
        }
        results.push_back(row);
    }

    resetGame();

    std::ostringstream report;
    report << "Tournament mode:\nM: ";
    for (std::size_t i = 0; i < maps.size(); ++i)
    {
        report << (i > 0 ? ", " : "") << maps[i];
    }
    report << "\nP: ";
    for (std::size_t i = 0; i < strategies.size(); ++i)
    {
        report << (i > 0 ? ", " : "") << strategies[i];
    }
    report << "\nG: " << games << "\nD: " << maxTurns << "\n\nResults:\n";

    std::size_t mapColumn = 6;
    for (const std::string &mapFile : maps)
    {
        mapColumn = std::max(mapColumn, mapFile.size() + 2);
    }
    const int resultColumn = 14;
    report << std::left << std::setw(static_cast<int>(mapColumn)) << "";
    for (int game = 1; game <= games; ++game)
    {
        report << std::setw(resultColumn) << ("Game " + std::to_string(game));
    }
    report << '\n';
    for (std::size_t i = 0; i < maps.size(); ++i)
    {
        report << std::setw(static_cast<int>(mapColumn)) << maps[i];
        for (const std::string &result : results[i])
        {
            report << std::setw(resultColumn) << result;
        }
        report << '\n';
    }

    *tournamentReport = report.str();
    std::cout << '\n' << *tournamentReport;

    setState(new Win());
}


// called to start the game
void GameEngine::startupPhase()
{
    namespace fs = std::filesystem;

    std::cout << "Available maps:\n";
    std::error_code ec;
    // recursively gets all files and their path extensions so we can list all the maps in the maps/
    for (const auto &entry : fs::recursive_directory_iterator("maps", ec))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".map")
        {
            std::cout << "  " << entry.path().generic_string() << '\n';
        }
    }
    // command line input
    std::string input;

    while (currentPhase != nullptr &&
           running)
    {
        std::cout << "Enter startup command: ";

        // cmd input
        if (!std::getline(std::cin, input))
        {
            return; // Stop cleanly at EOF
        }

        std::string effect;
        Command command(&input, &effect);

        try
        {
            // calls executeCommand and delegates the command inputted
            executeCommand(command);

            if (currentPhase != nullptr &&
                *currentPhase->getName() == "Play")
            {
                playPhase();
            }
        }
        catch (const std::exception &)
        {
            // executeCommand saved the error in the command's effect
        }

        std::cout << command;
    }
}

// setters
// Replace the current phase after validating the supplied pointer.
void GameEngine::setPhase(Phase *phase)
{
    if (phase == nullptr)
    {
        throw invalid_argument("Phase cannot be null");
    }
    if (this->currentPhase != phase)
    {
        delete this->currentPhase;
        this->currentPhase = phase;
    }
}
// Replace the current state after validating the supplied pointer.
void GameEngine::setState(State *state)
{
    if (state == nullptr)
    {
        throw invalid_argument("State cannot be null");
    }
    if (this->currentState != state)
    {
        delete this->currentState;
        this->currentState = state;
        
        Notify(this);
    }
}

//return game state as string for game log
std::string GameEngine::stringToLog() const   
{  
    return "Game State: " + *currentState->getName();  
}   

// default constructor
Phase::Phase() : name(new string()) {}
// destructor for releasing the memory of the string pointer
Phase::~Phase()
{
    delete name;
}
// custom constructor
Phase::Phase(const string &name) : name(new string(name)) {}
// copy constructor
Phase::Phase(const Phase &phase)
{
    // checking if the phase pointer passed isn't a null pointer and if not then making a new string pointer with the same value of phase.name
    this->name = (phase.name != nullptr) ? new std::string(*phase.name) : nullptr;
}
// assignment operator
Phase &Phase::operator=(const Phase &phase)
{
    // checking if the phase pointer passed isn't a null pointer
    if (this != &phase)
    {
        // Allocate before deleting so a failed allocation leaves this object intact.
        const string *replacement = phase.name != nullptr ? new string(*phase.name) : nullptr;
        delete this->name;
        this->name = replacement;
    }
    return *this;
}
// output stream to simply print the phase name
std::ostream &operator<<(std::ostream &os, const Phase &phase)
{
    os << *phase.getName();
    return os;
}
// getter
const string *Phase::getName() const
{
    return this->name;
}

// Startup default constructor calls the parent constructor with its class name as a passed parameter which will end up being a new string *
Startup::Startup() : Phase("Startup") {}
// copy constructor will simply call the parent constructor which in turn will assign a new string pointer with the passed name value to the new Startup object
Startup::Startup(const Startup &startup) : Phase(startup) {}
// assignment operator simply calls the parent's assignment operator since the object members are managed by the parents logic
Startup &Startup::operator=(const Startup &startup)
{
    if (this != &startup)
    {
        Phase::operator=(startup);
    }
    return *this;
}
// output stream operator simply prints the phase name
std::ostream &operator<<(std::ostream &os, const Startup &startup)
{
    os << *startup.getName();
    return os;
}

// same as startup class
Play::Play() : Phase("Play") {}
Play::Play(const Play &play) : Phase(play) {}
// Copy the name of another Play phase
Play &Play::operator=(const Play &play)
{
    if (this != &play)
    {
        Phase::operator=(play);
    }
    return *this;
}
// Print the Play phase name.
std::ostream &operator<<(std::ostream &os, const Play &play)
{
    os << *play.getName();
    return os;
}

// State default constructor
// Construct an unnamed state.
State::State() : name(new string()) {}
// destructor deallocates the string pointer of name
State::~State()
{
    delete name;
}
// copy constructor
// checking if the phase pointer passed isn't a null pointer and if not then making a new string pointer with the same value of phase.name
State::State(const State &state)
{
    this->name = (state.name != nullptr) ? new std::string(*state.name) : nullptr;
}
// Copy the state's name into owned storage without allocating a temporary string.
// Construct a state with the supplied name.
State::State(const std::string &name) : name(new string(name)) {}
// assignment operator checks if the pointer passed is nullptr if not then release the memory for the previous string* and then assign the passed state name pointer to a newly created string pointer
State &State::operator=(const State &state)
{
    if (this != &state)
    {
        const string *replacement = state.name != nullptr ? new string(*state.name) : nullptr;
        delete name;
        this->name = replacement;
    }
    return *this;
}
// output stream prints the states name
// Print the state name.
std::ostream &operator<<(std::ostream &os, const State &state)
{
    os << *state.getName();
    return os;
}
// getter
// Return the state's name.
const string *State::getName() const
{
    return this->name;
}

// The base class allocates and owns the state name.
Start::Start() : State("Start") {}
// copy constructor passes a pointer to a Start class, we then call the parent's copy constructor to sets its name value to our newly created Start class
Start::Start(const Start &start) : State(start) {}
// calls parents assignment operator since it handles the assignment of the class members
Start &Start::operator=(const Start &start)
{
    if (this != &start)
    {
        State::operator=(start);
    }
    return *this;
}
// output stream prints the name of the state
std::ostream &operator<<(std::ostream &os, const Start &start)
{
    os << *start.getName();
    return os;
}
// transition checks if the passed command is correct for a transition or else throws invalid argument exception
bool Start::canExecute(const string &command, const string &argument, string *effect) const
{
       bool valid = (command == "loadmap" || command == "tournament") && hasArgument(argument); 
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
void Start::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    if (*command == "tournament")
    {
        vector<string> maps;
        vector<string> strategies;
        int games = 0;
        int maxTurns = 0;
        string error;
        if (!parseTournamentCommand(argument, maps, strategies, games, maxTurns, error))
        {
            throw invalid_argument(error);
        }
        gameEngine->runTournament(maps, strategies, games, maxTurns);
        return;
    }
    
    std::string error;
    if (!gameEngine->loadMap(argument, error))
    {
        throw std::invalid_argument("Could not load map: " + error);
    }
    gameEngine->setState(new MapLoaded());
}
// from here on out these are all implementations of a state pattern
// therefore their implementations are identical to the Start class and are all children of State
// MapLoaded
MapLoaded::MapLoaded() : State("MapLoaded") {}
MapLoaded::MapLoaded(const MapLoaded &mapLoaded) : State(mapLoaded) {}
MapLoaded &MapLoaded::operator=(const MapLoaded &mapLoaded)
{
    if (this != &mapLoaded)
    {
        State::operator=(mapLoaded);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const MapLoaded &mapLoaded)
{
    os << *mapLoaded.getName();
    return os;
}
// Validate commands accepted after a map has been loaded.
bool MapLoaded::canExecute(const string &command, const string &argument, string *effect) const
{
    bool valid = (command == "loadmap" && hasArgument(argument)) || command == "validatemap";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
// Apply a valid command transition from the map-loaded state.
void MapLoaded::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    if (command->compare("loadmap") == 0)
    {
        std::string error;
        if (!gameEngine->loadMap(argument, error))
        {
            throw std::invalid_argument("Could not load map: " + error);
        }
        gameEngine->setState(new MapLoaded());
    }
    else
    {
        std::string error;
        if (!gameEngine->validateMap())
        {
            throw std::invalid_argument("Could not validate map: " + error);
        }
        gameEngine->setState(new MapValidated());
    }
}

// MapValidated
MapValidated::MapValidated() : State("MapValidated") {}
MapValidated::MapValidated(const MapValidated &mapValidated) : State(mapValidated) {}
MapValidated &MapValidated::operator=(const MapValidated &mapValidated)
{
    if (this != &mapValidated)
    {
        State::operator=(mapValidated);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const MapValidated &mapValidated)
{
    os << *mapValidated.getName();
    return os;
}
// Validate commands accepted after map validation.
bool MapValidated::canExecute(const string &command, const string &argument, string *effect) const
{
    string playerName;
    string strategy;
    bool valid = command == "addplayer" && parsePlayerSpecification(argument, playerName, strategy);
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
// Apply a valid command transition from the map-validated state.
void MapValidated::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    std::string playerName;
    std::string strategy;
    if (!parsePlayerSpecification(argument, playerName, strategy))
        throw invalid_argument("Use: addplayer <name> <strategy>");
    gameEngine->addPlayer(playerName, strategy);
    gameEngine->setState(new PlayersAdded());
}

// PlayersAdded
PlayersAdded::PlayersAdded() : State("PlayersAdded") {}
PlayersAdded::PlayersAdded(const PlayersAdded &playersAdded) : State(playersAdded) {}
PlayersAdded &PlayersAdded::operator=(const PlayersAdded &playersAdded)
{
    if (this != &playersAdded)
    {
        State::operator=(playersAdded);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const PlayersAdded &playersAdded)
{
    os << *playersAdded.getName();
    return os;
}
// Validate commands accepted while players are being added.
bool PlayersAdded::canExecute(const string &command, const string &argument, string *effect) const
{
    string playerName;
    string strategy;
    bool valid = (command == "addplayer" && parsePlayerSpecification(argument, playerName, strategy)) || command == "gamestart";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
// Apply a valid command transition from the players-added state.
void PlayersAdded::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    if (command->compare("addplayer") == 0)
    {
        std::string playerName;
        std::string strategy;
        if (!parsePlayerSpecification(argument, playerName, strategy))
            throw invalid_argument("Use: addplayer <name> <strategy>");
        gameEngine->addPlayer(playerName, strategy);
        gameEngine->setState(new PlayersAdded());
    }
    else
    {
        gameEngine->gameStart();
        gameEngine->setState(new AssignReinforcement());
        gameEngine->setPhase(new Play());
    }
}

// AssignReinforcement
AssignReinforcement::AssignReinforcement() : State("AssignReinforcement") {}
AssignReinforcement::AssignReinforcement(const AssignReinforcement &assignReinforcement) : State(assignReinforcement) {}
AssignReinforcement &AssignReinforcement::operator=(const AssignReinforcement &assignReinforcement)
{
    if (this != &assignReinforcement)
    {
        State::operator=(assignReinforcement);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const AssignReinforcement &assignReinforcement)
{
    os << *assignReinforcement.getName();
    return os;
}
// Validate commands accepted during reinforcement assignment.
bool AssignReinforcement::canExecute(const string &command, const string &argument, string *effect) const
{
    bool valid = command == "issueorder";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
// Apply a valid transition out of reinforcement assignment.
void AssignReinforcement::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    gameEngine->reinforcementPhase();
    gameEngine->issueOrdersPhase();
    gameEngine->setState(new IssueOrders());
}

// IssueOrders
IssueOrders::IssueOrders() : State("IssueOrders") {}
IssueOrders::IssueOrders(const IssueOrders &issueOrders) : State(issueOrders) {}
IssueOrders &IssueOrders::operator=(const IssueOrders &issueOrders)
{
    if (this != &issueOrders)
    {
        State::operator=(issueOrders);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const IssueOrders &issueOrders)
{
    os << *issueOrders.getName();
    return os;
}
// Validate commands accepted while orders are being issued.
bool IssueOrders::canExecute(const string &command, const string &argument, string *effect) const
{
    bool valid = command == "endissueorders";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
// Apply a valid transition out of order issuance.
void IssueOrders::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    gameEngine->setState(new ExecuteOrders());
}

// ExecuteOrders
ExecuteOrders::ExecuteOrders() : State("ExecuteOrders") {}
ExecuteOrders::ExecuteOrders(const ExecuteOrders &executeOrders) : State(executeOrders) {}
ExecuteOrders &ExecuteOrders::operator=(const ExecuteOrders &executeOrders)
{
    if (this != &executeOrders)
    {
        State::operator=(executeOrders);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const ExecuteOrders &executeOrders)
{
    os << *executeOrders.getName();
    return os;
}
// Validate commands accepted while orders are being executed.
bool ExecuteOrders::canExecute(const string &command, const string &argument, string *effect) const
{
    bool valid = command == "execorder" || command == "endexecorders" || command == "win";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
// Apply a valid transition out of order execution.
void ExecuteOrders::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    if (command->compare("execorder") == 0)
    {
        gameEngine->executeOrdersPhase();
        gameEngine->setState(new ExecuteOrders());
    }
    else if (command->compare("endexecorders") == 0)
    {
        gameEngine->setState(new AssignReinforcement());
    }
    else
    {
        gameEngine->setState(new Win());
    }
}

// Win
Win::Win() : State("Win") {}
Win::Win(const Win &win) : State(win) {}
Win &Win::operator=(const Win &win)
{
    if (this != &win)
    {
        State::operator=(win);
    }
    return *this;
}
std::ostream &operator<<(std::ostream &os, const Win &win)
{
    os << *win.getName();
    return os;
}
// Validate commands accepted after a winner is found.
bool Win::canExecute(const string &command, const string &argument, string *effect) const
{
    bool valid = command == "quit" || command == "replay";
    return setCheckEffect(command, argument, *getName(), valid, effect);
}
// Apply a valid command transition from the win state.
void Win::transition(const string *command, GameEngine *gameEngine, const string &argument)
{
    if (command == nullptr || gameEngine == nullptr || !canExecute(*command, argument))
    {
        throw invalid_argument("Wrong command for this state");
    }
    if (command->compare("quit") == 0)
    {
        // executeCommand saves the effect and tells the driver to stop.
        return;
    }
    else
    {
        gameEngine->setState(new Start());
        gameEngine->setPhase(new Startup());
    }
}
