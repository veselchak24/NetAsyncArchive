cmake_minimum_required(VERSION 3.31)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(ServerProject)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_SELF_CQ")

add_executable(main main.cpp
        utils/utils.cpp
        Server/Server.cpp
        SwitchConcurrentQueue.h
)

target_link_libraries(main PRIVATE ws2_32)

#include concurrentqueue lib for assync archiving
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
target_link_libraries(main PRIVATE unofficial::concurrentqueue::concurrentqueue)

#include lib /Common, includes global variables and functions
add_subdirectory(../Common Common_build)
target_link_libraries(main PRIVATE socketUtils)
target_include_directories(main PRIVATE ../Common)

set(CMAKE_EXE_LINKER_FLAGS "-static")