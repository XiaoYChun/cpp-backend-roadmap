add_library(BlockQueue INTERFACE)
target_include_directories(BlockQueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(BlockQueueTest test/BlockQueueTest.cpp)
target_link_libraries(BlockQueueTest PRIVATE BlockQueue)
