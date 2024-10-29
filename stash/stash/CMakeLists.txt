cmake_minimum_required(VERSION 3.5)

project(stash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

find_package(OpenSSL REQUIRED)

if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found!")
endif()

add_executable(stash main.cpp)

target_include_directories(stash PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(stash PRIVATE OpenSSL::SSL OpenSSL::Crypto)
