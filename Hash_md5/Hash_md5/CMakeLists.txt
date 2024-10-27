cmake_minimum_required(VERSION 3.5)

project(Hash_md5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found!")
endif()
add_library(thread_status threadstatus.cpp)
add_library(md5_calcer md5calcer.cpp)

target_include_directories(md5_calcer PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(md5_calcer PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_library(bidirectional_pipe bidirectionalpipe.cpp)
add_library(thread_guard threadguard.cpp)

add_library(child_session childsession.cpp)
target_link_libraries(child_session PUBLIC bidirectional_pipe thread_guard md5_calcer)

add_executable(Hash_md5 main.cpp)
add_executable(utils utils.cpp)

target_link_libraries(utils PRIVATE md5_calcer)
target_link_libraries(Hash_md5 PRIVATE bidirectional_pipe md5_calcer)
