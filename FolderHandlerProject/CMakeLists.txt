cmake_minimum_required(VERSION 3.31)
project(FolderHandlerProject)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_executable(FolderHandlerProject main.cpp
        utils.cpp
        Server/Server.cpp)

target_link_libraries(FolderHandlerProject PRIVATE ws2_32)

find_package(unofficial-concurrentqueue CONFIG REQUIRED)
target_link_libraries(FolderHandlerProject PRIVATE unofficial::concurrentqueue::concurrentqueue)

set(CMAKE_EXE_LINKER_FLAGS "-static")