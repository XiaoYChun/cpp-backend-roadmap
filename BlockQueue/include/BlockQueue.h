#pragma once

#include <queue>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <cassert>

template <typename T>
class BlockQueue {
public:
    BlockQueue() = default;
    ~BlockQueue();
    BlockQueue(const BlockQueue&) = delete;
    BlockQueue& operator=(const BlockQueue&) = delete;

    bool push(const T& task);
    std::optional<T> pop();
    void close();

    bool empty();
    size_t size();

private:
    std::queue<T> m_taskQueue;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    bool m_isClosed = false;
};

template <typename T>
BlockQueue<T>::~BlockQueue() {
    assert(m_taskQueue.empty() && "BlockQueue isn't empty before ~BlockQueue!");
    assert(m_isClosed && "BlockQueue isn't closed before ~BlockQueue!");
}

template <typename T>
bool BlockQueue<T>::push(const T& task) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_isClosed) {
            m_taskQueue.push(task);
        } else {
            return false;
        }
    }

    m_cv.notify_one();

    return true;
}

template <typename T>
std::optional<T> BlockQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    m_cv.wait(lock, [this]() {
        return !m_taskQueue.empty() || m_isClosed;
    });
    
    if (m_taskQueue.empty() && m_isClosed) {
        return std::nullopt;
    }

    T task = m_taskQueue.front();
    m_taskQueue.pop();

    return task;
}

template <typename T>
void BlockQueue<T>::close() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_isClosed = true;
    }

    m_cv.notify_all();
}

template <typename T>
bool BlockQueue<T>::empty() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    return m_taskQueue.empty();
}

template <typename T>
size_t BlockQueue<T>::size() {
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_taskQueue.size();
}


