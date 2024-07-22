// DownloadTask.hpp
#ifndef DOWNLOADTASK_HPP
#define DOWNLOADTASK_HPP

#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include "FileWritter.hpp"
#include <curl/curl.h>
#include <iostream>
#include <mutex>

enum class DownloadStatus {
    Starting,
    Pending,
    Downloading,
    Paused,
    Completed,
    Failed
};

class DownloadTask {
private:
    std::string url;
    std::string destinationPath;
    DownloadStatus status;
    float progress; // Percentage of download completed
    
    CURL* curlHandle;

public:
    FileWriter* writer;
    DownloadTask(const std::string& url, const std::string& destination);
    ~DownloadTask();
    bool getStartCommand() const;
    std::string getUrl() const;
    void setStartCommand();
    void start();
    void pause();
    void resume();
    void cancel();
    DownloadStatus getStatus() const;
    float getProgress() const;
};

#endif // DOWNLOADTASK_HPP