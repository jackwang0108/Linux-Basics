cmake_minimum_required(VERSION 3.20)

project(cpp-server VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# 添加模式匹配的源文件
file(GLOB SOURCES "linux-basics/*.cpp" "linux-signal/*.cpp")

# 遍历源文件列表，为每个源文件添加add_executable
foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE})
endforeach()
