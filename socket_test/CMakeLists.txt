cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME socket_test)
set(PROJECT_VERSION 1.0)
project(
    ${PROJECT_NAME}
    LANGUAGES CXX
    ${PROJECT_VERSION}
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(client client.cpp socket_stuff.h)
add_executable(server server.cpp socket_stuff.h)