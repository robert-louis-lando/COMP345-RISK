#ifndef GAME_STATES_H
#define GAME_STATES_H
#define STRINGIFY(x) #x

class GameEngine{
    public:
        GameEngine();
        ~GameEngine();
        void executeCommand(char* command);
        char* getCurrentPhase();
        char* getCurrentState();
        void setPhase(Phase* newPhase);
        void setState(State* newState);
    private:
        Phase* currentPhase;
        State* currentState;
};

class Phase {
    public:
        virtual ~Phase() = default;
        const char* getName() const;

    protected:
        explicit Phase(const char* name);

    private:
        const char* name;
};
class Startup : public Phase{
    public:
        Startup();
    };
class Play : public Phase{
    public:
        Play();
    };
class State {
    public:
        virtual ~State() = default;
        const char* getName() const;
        virtual void transition(const char* command, GameEngine* gameEngine) = 0;

    protected:
        explicit State(const char* name);

    private:
        const char* name;
};
class Start : public State {
    public:
        Start();
        void transition(const char* command, GameEngine* gameEngine) override;
    };
class MapLoaded : public State{
    public:
        MapLoaded();
        void transition(const char* command, GameEngine* gameEngine) override;
    };
class MapValidated : public State{
    public:
        MapValidated();
        void transition(const char* command, GameEngine* gameEngine) override;
    };
class PlayersAdded : public State{
    public:
        PlayersAdded();
        void transition(const char* command, GameEngine* gameEngine) override;
    };
class AssignReinforcement : public State{
    public:
        AssignReinforcement();
        void transition(const char* command, GameEngine* gameEngine) override;
    };
class IssueOrders : public State{
    public:
        IssueOrders();
        void transition(const char* command, GameEngine* gameEngine) override;
    };
class ExecuteOrders : public State{
    public:
        ExecuteOrders();
        void transition(const char* command, GameEngine* gameEngine) override;
    };
class Win : public State{
    public:
        Win();
        void transition(const char* command, GameEngine* gameEngine) override;
    };

#endif