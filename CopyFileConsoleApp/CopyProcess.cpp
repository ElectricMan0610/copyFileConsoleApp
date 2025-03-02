#include "CopyProcess.h"
#include "FileCopier.h"
#include <sstream>
#include <iostream>

using namespace std;

CopyProcess::CopyProcess(std::shared_ptr<ICopyView> view) : view(view) {}

void CopyProcess::run() {
    // Hiển thị hướng dẫn nhập lệnh.
    view->displayMessage("Enter the copy command in the following format:");
    view->displayMessage("copy <source_file> <destination_file>");
    view->displayMessage("Enter 'exit' to quit the program.");

    // Vòng lặp: Liên tục nhận lệnh từ người dùng.
    while (true) {
        view->displayMessage("> ", false);
        std::string line = view->getInput();

        // Nếu người dùng chỉ nhấn phím Enter (dữ liệu rỗng), in xuống dòng và quay lại chờ lệnh.
        if (line.empty()) {
            // In dòng trống để thực hiện "xuống dòng" và tiếp tục vòng lặp.
            view->displayMessage("");
            continue;
        }

        // Nếu người dùng nhập "exit", kết thúc ứng dụng.
        if (line == "exit") {
            break;
        }

        // Phân tích cú pháp của dòng lệnh.
        istringstream iss(line);
        string command, src, dest;
        iss >> command >> src >> dest;

        // Kiểm tra cú pháp: lệnh phải là "copy" và các đường dẫn không được rỗng.
        if (command != "copy" || src.empty() || dest.empty()) {
            view->displayMessage("Invalid command. Please enter the following format:");
            view->displayMessage("  copy <source_file> <destination_file>");
            continue;
        }

        // Tạo đối tượng FileCopier (Model) với đường dẫn file nguồn và file đích.
        FileCopier copier(src, dest);
        // Thực hiện sao chép file và hiển thị kết quả.
        if (copier.copyFile()) {
            view->displayMessage("File copy successful!");
        }
        else {
            view->displayMessage("File copy failed.");
        }
    }
}
