#include "EpicNovaInstaller.h"
#include "Screens.h"

using namespace std;

int main()
{
    //get userid 
    int uid  = getuid();
    //check if user isn't root root's uid = 0
    if(uid != 0){
        //print non root error and quit the programm
        system("whiptail --title 'Epic Nova Hosting Setup' --msgbox 'You need to be root to use this installer' 10 60"); 
        return 0;
    }
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