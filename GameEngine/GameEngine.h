#ifndef GAME_STATES_H
#define GAME_STATES_H
#include <string>
#include <iostream>

class Phase;
class State;
class GameEngine{
    public:
        static GameEngine& getInstance();
        GameEngine(const GameEngine& gameEngine)=delete;
        GameEngine& operator=(const GameEngine& gameEngine)=delete;
        friend std::ostream& operator <<(std::ostream os, const GameEngine& gameEngine);
        void executeCommand(const std::string* command);
        const std::string* getCurrentPhase() const;
        const std::string* getCurrentState() const;
        void setPhase(Phase* newPhase);
        void setState(State* newState);
    private:
        GameEngine();
        ~GameEngine();
        Phase* currentPhase;
        State* currentState;
};

class Phase {
    public:
        Phase();
        virtual ~Phase();
        Phase(const std::string* name);
        Phase(const Phase& phase);
        Phase& operator=(const Phase& phase);
        friend std::ostream& operator <<(std::ostream os, const Phase& phase);
        const std::string* getName() const;

    private:
        const std::string* name;
};
class Startup : public Phase{
    public:
        Startup();
        Startup(const Startup& startup);
        Startup& operator=(const Startup& startup);
        friend std::ostream& operator <<(std::ostream os, const Startup& startup);
    };
class Play : public Phase{
    public:
        Play();
        Play(const Play& play);
        Play& operator=(const Play& play);
        friend std::ostream& operator <<(std::ostream os, const Play& play);
    };
class State {
    public:
        State();
        virtual ~State();
        State(const State& state);
        State& operator=(const State& state);
        friend std::ostream& operator <<(std::ostream os, const State& state);
        const std::string* getName() const;
        virtual void transition(const std::string* command, GameEngine* gameEngine) = 0;

    protected:
        explicit State(const std::string* name);

    private:
        const std::string* name;
};
class Start : public State {
    public:
        Start();
        Start(const Start& start);
        Start& operator=(const Start& start);
        friend std::ostream& operator <<(std::ostream os, const Start& start);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
class MapLoaded : public State{
    public:
        MapLoaded();
        MapLoaded(const MapLoaded& maploaded);
        MapLoaded& operator=(const MapLoaded& maploaded);
        friend std::ostream& operator <<(std::ostream os, const MapLoaded& maploaded);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
class MapValidated : public State{
    public:
        MapValidated();
        MapValidated(const MapValidated& mapValidated);
        MapValidated& operator=(const MapValidated& mapValidated);
        friend std::ostream& operator <<(std::ostream os, const MapValidated& mapValidated);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
class PlayersAdded : public State{
    public:
        PlayersAdded();
        PlayersAdded(const PlayersAdded& playersAdded);
        PlayersAdded& operator=(const PlayersAdded& playersAdded);
        friend std::ostream& operator <<(std::ostream os, const PlayersAdded& playersAdded);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
class AssignReinforcement : public State{
    public:
        AssignReinforcement();
        AssignReinforcement(const AssignReinforcement& assignReinforcement);
        AssignReinforcement& operator=(const AssignReinforcement& assignReinforcement);
        friend std::ostream& operator <<(std::ostream os, const AssignReinforcement& assignReinforcement);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
class IssueOrders : public State{
    public:
        IssueOrders();
        IssueOrders(const IssueOrders& issueOrders);
        IssueOrders& operator=(const IssueOrders& issueOrders);
        friend std::ostream& operator <<(std::ostream os, const IssueOrders& issueOrders);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
class ExecuteOrders : public State{
    public:
        ExecuteOrders();
        ExecuteOrders(const ExecuteOrders& executeOrders);
        ExecuteOrders& operator=(const ExecuteOrders& executeOrders);
        friend std::ostream& operator <<(std::ostream os, const ExecuteOrders& executeOrders);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };
class Win : public State{
    public:
        Win();
        Win(const Win& win);
        Win& operator=(const Win& win);
        friend std::ostream& operator <<(std::ostream os, const Win& win);
        void transition(const std::string* command, GameEngine* gameEngine) override;
    };

#endif