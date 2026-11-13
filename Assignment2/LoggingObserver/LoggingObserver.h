#ifndef LOGGING_OBSERVER_H
#define LOGGING_OBSERVER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Interface for the classes that have events to log
class ILoggable {
public:
    ILoggable();
    ILoggable(const ILoggable& other);
    ILoggable& operator=(const ILoggable& other);
    virtual ~ILoggable();

    // Returns the line thats logged
    virtual std::string stringToLog() const = 0;

    friend std::ostream& operator<<(std::ostream& out, const ILoggable& loggable);
};

// Interface for observers who will be notified when subjects change
class Observer {
public:
    Observer();
    Observer(const Observer& other);
    Observer& operator=(const Observer& other);
    virtual ~Observer();

    // Called by subject when it notifies its observers.
    virtual void update(ILoggable* loggable) = 0;

    friend std::ostream& operator<<(std::ostream& out, const Observer& observer);
};

// class for everything that needs to be observed, it keeps track of observers and notifies them when an event occurs
class Subject {
public:
    Subject();
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);
    virtual ~Subject();

    // Adds an observer 
    void attach(Observer* observer);

    // Removes an observer
    void detach(Observer* observer);

    // calls update on every attached observer
    void Notify(ILoggable* loggable);

    //copies the observer of  a subject to another
    void passObserversTo(Subject& target) const;

    friend std::ostream& operator<<(std::ostream& out, const Subject& subject);

private:
    // stores pointers to observers, instead of them owning them
    std::vector<Observer*> observers;
};

// receive notification and write msgs to the log file
class LogObserver : public Observer {
public:
    explicit LogObserver(const std::string& fileName = "gamelog.txt");
    LogObserver(const LogObserver& other);
    LogObserver& operator=(const LogObserver& other);
    ~LogObserver() override;

    //writes event message to log file
    void update(ILoggable* loggable) override;

    friend std::ostream& operator<<(std::ostream& out, const LogObserver& observer);

private:
    std::string fileName;
    std::ofstream logFile;
};

#endif