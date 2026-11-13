cmake_minimum_required(VERSION 3.16)
project(COMP345_RISK LANGUAGES CXX)

# Each component has its own executable because every driver defines main().
add_executable(MapDriver map/Map.cpp map/MapDriver.cpp map/Map.h)
add_executable(PlayerDriver
    Player/Player.cpp Player/PlayerDriver.cpp Player/Player.h
    Orders/Orders.cpp Orders/Orders.h Cards/Cards.cpp Cards/Cards.h
    map/Map.cpp map/Map.h
    LoggingObserver/LoggingObserver.cpp LoggingObserver/LoggingObserver.h) 
target_include_directories(PlayerDriver PRIVATE Player Orders Cards map)
add_executable(CardsDriver Cards/Cards.cpp Cards/CardsDriver.cpp Cards/Cards.h)
add_executable(OrdersDriver
    Orders/Orders.cpp Orders/OrdersDriver.cpp Orders/Orders.h
    Player/Player.cpp Player/Player.h Cards/Cards.cpp Cards/Cards.h
    map/Map.cpp map/Map.h
    LoggingObserver/LoggingObserver.cpp LoggingObserver/LoggingObserver.h)
target_include_directories(OrdersDriver PRIVATE Orders Player Cards map)
add_executable(OrderDriver2
    Orders/Orders.cpp Orders/OrderDriver2.cpp Orders/Orders.h
    Player/Player.cpp Player/Player.h Cards/Cards.cpp Cards/Cards.h
    map/Map.cpp map/Map.h GameEngine/GameEngine.cpp GameEngine/GameEngine.h
    CommandProcessing/CommandProcessing.cpp CommandProcessing/CommandProcessing.h
    LoggingObserver/LoggingObserver.cpp LoggingObserver/LoggingObserver.h)
target_include_directories(OrderDriver2 PRIVATE Orders Player Cards map GameEngine CommandProcessing)
add_executable(GameEngineDriver
    GameEngine/GameEngine.cpp
    GameEngine/GameEngineDrive.cpp
    GameEngine/GameEngine.h
    CommandProcessing/CommandProcessing.cpp
    CommandProcessing/CommandProcessing.h
    Cards/Cards.cpp
    Cards/Cards.h
    map/Map.cpp
    map/Map.h
    Player/Player.cpp
    Player/Player.h
    Orders/Orders.cpp
    Orders/Orders.h
    LoggingObserver/LoggingObserver.cpp  
    LoggingObserver/LoggingObserver.h  
)
target_include_directories(GameEngineDriver PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommandProcessing"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cards"
    "${CMAKE_CURRENT_SOURCE_DIR}/map"
    "${CMAKE_CURRENT_SOURCE_DIR}/Player"
)

add_executable(GameEngineDriverPart2
    GameEngine/GameEngine.cpp
    GameEngine/GameEngineDriverPart2.cpp
    GameEngine/GameEngine.h
    CommandProcessing/CommandProcessing.cpp
    CommandProcessing/CommandProcessing.h
    Cards/Cards.cpp
    Cards/Cards.h
    map/Map.cpp
    map/Map.h
    Player/Player.cpp
    Player/Player.h
    Orders/Orders.cpp
    Orders/Orders.h
    LoggingObserver/LoggingObserver.cpp   
    LoggingObserver/LoggingObserver.h  
)
target_include_directories(GameEngineDriverPart2 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommandProcessing"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cards"
    "${CMAKE_CURRENT_SOURCE_DIR}/map"
    "${CMAKE_CURRENT_SOURCE_DIR}/Player"
)

add_executable(CommandInputTests
    tests/CommandInputTests.cpp
    GameEngine/GameEngine.cpp
    CommandProcessing/CommandProcessing.cpp
    Cards/Cards.cpp
    map/Map.cpp
    Player/Player.cpp
    Orders/Orders.cpp
    LoggingObserver/LoggingObserver.cpp
)
target_include_directories(CommandInputTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommandProcessing"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cards"
    "${CMAKE_CURRENT_SOURCE_DIR}/map"
    "${CMAKE_CURRENT_SOURCE_DIR}/Player"
)

add_executable(LogObserverDriver
    LoggingObserver/LoggingObserver.cpp
    LoggingObserver/LogObserverDriver.cpp
    LoggingObserver/LoggingObserver.h
    GameEngine/GameEngine.cpp
    GameEngine/GameEngine.h
    CommandProcessing/CommandProcessing.cpp
    CommandProcessing/CommandProcessing.h
    Cards/Cards.cpp
    Cards/Cards.h
    map/Map.cpp
    map/Map.h
    Player/Player.cpp
    Player/Player.h
    Orders/Orders.cpp
    Orders/Orders.h
)
target_include_directories(LogObserverDriver PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/LoggingObserver"
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommandProcessing"
    "${CMAKE_CURRENT_SOURCE_DIR}/Cards"
    "${CMAKE_CURRENT_SOURCE_DIR}/map"
    "${CMAKE_CURRENT_SOURCE_DIR}/Player"
    "${CMAKE_CURRENT_SOURCE_DIR}/Orders"
)


foreach(driver IN ITEMS MapDriver PlayerDriver CardsDriver OrdersDriver OrderDriver2 GameEngineDriver GameEngineDriverPart2 CommandInputTests LogObserverDriver)
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
add_test(NAME order_driver2 COMMAND OrderDriver2)
set_tests_properties(orders_driver PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set_tests_properties(order_driver2 PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
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
