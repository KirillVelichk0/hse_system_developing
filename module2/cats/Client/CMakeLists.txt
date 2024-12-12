project(client LANGUAGES CXX)

add_library(client Client.cpp ClientSession.cpp)

target_link_libraries(client Boost::boost Boost::thread executor)
