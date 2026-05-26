#include <queue>
#include <optional>
#include <mutex>
#include <condition_variable>

templete <typename T>
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
}

templete <typename T>
void BlockQueue::push(const T& task) {
    std::lock_guard<std::mutex> pushLock(m_mutex);
    
    m_taskQueue.push(task);

    cv.notify_one();
}

templete <typename T>
std::optional<T> BlockQueue::pop() {
    std::unique_lock<std::mutex> popLock(m_mutex);
    
    m_cv.wait(popLock, [this]() {
        return !m_taskQueue.empty() || m_isClosed;
    });
    
    if (!m_taskQueue.empty()) {
        T task = m_taskQueue.front();
        m_taskQueue.pop();
    } else {
        return std::nullopt;
    }

    return task;
}

templete <typename T>
void BlockQueue::close() {
    std::lock_guard<mutex> closeLock(m_mutex);

    m_isClosed = true;

    cv.notify_all();
}