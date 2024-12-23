add_library(cats Cats.cpp UniqueRegistry.cpp)

target_link_libraries(cats PUBLIC my_executor client ${OpenCV_LIBS} ZLIB::ZLIB ${MINIZIP_LIB})
