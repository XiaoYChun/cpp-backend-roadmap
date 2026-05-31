add_library(ThreadPool ${CMAKE_CURRENT_SOURCE_DIR}/src/ThreadPool.cpp)
target_include_directories(ThreadPool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ThreadPool PRIVATE BlockQueue)

add_executable(ThreadPoolTest test/ThreadPoolTest.cpp)
target_link_libraries(ThreadPoolTest PRIVATE ThreadPool)