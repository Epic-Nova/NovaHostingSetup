#pragma once

#include "GitInstallHelper.h"

namespace Core::Helpers
{
    class GitInstallHelper_Darwin : public GitInstallHelper
    {
    public:
        
        // This function initializes different classes depending on the operating system
        virtual bool Initialize() override;

        // This function removes the Git install helper from the progress
        virtual void Shutdown() override;

        // This function executes the Git install command
        virtual void Execute(std::function<bool()> callback) override;

        // This function resets the activity and acts like there was never a Git install requested
        virtual void Reset() override;

        // This function aborts the activity and acts like there was never a Git install requested
        virtual void Abort() override;

        // This function checks if the requirements for Git are met, like if brew is installed on macOS or if Git is installed on Windows
        virtual bool HasMetRequirements() const override;

        // This function checks if Git is installed
        virtual bool IsGitInstalled() const override;

        // This function installs a repository using Git - Modified to be non-const
        virtual bool CloneRepository(const std::string& url, const std::string& destination,
                                     const std::string& username = "", const std::string& token = "") override;
                                    
    };
}
