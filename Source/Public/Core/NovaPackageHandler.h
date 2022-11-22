#include "NovaMinimal.h"

namespace Core
{
    /**
     * This class is a interface for the APT package manager and includes all the
     * functions that are needed to install and remove packages.
    */
    class NovaPackageHandler
    {
    public:


        /**
         * This function is used to install a package.
         * @param PackageName The name of the package that should be installed.
         * @return Returns true if the package was installed successfully.
        */
        static bool InstallPackage(const char* PackageName);

        /**
         * This function is used to remove a package.
         * @param PackageName The name of the package that should be removed.
         * @return Returns true if the package was removed successfully.
        */
        static bool RemovePackage(const char* PackageName);

        /**
         * This function is used to check if a package is installed.
         * @param PackageName The name of the package that should be checked.
         * @return Returns true if the package is installed.
        */
        static bool IsPackageInstalled(const char* PackageName);
    };
}

