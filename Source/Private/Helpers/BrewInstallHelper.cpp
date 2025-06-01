/// @file BrewInstallHelper.cpp
/// @brief macOS-specific Homebrew installation routines.

#include "NovaCore.h"
#include "NovaMinimal.h"

#include "Helpers/RootAccessHelper/RootAccessHelper.h"

#ifdef __APPLE__

#include "Helpers/BrewInstallHelper.h"

namespace Core::Helpers
{
    void BrewInstallHelper::Initialize()
    {
        FireInstallCallback("Initializing Homebrew Helper...");
        
        // Check if Homebrew is installed
        bIsBrewInstalled = IsBrewInstalled();
        if (!bIsBrewInstalled)
        {
            FireInstallCallback("Homebrew not found, will need to install it");
        }
        else
        {
            FireInstallCallback("Homebrew is already installed");
        }
        
        bIsInitialized = true;
        FireInstallCallback("Homebrew Helper initialized successfully");
    }

    void BrewInstallHelper::Execute(std::function<bool()> callback)
    {
        bIsRunning = true;
        FireInstallCallback("Starting Homebrew installation process...");
        
        if (IsBrewInstalled())
        {
            bIsBrewInstalled = true;
            FireInstallCallback("Homebrew is already installed - skipping installation");
            callback();
            bIsRunning = false;
            return;
        }

        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        rootAccessHelper->SetInstallCallbackFunction([this](const std::string& msg) {
            FireInstallCallback("Root Access: " + msg);
        });
        rootAccessHelper->Initialize();
        
        rootAccessHelper->Execute([&]() {
            FireInstallCallback("Downloading Homebrew installation script...");
            
            bool success = ExecuteCommandBlocking("bash -c \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"");
            
            if (success) {
                FireInstallCallback("Verifying Homebrew installation...");
                bIsBrewInstalled = IsBrewInstalled();
                
                if (bIsBrewInstalled) {
                    FireInstallCallback("Homebrew installation completed successfully");
                } else {
                    FireInstallCallback("Installation script completed but Homebrew not found in PATH");
                }
            } else {
                FireInstallCallback("Homebrew installation failed");
            }
            
            callback();
            bIsRunning = false;
            delete rootAccessHelper;
            return success;
        });
        
        // Wait for completion
        WaitForCompletion();
    }

    void BrewInstallHelper::Shutdown()
    {
        //Uninstall Homebrew if it was installed, before request root access
        if (IsBrewInstalled())
        {
            RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
            rootAccessHelper->Initialize();
            rootAccessHelper->Execute([&]() {
                if (rootAccessHelper->RunCommandWithElevatedPrivileges("brew uninstall --force brew"))
                {
                    bIsBrewInstalled = false; // Update the flag after uninstallation
                    NOVA_LOG("Homebrew has been uninstalled successfully.", Core::LogType::Log);
                    FireInstallCallback("Homebrew has been uninstalled successfully.");
                    return true;
                }
                else
                {
                    NOVA_LOG("Failed to uninstall Homebrew.", Core::LogType::Error);
                    FireInstallCallback("Failed to uninstall Homebrew.");
                    return false;
                }
            });
        }
        else
        {
            NOVA_LOG("Homebrew is not installed, nothing to uninstall.", Core::LogType::Debug);
            FireInstallCallback("Homebrew is not installed, nothing to uninstall.");
        }
        bIsBrewInstalled = false; // Reset the flag after shutdown
        NOVA_LOG("BrewInstallHelper_Darwin shutdown completed.", Core::LogType::Debug);
        FireInstallCallback("BrewInstallHelper_Darwin has been shut down.");
        bIsInitialized = false; // Reset the initialization flag
    }

    void BrewInstallHelper::Reset()
    {
        bIsInitialized = false; // Reset the initialization flag
        bIsBrewInstalled = false; // Reset the Homebrew installation flag
        NOVA_LOG("Brew installation process has been reset.", Core::LogType::Debug);
        FireInstallCallback("Brew installation process has been reset.");
    }

    void BrewInstallHelper::Abort()
    {
        //we need to fetch the current terminal instance that handles the installation and kill it
        Reset();
        NOVA_LOG("Brew installation process has been aborted.", Core::LogType::Debug);
        FireInstallCallback("Brew installation process has been aborted.");
        bIsAborted = true; // Set the aborted flag
        bIsRunning = false; // Reset the running flag
        bIsInitialized = false; // Reset the initialization flag
    }

    bool BrewInstallHelper::IsBrewInstalled() const
    {
        // Check if Homebrew is installed by checking the command availability
        int result = system("which brew > /dev/null 2>&1");
        return (result == 0);
    }

    void BrewInstallHelper::InstallPackage(const std::string& packageName, std::function<void(bool)> callback)
    {
        if (!bIsBrewInstalled)
        {
            FireInstallCallback("Cannot install package: Homebrew is not installed");
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