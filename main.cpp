#include <iostream>
#include <vector>
#include "DownloadManager.hpp"
#include "DownloadTask.hpp"
#include "FileWritter.hpp"
#include "TaskQueue.hpp"
#include "ThreadPool.hpp"
#include <curl/curl.h>
#include <thread>
#include <algorithm>


class DownloadApplication {
    public:
        DownloadApplication(): stopFlag(false), manager(5) {
            curl_global_init(CURL_GLOBAL_DEFAULT);

            // Run the cleanup thread in background
            t2 = std::thread(&DownloadApplication::usualCleanup, this);
        }

        ~DownloadApplication() {
            stopFlag = true;
            if (t1.joinable()) {
                    t1.join();
            }

            if (t2.joinable()) {
                    t2.join();
            }


            manager.waitForCompletion();
            curl_global_cleanup();
        }

        void startAllDownloads() {
            if (t1.joinable()) {
                t1.join();
            }
            t1 = std::thread([this](){
                manager.startDownloads();
            });
        }

        void startDownload() {
            int urlNo;
             if (t1.joinable()) {
                t1.join();
             }
            showDownloadList();
            std::cout << "Enter the url number to start: ";
            std::cin >> urlNo;
            if(urlNo > filesToDownload.size() || urlNo < 1)
            {
                std::cout << "Invalid url number\n";
                return;
            }
            t1 = std::thread([this, urlNo](){
                manager.startDownload(filesToDownload[urlNo-1]);
            });
        }

        void pauseDownload() {
            int urlNo;
            showDownloadList();
            std::cout << "Enter the url number to pause: ";
            std::cin >> urlNo;
            if(urlNo > filesToDownload.size() || urlNo < 1)
            {
                std::cout << "Invalid url number\n";
                return;
            }
            manager.pauseDownload(filesToDownload[urlNo-1]);
        }

        void resumeDownload() {
            int urlNo;
            showDownloadList();
            std::cout << "Enter the url number to resume: ";
            std::cin >> urlNo;
            if(urlNo > filesToDownload.size() || urlNo < 1)
            {
                std::cout << "Invalid url number\n";
                return;
            }
            manager.resumeDownload(filesToDownload[urlNo-1]);
        }

        void cancelDownload() {
            int urlNo;
            showDownloadList();
            std::cout << "Enter the url number to cancel: ";
            std::cin >> urlNo;
            if(urlNo > filesToDownload.size() || urlNo < 1)
            {
                std::cout << "Invalid url number\n";
                return;
            }
            manager.cancelDownload(filesToDownload[urlNo-1]);
        }

        void removeDownload() {
            int urlNo;
            std::vector<std::string>::iterator it;
            showDownloadList();
            std::cout << "Enter the url number to remove: ";
            std::cin >> urlNo;
            it = std::find(filesToDownload.begin(), filesToDownload.end(), filesToDownload[urlNo-1]);
            // If element is found, erase it 
            if (it != filesToDownload.end()) { 
                filesToDownload.erase(it); 
            }else {
                std::cout << "Url not found in download list\n";
            }
        }

        void addFilesToDownload(std::string url) {
            filesToDownload.push_back(url);
            manager.addDownload(url, url.substr(url.find_last_of('/') + 1));
        }

        

        void CLITest() {
            std::string url;

            while(1) {
                printMenu();
                int option;
                std::cin >> option;
                switch(option) {
                    case 1:
                        std::cout << "Enter the url to add: ";                
                        std::cin >> url;
                        addFilesToDownload(url);
                        showDownloadList();
                        break;  
                    case 2:
                        removeDownload();
                        break;
                    case 3:
                        startAllDownloads();
                        break;
                    case 4:
                        startDownload();
                        break;
                    case 5:
                        pauseDownload();
                        break;
                    case 6:
                        resumeDownload();
                        break;
                    case 7:
                        cancelDownload();
                        break;
                    case 8:
                        stopFlag = true;
                        return;
                    default:
                        std::cout << "Invalid option. Exiting...\n";
                        stopFlag = true;
                        return;
                }
                
                
            }
        }
    private:
        DownloadManager manager; // Start with 5 threads in the thread pool
        std::vector<std::string> filesToDownload;
        std::thread t1, t2;
        bool stopFlag;

        std::string downloadStatusToString(DownloadStatus status) {
            switch (status) {
                case DownloadStatus::Starting:
                    return "Starting";
                case DownloadStatus::Pending:
                    return "Pending";
                case DownloadStatus::Downloading:
                    return "Downloading";
                case DownloadStatus::Paused:
                    return "Paused";
                case DownloadStatus::Completed:
                    return "Completed";
                case DownloadStatus::Failed:
                    return "Failed";
                // Add other cases as needed
                default:
                    return "Unknown";
            }
}

        void showDownloadList() {
            int i = 1;
            for (const auto& url : filesToDownload) {
                if(manager.getDownloadStatus(url) == DownloadStatus::Completed)
                {
                    // Remove the completed downloads from the list
                    filesToDownload.erase(std::remove(filesToDownload.begin(), filesToDownload.end(), url), filesToDownload.end());
                }
            }
            for (const auto& url : filesToDownload) {
                std::cout << i++ << ". " << url << " "<<downloadStatusToString(manager.getDownloadStatus(url))<<"\n";
            }
        }

        void usualCleanup() {
            while(!stopFlag) {
                //clear the downloaded or faliled tasks
                manager.clearTasks();
            }
        }

        void printMenu() {
            std::cout << "1. Add file to download list\n";
            std::cout << "2. Remove file from download list\n";
            std::cout << "3. Start all downloads\n";
            std::cout << "4. Start a download\n";
            std::cout << "5. Pause a download\n";
            std::cout << "6. Resume a download\n";
            std::cout << "7. Cancel a download\n";
            std::cout << "8. Exit\n";
            std::cout << "Enter your choice: ";
        }

        
};

int main() {
    DownloadApplication app;
    app.CLITest();
    return 0;
}