cmake_minimum_required(VERSION 3.20)

project(cpp-server VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    time
    "./linux-basics/time.cpp"
)

add_executable(
    dir
    "./linux-basics/dir.cpp"
)

add_executable(
    error
    "./linux-basics/error.cpp"
)