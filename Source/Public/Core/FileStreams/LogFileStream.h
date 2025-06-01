#pragma once

#include "NovaMinimal.h"
#include "NovaFileStream.h"

namespace Core
{
    class LogFileStream : public NovaFileStream
    {
        public:
            LogFileStream();
            ~LogFileStream();
    };
}