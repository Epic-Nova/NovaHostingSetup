#include "NovaPackageHandler.h"

namespace Core
{
    bool NovaPackageHandler::InstallPackage(const char* PackageName)
    {
        std::string Command = "sudo apt-get install -y " + std::string(PackageName);
        int Result = system(Command.c_str());
        return Result == 0;
    }

    bool NovaPackageHandler::RemovePackage(const char* PackageName)
    {
        std::string Command = "sudo apt-get remove -y " + std::string(PackageName);
        int Result = system(Command.c_str());
        return Result == 0;
    }

    bool NovaPackageHandler::IsPackageInstalled(const char* PackageName)
    {
        std::string Command = "dpkg -s " + std::string(PackageName) + " | grep Status";
        FILE* Pipe = popen(Command.c_str(), "r");
        if (!Pipe)
        {
            return false;
        }
        char Buffer[128];
        std::string Result = "";
        while (!feof(Pipe))
        {
            if (fgets(Buffer, 128, Pipe) != NULL)
            {
                Result += Buffer;
            }
        }
        pclose(Pipe);

-        if (Result.find("Status: install ok installed") != std::string::npos)
        {
            return true;
        }
        return false;
    }
}