#pragma once

#include "BlockQueue.h"

#include <thread>
#include <functional>
#include <vector>
#include <atomic>

class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool();
    ThreadPool(size_t threadCount);
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<typename TaskFunc, typename... TaskArgs>
    bool submit(TaskFunc&& taskFunc, TaskArgs&&... taskArgs);
    void stop();

private:
    // 配置、状态、资源
    size_t m_threadCount = 1;

    std::atomic<bool> m_isStopping = false;

    BlockQueue<Task> m_taskQueue;
    std::vector<std::thread> m_threads;
};

// 模板实现应该写在 .h/.inl 中，而不是 .cpp 中
// 写在 .cpp 文件中无法在编译阶段正确生成具体的实例化模板函数
// 后续链接阶段无法正确链接
template<typename TaskFunc, typename... TaskArgs>
bool ThreadPool::submit(TaskFunc&& taskFunc, TaskArgs&&... taskArgs) {
    // bind 的返回值是一个匿名类型，但是 function<void()> 会将其隐式转化
    const std::function<void()> boundTaskFunc = std::bind(
        std::forward<TaskFunc>(taskFunc), std::forward<TaskArgs>(taskArgs)...);

    // 这里不需要关心 BlockQueue 是否 close，这已经由其自己确保了安全，不是本层的任务
    if (!m_isStopping) {
        m_taskQueue.push(boundTaskFunc);

        return true;
    }

    return false;
}