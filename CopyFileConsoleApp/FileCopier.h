#pragma once

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// Lớp FileCopier: Đóng gói các thao tác sao chép file.
// Nhiệm vụ: Mở file nguồn và file đích, sau đó sao chép dữ liệu theo từng khối.
class FileCopier {
private:
    // Đường dẫn file nguồn.
    string source;
    // Đường dẫn file đích.
    string destination;
    // Kích thước của buffer dùng để đọc dữ liệu theo từng khối (4096 byte).
    static const size_t BUFFER_SIZE = 4096;

    // Phương thức copyProgress: Thực hiện sao chép dữ liệu từ file nguồn sang file đích theo từng khối.
    // Tham số:
    //   inFile: đối tượng ifstream đã mở file nguồn ở chế độ nhị phân.
    //   outFile: đối tượng ofstream đã mở file đích ở chế độ nhị phân.
    // Trả về: true nếu sao chép thành công, false nếu có lỗi.
    bool copyProgress(ifstream& inFile, ofstream& outFile);

public:
    // Constructor: Khởi tạo đối tượng FileCopier với đường dẫn file nguồn và file đích.
    FileCopier(const string& src, const string& dest);

    // Phương thức copyFile: Mở file nguồn, file đích và sao chép dữ liệu.
    // Trả về: true nếu sao chép thành công, false nếu có lỗi.
    bool copyFile();
};



