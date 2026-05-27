#include "BlockQueue.h"

#include <thread>

void test_single_thread_basic() {
    BlockQueue<int> taskQueue;

    taskQueue.push(1);
    taskQueue.pop();
    taskQueue.close();
}

int main() {
    test_single_thread_basic();
    
    return 0;
}