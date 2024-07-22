// TaskQueue.hpp
#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include <queue>
#include <mutex>
#include <memory>
#include "DownloadTask.hpp"

class TaskQueue {
private:
    std::queue<std::shared_ptr<DownloadTask>> queue;
    std::mutex mutex;

public:
    void addTask(const std::shared_ptr<DownloadTask>& task);
    std::shared_ptr<DownloadTask> getNextTask();
    bool isEmpty();
};

#endif // TASKQUEUE_HPP