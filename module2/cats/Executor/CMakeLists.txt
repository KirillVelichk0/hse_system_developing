project(executor LANGUAGES CXX)

add_library(executor BaseExecutor.cpp IExecutor.cpp StrandExecutor.cpp IResourceProvider.cpp)

target_link_libraries(executor Boost::boost Boost::thread)
