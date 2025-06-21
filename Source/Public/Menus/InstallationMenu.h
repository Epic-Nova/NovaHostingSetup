#pragma once

#include "BaseMenu.h"
#include <mutex>

namespace Menus
{
    class InstallationMenu : public BaseMenu
    {
    public:
        static std::shared_ptr<InstallationMenu> Create();
        
        void Show() override;
        void Hide() override;
        virtual ~InstallationMenu();

        // Progress tracking methods
        void AddProgressStep(const std::string& stepName);
        void AddSubStep(const std::string& parentStep, const std::string& subStep);
        void UpdateProgress(const std::string& message);
        void SetCurrentStep(const std::string& stepName);
        void SetCurrentSubStep(const std::string& parentStep, const std::string& subStep);
        void CompleteCurrentStep();
        void CompleteCurrentSubStep();
        void SetError(const std::string& errorMessage);
        void SetComplete();

        // Install callback function for helpers
        std::function<void(const std::string&)> GetProgressCallback();
        
        void StartInstallation();
        void ViewProgress();

        enum class InstallationMethod {
            None,
            AutoDetect,
            GitDownload,
            ManualPath
        };

    private:
        // Add static mutex declaration
        static std::mutex progressMutex_;
        
        // Add logComponent as a member
        // TODO: Re-add logComponent_ once the bug is fixed
        //std::shared_ptr<ftxui::Component> logComponent_;
        
        struct SubStep {
            std::string name;
            std::string status = "Waiting...";
            bool completed = false;
            bool error = false;
        };

        struct ProgressStep {
            std::string name;
            std::string status = "Waiting...";
            bool completed = false;
            bool error = false;
            std::vector<SubStep> subSteps;
            int currentSubStepIndex = -1;
        };

        std::vector<ProgressStep> steps_;
        std::atomic<int> currentStepIndex_{-1};
        std::atomic<bool> isComplete_{false};
        std::atomic<bool> hasError_{false};
        
        std::string currentMessage_;
        std::string errorMessage_;
        std::string projectPath_;
        std::atomic<bool> startInstallation_{false};
        InstallationMethod installationMethod_ = InstallationMethod::None;
        
        // Project paths
        std::string mkdocsProjectPath_;
        std::string gitUsername_;
        std::string gitToken_;
        std::string gitRepoUrl_ = "https://github.com/Epic-Nova/FH-POI-Documentation";
        std::string gitDestination_ = "Content/Website";
        
        // Methods for handling different installation paths
        bool DetectInstallationAutomatically();
        bool SetupGitDownload();
        bool SetupManualPath();
        
        // Execute installation based on method
        bool ExecuteInstallation();
        
        ftxui::Element RenderProgressList();
        ftxui::Element RenderCurrentStatus();
    };
}
