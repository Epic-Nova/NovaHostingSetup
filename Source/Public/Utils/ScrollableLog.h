#pragma once

#include "NovaMinimal.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace Utils
{
    class ScrollableLog
    {
    public:
        static std::shared_ptr<ScrollableLog> Create();
        void AddMessage(const std::string& message);
        ftxui::Component GetComponent();
        void SetAlwaysVisible(bool visible);
        bool IsAlwaysVisible() const;

        ScrollableLog() = default; // Move constructor to public section

    private:
        std::mutex logMutex_;
        std::vector<std::string> logMessages_;
        bool alwaysVisible_ = false;
        ftxui::Component logComponent_;
    };
}
