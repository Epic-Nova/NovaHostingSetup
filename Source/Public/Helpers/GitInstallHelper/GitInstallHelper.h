#pragma once

#include "BaseHelper.h"

namespace Core::Helpers
{
    class GitInstallHelper : public BaseHelper
    {
    public:
        GitInstallHelper()
            : BaseHelper("GitInstallHelper", "Helper for managing Git installations") {}

        static GitInstallHelper* CreatePlatformSpecific();

        virtual BaseHelper* GetHelperInstance() override
        {
            return PlatformHelperInstance;
        }

        // This function initializes different classes depending on the operating system, normally it installs Git, but when already installed it returns the version instead by the callback function
        virtual bool Initialize() override;

        // This function removes the Git installation from the system
        virtual void Shutdown() override;

        // This function executes the Git installation process and calls the callback function when done
        virtual void Execute(std::function<bool()> callback) override;

        // This function resets the Git installation process and acts like there was never Git requested
        virtual void Reset() override;

        // This function aborts the Git installation process and acts like there was never Git requested
        virtual void Abort() override;

        // This function checks if the requirements for Git are met, like if brew is installed on macOS or if Git is installed on Windows
        virtual bool HasMetRequirements() const override;

        // This function checks if Git is installed
        virtual bool IsGitInstalled() const;

        // This function installs a repository using Git - Modified to be non-const
        virtual bool CloneRepository(const std::string& url, const std::string& destination,
                                     const std::string& username = "", const std::string& token = "") = 0;

    protected:
        static GitInstallHelper* PlatformHelperInstance;
    };
}
