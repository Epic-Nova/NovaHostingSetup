/// @file DocumentationSetup.cpp
/// @brief Main entry point for the setup process.

#include "NovaCore.h"
#include "NovaMinimal.h"
#include "Menus/MainMenu.h"
#include "Utils/CommandLineParsing.h"
#include "Utils/Interactables.h"
#include "Utils/TerminalUtils.h"
#include "UnitTests/BaseUnitTest.h"
#include "UnitTests/UnitTestManager.h"
#include "Menus/OptionsMenu.h"


using namespace Core;
using namespace Utils;

int main(int argc, const char* argv[]) 
{
    // Ensure terminal size is set
    Utils::TerminalUtils::SetTerminalSize(65, 40);

    // Initialize Logging system & Default directories
    NovaLog::StartLogFile();
    NovaLog::CreateRequiredDirectories();

    NOVA_LOG("Starting Nova Documentation Setup", LogType::Log);

    if (std::string(BUILD_CONFIGURATION) == "Development" || std::string(BUILD_CONFIGURATION) == "Testing") 
    {
        NOVA_LOG("Running in Development or Testing mode", LogType::Warning);
        NovaLog::SetVerbose(true);

        NOVA_LOG("Checking for unit tests...", LogType::Log);
    
        // Initialize unit test 
        UnitTestManager unitTestManager;
        unitTestManager.Initialize(argc, argv);

        // Run unit tests if requested
        if (unitTestManager.RunUnitTests()) {
            return 0; 
        }
    }
    else if(std::string(BUILD_CONFIGURATION) == "Production")
    {
        NOVA_LOG("Running in Production mode", LogType::Log);
        NovaLog::SetVerbose(false); 
    }

    
    // Initialize command-line options structure
    CommandLineOptionsStruct cmdOptions;
    {
        NOVA_LOG("Command line arguments received:", LogType::Log);
        for (int i = 0; i < argc; ++i) {
            NOVA_LOG(("Argument " + std::to_string(i) + ": " + argv[i]).c_str(), LogType::Log);
        }

        // Create options menu
        auto optionsMenu = Menus::OptionsMenu::Create(cmdOptions);

        // Register command-line options dynamically
        optionsMenu->RegisterCommandLineOptions();

        // Parse command-line arguments using dynamically registered options
        cmdOptions = CommandLineParsing::ParseArguments(argc, argv, optionsMenu->GetCommandLineMapping());

        // Apply command-line arguments to the options menu
        optionsMenu->ApplyCommandLineOptions(cmdOptions);
    }

    // Initialize main menu
    std::shared_ptr<Menus::MainMenu> MainMenuPtr;
    MainMenuPtr = Menus::MainMenu::InitializeMainMenu();

    // Register all menu callbacks
    Interactables::RegisterMenuCallbacks(MainMenuPtr, cmdOptions, "Content");
    MainMenuPtr->Show();

    return 0;
}