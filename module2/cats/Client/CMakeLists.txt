add_library(client Client.cpp ClientSession.cpp)

target_link_libraries(client PUBLIC my_executor)
