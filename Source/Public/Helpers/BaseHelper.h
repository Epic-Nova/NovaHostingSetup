#pragma once

#include "NovaMinimal.h"

namespace Core::Helpers
{
    class BaseHelper
    {
        public:
            BaseHelper() = default;

            virtual BaseHelper* GetHelperInstance() = 0;
            virtual void Initialize() = 0;
            virtual void Shutdown() = 0;

            bool IsInitialized() { return bIsInitialized; }
            std::string GetName() { return Name; }
            std::string GetDescription() { return Description; }

            virtual void Execute(std::function<bool()> callback) = 0;
            virtual void Reset() = 0;
            virtual void Abort() = 0;
            bool IsRunning() { return bIsRunning; }
            bool IsAborted() { return bIsAborted; } 

        protected:

            BaseHelper(std::string NewName, std::string NewDescription)
                : Name(std::move(NewName)), Description(std::move(NewDescription)) {};
            virtual ~BaseHelper() = default;

        private:

            std::string Name;
            std::string Description;

        protected:
            bool bIsInitialized = false;
            bool bIsRunning = false;
            bool bIsAborted = false;

            
    };  
}