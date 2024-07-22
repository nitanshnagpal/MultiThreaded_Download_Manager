// DownloadManager.cpp
#include "DownloadManager.hpp"

DownloadManager::DownloadManager(size_t threadCount) : threadPool(threadCount) {}

void DownloadManager::addDownload(const std::string& url, const std::string& destinationPath) {
    std::lock_guard<std::mutex> lock(taskMutex);
    auto task = std::make_shared<DownloadTask>(url, destinationPath);
    tasks[url] = task;
    threadPool.enqueueTask(task);
}

void DownloadManager::startDownloads() {
    std::lock_guard<std::mutex> lock(taskMutex);
    for (const auto& task : tasks) {
        task.second->setStartCommand();
    }
}

void DownloadManager::startDownload(const std::string& url) {
    std::lock_guard<std::mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end()) {
        tasks[url]->setStartCommand();
    }else {
        std::cout << "Url not in download queue" << std::endl;
    }
}

void DownloadManager::pauseDownload(const std::string& url) {
    std::lock_guard<std::mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end()) {
        tasks[url]->pause();
    }else {
        std::cout << "Url not in download queue" << std::endl;
    }
}

void DownloadManager::resumeDownload(const std::string& url) {
    std::lock_guard<std::mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end()) {
        tasks[url]->resume();
    }else {
        std::cout << "Url not in download queue" << std::endl;
    }
}

void DownloadManager::cancelDownload(const std::string& url) {
    std::lock_guard<std::mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end()) {
        tasks[url]->cancel();
    }else {
        std::cout << "Url not in download queue" << std::endl;
    }
}

DownloadStatus DownloadManager::getDownloadStatus(const std::string& url) {
    std::lock_guard<std::mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end()) {
        return tasks.at(url)->getStatus();
    }else {
        return DownloadStatus::Completed;
    }
}

void DownloadManager::waitForCompletion() {
    std::lock_guard<std::mutex> lock(taskMutex);
    for (const auto& task : tasks) {
        while (task.second->getStatus() == DownloadStatus::Downloading) {
            // Wait for completion
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void DownloadManager::clearTasks() {
    std::lock_guard<std::mutex> lock(taskMutex);
    if(!(tasks.empty()))
    {
        
        auto it = tasks.begin();
        while (it != tasks.end()) {
            //std::cout<<it->first<<std::endl;
            if (it->second->getStatus() == DownloadStatus::Completed) {
                std::cout<<"Erasing completed task"<<std::endl;
                it = tasks.erase(it);
            }
            else if (it->second->getStatus() == DownloadStatus::Failed)
            {
                // put the failed task back in the queue
                std::cout<<"Adding failed task back to queue"<<std::endl;
                threadPool.enqueueTask(it->second);
                ++it;
            }
            else {
                //std::cout<<"Task is still in progress"<<std::endl;
                ++it;
            }
        }
    }
}