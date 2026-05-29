#include "BlockQueue.h"

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

void TestSingleThreadBasic() {
    std::cout << "Test BlockQueue Single Thread Basic Feature!" << std::endl;

    BlockQueue<int> taskQueue;
    for (int taskId = 1; taskId <= 5; taskId++) {
        taskQueue.push(taskId);
    }
    assert(!taskQueue.empty() && "TestSingleThreadBasic: Queue Is Empty!"); 
    assert(taskQueue.size() == 5 && "TestSingleThreadBasic: QueueSize ERROR!"); 

    for (int trueId = 1; trueId <= 5; trueId++) {
        std::optional<int> taskpacket = taskQueue.pop();
        int task = taskpacket.value();

        assert(task == trueId && "TestSingleThreadBasic: Pop ERROR!");
    }
    taskQueue.close();

    std::cout << "TestSingleThreadBasic Success!" << std::endl;
}

void TestMutiThreadBasic() {
    std::cout << "Test BlockQueue Mutiple Thread Basic Feature!" << std::endl;

    BlockQueue<int> taskQueue;
    std::vector<std::thread> pushThreads;
    std::vector<std::thread> popThreads;

    const int PUSH_THREAD_COUNT = 5;
    const int POP_THREAD_COUNT = 5;
    const int TASKS_PER_THREAD = 5;

    std::atomic<int> popCount{0};

    for (int threadId = 1; threadId <= POP_THREAD_COUNT; threadId++) {
        popThreads.emplace_back([&taskQueue, &popCount]() {
            while (true) {
                std::optional<int> taskpacket = taskQueue.pop();

                if (taskpacket.has_value()) {
                    popCount++;
                } else {
                    break;
                }
            }
        });
    }
    for (int threadId = 1; threadId <= PUSH_THREAD_COUNT; threadId++) {
        pushThreads.emplace_back([&taskQueue, threadId, TASKS_PER_THREAD]() {
            for (int taskId = (threadId - 1) * TASKS_PER_THREAD + 1; taskId <= threadId * TASKS_PER_THREAD; taskId++) {
                taskQueue.push(taskId);
            }
        });
    }

    for (auto& pushThread : pushThreads) {
        if (pushThread.joinable()) {
            pushThread.join();
        }
    }
    taskQueue.close();

    for (auto& popThread : popThreads) {
        if (popThread.joinable()) {
            popThread.join();
        }
    }

    int expectedTasks = PUSH_THREAD_COUNT * TASKS_PER_THREAD;
    assert(popCount.load() == expectedTasks && "TestMutiThreadBasic: ERROR!");

    std::cout << "TestMutiThreadBasic Success!" << std::endl;
}

void TestBoundary() {
    std::cout << "Test BlockQueue Boundary Feature!" << std::endl;

    BlockQueue<int> pushAfterCloseQueue;
    const int pushAfterCloseTask = 1;
    pushAfterCloseQueue.close();
    bool pushStatus = pushAfterCloseQueue.push(pushAfterCloseTask);
    assert(pushStatus == false && "TestBoundary_1: ERROR!");

    BlockQueue<int> mutiCloseQueue;
    mutiCloseQueue.close();
    mutiCloseQueue.close();
    mutiCloseQueue.close();

    BlockQueue<int> closeFeatureQueue;
    std::optional<int> taskPacket;
    std::thread popThread([&closeFeatureQueue, &taskPacket]() {
        while (true) {
            taskPacket = closeFeatureQueue.pop();
            if (!taskPacket.has_value()) {
                break;
            }
        }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    closeFeatureQueue.close();
    if (popThread.joinable()) {
        popThread.join();
    }
    assert(!taskPacket.has_value() && "TestBoundary_3: taskPacket isn't nullopt!");
    
    BlockQueue<int> competitveQueue;
    std::vector<std::thread> popThreads;
    const int POP_THREADS_COUNT = 5;
    const int competitveTask = 1;
    std::atomic<int> successCount{0};
    for (int i = 1; i <= POP_THREADS_COUNT; i++) {
        popThreads.emplace_back([&competitveQueue, &successCount]() {
            while (true) {
                std::optional<int> taskPacket = competitveQueue.pop();
                if (taskPacket.has_value()) {
                    successCount++;
                } else {
                    break;
                }
            }
        });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    competitveQueue.push(competitveTask);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    competitveQueue.close();
    for (auto& t : popThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    assert(successCount == competitveTask && "TestBoundary_4: ERROR!");

    std::cout << "TestBoundary Success!" << std::endl;
}

int main() {
    std::cout << "-- BLOCKQUEUE TEST START --" << std::endl;

    TestSingleThreadBasic();
    TestMutiThreadBasic();
    TestBoundary();

    std::cout << "-- BLOCKQUEUE TEST SUCCESS --" << std::endl;
    
    return 0;
}