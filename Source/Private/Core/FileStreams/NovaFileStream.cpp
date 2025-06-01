#include "NovaFileStream.h"

namespace Core
{
    NovaFileHandle::NovaFileHandle(string FilePath, string FileName, string FileExtension): 
    m_FilePath(FilePath), 
    m_FileName(FileName), 
    m_FileExtension(FileExtension) {}
}