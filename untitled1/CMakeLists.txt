cmake_minimum_required(VERSION 3.29)
project(untitled1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "C:/Qt/6.7.2/mingw_64")



find_package(Qt6 COMPONENTS
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
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

