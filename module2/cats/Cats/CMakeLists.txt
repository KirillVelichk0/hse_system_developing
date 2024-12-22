project(cats LANGUAGES CXX)

add_library(cats Cats.cpp UniqueRegistry.cpp)

target_link_libraries(cats Boost::boost Boost::thread client executor ${OpenCV_LIBS})
