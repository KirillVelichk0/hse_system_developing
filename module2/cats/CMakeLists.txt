cmake_minimum_required(VERSION 3.5)

project(cat_proj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(MINIZIP_LIB minizip)
find_package(ZLIB REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost 1.74 REQUIRED COMPONENTS system thread)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "OpenSSL not found!")
endif()

set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${INCLUDE_ROOT})

add_subdirectory(Executor)
add_subdirectory(Client)
add_subdirectory(Cats)

add_executable(cat main.cpp)

target_link_libraries(cat PUBLIC my_executor client cats)
