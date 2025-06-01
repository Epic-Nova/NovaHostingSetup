#pragma once 

#ifdef __APPLE__

#include "NovaMinimal.h"
#include "Helpers/PipInstallHelper/PipInstallHelper.h"

namespace Core::Helpers
{

    class PipInstallHelper_Darwin : public PipInstallHelper
    {
    public:
        // This function initializes different classes depending on the operating system
        virtual void Initialize() override;

        // This function removes the pip install helper from the progress
        virtual void Shutdown() override;

        // This function executes the pip install command
        virtual void Execute(std::function<bool()> callback) override;

        // This function resets the activity and acts like there was never a pip install requested
        virtual void Reset() override;

        // This function aborts the activity and acts like there was never a pip install requested
        virtual void Abort() override;

        // This function checks if the requirements for pip are met, like if brew is installed on macOS or if python is installed on Windows
        virtual bool HasMetRequirements() const override;

        virtual bool HasPipInstalled() const override;

        // This function installs a package using pip
        virtual void InstallPackage(const std::string& packageName, std::function<void(std::string)> callback) override;

    private:
        // Add any macOS specific members here if needed
    };
}

/// @file PipInstallHelper_Darwin.h
/// @brief macOS-specific pip installation helper.

#endif // __APPLE__