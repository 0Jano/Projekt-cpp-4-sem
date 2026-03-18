cmake_minimum_required(VERSION 3.16)

project(SyncCal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(SyncCal
    src/main.cpp
    src/models/User.cpp
    src/models/AuthManager.cpp
    src/models/MainWindow.cpp
)

target_include_directories(SyncCal PRIVATE
    src
    src/models
)

target_link_libraries(SyncCal PRIVATE
    Qt6::Widgets
)