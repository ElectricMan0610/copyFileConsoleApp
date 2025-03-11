#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <functional>

using namespace std;

// Lớp FileCopier: Đóng gói các thao tác sao chép file.
// Có 2 phương thức copy:
// - copyFile() : copy đơn luồng.
// - copyFileMultiThreaded() : copy đa luồng với progress callback.
class FileCopier {
private:
    // Đường dẫn file nguồn.
    string source;
    // Đường dẫn file đích.
    string destination;
    // Kích thước của buffer dùng để sao chép (4096 byte).
    static const size_t BUFFER_SIZE = 4096;

    // Phương thức copyProgress: sao chép dữ liệu từ inFile sang outFile theo từng khối.
    bool copyProgress(ifstream& inFile, ofstream& outFile);

public:
    // Constructor: khởi tạo với đường dẫn file nguồn và đích.
    FileCopier(const string& src, const string& dest);

    // Sao chép file đơn luồng.
    bool copyFile();

    // Sao chép file đa luồng với progress callback.
    // progressCallback: hàm callback nhận tham số progress (0 đến 100).
    // numThreads: số luồng sử dụng để copy.
    bool copyFileMultiThreaded(function<void(double)> progressCallback);
};
