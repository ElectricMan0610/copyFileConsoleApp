#include "FileCopier.h"
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

// Constructor: Gán giá trị cho source và destination.
FileCopier::FileCopier(const string& src, const string& dest)
    : source(src), destination(dest) {
}

// Phương thức copyProgress: sao chép dữ liệu từ inFile sang outFile theo từng khối.
bool FileCopier::copyProgress(ifstream& inFile, ofstream& outFile) {
    char buffer[BUFFER_SIZE];
    while (inFile) {
        inFile.read(buffer, BUFFER_SIZE);
        streamsize bytesRead = inFile.gcount();
        if (bytesRead > 0) {
            outFile.write(buffer, bytesRead);
        }
    }
    return outFile.good();
}

// Phương thức copyFile: copy file đơn luồng.
bool FileCopier::copyFile() {
    ifstream inFile(source, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Error: cannot open source file: " << source << endl;
        return false;
    }
    ofstream outFile(destination, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Error: cannot open destination file: " << destination << endl;
        inFile.close();
        return false;
    }
    bool success = copyProgress(inFile, outFile);
    inFile.close();
    outFile.close();
    if (!success) {
        cerr << "Error writing to destination file: " << destination << endl;
    }
    return success;
}

// Phương thức copyFileMultiThreaded: sao chép file đa luồng với progress callback.
bool FileCopier::copyFileMultiThreaded(function<void(double)> progressCallback) {
    // Mở file nguồn ở chế độ nhị phân để lấy kích thước của file.
    ifstream srcFile(source, ios::binary);
    if (!srcFile.is_open()) {
        cerr << "Error: cannot open source file: " << source << endl;
        return false;
    }
    srcFile.seekg(0, ios::end);
    auto fileSize = srcFile.tellg();
    srcFile.close();

    if (fileSize <= 0) {
        cerr << "Error: source file is empty or invalid." << endl;
        return false;
    }

    // Pre-allocate file đích: mở file đích và đặt kích thước giống file nguồn.
    {
        ofstream destFile(destination, ios::binary);
        if (!destFile.is_open()) {
            cerr << "Error: cannot open destination file: " << destination << endl;
            return false;
        }
        // Đưa con trỏ đến vị trí cuối file (fileSize - 1) và ghi 1 byte để đặt kích thước file.
        destFile.seekp(static_cast<std::streamoff>(fileSize) - 1);
        destFile.write("", 1);
        destFile.close();
    }

    // Khởi tạo biến atomic để theo dõi số byte đã copy.
    atomic<long long> bytesCopied(0);
    const long long totalSize = fileSize;

    // Tạo thread riêng để cập nhật tiến độ.
    bool copyFinished = false;
    thread progressThread([&]() {
        while (!copyFinished) {
            // Tính phần trăm tiến độ dựa trên bytesCopied và totalSize.
            double progress = (static_cast<double>(bytesCopied.load()) / totalSize) * 100.0;
            // Gọi callback với giá trị tiến độ hiện tại.
            progressCallback(progress);
            // Tạm dừng 100ms trước khi cập nhật tiếp.
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        // Khi copy hoàn thành, đảm bảo gọi callback với 100%.
        progressCallback(100.0);
        });

    // Copy file một cách tuần tự trong một vòng lặp.
    ifstream in(source, ios::binary);
    ofstream out(destination, ios::binary);
    if (!in.is_open() || !out.is_open()) {
        cerr << "Error: cannot open source or destination file." << endl;
        return false;
    }
    const size_t bufferSize = 4096;
    vector<char> buffer(bufferSize);
    while (in) {
        in.read(buffer.data(), bufferSize);
        streamsize bytesRead = in.gcount();
        if (bytesRead <= 0) break;
        out.write(buffer.data(), bytesRead);
        // Cập nhật số byte đã copy (atomic đảm bảo an toàn giữa các thread).
        bytesCopied += bytesRead;
    }

    // Đóng file nguồn và file đích.
    in.close();
    out.close();

    // Đánh dấu quá trình copy đã hoàn thành.
    copyFinished = true;
    if (progressThread.joinable()) {
        progressThread.join();
    }

    return true;
}
