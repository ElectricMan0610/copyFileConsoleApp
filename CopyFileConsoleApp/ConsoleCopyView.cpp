#include "ConsoleCopyView.h"
#include <iostream>
#include <string>

using namespace std;

// Triển khai hàm displayMessage: Hiển thị thông báo ra console.
void ConsoleCopyView::displayMessage(const string& message, const bool isBreakLine) const {
    if (isBreakLine)
    {
        std::cout << message << std::endl;
    }
    else
    {
        std::cout << message;
    }
}

// Triển khai hàm getInput: Lấy một dòng đầu vào từ người dùng.
std::string ConsoleCopyView::getInput() const {
    std::string input;
    getline(std::cin, input);
    return input;
}
