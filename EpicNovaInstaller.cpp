#include "EpicNovaInstaller.h"
#include "Screens.h"

using namespace std;

int main()
{
    Screens ScreenList;
    ScreenList.BeginScreenLifecircle();
}

void EpicNovaInstaller::HandlePackage(const char* Package, bool bInstall)
{
    string cmd("apt ");
    cmd += bInstall ? "install " : "remove ";
    cmd += Package;
    cmd += " -y";
    system(cmd.c_str());
}

//6qLNp2cgmrLSqX3e7uUT