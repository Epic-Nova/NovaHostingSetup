#include "Helpers/PipInstallHelper/PipInstallHelper.h"

#ifdef __APPLE__
#include "Helpers/PipInstallHelper/Platforms/PipInstallHelper_Darwin.h"
#endif

#ifdef _WIN32
#include "Helpers/PipInstallHelper/Platforms/PipInstallHelper_Windows.h"
#endif 

#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include "Helpers/PipInstallHelper/Platforms/PipInstallHelper_Linux.h"
#endif

namespace Core::Helpers
{
    PipInstallHelper* PipInstallHelper::PlatformHelperInstance = nullptr;

    PipInstallHelper* PipInstallHelper::CreatePlatformSpecific()
    {
        #ifdef __APPLE__
        PlatformHelperInstance = new PipInstallHelper_Darwin();
        #elif defined(_WIN32) || defined(_WIN64)
        PlatformHelperInstance = new PipInstallHelper_Windows();
        #elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
        PlatformHelperInstance = new PipInstallHelper_Linux();
        #else
        PlatformHelperInstance = nullptr; // Unsupported platform
        #endif

        return PlatformHelperInstance;
    }
    
    void PipInstallHelper::Initialize()
    {
        if(PlatformHelperInstance)
        {
            PlatformHelperInstance->Initialize();
            bIsInitialized = true;
        }
        else
        {
            throw std::runtime_error("Failed to create PipInstallHelper instance for the current platform.");
        }
    }

    void PipInstallHelper::Shutdown()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Shutdown();
            delete PlatformHelperInstance;
            PlatformHelperInstance = nullptr;
        }
        bIsInitialized = false;
    }

    void PipInstallHelper::Execute(std::function<bool()> callback)
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

    void PipInstallHelper::Reset()
    {
        if(PlatformHelperInstance)
        {
            PlatformHelperInstance->Reset();
            bIsRunning = false;
            bIsAborted = false;
            bHasPipInstalled = false; // Reset the pip installation state
        }
    }

    void PipInstallHelper::Abort()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Abort();
            bIsAborted = true;
            bIsRunning = false;
            bHasPipInstalled = false; // Reset the pip installation state
        }
    }

    bool PipInstallHelper::HasMetRequirements() const
    {
        if (PlatformHelperInstance)
        {
            return PlatformHelperInstance->HasMetRequirements();
        }
        return false; // Default to false if no platform-specific instance is available
    }

    void PipInstallHelper::InstallPackage(const std::string& packageName, std::function<void(std::string)> callback)
    {
        if (PlatformHelperInstance && bIsInitialized)
        {
            PlatformHelperInstance->InstallPackage(packageName, [&](std::string result) {
                bHasPipInstalled = true; // Set the flag to true when a package is installed
                callback(result);
            });
        }
        else
        {
            throw std::runtime_error("PipInstallHelper is not initialized or platform instance is null.");
        }
    }

}