#pragma once

#ifdef __APPLE__

#include "Helpers/PipInstallHelper/PipInstallHelper.h"

namespace Core::Helpers
{
    class PipInstallHelper_Darwin : public PipInstallHelper
    {
    public:
        PipInstallHelper_Darwin() : PipInstallHelper() {}
        
        virtual bool Initialize() override;
        virtual void Shutdown() override;
        virtual void Execute(std::function<bool()> callback) override;
        virtual void Reset() override;
        virtual void Abort() override;
        virtual bool HasMetRequirements() const override;
        virtual bool HasPipInstalled() const override;
        virtual void InstallPackage(const std::string& packageName, std::function<void(std::string)> callback) override;
        
    protected:
        bool ExecuteCommandBlocking(const std::string& command) override;
        void KillRunningProcess();
        
    private:
        std::string currentCommand;
        bool bHasPipInstalled = false;
    };
}

#endif // __APPLE__

/// @file PipInstallHelper_Darwin.h
/// @brief macOS-specific pip install helper.