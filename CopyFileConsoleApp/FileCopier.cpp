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
bool FileCopier::copyFileMultiThreaded(function<void(double)> progressCallback, int numThreads) {
    // Mở file nguồn để lấy kích thước file.
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

    // Pre-allocate destination file: mở file đích và đặt kích thước.
    {
        ofstream destFile(destination, ios::binary);
        if (!destFile.is_open()) {
            cerr << "Error: cannot open destination file: " << destination << endl;
            return false;
        }
        destFile.seekp(static_cast<std::streamoff>(fileSize) - 1);
        destFile.write("", 1);
        destFile.close();
    }

    atomic<long long> bytesCopied(0);
    const long long totalSize = fileSize;

    // Tính kích thước của mỗi đoạn.
    long long segmentSize = totalSize / numThreads;
    long long remainder = totalSize % numThreads;

    vector<thread> threads;

    // Lambda copySegment: copy một đoạn của file.
    auto copySegment = [&](int threadIndex, long long offset, long long size) {
        ifstream in(source, ios::binary);
        ofstream out(destination, ios::binary);
        if (!in.is_open() || !out.is_open()) {
            return;
        }
        in.seekg(offset);
        out.seekp(offset);
        const size_t localBufferSize = 4096;
        char localBuffer[localBufferSize];
        long long bytesRemaining = size;
        while (bytesRemaining > 0 && in) {
            size_t toRead = (bytesRemaining > localBufferSize) ? localBufferSize : static_cast<size_t>(bytesRemaining);
            in.read(localBuffer, toRead);
            streamsize bytesRead = in.gcount();
            if (bytesRead <= 0) break;
            out.write(localBuffer, bytesRead);
            bytesCopied += bytesRead;
            bytesRemaining -= bytesRead;
        }
        };

    long long currentOffset = 0;
    for (int i = 0; i < numThreads; i++) {
        long long currentSegmentSize = segmentSize;
        if (i == numThreads - 1) {
            currentSegmentSize += remainder;
        }
        threads.push_back(thread(copySegment, i, currentOffset, currentSegmentSize));
        currentOffset += currentSegmentSize;
    }

    // Thread cập nhật progress.
    bool copyFinished = false;
    thread progressThread([&]() {
        while (!copyFinished) {
            double progress = (static_cast<double>(bytesCopied.load()) / totalSize) * 100.0;
            progressCallback(progress);
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        progressCallback(100.0);
        });

    // Chờ các thread copy hoàn thành.
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    copyFinished = true;
    if (progressThread.joinable()) {
        progressThread.join();
    }
    return true;
}
