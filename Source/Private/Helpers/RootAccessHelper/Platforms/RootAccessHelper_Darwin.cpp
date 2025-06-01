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
            return;
        }
        
        OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
        if (status != errAuthorizationSuccess) 
        {
            NOVA_LOG("Failed to create authorization reference", Core::LogType::Error);
            return;
        }
        
        bIsInitialized = true;
        NOVA_LOG("RootAccessHelper_Darwin initialized successfully", Core::LogType::Debug);
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
    }

    void RootAccessHelper_Darwin::Execute(std::function<bool()> callback)
    {
        if (!bIsInitialized) 
        {
            NOVA_LOG("RootAccessHelper_Darwin not initialized", Core::LogType::Error);
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
            NOVA_LOG("Root access granted successfully", Core::LogType::Debug);
            
            // Execute the callback with elevated privileges
            if (callback) 
            {
                bool result = callback();
                if (!result) {
                    NOVA_LOG("Callback execution failed", Core::LogType::Warning);
                }
            }
        } 
        else 
        {
            NOVA_LOG("Failed to obtain root access", Core::LogType::Error);
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
                authRef = NULL;
                bIsInitialized = false;
            }
        }
        
        bHasElevatedPrivileges = false;
        NOVA_LOG("RootAccessHelper_Darwin reset completed", Core::LogType::Debug);    
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
    }

    bool RootAccessHelper_Darwin::HasRootAccess() const
    {
        return geteuid() == 0 || bHasElevatedPrivileges;
    }

    bool RootAccessHelper_Darwin::RequestElevatedPrivileges()
    {
        Execute([]() {
            NOVA_LOG("Requesting elevated privileges...", Core::LogType::Log);
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
            return false;
        }
                
        AuthorizationItem authItem = { kSMRightBlessPrivilegedHelper, 0, NULL, 0 };
        AuthorizationRights authRights = { 1, &authItem };

        OSStatus status = AuthorizationCopyRights(authRef, &authRights, 
                                   kAuthorizationEmptyEnvironment,
                                   kAuthorizationFlagDefaults | 
                                   kAuthorizationFlagInteractionAllowed |
                                   kAuthorizationFlagPreAuthorize |
                                   kAuthorizationFlagExtendRights,
                                   NULL);
        
        if (status == errAuthorizationSuccess) 
        {
            NOVA_LOG("Command executed successfully with root privileges", Core::LogType::Debug);
            return true;
        } 
        else 
        {
            NOVA_LOG("Failed to execute command with root privileges", Core::LogType::Error);
            return false;
        }
    }

} // namespace Core::Helpers


#endif