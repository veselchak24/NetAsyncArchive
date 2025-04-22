cmake_minimum_required(VERSION 3.31)
project(ClientProject)

set(CMAKE_CXX_STANDARD 20)

# set DEBUG definition
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_executable(ClientProject main.cpp
        Client.cpp)

#link winsock2
target_link_libraries(ClientProject PRIVATE ws2_32)

#include Snappy Google lib for compression files
find_package(Snappy CONFIG REQUIRED)
target_link_libraries(ClientProject PRIVATE Snappy::snappy)

#set static linker lib
set(CMAKE_EXE_LINKER_FLAGS "-static")