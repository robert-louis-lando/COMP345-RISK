#ifndef GAME_STATES_H
#define GAME_STATES_H
#define STRINGIFY(x) #x



class Phase {
    public:
        virtual ~Phase() = default;
        const char* getName() const;
        virtual void transition(const char* command) = 0;

    protected:
        explicit Phase(const char* name);

    private:
        const char* name;
};
class Startup : public Phase{
    public:
        Startup();
        void transition(const char* command) override;
    };
class Play : public Phase{
    public:
        Play();
        void transition(const char* command) override;
    };
class State {
    public:
        virtual ~State() = default;
        const char* getName() const;
        virtual void transition(const char* command) = 0;

    protected:
        explicit State(const char* name);

    private:
        const char* name;
};

class Start : public State {
    public:
        Start();
        void transition(const char* command) override;
    };
class MapLoaded : public State{
    public:
        MapLoaded();
        void transition(const char* command) override;
    };
class MapValidated : public State{
    public:
        MapValidated();
        void transition(const char* command) override;
    };
class PlayersAdded : public State{
    public:
        PlayersAdded();
        void transition(const char* command) override;
    };
class AssignReinforcement : public State{
    public:
        AssignReinforcement();
        void transition(const char* command) override;
    };
class IssueOrders : public State{
    public:
        IssueOrders();
        void transition(const char* command) override;
    };
class ExecuteOrders : public State{
    public:
        ExecuteOrders();
        void transition(const char* command) override;
    };
class Win : public State{
    public:
        Win();
        void transition(const char* command) override;
    };
class CurrentPhase{
    public:
        const char* getCurrentPhase();
    private:
        CurrentPhase();
        Phase* currentPhase;
};
class CurrentState{
    public:
        const char* getCurrentState();
    private:
        CurrentState();
        State* currentState;
};

class GameEngine{
    public:
        void executeCommand(char* command);
        char* getCurrentPhase();
        char* getCurrentState();
        void setPhase(State* newState);
    private:
        CurrentPhase* currentPhase;
        CurrentState* currentState;
};

#endif