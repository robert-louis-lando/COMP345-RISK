#ifndef GAME_STATES_H
#define GAME_STATES_H
#include <string>
#include <iostream>

class Phase;
class State;

//Singleton GameEngine which controls the state of the game
class GameEngine{
    public:
        //since its a singleton to always get the same instance
        static GameEngine& getInstance();
        //No copy constructor since its a singleton
        GameEngine(const GameEngine& gameEngine)=delete;
        //No assigment operator since its a singleton with a precise starting point
        GameEngine& operator=(const GameEngine& gameEngine)=delete;
        //custom output stream operator to print the phase and state to the output stream
        friend std::ostream& operator <<(std::ostream& os, const GameEngine& gameEngine);
        //executes a command coming from the console
        void executeCommand(const std::string* command);
        //getters
        const std::string* getCurrentPhase() const;
        const std::string* getCurrentState() const;
        //setters
        void setPhase(Phase* newPhase);
        void setState(State* newState);
    private:
        //private default constructor + destructor because its a singleton
        GameEngine();
        ~GameEngine();

        //contains both current phase and state
        Phase* currentPhase;
        State* currentState;
};

//parent class phase which contains the string pointer name
class Phase {
    public:
        Phase();
        //allowing to clean up memory, clean up the string pointer name
        virtual ~Phase();
        Phase(const Phase& phase);
        Phase& operator=(const Phase& phase);
        friend std::ostream& operator <<(std::ostream os, const Phase& phase);
        const std::string* getName() const;

    protected:
        //made this custom constructor explicit and protected since the only way to call this constructor is when a child class is being initialized with its class name as the name variable
        explicit Phase(const std::string* name);
    private:
        const std::string* name;
};
//inherits from phase
class Startup : public Phase{
    public:
        Startup();
        Startup(const Startup& startup);
        Startup& operator=(const Startup& startup);
        friend std::ostream& operator <<(std::ostream os, const Startup& startup);
    };
//inherits from phase
class Play : public Phase{
    public:
        Play();
        Play(const Play& play);
        Play& operator=(const Play& play);
        friend std::ostream& operator <<(std::ostream os, const Play& play);
    };

//parent class state which contains the string pointer name 
class State {
    public:
        State();
        virtual ~State();
        State(const State& state);
        State& operator=(const State& state);
        friend std::ostream& operator <<(std::ostream os, const State& state);
        const std::string* getName() const;

        //this is a abstract method since the parent class State will never be instantiated
        //this method transition is to check if the passed command is valid and if so transitions to a different state + phase and changes the current state/phase in the game engine, if not throws error
        virtual void transition(const std::string* command, GameEngine* gameEngine) = 0;

    protected:
        //explicit custom constructor to avoid the automatic type conversion, also its protected since only the child class itself knows its name and will set it on creation
        explicit State(const std::string* name);

    private:
        const std::string* name;
};
//inherits from state
class Start : public State {
    public:
        Start();
        Start(const Start& start);
        Start& operator=(const Start& start);
        friend std::ostream& operator <<(std::ostream os, const Start& start);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
//inherits from state
class MapLoaded : public State{
    public:
        MapLoaded();
        MapLoaded(const MapLoaded& maploaded);
        MapLoaded& operator=(const MapLoaded& maploaded);
        friend std::ostream& operator <<(std::ostream os, const MapLoaded& maploaded);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
//inherits from state
class MapValidated : public State{
    public:
        MapValidated();
        MapValidated(const MapValidated& mapValidated);
        MapValidated& operator=(const MapValidated& mapValidated);
        friend std::ostream& operator <<(std::ostream os, const MapValidated& mapValidated);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
//inherits from state
class PlayersAdded : public State{
    public:
        PlayersAdded();
        PlayersAdded(const PlayersAdded& playersAdded);
        PlayersAdded& operator=(const PlayersAdded& playersAdded);
        friend std::ostream& operator <<(std::ostream os, const PlayersAdded& playersAdded);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
//inherits from state
class AssignReinforcement : public State{
    public:
        AssignReinforcement();
        AssignReinforcement(const AssignReinforcement& assignReinforcement);
        AssignReinforcement& operator=(const AssignReinforcement& assignReinforcement);
        friend std::ostream& operator <<(std::ostream os, const AssignReinforcement& assignReinforcement);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
//inherits from state
class IssueOrders : public State{
    public:
        IssueOrders();
        IssueOrders(const IssueOrders& issueOrders);
        IssueOrders& operator=(const IssueOrders& issueOrders);
        friend std::ostream& operator <<(std::ostream os, const IssueOrders& issueOrders);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
//inherits from state
class ExecuteOrders : public State{
    public:
        ExecuteOrders();
        ExecuteOrders(const ExecuteOrders& executeOrders);
        ExecuteOrders& operator=(const ExecuteOrders& executeOrders);
        friend std::ostream& operator <<(std::ostream os, const ExecuteOrders& executeOrders);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
//inherits from state
class Win : public State{
    public:
        Win();
        Win(const Win& win);
        Win& operator=(const Win& win);
        friend std::ostream& operator <<(std::ostream os, const Win& win);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };

#endif