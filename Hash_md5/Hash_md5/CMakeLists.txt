cmake_minimum_required(VERSION 3.5)

project(Hash_md5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(bidirectional_pipe bidirectionalpipe.cpp)
add_executable(Hash_md5 main.cpp)
target_link_libraries(Hash_md5 PRIVATE bidirectional_pipe)
