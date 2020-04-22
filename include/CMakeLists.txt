CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
add_compile_options(-std=c++11 -O2)
project(code)
include_directories(
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/data/
)
add_executable(code data/two/code.cpp)
