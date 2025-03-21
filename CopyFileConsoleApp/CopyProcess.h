﻿#pragma once
#include "ICopyView.h"
#include <memory>
#include <string>
#include <vector>
#include <thread>

// Lớp CopyProcess: Điều khiển luồng chương trình, nhận lệnh từ người dùng qua View,
// sau đó sử dụng Model (FileCopier) để thực hiện sao chép file.
class CopyProcess {
private:
    // Con trỏ đến interface ICopyView, đảm bảo không phụ thuộc vào một View cụ thể.
    // Sử dụng std::shared_ptr để lưu trữ đối tượng View, giúp quản lý bộ nhớ tự động.
    std::shared_ptr<ICopyView> view;
    // Danh sách các thread đang chạy copy job.
    std::vector<std::thread> jobThreads;
public:
    // Constructor: Tiêm phụ thuộc vào ICopyView thông qua std::shared_ptr.
    CopyProcess(std::shared_ptr<ICopyView> view);
    // Phương thức run: Điều khiển luồng ứng dụng.
    void run();
};