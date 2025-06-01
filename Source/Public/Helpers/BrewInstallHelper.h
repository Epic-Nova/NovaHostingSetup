/// @file BrewInstallHelper.h
/// @brief Manages Homebrew installation and package handling on macOS.

#pragma once 

#ifdef __APPLE__

#include "Helpers/BaseHelper.h"

namespace Core::Helpers
{
    class BrewInstallHelper : public BaseHelper
    {
    public:
        BrewInstallHelper() = default;

        static BrewInstallHelper* CreatePlatformSpecific() 
        {
            return new BrewInstallHelper();
        }

        virtual BaseHelper* GetHelperInstance() override
        {
            return this; // Return the current instance
        }

        // This function initializes the Brew installation process
        virtual void Initialize() override;

        // This function removes the Brew installation from the system
        virtual void Shutdown() override;

        // This function executes the Brew installation process and calls the callback function when done
        virtual void Execute(std::function<bool()> callback) override;

        // This function resets the Brew installation process and acts like there was never Brew requested
        virtual void Reset() override;

        // This function aborts the Brew installation process and acts like there was never Brew requested
        virtual void Abort() override;



        // This function checks if Homebrew is installed
        bool IsBrewInstalled() const;

        // This function installs a package using Homebrew
        void InstallPackage(const std::string& packageName, std::function<void(bool)> callback);
        
        // Add method to run commands with progress tracking
        bool RunCommandWithProgress(const std::string& command) override;
        
        private:
        bool bIsBrewInstalled = false; // Flag to check if Homebrew is installed
    };
}

#endif