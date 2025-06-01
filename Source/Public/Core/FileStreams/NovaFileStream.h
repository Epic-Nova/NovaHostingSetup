#pragma once

#include "NovaMinimal.h"

namespace Core
{
    using namespace std;

    class NovaFileHandle
    {
        NovaFileHandle(string FilePath, string FileName, string FileExtension);
        ~NovaFileHandle();

        private:
            string m_FilePath;
            string m_FileName;
            string m_FileExtension;
    };
    
    class NovaFileStream
    {
        virtual NovaFileHandle Open() = 0;
        virtual bool Close() = 0;
        virtual bool Write() = 0;
        virtual NovaFileHandle Read() = 0;
    };

}