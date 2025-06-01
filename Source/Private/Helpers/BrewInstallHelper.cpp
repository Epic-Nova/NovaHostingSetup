#ifdef __APPLE__

#include "Helpers/BrewInstallHelper.h"
#include <cstdlib>
#include "NovaCore.h"

namespace Core::Helpers
{
    void BrewInstallHelper::Initialize()
    {
        // Check if Homebrew is installed
        bIsBrewInstalled = IsBrewInstalled();
        if (!bIsBrewInstalled)
        {
            Execute([&]() {
                if (bIsBrewInstalled)
                {
                    FireInstallCallback("Homebrew was successfully installed.");
                    NOVA_LOG("Homebrew was successfully installed.", Core::LogType::Debug);
                    return true;
                }
                else
                {
                    FireInstallCallback("Failed to verify Homebrew installation.");
                    NOVA_LOG("Failed to verify Homebrew installation.", Core::LogType::Error);
                    return false;
                }
            });
        }
    }

    void BrewInstallHelper::Execute(std::function<bool()> callback)
    {
        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        if (rootAccessHelper)
        {
            rootAccessHelper->Initialize();
            if (!rootAccessHelper->HasRootAccess())
            {
                NOVA_LOG("Requesting elevated privileges for Homebrew installation...", Core::LogType::Log);
                rootAccessHelper->Execute([&]() {
                    
                    rootAccessHelper->RunCommandWithElevatedPrivileges("bash -c \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"");
                    //We need to forward the prgress of the download and installation to our install screen

                    callback();
                    NOVA_LOG("Homebrew installation process completed.", Core::LogType::Debug);
                    return true;
                });
            }
        }
        else
        {
            NOVA_LOG("RootAccessHelper instance is null. Cannot proceed with Homebrew installation.", Core::LogType::Error);
            return;
        }
    }

    void BrewInstallHelper::Shutdown()
    {
        //Uninstall Homebrew if it was installed, before request root access
        if (bIsBrewInstalled)
        {
            RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
            if (rootAccessHelper)
            {
                rootAccessHelper->Initialize();
                if (!rootAccessHelper->HasRootAccess())
                {
                    NOVA_LOG("Requesting elevated privileges for Homebrew uninstallation...", Core::LogType::Log);
                    rootAccessHelper->Execute([&]() {
                        rootAccessHelper->RunCommandWithElevatedPrivileges("brew uninstall --force brew");
                        //we need to forward the progress of the uninstallation to our install screen
                        return true;
                    });
                }
            }
            else
            {
                NOVA_LOG("RootAccessHelper instance is null. Cannot proceed with Homebrew uninstallation.", Core::LogType::Error);
                return;
            }
        }
        else
        {
            NOVA_LOG("Homebrew is not installed, nothing to uninstall.", Core::LogType::Debug);
        }
    }

    void BrewInstallHelper::Reset()
    {
        bIsBrewInstalled = false;
        NOVA_LOG("Brew installation process has been reset.", Core::LogType::Debug);
    }

    void BrewInstallHelper::Abort()
    {
        //we need to fetch the current terminal instance that handles the installation and kill it
        Reset();
        NOVA_LOG("Brew installation process has been aborted.", Core::LogType::Debug);
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
            NOVA_LOG("Cannot install package: Homebrew is not installed.", Core::LogType::Error);
            callback(false);
            return;
        }

        std::string command = "brew install " + packageName;
        //We need to forward the progress of the installation to our install screen
        int result = system(command.c_str());
        
        if (result == 0)
        {
            NOVA_LOG(("Package " + packageName + " installed successfully.").c_str(), Core::LogType::Debug);
            callback(true);
        }
        else
        {
            NOVA_LOG(("Failed to install package " + packageName + ".").c_str(), Core::LogType::Error);
            callback(false);
        }
    }

} // namespace Core::Helpers

#endif