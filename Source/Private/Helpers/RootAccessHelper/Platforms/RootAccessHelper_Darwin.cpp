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
    bool RootAccessHelper_Darwin::Initialize()
    {
        NOVA_LOG_VERBOSE("Initializing RootAccessHelper_Darwin", Core::LogType::Debug);
        
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.initializing");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.already_initialized");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.authorization_creation_failed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.initialized");
        
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.shutdown");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.shutdown_completed");
        
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.not_initialized");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.elevated_privileges_granted");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.elevated_privileges_failed");
        
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.reset");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.authorization_recreation_failed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.reset_completed");

        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.aborted");
        
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.initializing");


        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.requesting_elevated_privileges");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.no_elevated_privileges");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_starting");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_execution_failed");

        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_success");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_failed");
        RegisterEmptyInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_timed_out");



        if(bIsInitialized) 
        {
            NOVA_LOG("RootAccessHelper_Darwin is already initialized", Core::LogType::Warning);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.already_initialized");
            
            return true;
        }
        
        OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
        if (status != errAuthorizationSuccess) 
        {
            NOVA_LOG("Failed to create authorization reference", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.authorization_creation_failed");
            return false;
        }
        
        NOVA_LOG("RootAccessHelper_Darwin initialized successfully", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.initialized");
        
        bHasElevatedPrivileges = false;
        bIsInitialized = true;
        authRef = NULL; 

        return bIsInitialized;
    }

    void RootAccessHelper_Darwin::Shutdown()
    {
        NOVA_LOG("Shutting down RootAccessHelper_Darwin", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.shutdown");

        if (authRef != NULL) 
        {
            AuthorizationFree(authRef, kAuthorizationFlagDefaults);
            authRef = NULL;
        }
        
        bIsInitialized = false;

        bHasElevatedPrivileges = false;
        NOVA_LOG("RootAccessHelper_Darwin shutdown completed", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.shutdown_completed");
    }

    void RootAccessHelper_Darwin::Execute(std::function<bool()> callback)
    {
        if (!bIsInitialized) 
        {
            NOVA_LOG_VERBOSE("RootAccessHelper_Darwin not initialized", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.not_initialized");
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
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.elevated_privileges_granted");
            callback();
        } 
        else 
        {
            NOVA_LOG("Failed to obtain elevated privileges", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.elevated_privileges_failed");
            bHasElevatedPrivileges = false;
        }
    }

    void RootAccessHelper_Darwin::Reset()
    {
        NOVA_LOG_VERBOSE("Resetting RootAccessHelper_Darwin", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.reset");

        if (authRef != NULL) 
        {
            // Revoke the authorization
            AuthorizationFree(authRef, kAuthorizationFlagDefaults);
            
            // Recreate the authorization reference
            OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
            if (status != errAuthorizationSuccess) 
            {
                NOVA_LOG("Failed to recreate authorization reference during reset", Core::LogType::Error);
                FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.authorization_recreation_failed");

                authRef = NULL;
                bIsInitialized = false;
            }
        }
        
        bHasElevatedPrivileges = false;

        NOVA_LOG("RootAccessHelper_Darwin reset completed", Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.reset_completed");  
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
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.aborted");
    }

    bool RootAccessHelper_Darwin::HasRootAccess() const
    {
        return geteuid() == 0 || bHasElevatedPrivileges;
    }

    bool RootAccessHelper_Darwin::RequestElevatedPrivileges()
    {
        Execute([this]() {
            NOVA_LOG("Requesting elevated privileges...", Core::LogType::Log);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.requesting_elevated_privileges");
            return true; 
        });
        return bHasElevatedPrivileges;
    }

    std::string RootAccessHelper_Darwin::GetElevationCommand(const std::string& command) const
    {
        return "sudo " + command; 
    }

    bool RootAccessHelper_Darwin::RunCommandWithElevatedPrivileges(const std::string& command, std::function<void(std::string)> callback)
    {
        if (!bHasElevatedPrivileges && !RequestElevatedPrivileges()) 
        {
            NOVA_LOG("Cannot run command as root: no elevated privileges", Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.no_elevated_privileges");

            return false;
        }

        std::string startMsg = "Executing command: " + command;
        NOVA_LOG(startMsg.c_str(), Core::LogType::Debug);
        FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_starting");

        if (callback) callback(startMsg);

        // Don't suppress output so we can capture progress
        std::string fullCommand = command + " 2>&1";
        FILE* pipe = popen(fullCommand.c_str(), "r");
        
        if (!pipe) 
        {
            std::string errorMsg = "Failed to open pipe for command execution";
            NOVA_LOG(errorMsg.c_str(), Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_execution_failed");

            return false;
        }

        // Read output line by line and pass it to the callback
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string outputLine(buffer);
            // Remove trailing newline if present
            if (!outputLine.empty() && outputLine[outputLine.length()-1] == '\n') {
                outputLine.erase(outputLine.length()-1);
            }
            
            // Send output to callback
            if (callback) callback(outputLine);
            
            // Also log the output at debug level
            NOVA_LOG(("Command output: " + outputLine).c_str(), Core::LogType::Debug);
        }
        
        int returnCode = pclose(pipe);
        
        if (returnCode == 0) 
        {
            std::string successMsg = "Command executed successfully";
            NOVA_LOG(successMsg.c_str(), Core::LogType::Debug);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_success");

            if (callback) callback(successMsg);
            return true;
        } 
        else 
        {
            std::string errorMsg = "Command failed with return code: " + std::to_string(returnCode);
            NOVA_LOG(errorMsg.c_str(), Core::LogType::Error);
            FireInstallCallback("com.epicnova.adi.fh.ds.root_access_helper_darwin.command_failed");

            return false;
        }
    }

} // namespace Core::Helpers


#endif