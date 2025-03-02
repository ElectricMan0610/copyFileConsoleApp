#pragma once
#include "ICopyView.h"

// Lớp ConsoleCopyView: Cài đặt giao diện nhập xuất trên console, thực hiện theo interface ICopyView.
class ConsoleCopyView : public ICopyView {
public:
    virtual void displayMessage(const std::string& message, const bool isBreakLine = true) const override;
    virtual std::string getInput() const override;
};