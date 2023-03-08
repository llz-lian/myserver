
# set(SRC_LIST main.cpp)
aux_source_directory(. SRC_LIST)
aux_source_directory(./include DIR_SRCS)
# add_definitions(DEBUG_POOL)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} ${DIR_SRCS})