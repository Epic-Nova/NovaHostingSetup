/// @file BrewInstallHelper.cpp
/// @brief macOS-specific Homebrew installation routines.

#include "NovaCore.h"
#include "NovaMinimal.h"
#include "NovaLog.h"

#include "Helpers/RootAccessHelper/RootAccessHelper.h"

#ifdef __APPLE__

#include "Helpers/BrewInstallHelper.h"

namespace Core::Helpers
{
    bool BrewInstallHelper::Initialize()
    {
        NOVA_LOG_VERBOSE("BrewInstallHelper_Darwin initializing...", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.initializing");
        
        bIsInitialized = IsBrewInstalled();
        if (!bIsInitialized)
        {
            NOVA_LOG("Homebrew is not installed, will need to install it.", Core::LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.not_installed");
        }
        else
        {
            NOVA_LOG("Homebrew is already installed on macOS.", Core::LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.already_installed");
        }
        
        NOVA_LOG_VERBOSE("BrewInstallHelper_Darwin initialized successfully.", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.initialized");
        return bIsInitialized;
    }
    
    void BrewInstallHelper::Shutdown()
    {
        if (IsBrewInstalled())
        {
            RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
            if (!rootAccessHelper->Initialize())
            {
                NOVA_LOG("Failed to initialize RootAccessHelper for shutdown.", Core::LogType::Error);
                FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.shutdown_failed");
                return;
            }
            
            rootAccessHelper->Execute([&]() {
                if (rootAccessHelper->RunCommandWithElevatedPrivileges("brew uninstall --force brew", [](const std::string& msg) {
                    // Empty callback
                }))
                {
                    bIsInitialized = false; 
                    NOVA_LOG("Homebrew has been uninstalled successfully.", Core::LogType::Log);
                    FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.uninstalled");
                    return true;
                }
                else
                {
                    NOVA_LOG("Failed to uninstall Homebrew.", Core::LogType::Error);
                    FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.uninstall_failed");
                    return false;
                }
            });
        }
        else
        {
            bIsInitialized = false; 
            NOVA_LOG("Homebrew is not installed, nothing to uninstall.", Core::LogType::Debug);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.not_installed");
        }
        NOVA_LOG("BrewInstallHelper_Darwin shutdown completed.", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.shutdown");
    }
    void BrewInstallHelper::Execute(std::function<bool()> callback)
    {
        bIsRunning = true;

        NOVA_LOG_VERBOSE("BrewInstallHelper_Darwin executing installation...", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.executing");
        
        if (IsBrewInstalled())
        {
            NOVA_LOG("Homebrew is already installed, skipping installation.", Core::LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.already_installed");
            callback();
            bIsRunning = false;
            return;
        }

        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        rootAccessHelper->SetInstallCallbackFunction("root_access_progress", [this](const std::string& msg) {
            FireInstallCallback("Root Access: " + msg);
        });
        
        if (!rootAccessHelper->Initialize())
        {
            NOVA_LOG("Failed to initialize RootAccessHelper for installation.", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.root_access_not_initialized");
            bIsRunning = false;
            return;
        }
        
        rootAccessHelper->Execute([&]() {
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.installing");
            
            bool success = ExecuteCommandBlocking("bash -c \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"");
            
            if (success) 
            {
                NOVA_LOG("Homebrew installation script executed successfully.", Core::LogType::Log);
                FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.installation_completed");
                callback();
                
                if (bIsInitialized) {

                    NOVA_LOG("Homebrew is now installed and available in PATH.", Core::LogType::Log);
                    FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.installation_script_completed");
                    callback();
                } 
                else 
                {
                    NOVA_LOG("Homebrew installation script completed but Homebrew not found in PATH.", Core::LogType::Warning);
                    FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.installation_script_completed");
                    callback();
                }
            } 
            else 
            {
                NOVA_LOG("Homebrew installation failed.", Core::LogType::Error);
                FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.installation_failed");
            }
            
            bIsRunning = false;
            delete rootAccessHelper;
            return success;
        });
        
        WaitForCompletion();
    }


    void BrewInstallHelper::Reset()
    {
        NOVA_LOG("Brew installation process has been reset.", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.reset");

        bIsInitialized = false;
    }

    void BrewInstallHelper::Abort()
    {
        NOVA_LOG("Brew installation process has been aborted.", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.aborted");

        bIsRunning = false;
        bIsInitialized = false;
    }

    bool BrewInstallHelper::IsBrewInstalled() const
    {
        int result = system("which brew > /dev/null 2>&1");
        return (result == 0);
    }

    void BrewInstallHelper::InstallPackage(const std::string& packageName, std::function<void(bool)> callback)
    {
        if (!bIsInitialized)
        {
            NOVA_LOG("Cannot install package: Homebrew is not installed", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.package_install_failed");
            callback(false);
            return;
        }

        bIsRunning = true;
        FireInstallCallback("Installing package: " + packageName);
        
        std::string command = "brew install " + packageName;
        bool success = ExecuteCommandBlocking(command);
        
        if (success) {
            FireInstallCallback("Package " + packageName + " installed successfully");
            callback(true);
        } else {
            FireInstallCallback("Failed to install package " + packageName);
            callback(false);
        }
        
        bIsRunning = false;
    }

    void BrewInstallHelper::UninstallPackage(const std::string& packageName, std::function<void(bool)> callback)
    {
        if (!bIsInitialized)
        {
            NOVA_LOG("Cannot uninstall package: Homebrew is not installed", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.package_uninstall_failed");
            callback(false);
            return;
        }

        bIsRunning = true;
        FireInstallCallback("Uninstalling package: " + packageName);
        
        std::string command = "brew uninstall " + packageName;
        bool success = ExecuteCommandBlocking(command);
        
        if (success)
        {
            std::string successMsg = "Package " + packageName + " uninstalled successfully";
            NOVA_LOG(successMsg.c_str(), Core::LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.package_uninstalled");
            callback(true);
        }
        else 
        {
            std::string errorMsg = "Failed to uninstall package " + packageName;
            NOVA_LOG(errorMsg.c_str(), Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.brew_install_helper.package_uninstall_failed");
            callback(false);
        }
        
        bIsRunning = false;
    }

    bool BrewInstallHelper::RunCommandWithProgress(const std::string& command)
    {
        FireInstallCallback("Executing: " + command);

        // Suppress output to prevent GUI interference
        std::string fullCommand = command + " > /dev/null 2>&1";
        int result = system(fullCommand.c_str());
        
        if (result == 0) {
            FireInstallCallback("Command completed successfully");
            return true;
        } else {
            FireInstallCallback("Command failed with return code: " + std::to_string(result));
            return false;
        }
    }

} // namespace Core::Helpers

#endif