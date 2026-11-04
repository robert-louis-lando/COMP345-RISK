cmake_minimum_required(VERSION 3.16)
project(COMP345_RISK LANGUAGES CXX)

# Each component has its own executable because every driver defines main().
add_executable(MapDriver map/Map.cpp map/MapDriver.cpp map/Map.h)
add_executable(PlayerDriver Player/Player.cpp Player/PlayerDriver.cpp Player/Player.h)
add_executable(CardsDriver Cards/Cards.cpp Cards/CardsDriver.cpp Cards/Cards.h)
add_executable(OrdersDriver Orders/Orders.cpp Orders/OrdersDriver.cpp Orders/Orders.h)
add_executable(GameEngineDriver
    GameEngine/GameEngine.cpp
    GameEngine/GameEngineDrive.cpp
    GameEngine/GameEngine.h
    CommandProcessing/CommandProcessing.cpp
    CommandProcessing/CommandProcessing.h
)
target_include_directories(GameEngineDriver PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommandProcessing"
)

add_executable(CommandInputTests
    tests/CommandInputTests.cpp
    GameEngine/GameEngine.cpp
    CommandProcessing/CommandProcessing.cpp
)
target_include_directories(CommandInputTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommandProcessing"
)

foreach(driver IN ITEMS MapDriver PlayerDriver CardsDriver OrdersDriver GameEngineDriver CommandInputTests)
    set_target_properties(${driver} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${driver} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

enable_testing()
add_test(NAME map_driver COMMAND MapDriver)
add_test(NAME player_driver COMMAND PlayerDriver)
add_test(NAME cards_driver COMMAND CardsDriver)
add_test(NAME orders_driver COMMAND OrdersDriver)
add_test(NAME command_input COMMAND CommandInputTests)
# GameEngineDriver is interactive, so it must not run unattended through CTest.

# Build and run the map driver: cmake --build build --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:MapDriver>
    DEPENDS MapDriver
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    USES_TERMINAL
    COMMENT "Run MapDriver"
)
