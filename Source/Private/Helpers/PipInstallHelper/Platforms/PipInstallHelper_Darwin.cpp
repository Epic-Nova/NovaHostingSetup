#ifdef __APPLE__

#include "Helpers/PipInstallHelper/Platforms/PipInstallHelper_Darwin.h"
#include "Helpers/BrewInstallHelper.h"
#include "NovaCore.h"

namespace Core::Helpers
{
    void PipInstallHelper_Darwin::Initialize()
    {
        if(!HasMetRequirements())
        {
            NOVA_LOG("Requirements for pip installation not met", LogType::Error);
            FireInstallCallback("Requirements for pip installation not met.");
            return;
        }

        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        rootAccessHelper->Initialize();
        rootAccessHelper->Execute([&]() {
            // Check if pip is already installed
            if (HasPipInstalled())
            {
                NOVA_LOG("Pip is already installed.", LogType::Log);
                FireInstallCallback("Pip is already installed.");
                return true;
            }
            else
            {
                // Install pip using Homebrew
                rootAccessHelper->RunCommandWithElevatedPrivileges("brew install pip");
                // We need to forward the progress of the installation to our install screen
                NOVA_LOG("Installing pip using Homebrew...", LogType::Log);
                FireInstallCallback("Installing pip using Homebrew...");

                bHasPipInstalled = true;
                NOVA_LOG("Pip has been installed successfully.", LogType::Log);
                FireInstallCallback("Pip has been installed successfully.");
                return true;
            }
        });
        NOVA_LOG("PipInstallHelper_Darwin initialized", LogType::Log);
        FireInstallCallback("PipInstallHelper_Darwin has been initialized.");
        bIsInitialized = true;
    }

    void PipInstallHelper_Darwin::Shutdown()
    {
        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        rootAccessHelper->Initialize();
        rootAccessHelper->Execute([&]() {
        if (bHasPipInstalled)
        {
            rootAccessHelper->RunCommandWithElevatedPrivileges("pip uninstall -y pip");
            // We need to forward the progress of the uninstallation to our install screen

    
            bHasPipInstalled = false;
            NOVA_LOG("Pip has been uninstalled successfully.", LogType::Log);
            FireInstallCallback("Pip has been uninstalled successfully.");
        }
        else
        {
            NOVA_LOG("Pip is not installed, nothing to uninstall.", LogType::Debug);
            FireInstallCallback("Pip is not installed, nothing to uninstall.");
        }
        return true;
        });

        NOVA_LOG("PipInstallHelper_Darwin shutdown", LogType::Log);
        FireInstallCallback("PipInstallHelper_Darwin has been shut down.");
        bIsInitialized = false;
    }

    void PipInstallHelper_Darwin::Execute(std::function<bool()> callback)
    {
        //Request elevated privileges if necessary
        if (!HasMetRequirements())
        {
            NOVA_LOG("Requirements for pip installation not met", LogType::Error);
            FireInstallCallback("Requirements for pip installation not met.");
            return;
        }
        else
        {
            RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
            rootAccessHelper->Initialize();
            rootAccessHelper->Execute([&]() {
                // Check if pip is already installed
                if (HasPipInstalled())
                {
                    NOVA_LOG("Pip is already installed.", LogType::Log);
                    FireInstallCallback("Pip is already installed.");
                    callback();
                    return true;
                }
                else
                {
                    // Install pip using Homebrew
                    rootAccessHelper->RunCommandWithElevatedPrivileges("brew install pip");
                    // We need to forward the progress of the installation to our install screen
                    NOVA_LOG("Installing pip using Homebrew...", LogType::Log);
                    FireInstallCallback("Installing pip using Homebrew...");

                    bHasPipInstalled = true;
                    NOVA_LOG("Pip has been installed successfully.", LogType::Log);
                    FireInstallCallback("Pip has been installed successfully.");
                    callback();
                    return true;
                }
            });
            NOVA_LOG("PipInstallHelper_Darwin initialized", LogType::Log);
            FireInstallCallback("PipInstallHelper_Darwin has been initialized.");
            bIsInitialized = true;  
        }
    }

    void PipInstallHelper_Darwin::Reset()
    {
        Abort();
        bHasPipInstalled = false;
        NOVA_LOG("Pip installation reset", LogType::Log);
        FireInstallCallback("Pip installation has been reset.");
    }

    void PipInstallHelper_Darwin::Abort()
    {
        //We need to kill the terminal process that is running the pip installation
        bHasPipInstalled = false;
        NOVA_LOG("Pip installation aborted", LogType::Log);
        FireInstallCallback("Pip installation has been aborted.");
    }

    bool PipInstallHelper_Darwin::HasMetRequirements() const
    {
        BrewInstallHelper* brewHelper = BrewInstallHelper::CreatePlatformSpecific();
        brewHelper->Initialize();
        return brewHelper->IsBrewInstalled();
    }

    void PipInstallHelper_Darwin::InstallPackage(const std::string& packageName, std::function<void(std::string)> callback)
    {
        if(!HasPipInstalled())
        {
            NOVA_LOG(("Pip is not installed, cannot install package: " + packageName).c_str(), LogType::Error);
            callback("Pip is not installed.");
            FireInstallCallback("Pip is not installed, cannot install package: " + packageName);
            return;
        }

        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        rootAccessHelper->Initialize();
        rootAccessHelper->Execute([&]() {
            std::string command = "pip install " + packageName;
            // We need to forward the progress of the installation to our install screen
            if (rootAccessHelper->RunCommandWithElevatedPrivileges(command))
            {
                NOVA_LOG(("Package " + packageName + " installed successfully.").c_str(), LogType::Log);
                callback("Package " + packageName + " installed successfully.");
                FireInstallCallback("Package " + packageName + " installed successfully.");
                return true;
            }
            else
            {
                NOVA_LOG(("Failed to install package: " + packageName).c_str(), LogType::Error);
                callback("Failed to install package: " + packageName);
                FireInstallCallback("Failed to install package: " + packageName);
                return false;
            }
        });
    }
} // namespace Core::Helper

#endif