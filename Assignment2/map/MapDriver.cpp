#include "Map.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

// Validate repository maps, then demonstrate rejection of malformed map files.
int main()
{
    // CMake supplies an absolute source path, so this also works from the build directory.
    const fs::path mapDirectory = fs::path(__FILE__).parent_path().parent_path() / "maps";
    std::error_code fsError;
    std::vector<fs::path> paths;
    fs::recursive_directory_iterator entry(mapDirectory, fsError);
    const fs::recursive_directory_iterator end;
    for (; !fsError && entry != end; entry.increment(fsError))
    {
        if (entry->is_regular_file(fsError) && entry->path().extension() == ".map")
        {
            paths.push_back(entry->path());
        }
    }
    if (fsError)
    {
        std::cerr << "Cannot scan maps directory: " << fsError.message() << '\n';
        return 1;
    }
    if (paths.empty())
    {
        std::cerr << "No .map files found in " << mapDirectory << '\n';
        return 1;
    }
    std::sort(paths.begin(), paths.end());

    MapLoader loader;
    std::size_t validCount = 0;
    for (const fs::path &path : paths)
    {
        Map map;
        std::string error;
        std::ostringstream diagnostics;
        std::streambuf *previousErrors = std::cerr.rdbuf(diagnostics.rdbuf());
        const bool loaded = loader.loadMap(path.string(), map, error);
        const bool valid = loaded && map.validate();
        std::cerr.rdbuf(previousErrors);

        std::cout << (valid ? "[VALID] " : "[INVALID] ")
                  << path.lexically_relative(mapDirectory).generic_string() << '\n';
        if (valid)
        {
            ++validCount;
            // validate() returns true only when all three required checks pass.
            std::cout << "  [PASS] The map is a connected graph.\n"
                      << "  [PASS] Continents are connected subgraphs.\n"
                      << "  [PASS] Each country belongs to one and only one continent.\n";
        }
        else if (!loaded)
        {
            std::cout << "  " << error << '\n';
        }
        else
        {
            std::cout << "  " << diagnostics.str();
        }
    }

    std::cout << "Result: " << validCount << '/' << paths.size() << " maps valid\n\n";

    // Keep intentionally invalid fixtures outside maps/ so normal discovery is unaffected.
    const fs::path tempRoot = fs::temp_directory_path(fsError);
    if (fsError)
    {
        std::cerr << "Cannot locate a temporary directory: " << fsError.message() << '\n';
        return 1;
    }
    const fs::path demoDirectory = tempRoot / ("comp345-invalid-maps-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if (!fs::create_directory(demoDirectory, fsError))
    {
        std::cerr << "Cannot create demo directory: " << demoDirectory << '\n';
        return 1;
    }

    const char *labels[] = {
        "Malformed text",
        "Disconnected map",
        "Connected map with a disconnected continent",
        "Country references an unknown continent",
        "Country repeated in two continents"
    };
    const char *contents[] = {
        "This is not a Domination map.\n",
        R"([continents]
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
)",
        R"([continents]
North 3 red
South 2 blue
[countries]
1 A 1 0 0
2 B 2 1 0
3 C 1 2 0
[borders]
1 2
2 1 3
3 2
)",
        R"([continents]
North 3 red
[countries]
1 A 2 0 0
[borders]
1
)",
        R"([continents]
North 3 red
South 2 blue
[countries]
1 A 1 0 0
1 A 2 0 0
[borders]
1
)"
    };
    const bool expectedLoad[] = {false, true, true, false, false};
    const char *expectedReasons[] = {
        "Missing [continents] records",
        "Map is not a connected graph.",
        "Continent 1 is not a connected subgraph.",
        "Continent with ID 2 does not exist.",
        "Territory with ID 1 already exists."
    };
    static_assert(std::size(labels) == std::size(contents));
    static_assert(std::size(labels) == std::size(expectedLoad));
    static_assert(std::size(labels) == std::size(expectedReasons));

    std::cout << "Malformed-map demonstrations (rejection is expected):\n";
    std::size_t demosPassed = 0;
    for (std::size_t index = 0; index < std::size(contents); ++index)
    {
        const fs::path path = demoDirectory / ("invalid-" + std::to_string(index) + ".map");
        std::ofstream file(path);
        file << contents[index];
        file.close();
        if (file.fail())
        {
            std::cerr << "[FAIL] Cannot write demo file: " << path << '\n';
            continue;
        }

        Map map;
        std::string error;
        std::ostringstream diagnostics;
        std::streambuf *previousErrors = std::cerr.rdbuf(diagnostics.rdbuf());
        const bool loaded = loader.loadMap(path.string(), map, error);
        const bool valid = loaded && map.validate();
        std::cerr.rdbuf(previousErrors);

        // Check the specific reason, not just that some unrelated failure occurred.
        const std::string reason = error + '\n' + diagnostics.str();
        const bool passed = loaded == expectedLoad[index] && !valid &&
            reason.find(expectedReasons[index]) != std::string::npos;
        demosPassed += static_cast<std::size_t>(passed);
        std::cout << (passed ? "[PASS] " : "[FAIL] ") << labels[index] << '\n'
                  << "  Parsing: " << (loaded ? "succeeded" : "rejected") << '\n'
                  << "  Map: " << (valid ? "accepted" : "rejected") << '\n';
        if (!error.empty())
        {
            std::cout << "  " << error << '\n';
        }
        if (!diagnostics.str().empty())
        {
            std::cout << "  " << diagnostics.str();
        }
    }

    // Remove only this run's temporary demo files.
    fs::remove_all(demoDirectory, fsError);
    if (fsError)
    {
        std::cerr << "Cannot remove demo files: " << fsError.message() << '\n';
        return 1;
    }
    std::cout << "Result: " << demosPassed << '/' << std::size(contents)
              << " malformed-map demos passed\n";
    return validCount == paths.size() && demosPassed == std::size(contents) ? 0 : 1;
}
