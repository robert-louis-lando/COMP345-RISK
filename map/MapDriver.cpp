#include "Map.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

// Write a small Domination-format map for a self-contained demonstration.
bool writeFixture(const fs::path &path, const std::string &contents)
{
    std::ofstream file(path);
    file << contents;
    return file.good();
}

// Show parsing separately from graph validation and check the expected verdict.
bool demonstrate(const std::string &label, const fs::path &path, bool expectedLoad, bool expectedValid)
{
    MapLoader loader;
    Map map;
    std::string error;
    std::ostringstream diagnostics;
    std::streambuf *previousErrors = std::cerr.rdbuf(diagnostics.rdbuf());
    const bool loaded = loader.loadMap(path.string(), map, error);
    const bool valid = loaded && map.validate();
    std::cerr.rdbuf(previousErrors);

    const bool passed = loaded == expectedLoad && (!loaded || valid == expectedValid);
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << label << '\n';
    std::cout << "  Load: " << (loaded ? "accepted" : "rejected") << '\n';
    if (!loaded)
    {
        std::cout << "  Reason: " << error << '\n';
    }
    else
    {
        std::cout << "  Validation: " << (valid ? "valid" : "invalid") << '\n';
        if (!valid)
        {
            std::cout << "  Reason: " << diagnostics.str();
        }
        else
        {
            std::cout << "  Map contents:\n" << map;
        }
    }
    std::cout << '\n';
    return passed;
}

// Generate representative files, including globally and continent-disconnected maps.
int main()
{
    std::error_code fsError;
    const fs::path tempRoot = fs::temp_directory_path(fsError);
    if (fsError)
    {
        std::cerr << "Cannot locate a temporary directory: " << fsError.message() << '\n';
        return 1;
    }

    const fs::path dir = tempRoot / ("comp345-map-driver-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if (!fs::create_directory(dir, fsError))
    {
        std::cerr << "Cannot create temporary directory: " << dir << " (" << fsError.message() << ")\n";
        return 1;
    }

    const fs::path validPath = dir / "valid.map";
    const fs::path malformedPath = dir / "malformed.map";
    const fs::path disconnectedPath = dir / "disconnected.map";
    const fs::path continentDisconnectedPath = dir / "continent-disconnected.map";
    const fs::path zeroBonusPath = dir / "zero-bonus.map";
    const fs::path negativeBonusPath = dir / "negative-bonus.map";

    const bool written =
        writeFixture(validPath, R"([continents]
North 3 red
South 2 blue
[countries]
1 A 1 0 0
2 B 1 1 0
3 C 2 2 0
[borders]
1 2 3
2 1
3 1
)") &&
        writeFixture(malformedPath, "This is not a Domination map.\n") &&
        writeFixture(disconnectedPath, R"([continents]
North 3 red
South 2 blue
[countries]
1 A 1 0 0
2 B 1 1 0
3 C 2 2 0
[borders]
1 2
2 1
3
)") &&
        writeFixture(continentDisconnectedPath, R"([continents]
A 3 red
B 2 blue
[countries]
1 A1 1 0 0
2 MiddleB 2 1 0
3 A2 1 2 0
[borders]
1 2
2 1 3
3 2
)") &&
        writeFixture(zeroBonusPath, R"([continents]
North 0 red
[countries]
1 A 1 0 0
[borders]
1
)") &&
        writeFixture(negativeBonusPath, R"([continents]
North -3 red
[countries]
1 A 1 0 0
[borders]
1
)");

    bool passed = written;
    if (written)
    {
        const bool validPassed = demonstrate("Connected map", validPath, true, true);
        const bool malformedPassed = demonstrate("Malformed text", malformedPath, false, false);
        const bool disconnectedPassed = demonstrate("Disconnected map", disconnectedPath, true, false);
        const bool continentDisconnectedPassed = demonstrate("Disconnected continent", continentDisconnectedPath, true, false);
        const bool zeroBonusPassed = demonstrate("Zero bonus", zeroBonusPath, false, false);
        const bool negativeBonusPassed = demonstrate("Negative bonus", negativeBonusPath, false, false);
        Map directMap;
        std::ostringstream directDiagnostics;
        std::streambuf *previousErrors = std::cerr.rdbuf(directDiagnostics.rdbuf());
        const bool zeroRejected = directMap.CreateContinent("Zero", 1, 0, "red") == nullptr;
        const bool negativeRejected = directMap.CreateContinent("Negative", 1, -1, "red") == nullptr;
        std::cerr.rdbuf(previousErrors);
        const bool directBonusPassed = zeroRejected && negativeRejected;
        std::cout << (directBonusPassed ? "[PASS] " : "[FAIL] ") << "Direct continent creation\n"
                  << "  Zero bonus: " << (zeroRejected ? "rejected" : "accepted") << '\n'
                  << "  Negative bonus: " << (negativeRejected ? "rejected" : "accepted") << "\n\n";

        const int passedCount = static_cast<int>(validPassed) + static_cast<int>(malformedPassed) +
                                static_cast<int>(disconnectedPassed) + static_cast<int>(continentDisconnectedPassed) +
                                static_cast<int>(zeroBonusPassed) + static_cast<int>(negativeBonusPassed) +
                                static_cast<int>(directBonusPassed);
        std::cout << "Result: " << passedCount << "/7 cases passed\n";
        passed = passedCount == 7;
    }
    else
    {
        std::cerr << "Could not write all demonstration map files.\n";
    }

    fs::remove_all(dir, fsError); // dir was uniquely created by this run.
    if (fsError)
    {
        std::cerr << "Could not remove temporary files: " << fsError.message() << '\n';
        return 1;
    }
    return passed ? 0 : 1;
}
