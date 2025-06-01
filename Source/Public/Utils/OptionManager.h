#pragma once

#include "NovaMinimal.h"
#include <unordered_map>
#include <functional>
#include <string>

namespace Utils
{
    class OptionManager
    {
    public:
        OptionManager() = default;

        // Register an option with its associated behavior
        void RegisterOption(const std::string& name, std::function<void()> handler)
        {
            optionHandlers_[name] = handler;
        }

        // Execute the behavior for a specific option
        void ExecuteOption(const std::string& name)
        {
            if (optionHandlers_.find(name) != optionHandlers_.end()) {
                optionHandlers_[name]();
            } else {
                NOVA_LOG(("Option not found: " + name).c_str(), LogType::Warning);
            }
        }

        // Execute all enabled options
        void ExecuteEnabledOptions(const std::unordered_map<std::string, bool*>& optionMapping)
        {
            for (const auto& [name, enabled] : optionMapping) {
                if (*enabled) {
                    ExecuteOption(name);
                }
            }
        }

    private:
        std::unordered_map<std::string, std::function<void()>> optionHandlers_;
    };
}
