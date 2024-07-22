// ThreadPool.hpp
#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include "TaskQueue.hpp"

class ThreadPool {
private:
    std::vector<std::thread> workers;
    TaskQueue taskQueue;
    std::atomic<bool> stopFlag;

    void workerFunction();

public:
    ThreadPool(size_t threads);
    ~ThreadPool();
    void enqueueTask(const std::shared_ptr<DownloadTask>& task);
    void shutdown();
};

#endif // THREADPOOL_HPP