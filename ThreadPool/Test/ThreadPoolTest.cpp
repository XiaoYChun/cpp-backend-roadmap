#include "ThreadPool.h"

#include <iostream>
#include <mutex>

void TestBasicFeature() {
    ThreadPool threadPool(4);

    const int taskCount = 10;
    std::mutex outMutex;
    for (int i = 0; i < taskCount; i++) {
        threadPool.submit([i, &outMutex]() {
            std::lock_guard<std::mutex> outLock(outMutex);
            std::cout << "[TestBasicFeature] ThreadPool Finished Task "
                      << i << "!" << std::endl;
        });
    }
}

int main() {
    std::cout << "-- THREADPOOL TEST START --" << std::endl;

    TestBasicFeature();

    std::cout << "-- THREADPOOL TEST SUCCESS --" << std::endl;
    
    return 0;
}