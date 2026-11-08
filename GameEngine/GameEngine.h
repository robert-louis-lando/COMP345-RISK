#ifndef GAME_STATES_H
#define GAME_STATES_H
#include <string>
#include <iostream>
#include "CommandProcessing.h"
#include "Cards.h"
#include "Map.h"
#include "Player.h"

class Phase;
class State;

// Singleton GameEngine which controls the state of the game
class GameEngine
{
public:
    // since its a singleton to always get the same instance
    static GameEngine &getInstance();
    // No copy constructor since its a singleton
    GameEngine(const GameEngine &gameEngine) = delete;
    // No assigment operator since its a singleton with a precise starting point
    GameEngine &operator=(const GameEngine &gameEngine) = delete;
    // custom output stream operator to print the phase and state to the output stream
    friend std::ostream &operator<<(std::ostream &os, const GameEngine &gameEngine);
    // Executes a command and saves the outcome on that same object.
    // Rejected commands retain their error before an exception is thrown.
    void executeCommand(Command &command);
    // Allows the driver to stop after quit while still destroying its input processor.
    bool isRunning() const;
    // checks whether the current state accepts a command without changing state
    // optionally writes a rejection error; success clears it without claiming execution
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *error = nullptr) const;
    // startup
    void startupPhase();
    // loadmap
    bool loadMap(const std::string &path, std::string &error);
    // validate if map is not null and delegate validation
    bool validateMap() const;
    // does check and add player to the vector of players
    void addPlayer(const std::string &name);
    // does some actions before transitioning to the play phase
    void gameStart();
    // adds each player's territory and continent-control reinforcements
    void reinforcementPhase();
    // issue orders
    void issueOrdersPhase();
    // executeorders
    void executeOrdersPhase();
    // Expose the current game objects for drivers and state inspection.
    Map *getMap() const;
    const std::vector<Player *> &getPlayers() const;
    // repeatedly runs the Play phase turn cycle
    void playPhase();
    // removes eliminated players and detects the game winner
    bool checkPlayPhaseResult();
    // getters
    const std::string *getCurrentPhase() const;
    const std::string *getCurrentState() const;
    // setters
    void setPhase(Phase *newPhase);
    void setState(State *newState);

private:
    // private default constructor + destructor because its a singleton
    GameEngine();
    ~GameEngine();

    // contains both current phase and state
    Phase *currentPhase;
    State *currentState;
    // deck of cards
    Deck *deck;
    // map of the game
    Map *map;
    // list of players
    std::vector<Player *> players;

    // command processor to read commands from the console or from a file
    CommandProcessor *commandProcessor;
    bool running;
};

// parent class phase which contains the string pointer name
class Phase
{
public:
    // Construct an unnamed phase.
    Phase();
    // allowing to clean up memory, clean up the string pointer name
    virtual ~Phase();
    // Copy a phase name.
    Phase(const Phase &phase);
    // Assign a phase name.
    Phase &operator=(const Phase &phase);
    // Print the phase name.
    friend std::ostream &operator<<(std::ostream &os, const Phase &phase);
    // Return the phase name.
    const std::string *getName() const;

protected:
    // made this custom constructor explicit and protected since the only way to call this constructor is when a child class is being initialized with its class name as the name variable
    explicit Phase(const std::string &name);

private:
    const std::string *name;
};
// inherits from phase
class Startup : public Phase
{
public:
    // Construct the startup phase.
    Startup();
    // Copy a startup phase.
    Startup(const Startup &startup);
    // Assign a startup phase.
    Startup &operator=(const Startup &startup);
    // Print the startup phase name.
    friend std::ostream &operator<<(std::ostream &os, const Startup &startup);
};
// inherits from phase
class Play : public Phase
{
public:
    // Construct the play phase.
    Play();
    // Copy a play phase.
    Play(const Play &play);
    // Assign a play phase.
    Play &operator=(const Play &play);
    // Print the play phase name.
    friend std::ostream &operator<<(std::ostream &os, const Play &play);
};

// parent class state which contains the string pointer name
class State
{
public:
    // Construct an unnamed state.
    State();
    // Release the state name.
    virtual ~State();
    // Copy a state name.
    State(const State &state);
    // Assign a state name.
    State &operator=(const State &state);
    // Print the state name.
    friend std::ostream &operator<<(std::ostream &os, const State &state);
    // Return the state name.
    const std::string *getName() const;

    // this is a abstract method since the parent class State will never be instantiated
    // this method transition is to check if the passed command is valid and if so transitions to a different state + phase and changes the current state/phase in the game engine, if not throws error
    virtual void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") = 0;
    // effect contains a rejection error only; success clears it and does not execute
    virtual bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const = 0;

protected:
    // explicit custom constructor to avoid the automatic type conversion, also its protected since only the child class itself knows its name and will set it on creation
    explicit State(const std::string &name);

private:
    const std::string *name;
};
// inherits from state
class Start : public State
{
public:
    // Construct the start state.
    Start();
    // Copy a start state.
    Start(const Start &start);
    // Assign a start state.
    Start &operator=(const Start &start);
    // Print the start state name.
    friend std::ostream &operator<<(std::ostream &os, const Start &start);
    // Validate a command in the start state.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a command transition from the start state.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};
// inherits from state
class MapLoaded : public State
{
public:
    // Construct the map-loaded state.
    MapLoaded();
    // Copy a map-loaded state.
    MapLoaded(const MapLoaded &maploaded);
    // Assign a map-loaded state.
    MapLoaded &operator=(const MapLoaded &maploaded);
    // Print the map-loaded state name.
    friend std::ostream &operator<<(std::ostream &os, const MapLoaded &maploaded);
    // Validate a command after loading a map.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a command transition after loading a map.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};
// inherits from state
class MapValidated : public State
{
public:
    // Construct the map-validated state.
    MapValidated();
    // Copy a map-validated state.
    MapValidated(const MapValidated &mapValidated);
    // Assign a map-validated state.
    MapValidated &operator=(const MapValidated &mapValidated);
    // Print the map-validated state name.
    friend std::ostream &operator<<(std::ostream &os, const MapValidated &mapValidated);
    // Validate a command after validating a map.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a command transition after validating a map.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};
// inherits from state
class PlayersAdded : public State
{
public:
    // Construct the players-added state.
    PlayersAdded();
    // Copy a players-added state.
    PlayersAdded(const PlayersAdded &playersAdded);
    // Assign a players-added state.
    PlayersAdded &operator=(const PlayersAdded &playersAdded);
    // Print the players-added state name.
    friend std::ostream &operator<<(std::ostream &os, const PlayersAdded &playersAdded);
    // Validate a command while adding players.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a command transition while adding players.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};
// inherits from state
class AssignReinforcement : public State
{
public:
    // Construct the reinforcement-assignment state.
    AssignReinforcement();
    // Copy a reinforcement-assignment state.
    AssignReinforcement(const AssignReinforcement &assignReinforcement);
    // Assign a reinforcement-assignment state.
    AssignReinforcement &operator=(const AssignReinforcement &assignReinforcement);
    // Print the reinforcement-assignment state name.
    friend std::ostream &operator<<(std::ostream &os, const AssignReinforcement &assignReinforcement);
    // Validate a command during reinforcement assignment.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a transition after reinforcement assignment.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};
// inherits from state
class IssueOrders : public State
{
public:
    // Construct the issue-orders state.
    IssueOrders();
    // Copy an issue-orders state.
    IssueOrders(const IssueOrders &issueOrders);
    // Assign an issue-orders state.
    IssueOrders &operator=(const IssueOrders &issueOrders);
    // Print the issue-orders state name.
    friend std::ostream &operator<<(std::ostream &os, const IssueOrders &issueOrders);
    // Validate a command while issuing orders.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a transition after issuing orders.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};
// inherits from state
class ExecuteOrders : public State
{
public:
    // Construct the execute-orders state.
    ExecuteOrders();
    // Copy an execute-orders state.
    ExecuteOrders(const ExecuteOrders &executeOrders);
    // Assign an execute-orders state.
    ExecuteOrders &operator=(const ExecuteOrders &executeOrders);
    // Print the execute-orders state name.
    friend std::ostream &operator<<(std::ostream &os, const ExecuteOrders &executeOrders);
    // Validate a command while executing orders.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a transition while executing orders.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};
// inherits from state
class Win : public State
{
public:
    // Construct the win state.
    Win();
    // Copy a win state.
    Win(const Win &win);
    // Assign a win state.
    Win &operator=(const Win &win);
    // Print the win state name.
    friend std::ostream &operator<<(std::ostream &os, const Win &win);
    // Validate a command after the game is won.
    bool canExecute(const std::string &command, const std::string &argument = "", std::string *effect = nullptr) const override;
    // Apply a command transition after the game is won.
    void transition(const std::string *command, GameEngine *gameEngine, const std::string &argument = "") override;
};

#endif
