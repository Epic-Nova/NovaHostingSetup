#pragma once

#include "NovaMinimal.h"
#include "Helpers/BaseHelper.h"

namespace Core::Helpers
{
    class RootAccessHelper : public BaseHelper
    {
        public:
        RootAccessHelper()
            : BaseHelper("RootAccessHelper", "Helper for managing root access") {} 

        static RootAccessHelper* CreatePlatformSpecific();

        virtual BaseHelper* GetHelperInstance() override
        {
            return PlatformHelperInstance;
        }

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


        virtual bool HasRootAccess() const { return bHasElevatedPrivileges; }

        virtual bool RequestElevatedPrivileges() = 0;

        virtual std::string GetElevationCommand(const std::string& command) const = 0;

        virtual bool RunCommandWithElevatedPrivileges(const std::string& command) = 0;

        bool bHasElevatedPrivileges = false;

        private:

        static RootAccessHelper* PlatformHelperInstance;

    };
}    