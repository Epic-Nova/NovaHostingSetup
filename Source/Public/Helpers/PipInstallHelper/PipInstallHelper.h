/// @file PipInstallHelper.h
/// @brief Manages pip installation and Python-package handling.

#pragma once

#include "Helpers/BaseHelper.h"

namespace Core::Helpers
{
    class PipInstallHelper : public BaseHelper
    {
    public:
        PipInstallHelper()
            : BaseHelper("PipInstallHelper", "Helper for managing pip installations") {}

        static PipInstallHelper* CreatePlatformSpecific();

        virtual BaseHelper* GetHelperInstance() override
        {
            return PlatformHelperInstance;
        }

        // This function initializes different classes depending on the operating system, normally it installs pip, but when already installed it returns the version instead by the callback function
        virtual void Initialize() override;

        // This function removes the pip installation from the system
        virtual void Shutdown() override;

        // This function executes the pip installation process and calls the callback function when done
        virtual void Execute(std::function<bool()> callback) override;

        // This function resets the pip installation process and acts like there was never pip requested
        virtual void Reset() override;

        // This function aborts the pip installation process and acts like there was never pip requested
        virtual void Abort() override;

        //This function check if the requirements for pip are met, like if brew is installed on macOS or if python is installed on Windows
        virtual bool HasMetRequirements() const override;

        // This function checks if pip is installed
        virtual bool HasPipInstalled() const { return bHasPipInstalled; }

        // This function installs a package using pip
        virtual void InstallPackage(const std::string& packageName, std::function<void(std::string)> callback) = 0;
        
        bool bHasPipInstalled = false; // Flag to check if pip is installed
    private:
        static PipInstallHelper* PlatformHelperInstance;


    };
}