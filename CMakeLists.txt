cmake_minimum_required(VERSION 3.20)

project(cpp-server VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(SOURCE_FOLDERS "linux-basics/*.cpp" "linux-signal/*.cpp")

# 添加模式匹配的源文件
file(GLOB SOURCES ${SOURCE_FOLDERS})

# 遍历源文件列表，为每个源文件添加add_executable
foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE})
endforeach()
