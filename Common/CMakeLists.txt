add_library(socketUtils STATIC socketUtils.cpp)
target_include_directories(socketUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(socketUtils PRIVATE ws2_32)