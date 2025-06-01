#pragma once

#include "NovaMinimal.h"
#include "Utils/CommandLineOptions.h"

namespace Utils
{
    class ConfigManager
    {
    public:
        static void SaveConfig(const CommandLineOptionsStruct& config);
        static CommandLineOptionsStruct LoadConfig();
        static void CreateConfigDirectory();
        static std::string GetConfigPath();
        
    private:
        static constexpr const char* CONFIG_FILENAME = "app_config.json";
    };
}
