#include "NovaMinimal.h"
#include "NovaCore.h"
#include "Helpers/BaseHelper.h" // Add this include for BaseHelper

#ifdef __APPLE__
#include "Helpers/BrewInstallHelper.h"
#endif

#include "Helpers/PipInstallHelper/PipInstallHelper.h"

using namespace Core;

int main(int argc, const char* argv[]) 
{
    using namespace Menus;
    using namespace Core::Helpers;
    
    NOVA_LOG("Nova Documentation Setup started!", LogType::Debug);

    Core::NovaLog::StartLogFile();

    auto MainMenuPtr = MainMenu::InitializeMainMenu();
      MainMenuPtr->SetMenuActionCallback("InstallRequirements", [&]() {
      NOVA_LOG("Installing requirements...", LogType::Debug);


      //if mac, then install brew. and also install pip and then install venv

        #ifdef __APPLE__
        BrewInstallHelper* BrewInstallHelperPtr = BrewInstallHelper::CreatePlatformSpecific();
        BrewInstallHelperPtr->Initialize();
        BrewInstallHelperPtr->Execute([&]() {
            return BrewInstallHelperPtr->IsBrewInstalled();
        });
        #endif

        PipInstallHelper* PipInstallHelperPtr = PipInstallHelper::CreatePlatformSpecific();
        PipInstallHelperPtr->Initialize();
        PipInstallHelperPtr->Execute([&]() {
            return PipInstallHelperPtr->HasPipInstalled();
        });

        NOVA_LOG("Requirements installed successfully!", LogType::Log);
        
        // Here you would typically ask for the git repository URL and clone it

      //We also need to ask for the working directory, then create a pip venv and install the requirementws from the git project (also we have to ask for the path of this project)
    });

    MainMenuPtr->SetMenuActionCallback("StartDocumentationWebpage", [&]() {
        NOVA_LOG("Starting documentation webpage...", LogType::Debug);
        // Here you would typically launch a web browser or server to display the documentation
        // For now, we will just log it

        //For this we just have to call mkdocs serve on the venv path
        NOVA_LOG("Documentation webpage started successfully!", LogType::Debug);
      });

    MainMenuPtr->SetMenuActionCallback("Quit", [&]() {
        NOVA_LOG("Quitting application...", LogType::Debug);
        MainMenuPtr->GetScreen().ExitLoopClosure()();
        NOVA_LOG("Application exited successfully!", LogType::Log);
    });
    
    MainMenuPtr->SetMenuActionCallback("ShowHelp", [&]() {
        NOVA_LOG("Showing help...", LogType::Debug);
        // Here you would typically display help information
        // For now, we will just log it
        NOVA_LOG("Help displayed successfully!", LogType::Log);
    });

    MainMenuPtr->Show();

    return 0;
}