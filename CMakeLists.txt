cmake_minimum_required(VERSION 3.25)
project(word_game)
include_directories("C:/Program Files/MySQL/MySQL Server 8.0/include")
link_directories("C:/Program Files/MySQL/MySQL Server 8.0/lib")
link_libraries(libmysql)    #添加libmysql.lib静态库，编译和链接时需要用到它
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")
file(GLOB SRC
        "${PROJECT_SOURCE_DIR}/src/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        "${PROJECT_SOURCE_DIR}/src/*.cc")
message(STATUS "SRC=${SRC}")
add_executable(${CMAKE_PROJECT_NAME} ${SRC})


