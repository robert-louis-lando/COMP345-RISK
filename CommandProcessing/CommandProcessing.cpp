#include "CommandProcessing.h"
#include "GameEngine.h"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
using namespace std;

CommandProcessor::CommandProcessor() = default;

// History copies own independent commands and effects.
CommandProcessor::CommandProcessor(const CommandProcessor &commandProcessor)
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

CommandProcessor &CommandProcessor::operator=(const CommandProcessor &commandProcessor)
{
    if (this != &commandProcessor)
    {
        CommandProcessor copy(commandProcessor);
        commands.swap(copy.commands);
    }
    return *this;
}

std::ostream &operator<<(std::ostream &os, const CommandProcessor &commandProcessor)
{
    for (const Command *command : commandProcessor.commands)
        os << *command;
    return os;
}

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
    stream >> name >> argument;

    string effect;
    if (stream >> extra)
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
}

Command::Command() : Command(nullptr, nullptr) {}

Command::Command(string *command, string *effect)
{
    unique_ptr<string> text(new string(command ? *command : ""));
    unique_ptr<string> outcome(new string(effect ? *effect : ""));
    this->command = text.release();
    this->effect = outcome.release();
}

Command::Command(const Command &command) : Command(command.command, command.effect) {}

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

Command::~Command()
{
    delete command;
    delete effect;
}

std::ostream &operator<<(std::ostream &os, const Command &command)
{
    os << "Command: " << *command.command << ", Effect: " << *command.effect << '\n';
    return os;
}

string *Command::getCommand()
{
    return command;
}

string *Command::getEffect()
{
    return effect;
}

void Command::saveEffect(string &effect)
{
    *this->effect = effect;
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const string &fileName)
    : fileLineReader(new FileLineReader(fileName)) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter &other)
    : CommandProcessor(other), fileLineReader(new FileLineReader(*other.fileLineReader)) {}

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

std::ostream &operator<<(std::ostream &os, const FileCommandProcessorAdapter &adapter)
{
    os << static_cast<const CommandProcessor &>(adapter)
       << "FileLineReader: " << *adapter.fileLineReader << '\n';
    return os;
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter()
{
    delete fileLineReader;
}

// Adapt FileLineReader to the base processor's private virtual input method.
string *FileCommandProcessorAdapter::readCommand()
{
    return fileLineReader->readLineFromFile();
}

FileLineReader::FileLineReader() : fileName(nullptr), input(nullptr), position(0), exhausted(false)
{
    unique_ptr<string> name(new string());
    unique_ptr<ifstream> stream(new ifstream());
    fileName = name.release();
    input = stream.release();
}

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

string *FileLineReader::readLineFromFile(string *name)
{
    if (name == nullptr)
        throw invalid_argument("Command filename cannot be null");
    if (!input->is_open() || *fileName != *name)
        open(*name);
    return readLineFromFile();
}

std::ostream &operator<<(std::ostream &os, const FileLineReader &reader)
{
    os << *reader.fileName << " (offset " << reader.position
       << (reader.exhausted ? ", EOF)" : ")");
    return os;
}
