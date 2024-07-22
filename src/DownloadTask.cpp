// DownloadTask.cpp
#include "DownloadTask.hpp"

#include <fstream>

static std::mutex progress_mutex;

// Helper function to write data received from libcurl
static size_t write_data(void *ptr, size_t size, size_t nmemb, DownloadTask* task) {
    int written = task->writer->write(static_cast<char*>(ptr), size * nmemb);
   // std::cout<<"Written: "<<written<<" "<<size * nmemb<<std::endl;
    return written;
}

// Progress callback function
static int progress_callback(DownloadTask* task, double dltotal, double dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    //std::cout<<dlnow<<" "<<dltotal<<std::endl;
    if (dltotal > 0) {
        double progress = dlnow / dltotal;
        std::string url = task->getUrl();
        std::lock_guard<std::mutex> lock(progress_mutex); // Locks the mutex for printing progress
        if(task->getStatus() == DownloadStatus::Downloading)
            std::cout<<"Progress: "<<url.substr(url.find_last_of('/') + 1)<<" - "<<progress * 100.0f<<"%"<<"\t\t\r";
    }
    return 0;
}

DownloadTask::DownloadTask(const std::string& url, const std::string& destination)
    : url(url), destinationPath(destination), status(DownloadStatus::Pending), progress(0.0f)
    {
        writer = new FileWriter(destinationPath);
        curlHandle = curl_easy_init();
    }

void DownloadTask::start() {
    // Implementation to start the download
    status = DownloadStatus::Downloading;
    progress = 0.0f;

    CURLcode res;
    if (curlHandle) {
        curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(curlHandle, CURLOPT_PROGRESSDATA, this);
        res = curl_easy_perform(curlHandle);
        if (res == CURLE_OK) {
            status = DownloadStatus::Completed;
        } else {
            std::cout<<"Download failed: "<<url.substr(url.find_last_of('/') + 1)<<std::endl;
            status = DownloadStatus::Failed;
        }
    }
}

std::string DownloadTask::getUrl() const {
    return url;
}

bool DownloadTask::getStartCommand() const {
    return (status == DownloadStatus::Starting);
}

void DownloadTask::setStartCommand() {
    //std::cout<<"Setting start command: "<<command<<std::endl;
    if(status == DownloadStatus::Pending || status == DownloadStatus::Failed)
        status = DownloadStatus::Starting;
    else
        std::cout<<"Download is already in progress"<<std::endl;
}

void DownloadTask::pause() {
    // Implementation to pause the download
    if(status == DownloadStatus::Downloading)
    {
        curl_easy_pause(curlHandle, CURLPAUSE_RECV | CURLPAUSE_SEND);
        status = DownloadStatus::Paused;
    }
    else
    {
        std::cout<<"Download is not in progress"<<std::endl;
    }

}

void DownloadTask::resume() {
    // Implementation to resume the download
    if(status != DownloadStatus::Paused)
    {
        std::cout<<"Download is not paused"<<std::endl;
        return;
    }else
    {
        curl_easy_pause(curlHandle, CURLPAUSE_CONT);
        status = DownloadStatus::Downloading;
    }
    
}

void DownloadTask::cancel() {
    // Implementation to cancel the download
    status = DownloadStatus::Failed;
}

DownloadStatus DownloadTask::getStatus() const {
    return status;
}

float DownloadTask::getProgress() const {
    return progress;
}

DownloadTask::~DownloadTask() {
    curl_easy_cleanup(curlHandle);
    delete writer;
}