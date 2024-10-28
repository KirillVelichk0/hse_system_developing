cmake_minimum_required(VERSION 3.5)

project(Hash_md5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

find_package(OpenSSL REQUIRED)

if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found!")
endif()

add_library(thread_status threadstatus.cpp)
add_library(ipc_exceptions ipcexceptions.cpp)
add_library(md5_calcer md5calcer.cpp)

target_link_libraries(ipc_exceptions PUBLIC thread_status)

target_include_directories(md5_calcer PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(md5_calcer PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_library(bidirectional_pipe bidirectionalpipe.cpp)
add_library(thread_guard threadguard.cpp)

add_library(child_session childsession.cpp)
target_link_libraries(child_session PUBLIC bidirectional_pipe thread_guard md5_calcer thread_status ipc_exceptions)

add_library(parent_session parentsession.cpp)
target_link_libraries(parent_session PUBLIC child_session)

add_executable(Hash_md5 main.cpp)
add_executable(utils utils.cpp)

target_link_libraries(utils PRIVATE md5_calcer)
target_link_libraries(Hash_md5 PRIVATE parent_session)
