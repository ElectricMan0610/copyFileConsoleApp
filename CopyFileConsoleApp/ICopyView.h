#pragma once
#include <string>

// Interface ICopyView: Định nghĩa các thao tác nhập xuất của giao diện người dùng.
class ICopyView {
public:
    virtual ~ICopyView() {}
    // Hiển thị thông báo ra màn hình. Nếu parameter newline là false thì không tự động xuống dòng.
    virtual void displayMessage(const std::string& message, bool newline = true) const = 0;
    // Lấy một dòng đầu vào từ người dùng.
    virtual std::string getInput() const = 0;
};
