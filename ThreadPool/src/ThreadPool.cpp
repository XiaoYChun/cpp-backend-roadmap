#include "ThreadPool.h"

ThreadPool::ThreadPool() 
    : ThreadPool(1) {
}

ThreadPool::ThreadPool(size_t threadCount)
    : m_threadCount(threadCount) {

    for (size_t i = 0; i < threadCount; i++) {
        m_threads.emplace_back([this]() {
            while (std::optional<Task> taskOpt = m_taskQueue.pop()) {
                Task task = std::move(taskOpt).value();
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::stop() {
    m_isStopping = true;

    m_taskQueue.close();

    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}