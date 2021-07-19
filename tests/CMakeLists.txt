# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
include_directories(include)


add_library(HelloModule SHARED "HelloModule.c")


add_executable(ConnectionTest "ConnectionTest.cpp")
target_link_libraries(ConnectionTest libCore libNetwork libWeb pthread)