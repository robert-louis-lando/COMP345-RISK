# COMP345-RISK

## Build and test the map component

Run commands from the repository root. You need CMake 3.16+ and a C++17 compiler. `ctest` runs every driver registered in `CMakeLists.txt`; currently that is `MapDriver`.

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

## Adding another component's driver

Put your source files in your component directory and add an executable and test to the root `CMakeLists.txt`. For example, if the Player files live in `player/`:

```cmake
add_executable(PlayerDriver player/Player.cpp player/PlayerDriver.cpp)
set_target_properties(PlayerDriver PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
add_test(NAME player_driver COMMAND PlayerDriver)
```

Include any other `.cpp` files your driver needs in `add_executable` (for example, `map/Map.cpp` if it calls map functions). Each driver needs its own executable because each has its own `main()`; do not add multiple driver files to one executable. Give each test a unique name.

After editing `CMakeLists.txt`, configure and build again, then test your driver:

| macOS (Terminal) | Windows (PowerShell / Visual Studio) |
| --- | --- |
| `cmake -S . -B build` | `cmake -S . -B build` |
| `cmake --build build --target PlayerDriver` | `cmake --build build --config Debug --target PlayerDriver` |
| `ctest --test-dir build -R '^player_driver$' --output-on-failure` | `ctest --test-dir build -C Debug -R '^player_driver$' --output-on-failure` |
| `./build/PlayerDriver` | `& .\build\Debug\PlayerDriver.exe` |

Replace `PlayerDriver` and `player_driver` with your executable and test names. To run *all* registered driver tests, build all targets first and use the platform's full `ctest` command above.
