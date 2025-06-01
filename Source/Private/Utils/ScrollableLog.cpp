#include "Utils/ScrollableLog.h"

namespace Utils
{
    std::shared_ptr<ScrollableLog> ScrollableLog::Create()
    {
        auto instance = std::make_shared<ScrollableLog>();
        instance->logComponent_ = ftxui::Renderer([instance] {
            std::lock_guard<std::mutex> lock(instance->logMutex_);
            ftxui::Elements elements;
            for (const auto& message : instance->logMessages_)
            {
                elements.push_back(ftxui::text(message));
            }
            return ftxui::vbox(elements) | ftxui::vscroll_indicator | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 10);
        });
        return instance;
    }

    void ScrollableLog::AddMessage(const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logMutex_);
        logMessages_.push_back(message);
    }

    ftxui::Component ScrollableLog::GetComponent()
    {
        return ftxui::Renderer([this] {
            std::lock_guard<std::mutex> lock(logMutex_);
            ftxui::Elements elements;
            for (const auto& message : logMessages_)
            {
                elements.push_back(ftxui::text(message));
            }
            auto logBox = ftxui::vbox(elements) | ftxui::vscroll_indicator;
            return alwaysVisible_ ? logBox | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 10) : logBox | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 10);
        });
    }

    void ScrollableLog::SetAlwaysVisible(bool visible)
    {
        alwaysVisible_ = visible;
    }

    bool ScrollableLog::IsAlwaysVisible() const
    {
        return alwaysVisible_;
    }
}
