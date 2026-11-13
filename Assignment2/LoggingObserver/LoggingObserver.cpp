#include "LoggingObserver.h"

#include <algorithm>
#include <stdexcept>

// Iloggable

// Default constructor
ILoggable::ILoggable() {}

// Copy constructor 
ILoggable::ILoggable(const ILoggable& other) { (void)other; }

// Assignment operator 
ILoggable& ILoggable::operator=(const ILoggable& other) {
    (void)other;
    return *this;
}

// Destructor
ILoggable::~ILoggable() {}

// Prints the line that would be logged.
std::ostream& operator<<(std::ostream& out, const ILoggable& loggable) {
    out << loggable.stringToLog();
    return out;
}

//Observer 

// Default constructor
Observer::Observer() {}

// Copy constructor
Observer::Observer(const Observer& other) { (void)other; }

// Assignment operator
Observer& Observer::operator=(const Observer& other) {
    (void)other;
    return *this;
}

// Destructor
Observer::~Observer() {}

// Prints the observer
std::ostream& operator<<(std::ostream& out, const Observer& observer) {
    (void)observer;
    out << "Observer";
    return out;
}

// Subject

// Default constructor
Subject::Subject() {}

// copied subject starts with own empty observer list
Subject::Subject(const Subject& other) { (void)other; }

// Keep the observers already registered with subject
Subject& Subject::operator=(const Subject& other) {
    (void)other;
    return *this;
}

// Destructor
Subject::~Subject() {}

// Attaches an observer to a subject
void Subject::attach(Observer* observer) {
    if (observer != nullptr &&
        std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

// Removes an observer from a subject
void Subject::detach(Observer* observer) {
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end());
}

// Notifies every attached observer
void Subject::Notify(ILoggable* loggable) {
    for (Observer* observer : observers) {
        observer->update(loggable);
    }
}

// Attaches every observer of this Subject to the target Subject
void Subject::passObserversTo(Subject& target) const {
    for (Observer* observer : observers) {
        target.attach(observer);
    }
}

// Prints how many observers are attached
std::ostream& operator<<(std::ostream& out, const Subject& subject) {
    out << "Subject with " << subject.observers.size() << " observer(s)";
    return out;
}

// LogObserver

// Opens the log file and clears it so it only holds the current game
LogObserver::LogObserver(const std::string& fileName)
    : fileName(fileName), logFile(fileName, std::ios::trunc) {
    if (!logFile.is_open()) {
        throw std::runtime_error("Could not open game log file: " + fileName);
    }
}

// Copy constructor 
LogObserver::LogObserver(const LogObserver& other)
    : Observer(other), fileName(other.fileName), logFile(other.fileName, std::ios::app) {
    if (!logFile.is_open()) {
        throw std::runtime_error("Could not open game log file: " + fileName);
    }
}

// closes the current file and appends to the other's file
LogObserver& LogObserver::operator=(const LogObserver& other) {
    if (this != &other) {
        Observer::operator=(other);
        logFile.close();
        fileName = other.fileName;
        logFile.open(fileName, std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Could not open game log file: " + fileName);
        }
    }
    return *this;
}

//Destructor
LogObserver::~LogObserver() {}

// Appends the loggable's line to the log file
void LogObserver::update(ILoggable* loggable) {
    if (loggable != nullptr) {
        logFile << loggable->stringToLog() << std::endl;
    }
}

// Prints the log file name
std::ostream& operator<<(std::ostream& out, const LogObserver& observer) {
    out << "LogObserver writing to " << observer.fileName;
    return out;
}