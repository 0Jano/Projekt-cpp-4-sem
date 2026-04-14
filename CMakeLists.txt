cmake_minimum_required(VERSION 3.16)

project(SyncCal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Core)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(SyncCal
    src/main.cpp

    src/ui/MainWindow.cpp
    src/ui/LoginDialog.cpp
    src/ui/RegisterDialog.cpp
    src/ui/CalendarListWidget.cpp
    src/ui/CreateCalendarDialog.cpp
    src/ui/CalendarGridWidget.cpp

    src/models/User.cpp
    src/models/Calendar.cpp
    src/models/Event.cpp

    src/managers/AuthManager.cpp
    src/managers/CalendarManager.cpp
    src/managers/DatabaseManager.cpp
    src/managers/EventManager.cpp
)

target_include_directories(SyncCal PRIVATE
    src
)

target_link_libraries(SyncCal PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Core
)
