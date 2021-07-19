# CMakeList.txt : CMake project for Server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (libCore  "ByteArray.cpp" "StringUtils.cpp" "IO/Directory.cpp" "IO/MemoryStream.cpp" "Threading/Task.cpp" "Threading/TaskPool.cpp")
target_link_libraries(libCore libLog)