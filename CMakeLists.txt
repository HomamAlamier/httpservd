# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_definitions(-DDEBUG -DLOG_DEBUG -DLOG_LEVEL_5 -DLOG_COUT -DMEMORY_MANGER_LOG)
include_directories(include)


project ("httpservd")

# Include sub-projects.
add_subdirectory ("src/Log")
add_subdirectory ("src/Core")
add_subdirectory ("src/Network")
add_subdirectory ("src/Web")
add_subdirectory ("src/httpservd")
add_subdirectory ("tests")
