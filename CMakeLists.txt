# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_definitions(-DLOG_DEBUG -DLOG_LEVEL_5 -DWINOS -DLOG_COUT)
include_directories(include)


project ("Server")

# Include sub-projects.
add_subdirectory ("Log")
add_subdirectory ("Core")
add_subdirectory ("Network")
add_subdirectory ("Web")
add_subdirectory ("Server")