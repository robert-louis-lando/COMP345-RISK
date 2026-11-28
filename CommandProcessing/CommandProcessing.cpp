#include "CommandProcessing.h"
#include "GameEngine.h"

//added this for std::find
#include <algorithm>


#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
using namespace std;


namespace
{
    // TOURNAMENT: Tournament limits given in the assignment description.
    const int MIN_MAPS = 1, MAX_MAPS = 5;
    const int MIN_PLAYERS = 2, MAX_PLAYERS = 4;
    const int MIN_GAMES = 1, MAX_GAMES = 5;
    const int MIN_TURNS = 10, MAX_TURNS = 50;

    // TOURNAMENT: Return true if the token is one of the four tournament flags.
    bool isTournamentFlag(const string &token)
    {
        return token == "-M" || token == "-P" || token == "-G" || token == "-D";
    }

    // TOURNAMENT: Convert a whole token to an int; fails on text such as "4x" or "".
    bool toInt(const string &token, int &value)
    {
        if (token.empty() || token.find_first_not_of("0123456789") != string::npos || token.size() > 6)
            return false;
        value = stoi(token);
        return true;
    }

    // TOURNAMENT: Map a strategy name (any case) to its display name; empty if not a computer strategy.
    string computerStrategyName(string strategy)
    {
        for (char &c : strategy)
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        if (strategy == "aggressive") return "Aggressive";
        if (strategy == "benevolent") return "Benevolent";
        if (strategy == "neutral") return "Neutral";
        if (strategy == "cheater") return "Cheater";
        return "";
    }
}

// TOURNAMENT: Parse "-M <maps> -P <strategies> -G <games> -D <turns>" and check every limit.
bool parseTournamentCommand(const string &argument, vector<string> &maps, vector<string> &strategies,
                            int &games, int &maxTurns, string &error)
{
    const string usage = "Use: tournament -M <maps> -P <strategies> -G <1-5> -D <10-50>";
    maps.clear();
    strategies.clear();
    games = 0;
    maxTurns = 0;
    error.clear();

    string spaced = argument;
    for (char &c : spaced)
        if (c == ',')
            c = ' ';

    istringstream stream(spaced);
    vector<string> tokens;
    string token;
    while (stream >> token)
        tokens.push_back(token);

    vector<string> mapTokens, playerTokens, gameTokens, turnTokens;
    bool seenM = false, seenP = false, seenG = false, seenD = false;
    vector<string> *current = nullptr;
    for (const string &t : tokens)
    {
        if (isTournamentFlag(t))
        {
            bool &seen = t == "-M" ? seenM : t == "-P" ? seenP : t == "-G" ? seenG : seenD;
            if (seen)
            {
                error = "Flag " + t + " given more than once. " + usage;
                return false;
            }
            seen = true;
            current = t == "-M" ? &mapTokens : t == "-P" ? &playerTokens : t == "-G" ? &gameTokens : &turnTokens;
        }
        else if (current == nullptr)
        {
            error = "Unexpected value '" + t + "' before any flag. " + usage;
            return false;
        }
        else
        {
            current->push_back(t);
        }
    }

    if (!seenM || !seenP || !seenG || !seenD)
    {
        error = "Missing one of -M, -P, -G, -D. " + usage;
        return false;
    }

    const int mapCount = static_cast<int>(mapTokens.size());
    if (mapCount < MIN_MAPS || mapCount > MAX_MAPS)
    {
        error = "Tournament needs 1 to 5 maps, got " + to_string(mapTokens.size());
        return false;
    }
    for (const string &map : mapTokens)
    {
        if (find(maps.begin(), maps.end(), map) != maps.end())
        {
            error = "Map listed more than once: " + map;
            return false;
        }
        maps.push_back(map);
    }

    const int playerCount = static_cast<int>(playerTokens.size());
    if (playerCount < MIN_PLAYERS || playerCount > MAX_PLAYERS)
    {
        error = "Tournament needs 2 to 4 player strategies, got " + to_string(playerTokens.size());
        return false;
    }
    for (const string &player : playerTokens)
    {
        string strategy = computerStrategyName(player);
        if (strategy.empty())
        {
            error = "Invalid tournament strategy '" + player + "' (allowed: Aggressive, Benevolent, Neutral, Cheater)";
            return false;
        }
        if (find(strategies.begin(), strategies.end(), strategy) != strategies.end())
        {
            error = "Strategy listed more than once: " + strategy;
            return false;
        }
        strategies.push_back(strategy);
    }

    if (gameTokens.size() != 1 || !toInt(gameTokens[0], games) || games < MIN_GAMES || games > MAX_GAMES)
    {
        error = "Number of games (-G) must be a single number from 1 to 5";
        return false;
    }

    if (turnTokens.size() != 1 || !toInt(turnTokens[0], maxTurns) || maxTurns < MIN_TURNS || maxTurns > MAX_TURNS)
    {
        error = "Maximum number of turns (-D) must be a single number from 10 to 50";
        return false;
    }
    return true;
}

// Construct an empty command processor.
CommandProcessor::CommandProcessor() = default;

// History copies own independent commands and effects.
CommandProcessor::CommandProcessor(const CommandProcessor &commandProcessor) : Subject(commandProcessor), ILoggable(commandProcessor)
{
    try
    {
        for (const Command *command : commandProcessor.commands)
        {
            unique_ptr<Command> copy(new Command(*command));
            commands.push_back(copy.get());
            copy.release();
        }
    }
    catch (...)
    {
        for (Command *command : commands)
            delete command;
        throw;
    }
}

// Replace the command history with an independent copy.
CommandProcessor &CommandProcessor::operator=(const CommandProcessor &commandProcessor)
{
    if (this != &commandProcessor)
    {
        Subject::operator=(commandProcessor); 
        ILoggable::operator=(commandProcessor);   


        CommandProcessor copy(commandProcessor);
        commands.swap(copy.commands);
    }
    return *this;
}

// Print the command processor's saved history.
std::ostream &operator<<(std::ostream &os, const CommandProcessor &commandProcessor)
{
    for (const Command *command : commandProcessor.commands)
        os << *command;
    return os;
}

// Delete all commands owned by the processor.
CommandProcessor::~CommandProcessor()
{
    for (Command *command : commands)
        delete command;
}

// Both console and file sources use this exact save/validate path.
Command *CommandProcessor::getCommand()
{
    unique_ptr<string> text(readCommand());
    if (!text)
        return nullptr;

    unique_ptr<Command> command(validate(*text));
    saveCommand(*command);

    if (!command->getEffect()->empty())   
    {  
        string rejection = *command->getEffect();  
        command->saveEffect(rejection);   
    }  


    return command.release();
}

// Read one whole line, including invalid commands, so rejection effects can be saved.
string *CommandProcessor::readCommand()
{
    string input;
    while (true)
    {
        cout << "Enter a command:" << endl;
        if (getline(cin, input))
            return new string(input);
        if (cin.bad())
            throw runtime_error("Failed to read console input");

        // Retain the requested interactive retry behavior after terminal EOF.
        cin.clear();
        cout << "Input ended. Please try again.\n";
    }
}

Command *CommandProcessor::validate(string &command)
{
    istringstream stream(command);
    string name;
    string argument;
    string extra;
    stream >> name;

    bool multiWordArgument = name == "addplayer" || name == "tournament";
    if (multiWordArgument)
    {
        // Keep the full player specification or tournament parameters for validation
        stream >> ws;
        getline(stream, argument);
    }
    else
    {
        stream >> argument;
    }

    string effect;
    
    if (!multiWordArgument && stream >> extra)
        effect = "Too many arguments for command: " + name;
    else
        GameEngine::getInstance().canExecute(name, argument, &effect);

    string emptyEffect;
    unique_ptr<Command> cmd(new Command(&command, &emptyEffect));
    cmd->saveEffect(effect);
    return cmd.release();
}

void CommandProcessor::saveCommand(Command &command)
{
    commands.push_back(&command);

    passObserversTo(command);  
    Notify(this);  
}

string CommandProcessor::stringToLog() const 
{  
    if (commands.empty())   
        return "Command: (none)";  
    return "Command: " + *commands.back()->getCommand();   
}   


// Construct an empty command.
Command::Command() : Command(nullptr, nullptr) {}

Command::Command(string *command, string *effect)
{
    unique_ptr<string> text(new string(command ? *command : ""));
    unique_ptr<string> outcome(new string(effect ? *effect : ""));
    this->command = text.release();
    this->effect = outcome.release();
}

// Deep-copy a command and its effect.
Command::Command(const Command &command) : Command(command.command, command.effect) {}

// Replace this command with an independent copy.
Command &Command::operator=(const Command &command)
{
    if (this != &command)
    {
        Command copy(command);
        std::swap(this->command, copy.command);
        std::swap(this->effect, copy.effect);
    }
    return *this;
}

// Release the command text and effect.
Command::~Command()
{
    delete command;
    delete effect;
}

// Print a command and its current effect.
std::ostream &operator<<(std::ostream &os, const Command &command)
{
    os << "Command: " << *command.command << ", Effect: " << *command.effect << '\n';
    return os;
}

// Return the stored command text.
string *Command::getCommand()
{
    return command;
}

// Return the stored command effect.
string *Command::getEffect()
{
    return effect;
}

// Store a command rejection or execution effect.
void Command::saveEffect(string &effect)
{
    *this->effect = effect;

    Notify(this);
}

string Command::stringToLog() const   
{  
    return "Command Effect (" + *command + "): " + *effect;   
}   

// Construct a file-backed command processor.
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const string &fileName)
    : fileLineReader(new FileLineReader(fileName)) {}

// Deep-copy a file-backed command processor and its reader position.
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter &other)
    : CommandProcessor(other), fileLineReader(new FileLineReader(*other.fileLineReader)) {}

// Replace the adapter history and reader with independent copies.
FileCommandProcessorAdapter &FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter &other)
{
    if (this != &other)
    {
        unique_ptr<FileLineReader> copy(new FileLineReader(*other.fileLineReader));
        CommandProcessor::operator=(other);
        delete fileLineReader;
        fileLineReader = copy.release();
    }
    return *this;
}

// Print the adapter history and reader status.
std::ostream &operator<<(std::ostream &os, const FileCommandProcessorAdapter &adapter)
{
    os << static_cast<const CommandProcessor &>(adapter)
       << "FileLineReader: " << *adapter.fileLineReader << '\n';
    return os;
}

// Release the file reader owned by the adapter.
FileCommandProcessorAdapter::~FileCommandProcessorAdapter()
{
    delete fileLineReader;
}

// Adapt FileLineReader to the base processor's private virtual input method.
// Read the next command line from the adapter's file.
string *FileCommandProcessorAdapter::readCommand()
{
    return fileLineReader->readLineFromFile();
}

// Construct an unopened file reader.
FileLineReader::FileLineReader() : fileName(nullptr), input(nullptr), position(0), exhausted(false)
{
    unique_ptr<string> name(new string());
    unique_ptr<ifstream> stream(new ifstream());
    fileName = name.release();
    input = stream.release();
}

// Construct and open a file reader.
FileLineReader::FileLineReader(const string &fileName) : FileLineReader()
{
    open(fileName);
}

// Streams are not copyable: reopen the file and seek to the saved byte position.
FileLineReader::FileLineReader(const FileLineReader &other) : FileLineReader()
{
    if (other.input->is_open())
    {
        open(*other.fileName);
        input->seekg(other.position);
        if (!*input)
            throw runtime_error("Cannot copy command-file reading position: " + *other.fileName);
    }
    position = other.position;
    exhausted = other.exhausted;
}

// Replace this reader with an independent copy.
FileLineReader &FileLineReader::operator=(const FileLineReader &other)
{
    if (this != &other)
    {
        FileLineReader copy(other);
        std::swap(fileName, copy.fileName);
        std::swap(input, copy.input);
        std::swap(position, copy.position);
        std::swap(exhausted, copy.exhausted);
    }
    return *this;
}

// Close the file and release reader storage.
FileLineReader::~FileLineReader()
{
    delete input; // ifstream closes the file automatically
    delete fileName;
}

// Commit a new file only after it opens successfully.
void FileLineReader::open(const string &name)
{
    if (name.empty())
        throw invalid_argument("Command filename cannot be empty");

    unique_ptr<string> newName(new string(name));
    unique_ptr<ifstream> newInput(new ifstream(name, ios::in | ios::binary));
    if (!newInput->is_open() || !*newInput)
        throw runtime_error("Cannot open command file: " + name);

    delete input;
    delete fileName;
    input = newInput.release();
    fileName = newName.release();
    position = 0;
    exhausted = false;
}

// Read the next line from the open command file.
string *FileLineReader::readLineFromFile()
{
    if (!input->is_open())
        throw runtime_error("No command file is open");
    if (exhausted)
        return nullptr;

    string line;
    if (!getline(*input, line))
    {
        if (input->bad() || !input->eof())
            throw runtime_error("Failed to read command file: " + *fileName);
        exhausted = true;
        return nullptr;
    }

    if (input->eof()) // the final line has no terminating newline
        exhausted = true;
    else
    {
        const auto next = input->tellg();
        if (next == std::streampos(-1))
            throw runtime_error("Cannot determine command-file position: " + *fileName);
        position = static_cast<std::streamoff>(next);
    }
    if (!line.empty() && line.back() == '\r')
        line.pop_back(); // accept CRLF files on every platform
    return new string(line);
}

// Open the requested file when needed, then read its next line.
string *FileLineReader::readLineFromFile(string *name)
{
    if (name == nullptr)
        throw invalid_argument("Command filename cannot be null");
    if (!input->is_open() || *fileName != *name)
        open(*name);
    return readLineFromFile();
}

// Print the reader's file and current position.
std::ostream &operator<<(std::ostream &os, const FileLineReader &reader)
{
    os << *reader.fileName << " (offset " << reader.position
       << (reader.exhausted ? ", EOF)" : ")");
    return os;
}
