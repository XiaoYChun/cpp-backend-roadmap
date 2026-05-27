#include <queue>
#include <optional>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockQueue {
public:
    BlockQueue() = default;
    ~BlockQueue() = default;
    BlockQueue(const BlockQueue&) = delete;
    BlockQueue& operator=(const BlockQueue&) = delete;

    void push(const T& task);
    std::optional<T> pop();
    void close();

private:
    std::queue<T> m_taskQueue;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    bool m_isClosed = false;
};

template <typename T>
void BlockQueue<T>::push(const T& task) {
    std::lock_guard<std::mutex> pushLock(m_mutex);
    
    m_taskQueue.push(task);

    m_cv.notify_one();
}

template <typename T>
std::optional<T> BlockQueue<T>::pop() {
    std::unique_lock<std::mutex> popLock(m_mutex);
    
    m_cv.wait(popLock, [this]() {
        return !m_taskQueue.empty() || m_isClosed;
    });
    
    if (!m_taskQueue.empty()) {
        T task = m_taskQueue.front();
        m_taskQueue.pop();

        return task;
    }

    return std::nullopt;
}

template <typename T>
void BlockQueue<T>::close() {
    std::lock_guard<std::mutex> closeLock(m_mutex);

    m_isClosed = true;

    m_cv.notify_all();
}