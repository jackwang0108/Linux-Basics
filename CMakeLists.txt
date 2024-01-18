cmake_minimum_required(VERSION 3.20)

project(cpp-server VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# add_library(
#     CircularQueue
#     STATIC
#     "./linux-process/CircularQueue.cpp"
# )

# target_include_directories(
#     CircularQueue
#     PUBLIC
#         "./linux-process"
# )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(SOURCE_FOLDERS FORCE "linux-basics/*.cpp" "linux-process/*.cpp" "linux-socket/*.cpp")

# 添加模式匹配的源文件
file(GLOB SOURCES ${SOURCE_FOLDERS})

list(REMOVE_ITEM SOURCES ${PROJECT_SOURCE_DIR}/linux-process/CircularQueue.cpp)

# 遍历源文件列表，为每个源文件添加add_executable
foreach(SOURCE ${SOURCES})
    message(${SOURCE})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE})
    # target_link_libraries(${EXECUTABLE_NAME} PUBLIC CircularQueue)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/linux-process ${PROJECT_SOURCE_DIR}/linux-socket)
endforeach()
