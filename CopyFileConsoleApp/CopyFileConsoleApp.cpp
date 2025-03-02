#include "CopyProcess.h"
#include "ConsoleCopyView.h"

int main() {
    // Sử dụng std::shared_ptr để quản lý đối tượng view theo interface ICopyView.
    std::shared_ptr<ICopyView> view = std::make_shared<ConsoleCopyView>();
    // Tạo đối tượng CopyProcess và tiêm đối tượng view vào.
    std::unique_ptr<CopyProcess> controller = std::make_unique<CopyProcess>(view);
    // Chạy ứng dụng.
    controller->run();
    return 0;
}
