#include "Helpers/GitInstallHelper/GitInstallHelper.h"
#include "NovaLog.h"

#ifdef __APPLE__
#include "Helpers/GitInstallHelper/GitInstallHelper_Darwin.h"
#elif defined(_WIN32) || defined(_WIN64)
#include "Helpers/GitInstallHelper/GitInstallHelper_Windows.h"
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include "Helpers/GitInstallHelper/GitInstallHelper_Linux.h"
#else
#error "Unsupported platform for GitInstallHelper"
#endif

namespace Core::Helpers
{
    GitInstallHelper* GitInstallHelper::PlatformHelperInstance = nullptr;

    GitInstallHelper* GitInstallHelper::CreatePlatformSpecific()
    {
        #ifdef __APPLE__
        PlatformHelperInstance = new GitInstallHelper_Darwin();
        #elif defined(_WIN32) || defined(_WIN64)
        PlatformHelperInstance = new GitInstallHelper_Windows();
        #elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
        PlatformHelperInstance = new GitInstallHelper_Linux();
        #else
        PlatformHelperInstance = nullptr; // Unsupported platform
        #endif

        return PlatformHelperInstance;
    }

    bool GitInstallHelper::Initialize()
    {
        if (PlatformHelperInstance)
        {
            return PlatformHelperInstance->Initialize();
        }
        else
        {
            throw std::runtime_error("Failed to create GitInstallHelper instance for the current platform.");
        }
    }

    void GitInstallHelper::Shutdown()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Shutdown();
            delete PlatformHelperInstance;
            PlatformHelperInstance = nullptr;
        }
        bIsInitialized = false;
    }

    void GitInstallHelper::Execute(std::function<bool()> callback)
    {
        if (PlatformHelperInstance && bIsInitialized)
        {
            PlatformHelperInstance->Execute([&]() {
                bIsRunning = true;
                bool result = callback();
                bIsRunning = false;
                return result;
            });
        }
    }

    void GitInstallHelper::Reset()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Reset();
            bIsInitialized = false;
            bIsRunning = false;
        }
    }

    void GitInstallHelper::Abort()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Abort();
            bIsRunning = false;
        }
    }

    bool GitInstallHelper::HasMetRequirements() const
    {
        if (PlatformHelperInstance)
        {
            return PlatformHelperInstance->HasMetRequirements();
        }
        return false; // No platform-specific instance available
    }

    bool GitInstallHelper::IsGitInstalled() const
    {
        if (PlatformHelperInstance)
        {
            return PlatformHelperInstance->IsGitInstalled();
        }
        return false; // No platform-specific instance available
    }

    bool GitInstallHelper::CloneRepository(const std::string& url, const std::string& destination,
                                        const std::string& username, const std::string& token)
    {
        if (PlatformHelperInstance)
        {
            return PlatformHelperInstance->CloneRepository(url, destination, username, token);
        }
        return false; // No platform-specific instance available
    }
}
