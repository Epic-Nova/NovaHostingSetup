/// @file RootAccessHelper_Darwin.cpp
/// @brief macOS-specific root/elevated access implementation.

#ifdef __APPLE__

#include "Helpers/RootAccessHelper/Platforms/RootAccessHelper_Darwin.h"
#include <unistd.h>
#include <sys/types.h>
#include <Security/Security.h>
#include <Security/AuthorizationTags.h>
#include <ServiceManagement/ServiceManagement.h>
#include "NovaCore.h"

namespace Core::Helpers
{
    void RootAccessHelper_Darwin::Initialize()
    {
        if(bIsInitialized) 
        {
            NOVA_LOG("RootAccessHelper_Darwin is already initialized", Core::LogType::Warning);
            FireInstallCallback("RootAccessHelper_Darwin is already initialized");

            return;
        }
        
        OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
        if (status != errAuthorizationSuccess) 
        {
            NOVA_LOG("Failed to create authorization reference", Core::LogType::Error);
            FireInstallCallback("Failed to create authorization reference");
            return;
        }
        
        bIsInitialized = true;
        NOVA_LOG("RootAccessHelper_Darwin initialized successfully", Core::LogType::Debug);
        FireInstallCallback("RootAccessHelper_Darwin has been initialized successfully");
        bHasElevatedPrivileges = false; // Initially, we do not have elevated privileges
    }

    void RootAccessHelper_Darwin::Shutdown()
    {
        if (authRef != NULL) 
        {
            AuthorizationFree(authRef, kAuthorizationFlagDefaults);
            authRef = NULL;
        }
        
        bIsInitialized = false;
        bHasElevatedPrivileges = false;
        NOVA_LOG("RootAccessHelper_Darwin shutdown completed", Core::LogType::Debug);
        FireInstallCallback("RootAccessHelper_Darwin has been shut down");
    }

    void RootAccessHelper_Darwin::Execute(std::function<bool()> callback)
    {
        if (!bIsInitialized) 
        {
            NOVA_LOG("RootAccessHelper_Darwin not initialized", Core::LogType::Error);
            FireInstallCallback("RootAccessHelper_Darwin not initialized");
            return;
        }

        // Request admin privileges
        AuthorizationItem authItem = { kAuthorizationRightExecute, 0, NULL, 0 };
        AuthorizationRights authRights = { 1, &authItem };
        
        OSStatus status = AuthorizationCopyRights(authRef, &authRights, NULL, 
                                                  kAuthorizationFlagDefaults | 
                                                  kAuthorizationFlagInteractionAllowed | 
                                                  kAuthorizationFlagPreAuthorize |
                                                  kAuthorizationFlagExtendRights,
                                                  NULL);
        
        if (status == errAuthorizationSuccess) 
        {
            bHasElevatedPrivileges = true;
            NOVA_LOG("Elevated privileges granted", Core::LogType::Debug);
            FireInstallCallback("Elevated privileges granted");
            callback();
        } 
        else 
        {
            NOVA_LOG("Failed to obtain elevated privileges", Core::LogType::Error);
            FireInstallCallback("Failed to obtain elevated privileges");
            bHasElevatedPrivileges = false;
        }
    }

    void RootAccessHelper_Darwin::Reset()
    {
        if (authRef != NULL) 
        {
            // Revoke the authorization
            AuthorizationFree(authRef, kAuthorizationFlagDefaults);
            
            // Recreate the authorization reference
            OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
            if (status != errAuthorizationSuccess) 
            {
                NOVA_LOG("Failed to recreate authorization reference during reset", Core::LogType::Error);
                FireInstallCallback("Failed to recreate authorization reference during reset");
                authRef = NULL;
                bIsInitialized = false;
            }
        }
        
        bHasElevatedPrivileges = false;
        NOVA_LOG("RootAccessHelper_Darwin reset completed", Core::LogType::Debug);
        FireInstallCallback("RootAccessHelper_Darwin has been reset");    
    }

    void RootAccessHelper_Darwin::Abort()
    {
        if (authRef != NULL) 
        {
            AuthorizationFree(authRef, kAuthorizationFlagDefaults);
            authRef = NULL;
        }
        
        bHasElevatedPrivileges = false;
        bIsInitialized = false;
        NOVA_LOG("RootAccessHelper_Darwin aborted", Core::LogType::Debug);
        FireInstallCallback("RootAccessHelper_Darwin has been aborted");    
    }

    bool RootAccessHelper_Darwin::HasRootAccess() const
    {
        return geteuid() == 0 || bHasElevatedPrivileges;
    }

    bool RootAccessHelper_Darwin::RequestElevatedPrivileges()
    {
        Execute([this]() {
            NOVA_LOG("Requesting elevated privileges...", Core::LogType::Log);
            FireInstallCallback("Requesting elevated privileges...");
            return true; 
        });
        return bHasElevatedPrivileges;
    }

    std::string RootAccessHelper_Darwin::GetElevationCommand(const std::string& command) const
    {
        return "sudo " + command; 
    }

    bool RootAccessHelper_Darwin::RunCommandWithElevatedPrivileges(const std::string& command)
    {
        if (!bHasElevatedPrivileges && !RequestElevatedPrivileges()) 
        {
            NOVA_LOG("Cannot run command as root: no elevated privileges", Core::LogType::Error);
            FireInstallCallback("Cannot run command as root: no elevated privileges");
            return false;
        }

        NOVA_LOG(("Executing command: " + command).c_str(), Core::LogType::Debug);
        FireInstallCallback("Executing command: " + command);

        // Suppress output to prevent GUI interference
        std::string fullCommand = command + " > /dev/null 2>&1";
        FILE* pipe = popen(fullCommand.c_str(), "r");
        
        if (!pipe) 
        {
            NOVA_LOG("Failed to open pipe for command execution", Core::LogType::Error);
            FireInstallCallback("Failed to open pipe for command execution");
            return false;
        }

        // Since output is redirected, we'll just wait for completion
        int returnCode = pclose(pipe);
        
        if (returnCode == 0) 
        {
            NOVA_LOG("Command executed successfully", Core::LogType::Debug);
            FireInstallCallback("Command executed successfully");
            return true;
        } 
        else 
        {
            NOVA_LOG(("Command failed with return code: " + std::to_string(returnCode)).c_str(), Core::LogType::Error);
            FireInstallCallback("Command failed with return code: " + std::to_string(returnCode));
            return false;
        }
    }

} // namespace Core::Helpers


#endif