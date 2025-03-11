#include "CopyProcess.h"
#include "FileCopier.h"
#include <sstream>
#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

CopyProcess::CopyProcess(std::shared_ptr<ICopyView> view) : view(view) {}

void CopyProcess::run() {
    // Hiển thị hướng dẫn nhập lệnh.
    view->displayMessage("Enter the copy command in the following format:");
    view->displayMessage("copy <source_file> <destination_file>");
    view->displayMessage("Enter 'exit' to quit the program.");

    // Các vector lưu trạng thái progress của từng job.
    vector<double> progressList;         // Lưu phần trăm tiến độ cho mỗi job.
    vector<string> jobDescriptions;        // Mô tả của mỗi job.
    mutex progressMutex;                   // Bảo vệ các biến trên.

    // Dùng vector để lưu giá trị tiến độ đã in cho mỗi job, giúp cập nhật lại chỉ khi có thay đổi.
    vector<int> lastPrinted;

    // Biến điều khiển vòng lặp cập nhật progress.
    atomic<bool> keepUpdating(true);

    // Thread cập nhật progress display cho tất cả các job.
    thread progressDisplayThread([&]() 
    {
        while (keepUpdating.load()) 
        {
                lock_guard<mutex> lock(progressMutex);
                if (!progressList.empty()) 
                {
                    // Nếu số lượng job tăng, resize vector lastPrinted
                    if (lastPrinted.size() < progressList.size()) 
                    {
                        lastPrinted.resize(progressList.size(), -1);
                    }
                    bool updateNeeded = false;
                    // Kiểm tra xem có job nào có tiến độ thay đổi không.
                    for (size_t i = 0; i < progressList.size(); i++) 
                    {
                        int currentProgress = static_cast<int>(progressList[i]);
                        if (currentProgress != lastPrinted[i]) 
                        {
                            updateNeeded = true;
                            lastPrinted[i] = currentProgress;
                        }
                    }
                    if (updateNeeded) 
                    {
                        // Lưu vị trí con trỏ hiện tại (vùng nhập).
                        cout << "\033[s";
                        // Di chuyển con trỏ lên số dòng tương ứng với số job.
                        cout << "\033[" << progressList.size() << "A";
                        // Cập nhật từng dòng progress cho mỗi job.
                        for (size_t i = 0; i < progressList.size(); i++) 
                        {
                            // "\r\033[K" xoá toàn bộ dòng hiện tại.
                            cout << "\r\033[K"
                                << jobDescriptions[i] << " Progress: "
                                << lastPrinted[i] << "%" << "\n";
                        }
                        // Khôi phục con trỏ ban đầu.
                        cout << "\033[u" << flush;
                    }
                }            
                this_thread::sleep_for(chrono::milliseconds(100));
        }
    });

    // Mutex để bảo vệ thông báo không thuộc progress display.
    mutex outputMutex;

    // Vòng lặp nhận lệnh copy từ người dùng.
    while (true) 
    
    {
        // In thêm một dòng trống để tách vùng progress và prompt.
        view->displayMessage("");
        // Hiển thị prompt ở dòng mới.
        view->displayMessage("> ", false);
        string line = view->getInput();

        // Nếu nhập rỗng, in dòng trống và tiếp tục.
        if (line.empty()) 
        {
            view->displayMessage("");
            continue;
        }
        // Nếu nhập "exit", thoát vòng lặp.
        if (line == "exit") 
        {
            break;
        }
        // Phân tích cú pháp lệnh.
        istringstream iss(line);
        string command, src, dest;
        iss >> command >> src >> dest;
        if (command != "copy" || src.empty() || dest.empty()) 
        {
            view->displayMessage("Invalid command. Please enter the following format:");
            view->displayMessage("  copy <source_file> <destination_file>");
            continue;
        }

        // Thêm thông tin của job mới vào các vector dùng cho progress.
        {
            lock_guard<mutex> lock(progressMutex);
            progressList.push_back(0.0);
            jobDescriptions.push_back("Job (" + src + " -> " + dest + ")");
        }
        // Lấy chỉ số job vừa được thêm.
        size_t jobIndex = progressList.size() - 1;

        // Khởi chạy job copy trên một thread riêng.
        jobThreads.push_back(thread([src, dest, jobIndex, this, &progressMutex, &progressList, &outputMutex]() {
            FileCopier copier(src, dest);
            // Callback cập nhật progress: cập nhật progressList[jobIndex].
            auto progressCallback = [jobIndex, &progressMutex, &progressList](double progress) {
                lock_guard<mutex> lock(progressMutex);
                progressList[jobIndex] = progress;
                };
            bool result = copier.copyFileMultiThreaded(progressCallback);
            {
                lock_guard<mutex> lock(progressMutex);
                progressList[jobIndex] = 100.0;
            }
            // Sau khi job copy kết thúc, in thông báo hoàn thành.
            {
                lock_guard<mutex> lock(outputMutex);
                if (result) 
                {
                    view->displayMessage("Job (" + src + " -> " + dest + ") completed successfully.");
                }
                else 
                {
                    view->displayMessage("Job (" + src + " -> " + dest + ") failed.");
                }
            }
            }));

        // In thông báo job đã được start (với dòng mới).
        view->displayMessage("Job (" + src + " -> " + dest + ") started.");
    }

    // Chờ tất cả các job copy hoàn thành.
    for (auto& t : jobThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    keepUpdating = false;
    if (progressDisplayThread.joinable()) {
        progressDisplayThread.join();
    }
    view->displayMessage("All jobs completed. Exiting program.");
}