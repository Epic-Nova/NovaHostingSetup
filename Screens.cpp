#include "Screens.h"
#include "EpicNovaInstaller.h"

Screens::Screens()
{}

void Screens::BeginScreenLifecircle()
{
    system("whiptail --title 'Epic Nova Hosting Setup' --msgbox 'This program will guide you through the epic nova installation program. Press OK to continue.' 10 60");
    ExecuteSelectedServerTypeInstallation();
}

void Screens::ExecuteSelectedServerTypeInstallation()
{
    system("mkdir /etc/nova");
    system("cp -R ./content/* /etc/nova");

    int i, ret = system("bash /etc/nova/scripts/setup/screens/server_type.bash");
    i=WEXITSTATUS(ret);

    system("bash /etc/nova/scripts/setup/screens/ip_block.bash");

    EpicNovaInstaller Nova;

    system("apt update && apt upgrade -y");
    Nova.HandlePackage("screen curl neofetch fish tmux unzip crudini", true);
    Nova.HandlePackage("ftp", false);
    system("chsh -s /usr/bin/fish");    

    //ONLY FOR NOT VPN HOST
    //ask for vpn access info (only if not vpn host)
    //configure vpn & ask vpn server for adding this server to the vpn client list
    //enable vpn and add this to the default commands on startup (the connecting process)

    InstallPackages(i);
}

void Screens::InstallPackages(int SelectedServerType) 
{
    EpicNovaInstaller Nova;
    //install packages (depending on the selected server type)
    //configure them
    switch (SelectedServerType)
    {
    case 1:
        break;
    case 2:
        system(" apt-get install curl gnupg2 wget unzip -y");
        system("curl -fsSL https://www.mongodb.org/static/pgp/server-4.4.asc | apt-key add -");
        system("echo 'deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu focal/mongodb-org/4.4 multiverse' | tee /etc/apt/sources.list.d/mongodb-org-4.4.lis4");

        system("apt update");
        
        Nova.HandlePackage("mongodb-server", true);
        system("systemctl start mongodb");
        system("systemctl enable mongodb");
        break;
    case 3:

        break;
    case 4:
        system("apt-key adv --keyserver hkp://keyserver.ubuntu.com --recv E162F504A20CDF15827F718D4B7C549A058F8B6B");
        system("apt-key adv --keyserver hkp://keyserver.ubuntu.com --recv 7568D9BB55FF9E5287D586017AE645C0CF8E292A");
        system("echo 'deb http://repo.pritunl.com/stable/apt focal main' | tee /etc/apt/sources.list.d/pritunl.list");

        system("apt update");
        
        Nova.HandlePackage("pritunl", true);
        system("systemctl start pritunl");
        system("systemctl enable pritunl");
    
        system("screen -S Pritunl");
        system("pritunl");
        break;
    case 5:
        //UPDATE CURRENT INSTALLATION (AFTER AN INSTALLATION WRITE DOWN THE TYPE IN THE ETC)
        break;
    default:
        
        break;
    }

    //add to default startup (some commands and the services)
    //finish up and delete the chache

    //show login credentials for this server (generated ssh key) and other generated infos
    Cleanup();
}

void Screens::Cleanup() {
    system("systemctl restart ssh && systemctl restart sshd");
    exit(200);
}