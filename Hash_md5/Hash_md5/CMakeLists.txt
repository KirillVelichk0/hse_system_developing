cmake_minimum_required(VERSION 3.5)

project(Hash_md5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found!")
endif()

add_library(md5_calcer md5calcer.cpp)

target_include_directories(md5_calcer PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(md5_calcer PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_library(bidirectional_pipe bidirectionalpipe.cpp)

add_executable(Hash_md5 main.cpp)

target_link_libraries(Hash_md5 PRIVATE bidirectional_pipe)
