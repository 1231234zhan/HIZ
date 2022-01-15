cmake_minimum_required(VERSION 3.10)

project(HIZ)

set(CMAKE_CXX_STANDARD 11)

# add_definitions(-DDEBUG)

include_directories(include)

SET(CMAKE_BUILD_TYPE "Release")

add_subdirectory(src)

add_executable(hiz src/main.cpp)

target_link_libraries(hiz wheels)