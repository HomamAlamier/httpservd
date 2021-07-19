# CMakeList.txt : CMake project for Server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (libNetwork "Socket.cpp" "IPEndPoint.cpp" "SocketStream.cpp")
target_link_libraries(libNetwork libLog libCore)