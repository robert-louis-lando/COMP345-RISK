/** @file CommandProcessing.h
 *  @brief Console/file command input, validation, history, and effect storage.
 */
#ifndef COMMAND_PROCESSING_H
#define COMMAND_PROCESSING_H

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class Command;
class FileLineReader;

/** @brief Reads and validates commands and owns their history.
 *  Execution belongs to the game engine, which updates the saved Command's effect.
 *  Derived adapters change only the input source through readCommand().
 */
class CommandProcessor
{
public:
    /** @brief User command names specified for the startup and game-control interface. */
    inline static constexpr std::string_view VALID_COMMANDS[] = {"loadmap", "validatemap", "addplayer", "gamestart", "replay", "quit"};
    /** @brief Commands requiring a map filename or player name argument. */
    inline static constexpr std::string_view NEEDS_ARG[] = {"loadmap", "addplayer"};

    /** @brief Construct a console processor with an empty command history. */
    CommandProcessor();
    /** @brief Deep-copy the history, including every command and its effect. */
    CommandProcessor(const CommandProcessor &commandProcessor);
    /** @brief Replace the history with an independent copy; supports self-assignment. */
    CommandProcessor &operator=(const CommandProcessor &commandProcessor);
    /** @brief Print all saved commands and their effects in input order. */
    friend std::ostream &operator<<(std::ostream &os, const CommandProcessor &commandProcessor);
    /** @brief Delete all saved commands; supports deletion through a base pointer. */
    virtual ~CommandProcessor();

    /** @brief Read, validate, and save the next command, including rejected commands.
     *  @return A borrowed pointer owned by this processor, or nullptr at source EOF.
     *  @note Do not delete the result. Its effect is empty when validation succeeds;
     *        otherwise it contains a rejection error. No game action is executed here.
     */
    Command *getCommand();
    /** @brief Check command syntax and whether the current engine state accepts it.
     */
    Command *validate(string &command);

private:
    /** @brief Read one complete console line, leaving validation to getCommand().
     *  @return A newly allocated string owned by the caller.
     *  @throws std::runtime_error If the console stream has a nonrecoverable read error.
     *  @note Console EOF clears the stream and prompts again. Adapters override this
     *        private virtual method to replace input without changing history handling.
     */
    virtual string *readCommand();
    /** @brief Append the same Command object to history and take ownership of it.
     *  @param command A heap-allocated command whose ownership is transferred here.
     *  @note Do not pass a stack object or save the same object more than once.
     */
    void saveCommand(Command &command);
    /** @brief Owns every Command pointer; copies of the processor deep-copy the entries. */
    vector<Command *> commands;
};

/** @brief Owns command text and its rejection error or execution outcome.
 *  A valid command has an empty effect until the engine records its execution result.
 */
class Command
{
public:
    /** @brief Construct a command with empty text and an empty effect. */
    Command();
    /** @brief Copy supplied strings into independent owned storage.
     *  @param command Command text, or nullptr for empty text.
     *  @param effect Initial effect, or nullptr for an empty effect.
     *  @note The caller retains ownership of both input pointers.
     */
    Command(string *command, string *effect);
    /** @brief Deep-copy command text and effect without sharing storage. */
    Command(const Command &command);
    /** @brief Replace text and effect with independent copies; supports self-assignment. */
    Command &operator=(const Command &command);
    /** @brief Print the command text and its current effect. */
    friend std::ostream &operator<<(std::ostream &os, const Command &command);
    /** @brief Delete the owned command and effect strings. */
    ~Command();

    /** @brief Return the borrowed command-text pointer; the caller must not delete it. */
    string *getCommand();
    /** @brief Return the borrowed effect pointer; the caller must not delete it. */
    string *getEffect();
    /** @brief Copy a rejection error or execution outcome into the owned effect string. */
    void saveEffect(string &effect);

private:
    /** @brief Owned command text. */
    string *command;
    /** @brief Owned effect; empty while a valid command awaits execution. */
    string *effect;
};

/** @brief Adapts sequential file lines to the CommandProcessor interface.
 *  Inherits the console processor's validation and history behavior, but obtains
 *  input through an owned FileLineReader instead of std::cin.
 */
class FileCommandProcessorAdapter : public CommandProcessor
{
public:
    /** @brief Open the command file and start with an empty history.
     *  @throws std::invalid_argument If fileName is empty.
     *  @throws std::runtime_error If the file cannot be opened.
     */
    explicit FileCommandProcessorAdapter(const string &fileName);
    /** @brief Deep-copy history and create an independent reader at the same position. */
    FileCommandProcessorAdapter(const FileCommandProcessorAdapter &fileCommandProcessorAdapter);
    /** @brief Replace history and reader with independent copies; supports self-assignment. */
    FileCommandProcessorAdapter &operator=(const FileCommandProcessorAdapter &fileCommandProcessorAdapter);
    /** @brief Print command history followed by the file reader's status. */
    friend std::ostream &operator<<(std::ostream &os, const FileCommandProcessorAdapter &fileCommandProcessorAdapter);
    /** @brief Delete the reader; the base destructor deletes command history. */
    ~FileCommandProcessorAdapter() override;

private:
    /** @brief Return the next allocated file line, or nullptr at EOF, without prompting. */
    string *readCommand() override;
    /** @brief Owned reader; its stream is never shared with adapter copies. */
    FileLineReader *fileLineReader;
};

/** @brief Owns a command file and reads its lines sequentially.
 *  Copies reopen the same file at the saved position and advance independently.
 *  Blank lines are returned as empty strings; EOF is represented by nullptr.
 */
class FileLineReader
{
public:
    /** @brief Construct a reader without an open file. */
    FileLineReader();
    /** @brief Open a command file for sequential reading from its beginning.
     */
    explicit FileLineReader(const string &fileName);
    /** @brief Reopen the source file independently, preserving its position and EOF status.
     *  @throws std::runtime_error If reopening or seeking fails.
     */
    FileLineReader(const FileLineReader &fileLineReader);
    /** @brief Replace this reader with an independent copy; supports self-assignment. */
    FileLineReader &operator=(const FileLineReader &fileLineReader);
    /** @brief Print the filename, saved byte offset, and EOF status. */
    friend std::ostream &operator<<(std::ostream &os, const FileLineReader &fileLineReader);
    /** @brief Close the file and release the owned stream and filename. */
    ~FileLineReader();

    /** @brief Read the next line, accepting LF and CRLF line endings.
     */
    string *readLineFromFile();
    /** @brief Open a different file if needed, then read its next line.
     */
    string *readLineFromFile(string *fileName);

private:
    /** @brief Open a file and reset position/EOF, preserving the old reader if opening fails. */
    void open(const string &fileName);
    /** @brief Owned copy of the current file's path. */
    string *fileName;
    /** @brief Owned binary input stream, closed automatically when deleted. */
    ifstream *input;
    /** @brief Saved seek offset used to reproduce this reader's position when copied. */
    std::streamoff position;
    /** @brief Whether the last line has been consumed or EOF has been encountered. */
    bool exhausted;
};

#endif
