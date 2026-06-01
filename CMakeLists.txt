cmake_minimum_required(VERSION 3.16)

project(SynCal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Core)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(SyncalCore
    src/models/User.cpp
    src/models/Calendar.cpp
    src/models/Event.cpp
    src/models/CalendarInvitation.cpp

    src/managers/AuthManager.cpp
    src/managers/CalendarManager.cpp
    src/managers/InvitationManager.cpp
    src/managers/DatabaseManager.cpp
    src/managers/EventManager.cpp
    src/managers/IcsManager.cpp
)

target_include_directories(SyncalCore PUBLIC
    src
)

target_link_libraries(SyncalCore PUBLIC
    Qt6::Core
    Qt6::Sql
)

add_executable(SynCal
    src/main.cpp

    src/ui/MainWindow.cpp
    src/ui/LoginDialog.cpp
    src/ui/RegisterDialog.cpp
    src/ui/CalendarListWidget.cpp
    src/ui/CreateCalendarDialog.cpp
    src/ui/ShareCalendarDialog.cpp
    src/ui/CalendarGridWidget.cpp
    src/ui/CreateEventDialog.cpp
    src/ui/InvitationsDialog.cpp
)

target_include_directories(SynCal PRIVATE
    src
)

target_link_libraries(SynCal PRIVATE
    SyncalCore
    Qt6::Widgets
)

# Enable testing and add GoogleTest via FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Unit test executable
add_executable(UnitTests
    tests/main.cpp
    tests/AuthManagerTest.cpp
    tests/DatabaseManagerTest.cpp
    tests/EventManagerTest.cpp
)

target_include_directories(UnitTests PRIVATE
    src
)

target_link_libraries(UnitTests PRIVATE
    SyncalCore
    GTest::gtest
)

include(GoogleTest)
# Discover tests
gtest_discover_tests(UnitTests)
