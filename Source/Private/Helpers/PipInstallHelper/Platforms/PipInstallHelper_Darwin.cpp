/// @file PipInstallHelper_Darwin.cpp
/// @brief macOS-specific pip install helper.

#ifdef __APPLE__

#include "Helpers/PipInstallHelper/Platforms/PipInstallHelper_Darwin.h"
#include "Helpers/BrewInstallHelper.h"
#include "Helpers/RootAccessHelper/RootAccessHelper.h"
#include "NovaCore.h"
#include <future>
#include <chrono>

namespace Core::Helpers
{
    bool PipInstallHelper_Darwin::Initialize()
    {
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.initializing");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_already_installed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_not_installed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.initialized");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.brew_not_initialized");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.brew_not_installed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.root_access_not_initialized");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.uninstalling_pip");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_uninstalled");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.shutdown");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.shutdown_failed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.starting_installation");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.requirements_not_met");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_installed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.installing_pip");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_install_failed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_install_success");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.package_installing");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.package_install_failed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.package_install_success");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.reset");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.aborted");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_verifying_installation");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_install_timed_out");


        bIsInitialized = HasPipInstalled();
        
        if (bIsInitialized) 
        {
            NOVA_LOG("Pip3 is installed and accessible.", Core::LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_installed");
        } 
        else 
        {
            NOVA_LOG("Pip is not installed, will need to install it.", Core::LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_not_installed");
        }
        
        NOVA_LOG("PipInstallHelper_Darwin initialized", Core::LogType::Log);
        FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.initialized");
        return bIsInitialized;
    }

    void PipInstallHelper_Darwin::Shutdown()
    {
        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        if(!rootAccessHelper->Initialize())
        {
            NOVA_LOG("Failed to initialize RootAccessHelper for shutdown", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.shutdown_failed");
            return;
        }

        rootAccessHelper->Execute([&]() {
            if (bIsInitialized)
            {
                FireInstallCallback("Uninstalling pip...");
                rootAccessHelper->RunCommandWithElevatedPrivileges("pip uninstall -y pip", [](const std::string& msg) {
                    // Empty callback
                });
    
                bIsInitialized = false;
                NOVA_LOG("Pip has been uninstalled successfully.", Core::LogType::Log);
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_uninstalled");
            }
            else
            {
                NOVA_LOG("Pip is not installed, nothing to uninstall.", Core::LogType::Debug);
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_not_installed");
                Reset();
            }
            return true;
        });

        delete rootAccessHelper;
        NOVA_LOG("PipInstallHelper_Darwin shutdown", Core::LogType::Log);
        FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.shutdown");
        bIsInitialized = false;
        bIsRunning = false;
    }

    void PipInstallHelper_Darwin::Execute(std::function<bool()> callback)
    {
        bIsRunning = true;
        FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.starting_installation");
        
        if (!HasMetRequirements())
        {
            NOVA_LOG("Requirements for pip installation not met", LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.requirements_not_met");
            Abort();
            return;
        }
        
        if (HasPipInstalled())
        {
            NOVA_LOG("Pip is already installed.", LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_already_installed");
            bIsRunning = false;
            if(callback) {callback(); }
            return;
        }
        
        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        if(!rootAccessHelper->Initialize())
        {
            NOVA_LOG("Failed to initialize RootAccessHelper", LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.root_access_not_initialized");
            Abort();
            return;
        }
        
        // Create a promise to track completion
        std::promise<bool> installationComplete;
        std::future<bool> futureResult = installationComplete.get_future();
        
        rootAccessHelper->Execute([&, this]() {
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.installing_pip");
            
            // Install python which includes pip
            bool success = ExecuteCommandBlocking("brew install python && brew link --overwrite python");
            
            if (success) {
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_verifying_installation");
                bHasPipInstalled = HasPipInstalled();
                
                if (bHasPipInstalled) {
                    FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_installed");
                    NOVA_LOG("Pip has been installed successfully.", LogType::Log);
                } else {
                    FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_installed");
                    NOVA_LOG("Installation completed but pip not found in PATH", LogType::Warning);
                }
            } else {
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_install_failed");
                NOVA_LOG("Pip installation failed", LogType::Error);
            }
            
            installationComplete.set_value(success);
            if (callback) {
                callback();
            }
            return success;
        });
        
        // Wait for the installation to complete with a timeout
        try {
            if (futureResult.wait_for(std::chrono::minutes(5)) == std::future_status::timeout) {
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_install_timed_out");
                NOVA_LOG("Pip installation timed out", LogType::Error);
            }
        } catch (const std::exception& e) {
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_install_failed");
            NOVA_LOG(("Error during pip installation: " + std::string(e.what())).c_str(), LogType::Error);
        }
        
        delete rootAccessHelper;
        bIsRunning = false;
    }

    void PipInstallHelper_Darwin::Reset()
    {
        if (bIsRunning) {
            KillRunningProcess();
        }
        
        bIsInitialized = false;
        NOVA_LOG("Pip installation reset", LogType::Log);
        FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.reset");
    }

    void PipInstallHelper_Darwin::Abort()
    {
        if (bIsRunning) {
            KillRunningProcess();
        }
        
        bIsRunning = false;
        NOVA_LOG("Pip installation aborted", LogType::Log);
        FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.aborted");
    }

    bool PipInstallHelper_Darwin::HasMetRequirements() const
    {
        BrewInstallHelper* brewHelper = BrewInstallHelper::CreatePlatformSpecific();
        brewHelper->Initialize();
        bool result = brewHelper->IsBrewInstalled();
        delete brewHelper;
        return result;
    }

    void PipInstallHelper_Darwin::InstallPackage(const std::string& packageName, std::function<void(std::string)> callback)
    {
        if(!HasPipInstalled())
        {
            NOVA_LOG(("Pip is not installed, cannot install package: " + packageName).c_str(), LogType::Error);
            callback("Pip is not installed.");
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_not_installed");
            return;
        }

        bIsRunning = true;
        FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.package_installing");
        
        RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
        rootAccessHelper->Initialize();
        
        // Create a promise to track completion
        std::promise<std::string> installationComplete;
        std::future<std::string> futureResult = installationComplete.get_future();
        
        rootAccessHelper->Execute([&, this, packageName]() {
            std::string command = "pip3 install " + packageName;
            FireInstallCallback("Executing: " + command);
            
            bool success = ExecuteCommandBlocking(command);
            std::string result;
            
            if (success) {
                result = "Package " + packageName + " installed successfully.";
                NOVA_LOG(result.c_str(), LogType::Log);
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.package_install_success");
            } else {
                result = "Failed to install package: " + packageName;
                NOVA_LOG(result.c_str(), LogType::Error);
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.package_install_failed");
            }
            
            installationComplete.set_value(result);
            return success;
        });
        
        // Wait for the installation to complete with a timeout
        try {
            if (futureResult.wait_for(std::chrono::minutes(5)) == std::future_status::timeout) {
                std::string timeoutMsg = "Package installation timed out: " + packageName;
                FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.pip_install_timed_out");
                NOVA_LOG(timeoutMsg.c_str(), LogType::Error);
                
                if (callback) {
                    callback(timeoutMsg);
                }
            } else {
                std::string result = futureResult.get();
                
                if (callback) {
                    callback(result);
                }
            }
        } catch (const std::exception& e) {
            std::string errorMsg = "Error during package installation: " + std::string(e.what());
            FireInstallCallback("com.epicnova.adi.fh.ds.pip_install_helper_darwin.package_install_failed");
            NOVA_LOG(errorMsg.c_str(), LogType::Error);
            
            if (callback) {
                callback(errorMsg);
            }
        }
        
        delete rootAccessHelper;
        bIsRunning = false;
    }

    bool PipInstallHelper_Darwin::HasPipInstalled() const
    {
        // Check if pip is installed by trying to run it
        int result = std::system("pip --version > /dev/null 2>&1");
        
        if (result == 0) {
            NOVA_LOG("Pip is installed and accessible.", LogType::Debug);
            return true;
        }
        
        // Also check for pip3 as an alternative
        result = std::system("pip3 --version > /dev/null 2>&1");
        
        if (result == 0) {
            NOVA_LOG("Pip3 is installed and accessible.", LogType::Debug);
            return true;
        }
        
        // Check if Python has pip module available
        result = std::system("python -m pip --version > /dev/null 2>&1");
        
        if (result == 0) {
            NOVA_LOG("Pip is available via Python module.", LogType::Debug);
            return true;
        }
        
        // Check if Python3 has pip module available
        result = std::system("python3 -m pip --version > /dev/null 2>&1");
        
        if (result == 0) {
            NOVA_LOG("Pip is available via Python3 module.", LogType::Debug);
            return true;
        }
        
        NOVA_LOG("Pip is not installed or not accessible.", LogType::Debug);
        return false;
    }
    
    // Add this member variable declaration in the PipInstallHelper_Darwin.h file
    std::string currentCommand;
    
    bool PipInstallHelper_Darwin::ExecuteCommandBlocking(const std::string& command)
    {
        FireInstallCallback("Executing: " + command);
        
        // Store the current command for potential killing
        currentCommand = command;
        
        // Suppress output to prevent GUI interference
        std::string fullCommand = command + " > /dev/null 2>&1";
        int result = system(fullCommand.c_str());
        
        // Clear the current command
        currentCommand.clear();
        
        if (result == 0) {
            FireInstallCallback("Command completed successfully");
            return true;
        } else {
            FireInstallCallback("Command failed with return code: " + std::to_string(result));
            return false;
        }
    }
    
    void PipInstallHelper_Darwin::KillRunningProcess()
    {
        if (!currentCommand.empty()) {
            // Extract the first word of the command which is typically the process name
            std::string processName = currentCommand.substr(0, currentCommand.find(' '));
            
            if (processName == "pip" || processName == "pip3" || processName == "python" || processName == "python3") {
                // Kill the process if it's a pip or python process
                std::string killCmd = "pkill -f '" + processName + "'";
                system(killCmd.c_str());
                FireInstallCallback("Terminated running process: " + processName);
                NOVA_LOG(("Terminated running process: " + processName).c_str(), LogType::Debug);
            }
            
            currentCommand.clear();
        }
    }
} // namespace Core::Helpers

#endif