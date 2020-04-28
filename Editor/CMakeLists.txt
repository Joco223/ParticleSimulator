cmake_minimum_required(VERSION 3.10)

#Project name
project(Editor VERSION 0.1)

#Include directories
include_directories(include)

#Source directories
file(GLOB SOURCES "source/*.cpp")

#Cpp version
set(CMAKE_CXX_STANDARD 17)

#Executable
add_executable(Editor ${SOURCES})