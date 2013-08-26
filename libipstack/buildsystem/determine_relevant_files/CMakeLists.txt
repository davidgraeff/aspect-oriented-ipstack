# Build tool
# Input: kconfig output file and feature-to-file-relation file
# Output: Selected files in file relevant_source_files.
cmake_minimum_required (VERSION 2.8.8)
project(determine_relevant_files)

#DEBUG
#SET(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS "-g -Wall -Werror")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(${PROJECT_NAME} main.cpp determine_files.cpp)