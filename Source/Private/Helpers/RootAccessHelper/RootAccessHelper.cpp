/// @file RootAccessHelper.cpp
/// @brief Root/elevated access factory and shared routines.

#include "Helpers/RootAccessHelper/RootAccessHelper.h"

#ifdef __APPLE__
#include "Helpers/RootAccessHelper/Platforms/RootAccessHelper_Darwin.h"
#endif // Apple

#ifdef _WIN32
#include "Helpers/RootAccessHelper/Platforms/RootAccessHelper_Windows.h"
#endif 

#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include "Helpers/RootAccessHelper/Platforms/RootAccessHelper_Linux.h"
#endif // Linux

namespace Core::Helpers
{
    RootAccessHelper* RootAccessHelper::PlatformHelperInstance = nullptr;

    RootAccessHelper* RootAccessHelper::CreatePlatformSpecific()
    {
        if(PlatformHelperInstance)
        {
            return PlatformHelperInstance; // Return existing instance if already created
        }
        
        #ifdef __APPLE__
        PlatformHelperInstance = new RootAccessHelper_Darwin();
        #elif defined(_WIN32) || defined(_WIN64)
        PlatformHelperInstance = new RootAccessHelper_Windows();
        #elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
        PlatformHelperInstance = new RootAccessHelper_Linux();
        #else
        PlatformHelperInstance = nullptr; // Unsupported platform
        #endif

        return PlatformHelperInstance;
    }
    bool RootAccessHelper::Initialize()
    {
        if (PlatformHelperInstance)
        {      
            return PlatformHelperInstance->Initialize();
        }
        else
        {
            throw std::runtime_error("Failed to create RootAccessHelper instance for the current platform.");
        }
    }

    void RootAccessHelper::Shutdown()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Shutdown();
            delete PlatformHelperInstance;
            PlatformHelperInstance = nullptr;
        }
        bIsInitialized = false;
    }

    void RootAccessHelper::Execute(std::function<bool()> callback)
    {
        if (PlatformHelperInstance && bIsInitialized)
        {
            bIsRunning = true;
            PlatformHelperInstance->Execute([&]() {
                bool result = callback();
                bIsRunning = false;
                return result;
            });
            
            // Wait for completion
            WaitForCompletion();
        }
        else
        {
            throw std::runtime_error("RootAccessHelper is not initialized or platform instance is null.");
        }
    }

    void RootAccessHelper::Reset()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Reset();
        }
        bIsRunning = false;
        bIsInitialized = false;
        bHasElevatedPrivileges = false;
    }
    void RootAccessHelper::Abort()
    {
        if (PlatformHelperInstance)
        {
            PlatformHelperInstance->Abort();
        }
        bIsRunning = false;
    }
}