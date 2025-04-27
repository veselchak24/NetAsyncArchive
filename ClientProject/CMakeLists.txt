cmake_minimum_required(VERSION 3.31)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_INSTALL_PREFIX "$ENV{VCPKG_ROOT}/installed/x64-mingw-static")

project(ClientProject)

set(CMAKE_CXX_STANDARD 20)

# set DEBUG definition
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG")

add_executable(main main.cpp
        Client.cpp)

#link winsock2
target_link_libraries(main PRIVATE ws2_32)

#include Snappy Google lib for compression files
find_package(Snappy CONFIG REQUIRED)
target_link_libraries(main PRIVATE Snappy::snappy)

#include lib /Common, includes global variables and functions
add_subdirectory(../Common Common_build)
target_link_libraries(main PRIVATE socketUtils)
target_include_directories(main PRIVATE ../Common)

#set static linker lib
set(CMAKE_EXE_LINKER_FLAGS "-static")