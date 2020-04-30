cmake_minimum_required(VERSION 3.10)

#Project name
project(Viewer VERSION 0.1)

#Include directories
include_directories(include)

#Source directories
file(GLOB SOURCES "source/*.cpp")

#Cpp version
set(CMAKE_CXX_STANDARD 20)

#Cpp flags
set(CMAKE_CXX_FLAGS "-O3")

#Executable
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
add_executable(Viewer ${SOURCES})
target_link_libraries(Viewer sfml-graphics sfml-audio)
