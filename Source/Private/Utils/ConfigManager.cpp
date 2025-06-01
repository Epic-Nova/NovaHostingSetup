#include "NovaCore.h"
#include "NovaMinimal.h"

namespace Utils
{
    using namespace Core;
    using json = nlohmann::json;

    void ConfigManager::CreateConfigDirectory()
    {
        // Use NovaFileOperations instead of raw filesystem operations
        Core::FileOperations::NovaFileOperations::CreateDirectory("Configs");
        NOVA_LOG("Created Configs directory", LogType::Debug);
    }

    std::string ConfigManager::GetConfigPath()
    {
        return (std::filesystem::current_path() / "Configs" / CONFIG_FILENAME).string();
    }

    void ConfigManager::SaveConfig(const CommandLineOptionsStruct& config)
    {
        CreateConfigDirectory();
        
        json j;
        j["cmdOptions"]["clearContent"] = config.clearContent;
        j["cmdOptions"]["noRoot"] = config.noRoot;
        j["cmdOptions"]["verbose"] = config.verbose;
        j["mkdocsProjectPath"] = config.mkdocsProjectPath;
        j["requestRootForBrew"] = config.requestRootForBrew;
        j["requestRootForPip"] = config.requestRootForPip;
        j["requestRootForVenv"] = config.requestRootForVenv;
        j["scrollableLogAlwaysVisible"] = config.scrollableLogAlwaysVisible;

        // Use NovaFileOperations for file writing
        bool success = Core::FileOperations::NovaFileOperations::WriteTextFile(
            GetConfigPath(), j.dump(4), false);
            
        if (success) {
            NOVA_LOG("Configuration saved successfully", LogType::Log);
        } else {
            NOVA_LOG("Failed to save configuration", LogType::Error);
        }
    }

    CommandLineOptionsStruct ConfigManager::LoadConfig()
    {
        CommandLineOptionsStruct config;
        
        // Use NovaFileOperations for file reading
        if (Core::FileOperations::NovaFileOperations::FileExists(GetConfigPath())) {
            try {
                std::string fileContent = Core::FileOperations::NovaFileOperations::ReadTextFile(GetConfigPath());
                json j = json::parse(fileContent);
                
                config.clearContent = j.value("clearContent", false);
                config.noRoot = j.value("noRoot", false);
                config.verbose = j.value("verbose", false);
                config.mkdocsProjectPath = j.value("mkdocsProjectPath", "");
                config.requestRootForBrew = j.value("requestRootForBrew", true);
                config.requestRootForPip = j.value("requestRootForPip", true);
                config.requestRootForVenv = j.value("requestRootForVenv", true);
                config.scrollableLogAlwaysVisible = j.value("scrollableLogAlwaysVisible", false);
                
                NOVA_LOG("Configuration loaded successfully", LogType::Log);
            } catch (const std::exception& e) {
                NOVA_LOG(("Failed to parse configuration: " + std::string(e.what())).c_str(), LogType::Warning);
            }
        } else {
            NOVA_LOG("No existing configuration found, using defaults", LogType::Debug);
        }
        
        return config;
    }
}
