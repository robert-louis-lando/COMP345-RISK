cmake_minimum_required(VERSION 3.16)
project(COMP345_RISK LANGUAGES CXX)

add_executable(MapDriver map/Map.cpp map/MapDriver.cpp map/Map.h)
set_target_properties(MapDriver PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(MapDriver PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME map_driver COMMAND MapDriver)

# Build and run in one command: cmake --build build-gcc --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:MapDriver>
    DEPENDS MapDriver
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    USES_TERMINAL
    COMMENT "Run MapDriver"
)
