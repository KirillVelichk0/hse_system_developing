cmake_minimum_required(VERSION 3.5)

project(stash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

find_package(OpenSSL REQUIRED)

if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found!")
endif()

add_library(gamma_generator gammagenerator.cpp)
target_include_directories(gamma_generator PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(gamma_generator PRIVATE OpenSSL::SSL OpenSSL::Crypto)


add_executable(stash main.cpp)

target_link_libraries(stash PRIVATE gamma_generator)
