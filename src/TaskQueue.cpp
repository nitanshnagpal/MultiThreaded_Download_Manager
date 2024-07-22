// TaskQueue.cpp
#include "TaskQueue.hpp"

void TaskQueue::addTask(const std::shared_ptr<DownloadTask>& task) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(task);
}

std::shared_ptr<DownloadTask> TaskQueue::getNextTask() {
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty()) {
        return nullptr;
    }
    auto task = queue.front();
    queue.pop();
    return task;
}

bool TaskQueue::isEmpty() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}