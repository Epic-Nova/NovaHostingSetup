#include "Helpers/BaseHelper.h"

namespace Core::Helpers
{

    // Default implementations for pure virtual methods
    void BaseHelper::Shutdown() 
    {
        bIsInitialized = false;
        bIsRunning = false;
        bIsAborted = false;
    }

    void BaseHelper::Reset() 
    {
        bIsRunning = false;
        bIsAborted = false;
    }

    void BaseHelper::Abort() 
    {
        bIsAborted = true;
        bIsRunning = false;
    }
} // namespace Core::Helpers