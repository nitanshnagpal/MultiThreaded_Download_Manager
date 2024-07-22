// ThreadPool.cpp
#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t threads) : stopFlag(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(&ThreadPool::workerFunction, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::enqueueTask(const std::shared_ptr<DownloadTask>& task) {
    taskQueue.addTask(task);
}

void ThreadPool::workerFunction() {
    while (!stopFlag) {
        auto task = taskQueue.getNextTask();
        if (task != nullptr) {

            if(task->getStartCommand())
            {
                // Execute the task
                task->start(); 
            }
            else
            {   // Add the task back to the queue so we will traverse it again to check if it has been started
                taskQueue.addTask(task); 
            }
            

        } else {
            // No task available, yield the current thread
            std::this_thread::yield();
        }
    }
}

void ThreadPool::shutdown() {
    stopFlag = true;
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}