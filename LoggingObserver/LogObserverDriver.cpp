#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "CommandProcessing.h"
#include "GameEngine.h"
#include "LoggingObserver.h"
#include "Orders.h"
#include "Player.h"

using namespace std;

// Prints whether the object is both a subject and ILoggable
void showSubjectAndLoggable(const string &className, Subject *subject)
{
    ILoggable *loggable = dynamic_cast<ILoggable *>(subject);
    cout << className << ": Subject = yes, ILoggable = " << (loggable != nullptr ? "yes" : "no") << endl;
}

// Shows that every logged class is a Subject and ILoggable
void demonstrateInheritance()
{
    cout << "\n=== 1. Subject and ILoggable inheritance ===" << endl;
    Command command;
    CommandProcessor commandProcessor;
    Deploy deploy("Alice", "Alpha", 1);
    OrdersList ordersList;

    showSubjectAndLoggable("Command", &command);
    showSubjectAndLoggable("CommandProcessor", &commandProcessor);
    showSubjectAndLoggable("Order (Deploy)", &deploy);
    showSubjectAndLoggable("OrdersList", &ordersList);
    showSubjectAndLoggable("GameEngine", &GameEngine::getInstance());
}

// Shows commands effects and engine state changes in log
void demonstrateCommands(CommandProcessor *processor, LogObserver *logObserver)
{
    cout << "\n=== 2. Commands, effects and game states ===" << endl;
    GameEngine &engine = GameEngine::getInstance();
    engine.attach(logObserver);
    processor->attach(logObserver);

    while (engine.isRunning())
    {
        Command *command = processor->getCommand();
        if (command == nullptr)
        {
            break;
        }
        if (command->getEffect()->empty())
        {
            try
            {
                engine.executeCommand(*command);
            }
            catch (const exception &)
            {
                // executeCommand is already saved
            }
        }
        cout << *command;
    }

    engine.detach(logObserver);
}

// Shows an order being added and executed in log
void demonstrateOrders(LogObserver *logObserver)
{
    cout << "\n=== 3. Orders added and executed ===" << endl;
    Territory alpha("Alpha", 0, 0, 1, 10, nullptr);
    Territory beta("Beta", 1, 0, 2, 10, nullptr);
    Territory gamma("Gamma", 2, 0, 3, 10, nullptr);
    Player alice("Alice");
    Player bob("Bob");
    alice.addTerritory(&alpha);
    alice.addTerritory(&beta);
    bob.addTerritory(&gamma);
    alice.setReinforcementPool(10);

    alice.getOrders()->attach(logObserver);

    alice.issueOrder(new Deploy(&alice, "Alpha", 5));
    alice.issueOrder(new Airlift(&alice, "Alpha", "Beta", 3));
    alice.issueOrder(new Negotiate(&alice, &bob));
    alice.issueOrder(new Bomb(&alice, "Alpha"));
    alice.issueOrder(new Blockade(&alice, "Beta"));

    cout << *alice.getOrders() << endl;
    for (int i = 0; i < alice.getOrders()->size(); i++)
    {
        alice.getOrders()->get(i)->execute();
    }
    cout << *alice.getOrders() << endl;
}

// Prints the log file
void printLogFile(const string &fileName)
{
    ifstream file(fileName);
    string line;
    cout << "\n=== " << fileName << " ===" << endl;
    while (getline(file, line))
    {
        cout << line << endl;
    }
}

// Reads commands from the console 
int main(int argc, char *argv[])
{
    unique_ptr<CommandProcessor> processor;
    if (argc == 1 || (argc == 2 && string(argv[1]) == "-console"))
    {
        processor.reset(new CommandProcessor());
    }
    else if (argc == 3 && string(argv[1]) == "-file")
    {
        processor.reset(new FileCommandProcessorAdapter(argv[2]));
    }
    else
    {
        cerr << "Usage: " << argv[0] << " [-console | -file <filename>]" << endl;
        return 2;
    }

    LogObserver logObserver("gamelog.txt");

    demonstrateInheritance();
    demonstrateCommands(processor.get(), &logObserver);
    demonstrateOrders(&logObserver);
    printLogFile("gamelog.txt");
    return 0;
}