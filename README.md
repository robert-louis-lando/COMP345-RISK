# COMP345-RISK

## Build and test the components

Run commands from the repository root (`COMP345-RISK`). You need CMake 3.16+ and a C++17 compiler. CMake builds all five drivers. `ctest` runs `MapDriver`, `PlayerDriver`, `CardsDriver`, `OrdersDriver`, and the command-input unit tests. Run `GameEngineDriver` manually with console or file input.

### macOS (Terminal, default Makefiles generator)

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the map driver directly with `./build/MapDriver`, or build and run it with `cmake --build build --target run`. To run only its test:

```sh
ctest --test-dir build -R '^map_driver$' --output-on-failure
```

### Windows (PowerShell, Visual Studio generator)

Install CMake and Visual Studio with the **Desktop development with C++** workload. From PowerShell:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Run the map driver directly with `& .\build\Debug\MapDriver.exe`, or build and run it with `cmake --build build --config Debug --target run`. To run only its test:

```powershell
ctest --test-dir build -C Debug -R '^map_driver$' --output-on-failure
```

Visual Studio places executables under `build/Debug/` (or `build/Release/` if you use `--config Release` and `-C Release`). On macOS with the Xcode generator, also use `--config Debug` and `-C Debug`, and find the executable under `build/Debug/`.

## Run the other drivers

After building, run any driver directly:

| Driver | macOS (Terminal) | Windows (PowerShell / Visual Studio) |
| --- | --- | --- |
| Player | `./build/PlayerDriver` | `& .\build\Debug\PlayerDriver.exe` |
| Cards | `./build/CardsDriver` | `& .\build\Debug\CardsDriver.exe` |
| Orders | `./build/OrdersDriver` | `& .\build\Debug\OrdersDriver.exe` |
| Game Engine | `./build/GameEngineDriver` | `& .\build\Debug\GameEngineDriver.exe` |

The Game Engine defaults to console input. Select the input source explicitly with:

```sh
./build/GameEngineDriver -console
./build/GameEngineDriver -file commands.txt
```

A command file contains one command per line, processed in order through `FileCommandProcessorAdapter` and `FileLineReader`. File EOF ends the program cleanly; rejected commands are recorded with an error effect and processing continues. Quote command-file paths containing spaces. Unknown flags or missing filenames print usage. In console mode, press **Ctrl+C** to stop immediately; `quit` exits normally when the engine is in the `Win` state.

The engine still performs state transitions, not the full map/player gameplay actions.

To build only one driver, use its target name, for example:

```sh
cmake --build build --target PlayerDriver
```

For Visual Studio, also add `--config Debug`. To run one non-interactive driver's test, use its test name: `map_driver`, `player_driver`, `cards_driver`, `orders_driver`, or `command_input`:

```sh
ctest --test-dir build -R '^player_driver$' --output-on-failure
```

For Visual Studio, also add `-C Debug`.

Each driver is a separate executable because each defines its own `main()`. New drivers must be added to `CMakeLists.txt` with their own executable target; only non-interactive drivers should be registered as unattended tests.
