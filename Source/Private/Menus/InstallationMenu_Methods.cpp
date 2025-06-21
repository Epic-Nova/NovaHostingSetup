#include "NovaCore.h"
#include "NovaMinimal.h"
#include "Menus/InstallationMenu.h"
#include "Menus/InputMenus.h"
#include "Core/NovaFileOperations.h"
#include "Helpers/GitInstallHelper/GitInstallHelper.h"
#include "Helpers/PipInstallHelper/PipInstallHelper.h"

// This file is intended to be included directly in InstallationMenu.cpp, not compiled separately.
// It uses the namespace context from the including file.

namespace Menus
{
    using namespace Core::Helpers;

bool Menus::InstallationMenu::DetectInstallationAutomatically()
{
    NOVA_LOG_VERBOSE("DetectInstallationAutomatically: starting auto-detection", Core::LogType::Debug);
    
    try {
        // Get the current directory
        std::filesystem::path currentPath = std::filesystem::current_path();
        NOVA_LOG_VERBOSE(("Current path: " + currentPath.string()).c_str(), Core::LogType::Debug);
        
        // Go up 2 directories from current directory (which contains Source/ & Content/)
        std::filesystem::path basePath = currentPath;
        for (int i = 0; i < 2; ++i) {
            if (basePath.has_parent_path()) {
                basePath = basePath.parent_path();
            } else {
                NOVA_LOG("Failed to go up directory levels for auto-detection", Core::LogType::Error);
                return false;
            }
        }
        
        NOVA_LOG_VERBOSE(("Base path for detection: " + basePath.string()).c_str(), Core::LogType::Debug);
        
        // Check if there's a documentation folder
        std::filesystem::path docPath = basePath / "documentation";
        
        if (Core::FileOperations::NovaFileOperations::DirectoryExists(docPath.string())) {
            NOVA_LOG(("Found documentation directory at: " + docPath.string()).c_str(), Core::LogType::Log);
            mkdocsProjectPath_ = docPath.string();
            return true;
        } else {
            NOVA_LOG("Documentation directory not found during auto-detection", Core::LogType::Warning);
            return false;
        }
    } catch (const std::exception& e1) {
        NOVA_LOG(("Auto-detection error: " + std::string(e1.what())).c_str(), Core::LogType::Error);
        return false;
    }
}

bool Menus::InstallationMenu::SetupGitDownload()
{
    NOVA_LOG_VERBOSE("SetupGitDownload: starting Git setup", Core::LogType::Debug);
    
    try {
        // First check if Git is installed
        GitInstallHelper* gitHelper = GitInstallHelper::CreatePlatformSpecific();
        
        if (!gitHelper->IsGitInstalled()) {
            NOVA_LOG("Git is not installed, attempting to install", Core::LogType::Log);
            
            gitHelper->Execute([this]() {
                return true;
            });
        } else {
            NOVA_LOG("Git is already installed", Core::LogType::Log);
        }
        
        // Get Git credentials from the user
        NOVA_LOG("Requesting Git credentials", Core::LogType::Log);
        auto [username, token] = CredentialsInputMenu::ShowCredentialsInput("GitHub Credentials");
        
        // Check if user canceled
        if (username.empty() || token.empty()) {
            NOVA_LOG("Git credential input canceled by user", Core::LogType::Warning);
            return false;
        }
        
        // Store the credentials
        gitUsername_ = username;
        gitToken_ = token;
        
        NOVA_LOG("Git credentials received, setup complete", Core::LogType::Log);
        return true;
        
    } catch (const std::exception& e2) {
        NOVA_LOG(("Git setup error: " + std::string(e2.what())).c_str(), Core::LogType::Error);
        return false;
    }
}

bool Menus::InstallationMenu::SetupManualPath()
{
    NOVA_LOG_VERBOSE("SetupManualPath: requesting path input", Core::LogType::Debug);
    
    try {
        std::string path = PathInputMenu::ShowPathInput(
            "Enter MkDocs Project Path", 
            "", 
            "Specify the full path to your MkDocs project directory"
        );
        
        // Check if user canceled
        if (path.empty()) {
            NOVA_LOG("Path input canceled by user", Core::LogType::Warning);
            return false;
        }
        
        // Store the path
        mkdocsProjectPath_ = path;
        
        // Validate that the path exists
        if (!Core::FileOperations::NovaFileOperations::DirectoryExists(path)) {
            NOVA_LOG(("Warning: Directory does not exist: " + path).c_str(), Core::LogType::Warning);
        }
        
        NOVA_LOG(("Manual path set to: " + path).c_str(), Core::LogType::Log);
        return true;
        
    } catch (const std::exception& e3) {
        NOVA_LOG(("Manual path setup error: " + std::string(e3.what())).c_str(), Core::LogType::Error);
        return false;
    }
}

bool Menus::InstallationMenu::ExecuteInstallation()
{
    NOVA_LOG_VERBOSE("ExecuteInstallation: starting installation process", Core::LogType::Debug);
    
    try {
        switch (installationMethod_) {
            case InstallationMethod::AutoDetect: {
                NOVA_LOG("Executing auto-detect installation", Core::LogType::Log);
                // Path is already set during detection, just validate
                if (mkdocsProjectPath_.empty() || !Core::FileOperations::NovaFileOperations::DirectoryExists(mkdocsProjectPath_)) {
                    NOVA_LOG("Invalid project path from auto-detection", Core::LogType::Error);
                    SetError("Invalid project path detected during auto-detection");
                    return false;
                }
                
                NOVA_LOG(("Using detected project path: " + mkdocsProjectPath_).c_str(), Core::LogType::Log);
                break;
            }
                
            case InstallationMethod::GitDownload: {
                NOVA_LOG("Executing Git download installation", Core::LogType::Log);
                
                AddProgressStep("Git Repository Clone");
                SetCurrentStep("Git Repository Clone");
                UpdateProgress("Cloning repository...");
                
                GitInstallHelper* gitHelper = GitInstallHelper::CreatePlatformSpecific();
                
                // Make sure destination directory exists
                std::filesystem::path destinationPath(gitDestination_);
                std::filesystem::path parentPath = destinationPath.parent_path();
                
                if (!parentPath.empty() && !Core::FileOperations::NovaFileOperations::DirectoryExists(parentPath.string())) {
                    NOVA_LOG(("Creating parent directory: " + parentPath.string()).c_str(), Core::LogType::Log);
                    Core::FileOperations::NovaFileOperations::CreateDirectoryStructure(parentPath.string());
                }
                
                // Clone the repository
                if (!gitHelper->CloneRepository(gitRepoUrl_, gitDestination_, gitUsername_, gitToken_)) {
                    NOVA_LOG("Failed to clone Git repository", Core::LogType::Error);
                    SetError("Failed to clone Git repository");
                    return false;
                }
                
                // Set the project path to the cloned repository
                mkdocsProjectPath_ = gitDestination_;
                
                CompleteCurrentStep();
                NOVA_LOG("Git repository cloned successfully", Core::LogType::Log);
                break;
            }
                
            case InstallationMethod::ManualPath: {
                NOVA_LOG("Executing manual path installation", Core::LogType::Log);
                // Path is already set during setup, just validate
                if (mkdocsProjectPath_.empty()) {
                    NOVA_LOG("No path was provided for manual installation", Core::LogType::Error);
                    SetError("No path was provided for manual installation");
                    return false;
                }
                
                NOVA_LOG(("Using manual project path: " + mkdocsProjectPath_).c_str(), Core::LogType::Log);
                break;
            }
                
            case InstallationMethod::None:
            default: {
                NOVA_LOG("No installation method selected", Core::LogType::Error);
                SetError("No installation method selected");
                return false;
            }
        }
        
        // At this point, we have mkdocsProjectPath_ set regardless of the method

        //Get the optione menu reference and set the project path
        auto optionsMenu = OptionsMenu::GetSingletonInstance();
        optionsMenu->options_.mkdocsProjectPath = mkdocsProjectPath_;
        optionsMenu->SaveConfig();

        NOVA_LOG("Installation method executed successfully", Core::LogType::Log);
        SetComplete();
        return true;
        
    } catch (const std::exception& e4) {
        NOVA_LOG(("Installation execution error: " + std::string(e4.what())).c_str(), Core::LogType::Error);
        SetError("Installation error: " + std::string(e4.what()));
        return false;
    }
}
#include "NovaCore.h"
#include "NovaMinimal.h"
#include "Menus/InstallationMenu.h"
#include "Menus/InputMenus.h"
#include "Core/NovaFileOperations.h"
#include "Helpers/GitInstallHelper/GitInstallHelper.h"

// This file is intended to be included directly in InstallationMenu.cpp, not compiled separately.
// It uses the namespace context from the including file.

}