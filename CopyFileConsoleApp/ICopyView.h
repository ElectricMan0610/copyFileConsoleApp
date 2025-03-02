#pragma once
#include <string>

// Interface ICopyView: Định nghĩa các thao tác nhập xuất của giao diện người dùng.
class ICopyView {
public:
    virtual ~ICopyView() {}
    // Hàm displayMessage: Hiển thị thông báo ra màn hình.
    virtual void displayMessage(const std::string& message, const bool isBreakLine = true) const = 0;
    // Hàm getInput: Lấy một dòng đầu vào từ người dùng.
    virtual std::string getInput() const = 0;
};
