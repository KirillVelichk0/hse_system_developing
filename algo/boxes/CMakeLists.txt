cmake_minimum_required(VERSION 3.5)

project(Boxes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(task_input TaskInput.cpp)

add_library(calculate CalculateDP.cpp)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} task_input calculate)

