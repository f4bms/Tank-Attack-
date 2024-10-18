cmake_minimum_required(VERSION 3.29)
project(untitled1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(untitled1 main.cpp
        Graph.h
        GameLaunch.h
        Tank.h
        Player.h)
target_link_libraries(untitled1
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

