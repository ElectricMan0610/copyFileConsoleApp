#include "FileCopier.h"

// Constructor: Gán giá trị cho các thuộc tính source và destination.
FileCopier::FileCopier(const string& src, const string& dest)
    : source(src), destination(dest) {
}

// Phương thức copyProgress: Đọc dữ liệu từ file nguồn và ghi vào file đích theo từng khối.
bool FileCopier::copyProgress(ifstream& inFile, ofstream& outFile) {
    // Khai báo buffer có kích thước BUFFER_SIZE để lưu dữ liệu tạm thời.
    char buffer[BUFFER_SIZE];

    // Vòng lặp: Đọc dữ liệu từ file nguồn cho đến khi không còn dữ liệu.
    while (inFile) {
        // Đọc tối đa BUFFER_SIZE byte từ file nguồn vào buffer.
        inFile.read(buffer, BUFFER_SIZE);
        // Lấy số byte thực sự đã đọc được (có thể ít hơn BUFFER_SIZE ở lần đọc cuối).
        streamsize bytesRead = inFile.gcount();
        // Nếu đã đọc được dữ liệu, ghi dữ liệu đó vào file đích.
        if (bytesRead > 0) {
            outFile.write(buffer, bytesRead);
        }
    }
    // Kiểm tra trạng thái của file đích; nếu có lỗi trong quá trình ghi, trả về false.
    return outFile.good();
}

// Phương thức copyFile: Mở file nguồn và file đích, sau đó gọi copyProgress để sao chép dữ liệu.
bool FileCopier::copyFile() {
    // Mở file nguồn ở chế độ nhị phân.
    ifstream inFile(source, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Err: Can't open source file" << source << endl;
        return false;
    }

    // Mở file đích ở chế độ nhị phân.
    ofstream outFile(destination, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Err: Can't open destination file" << destination << endl;
        inFile.close();
        return false;
    }

    // Gọi copyProgress để sao chép dữ liệu.
    bool success = copyProgress(inFile, outFile);

    // Đóng file nguồn và file đích.
    inFile.close();
    outFile.close();

    if (!success) {
        cerr << "Error when copy data from source to destination: " << destination << endl;
    }
    return success;
}
