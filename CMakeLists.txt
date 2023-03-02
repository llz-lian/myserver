cmake_minimum_required(VERSION 3.15)

# 设置工程名称
set(PROJECT_NAME thread_pool_test)
 
set(PROJECT_VERSION "0.01")
 
project(${PROJECT_NAME}
    LANGUAGES CXX
    VERSION ${PROJECT_VERSION}
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(src)
