#include "Helpers/BaseHelper.h"

namespace Core::Helpers
{

    // Default implementations for pure virtual methods
    void BaseHelper::Shutdown() 
    {
        bIsInitialized = false;
        bIsRunning = false;
    }

    void BaseHelper::Reset() 
    {
        bIsRunning = false;
    }

    void BaseHelper::Abort() 
    {
        bIsRunning = false;
    }
} // namespace Core::Helpers