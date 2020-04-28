cmake_minimum_required(VERSION 3.10)

#Project name
project(Simulator VERSION 0.1)

#Include directories
include_directories(include)

#Source directories
file(GLOB SOURCES "source/*.cpp")

#Cpp version
set(CMAKE_CXX_STANDARD 17)

#Cpp flags
set(CMAKE_CXX_FLAGS "-O3")

#Executable
add_executable(Simulator ${SOURCES})