// DownloadManager.hpp
#ifndef DOWNLOADMANAGER_HPP
#define DOWNLOADMANAGER_HPP

#include "ThreadPool.hpp"
#include <string>
#include <unordered_map>

class DownloadManager {
private:
    ThreadPool threadPool;
    std::unordered_map<std::string, std::shared_ptr<DownloadTask>> tasks;
    std::mutex taskMutex;

public:
    DownloadManager(size_t threadCount);
    void startDownloads();
    void addDownload(const std::string& url, const std::string& destinationPath);
    void startDownload(const std::string& url);
    void pauseDownload(const std::string& url);
    void resumeDownload(const std::string& url);
    void cancelDownload(const std::string& url);
    DownloadStatus getDownloadStatus(const std::string& url);
    void waitForCompletion();
    void clearTasks();
};

#endif // DOWNLOADMANAGER_HPP