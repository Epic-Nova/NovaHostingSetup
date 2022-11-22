#include "NovaMinimal.h"

namespace Core
{
    class NovaLog
    {
        public:
            static void Log(const char* Message);
            static void Debug(const std::string& message);
            static void Warning(const std::string& message);
            static void Error(const std::string& message);
            static void FatalError(const std::string& message);
            static void Assert(bool condition, const std::string& message);
    };
}