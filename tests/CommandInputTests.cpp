#include "CommandProcessing.h"
#include "GameEngine.h"
#include <chrono>
#include <filesystem>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace {
    int checks = 0;
    void expect(bool condition, const char* message){
        if(!condition)
            throw std::runtime_error(message);
        ++checks;
    }

    void writeFile(const std::filesystem::path& path, const std::string& text){
        std::ofstream file(path, std::ios::binary);
        file << text;
        expect(static_cast<bool>(file), "Could not write test input");
    }

    void expectLine(FileLineReader& reader, const std::string& expected){
        std::unique_ptr<std::string> line(reader.readLineFromFile());
        expect(line && *line == expected, "Wrong file line or unexpected EOF");
    }

    void testReader(const std::filesystem::path& directory){
        const auto file = directory / "reader.txt";
        const auto empty = directory / "empty.txt";
        writeFile(file, "loadmap world.map\r\n\r\nunknown command\nvalidatemap");
        writeFile(empty, "");
        FileLineReader reader(file.string());
        expectLine(reader, "loadmap world.map");
        FileLineReader copy(reader);
        expectLine(reader, "");
        expectLine(copy, "");
        FileLineReader assigned(empty.string());
        assigned = copy;
        expectLine(assigned, "unknown command");
        expectLine(copy, "unknown command");
        expectLine(reader, "unknown command");
        expectLine(reader, "validatemap");
        expect(reader.readLineFromFile() == nullptr, "EOF did not return nullptr");
        expect(reader.readLineFromFile() == nullptr, "Repeated EOF restarted the file");
        FileLineReader endCopy(reader);
        expect(endCopy.readLineFromFile() == nullptr, "Copied exhausted reader restarted");
        assigned = reader;
        expect(assigned.readLineFromFile() == nullptr, "Assigned exhausted reader restarted");

        FileLineReader compatibility;
        std::string name = file.string();
        std::unique_ptr<std::string> first(compatibility.readLineFromFile(&name));
        std::unique_ptr<std::string> second(compatibility.readLineFromFile(&name));
        expect(first && *first == "loadmap world.map", "Filename overload failed to open");
        expect(second && second->empty(), "Filename overload reopened the same file");
        std::string missing = (directory / "missing.txt").string();
        bool threw = false;
        try { std::unique_ptr<std::string> ignored(compatibility.readLineFromFile(&missing)); }
        catch(const std::runtime_error&) { threw = true; }
        expect(threw, "Missing file accepted");
        expectLine(compatibility, "unknown command");
        name = empty.string();
        expect(compatibility.readLineFromFile(&name) == nullptr, "Empty file did not reach EOF");
        threw = false;
        try { FileLineReader invalid(missing); }
        catch(const std::runtime_error&) { threw = true; }
        expect(threw, "Missing-file constructor succeeded");
        threw = false;
        try { std::unique_ptr<std::string> ignored(compatibility.readLineFromFile(nullptr)); }
        catch(const std::invalid_argument&) { threw = true; }
        expect(threw, "Null filename accepted");
        std::ostringstream output;
        output << reader;
        expect(output.str().find("EOF") != std::string::npos, "Reader output missing EOF status");
    }

    void testAdapter(const std::filesystem::path& directory){
        const auto file = directory / "commands.txt";
        const auto empty = directory / "empty.txt";
        writeFile(file, "unknown\nloadmap\nloadmap world.map\nvalidatemap\naddplayer Alice\n"
                        "gamestart\nissueorder\nendissueorders\nwin\nreplay\n");
        FileCommandProcessorAdapter adapter(file.string());
        CommandProcessor& base = adapter;
        GameEngine& engine = GameEngine::getInstance();
        Command* unknown = base.getCommand();
        expect(unknown && *unknown->getCommand() == "unknown", "Adapter skipped rejected input");
        expect(*unknown->getEffect() == "Unknown command: unknown", "Missing rejection effect");
        Command* missing = base.getCommand();
        expect(missing && *missing->getEffect() == "Missing map filename", "Missing-argument line skipped");

        FileCommandProcessorAdapter copied(adapter);
        FileCommandProcessorAdapter assigned(empty.string());
        assigned = adapter;
        Command* fromCopy = copied.getCommand();
        Command* fromAssignment = assigned.getCommand();
        Command* original = base.getCommand();
        expect(fromCopy && fromAssignment && original, "Copied adapter lost reading position");
        expect(*fromCopy->getCommand() == *original->getCommand(), "Copied adapter read different line");
        expect(*fromAssignment->getCommand() == *original->getCommand(), "Assigned adapter read different line");
        expect(fromCopy != original && fromAssignment != original, "Adapters shared command ownership");
        expect(original->getEffect()->empty(), "Valid command was marked executed before execution");
        engine.executeCommand(*original);
        expect(*engine.getCurrentState() == "MapLoaded", "Saved adapter command did not execute");
        expect(!original->getEffect()->empty(), "Execution effect was not saved");
        expect(fromCopy->getEffect()->empty(), "Copies shared effect storage");
        const char* states[] = {"MapValidated", "PlayersAdded", "AssignReinforcement", "IssueOrders",
                                "ExecuteOrders", "Win", "Start"};
        for(const char* state : states){
            Command* command = base.getCommand();
            expect(command && command->getEffect()->empty(), "File command unexpectedly rejected");
            engine.executeCommand(*command);
            expect(*engine.getCurrentState() == state, "Sequential command executed in wrong state");
        }
        expect(base.getCommand() == nullptr, "Adapter did not stop at file EOF");
        expect(base.getCommand() == nullptr, "Adapter restarted after EOF");
        std::ostringstream history;
        history << base;
        expect(history.str().find("State: Start -> MapLoaded") != std::string::npos, "History lost execution effect");
        expect(history.str().find("Unknown command: unknown") != std::string::npos, "History lost rejection");
        std::string changed = "Changed original history";
        unknown->saveEffect(changed);
        std::ostringstream copiedHistory;
        copiedHistory << copied;
        expect(copiedHistory.str().find(changed) == std::string::npos, "History copies shared commands");
        const FileCommandProcessorAdapter& same = assigned;
        assigned = same;
        std::unique_ptr<CommandProcessor> polymorphic(new FileCommandProcessorAdapter(empty.string()));
        expect(polymorphic->getCommand() == nullptr, "Polymorphic adapter ignored file EOF");
    }

    void testConsoleAndQuit(const std::filesystem::path& directory){
        CommandProcessor console;
        std::istringstream input("unknown\nloadmap world.map\n");
        std::streambuf* original = std::cin.rdbuf(input.rdbuf());
        try{
            Command* rejected = console.getCommand();
            expect(rejected && !rejected->getEffect()->empty(), "Console rejection skipped history");
            Command* valid = console.getCommand();
            expect(valid && valid->getEffect()->empty(), "Console and file validation differ");
            GameEngine::getInstance().executeCommand(*valid);
        }
        catch(...){
            std::cin.rdbuf(original);
            throw;
        }
        std::cin.rdbuf(original);

        const auto file = directory / "quit.txt";
        writeFile(file, "quit\n");
        FileCommandProcessorAdapter adapter(file.string());
        GameEngine& engine = GameEngine::getInstance();
        engine.setState(new Win());
        Command* quit = adapter.getCommand();
        expect(quit && quit->getEffect()->empty(), "Quit validation failed");
        engine.executeCommand(*quit);
        expect(!engine.isRunning(), "Quit did not stop the loop");
        expect(*quit->getEffect() == "Game ended", "Quit effect lost before cleanup");
    }
}

int main(){
    const auto directory = std::filesystem::temp_directory_path() /
        ("risk-command-input-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try{
        std::filesystem::create_directories(directory);
        testReader(directory);
        testAdapter(directory);
        testConsoleAndQuit(directory);
        std::filesystem::remove_all(directory);
        std::cout << "Passed " << checks << " command-input checks\n";
        return 0;
    }
    catch(const std::exception& error){
        std::filesystem::remove_all(directory);
        std::cerr << error.what() << '\n';
        return 1;
    }
}
