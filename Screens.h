#include "IncludeFile.h"

class Screens
{
public:
    Screens();

    void BeginScreenLifecircle();

    void ExecuteSelectedServerTypeInstallation();

    void InstallPackages(int SelectedServerType);

    void Cleanup();

};