#include "NovaMinimal.h"
#include "NovaCore.h"
#include "Helpers/BaseHelper.h" // Add this include for BaseHelper

using namespace Core;

int main(int argc, const char* argv[]) 
{
    using namespace Menus;
    using namespace Core::Helpers;

    Core::NovaLog::StartLogFile();

    auto MainMenuPtr = MainMenu::InitializeMainMenu();
      MainMenuPtr->SetMenuActionCallback("InstallRequirements", [&]() {
      NOVA_LOG("Installing requirements...", LogType::Log);

      Core::NovaPackageHandler::InstallPackage("pip");

      //We also need to ask for the working directory, then create a pip venv and install the requirementws from the git project (also we have to ask for the path of this project)
    });

    MainMenuPtr->SetMenuActionCallback("StartDocumentationWebpage", [&]() {
        NOVA_LOG("Starting documentation webpage...", LogType::Log);
        // Here you would typically launch a web browser or server to display the documentation
        // For now, we will just log it

        //For this we just have to call mkdocs serve on the venv path
        NOVA_LOG("Documentation webpage started successfully!", LogType::Log);
      });

    MainMenuPtr->SetMenuActionCallback("Quit", [&]() {
        NOVA_LOG("Quitting application...", LogType::Log);
        MainMenuPtr->GetScreen().ExitLoopClosure()();
        NOVA_LOG("Application exited successfully!", LogType::Log);
    });
    
    NOVA_LOG("Nova Documentation Setup started!", LogType::Log);

  
    RootAccessHelper* rootAccessHelper = RootAccessHelper::CreatePlatformSpecific();
    if (rootAccessHelper) {
        rootAccessHelper->Initialize();
        if (!rootAccessHelper->HasRootAccess()) {
            NOVA_LOG("Requesting root access...", LogType::Log);
            if (!rootAccessHelper->RequestElevatedPrivileges()) {
                NOVA_LOG("Failed to obtain root access. Exiting application.", LogType::Error);
                return 1;
            }
            rootAccessHelper->RunCommandWithElevatedPrivileges("osascript -e 'tell app \"Terminal\" to do script \"whoami\"'");
        }
    } else {
        NOVA_LOG("RootAccessHelper instance is null. Exiting application.", LogType::Error);
        return 1;
    }

    MainMenuPtr->Show();

    return 0;
}