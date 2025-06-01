#pragma once

#ifdef __APPLE__

#include "Helpers/RootAccessHelper/RootAccessHelper.h"
#include <Security/Security.h>

namespace Core::Helpers
{

    class RootAccessHelper_Darwin : public RootAccessHelper
    {
        public:

        //This function initializes different classes depending on the operating system
        virtual void Initialize() override;

        //This function removes the root access from the progress
        virtual void Shutdown() override;

        //This function requests root acccess for the process and every child process being spawned
        virtual void Execute(std::function<bool()> callback) override;

        //This function resets the activity and acts like there was never root requested
        virtual void Reset() override;

        //This function aborts the activity and acts like there was never root requested
        virtual void Abort() override;

        virtual bool HasRootAccess() const override;

    
        virtual bool RequestElevatedPrivileges() override;

        virtual std::string GetElevationCommand(const std::string& command) const override;

        virtual bool RunCommandWithElevatedPrivileges(const std::string& command) override;

        private:
            AuthorizationRef authRef = NULL; // Authorization reference for macOS root access
    };
}

#endif // __APPLE__