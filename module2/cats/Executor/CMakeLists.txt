add_library(my_executor BaseExecutor.cpp IExecutor.cpp StrandExecutor.cpp IResourceProvider.cpp)

target_link_libraries(my_executor PUBLIC Boost::boost Boost::thread)
