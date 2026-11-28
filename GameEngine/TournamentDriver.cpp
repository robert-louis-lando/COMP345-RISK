
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "GameEngine.h"
#include "LoggingObserver.h"
using namespace std;

// reads tournament commands from the console or a file, executes them and logs the results to gamelog.txt
void testTournament(CommandProcessor *processor)
{
    GameEngine &engine = GameEngine::getInstance();
    LogObserver logObserver("gamelog.txt");
    engine.attach(&logObserver);
    processor->attach(&logObserver);

    cout << "=== Tournament mode driver ===\n"
         << "Command: tournament -M <maps> -P <strategies> -G <1-5> -D <10-50>\n"
         << "Example: tournament -M maps/solar/solar.map maps/google/google.map "
         << "-P Aggressive Benevolent Neutral Cheater -G 3 -D 20\n"
         << "Strategies: Aggressive, Benevolent, Neutral, Cheater. Type quit after the tournament.\n";

    while (engine.isRunning())
    {

        //getCommand validates commands, if its invalid it throws an error
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
            catch (const std::exception &)
            {
            }
        }
        cout << *command;
    }

    engine.detach(&logObserver);
    processor->detach(&logObserver);
    cout << "\nResults and every command, order and state change were written to gamelog.txt\n";
}

//picks the command source from the command line: -console (default) or -file <filename>
int main(int argc, char *argv[])
{
    try
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
            cerr << "Usage: " << argv[0] << " [-console | -file <filename>]\n";
            return 2;
        }
        testTournament(processor.get());
        return 0;
    }
    catch (const std::exception &error)
    {
        cerr << error.what() << '\n';
        return 1;
    }
}