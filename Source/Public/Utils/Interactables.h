#pragma once

#include "NovaMinimal.h"
#include "Utils/CommandLineOptions.h"

// Forward declarations
namespace Menus {
    class MainMenu;
}

namespace Utils
{
    using namespace Menus;

    /**
     * Utility class for registering menu callbacks.
     */
    class Interactables
    {
    public:
        /**
         * Register all menu callbacks.
         */
        static void RegisterMenuCallbacks(std::shared_ptr<MainMenu> mainMenu, 
                                          CommandLineOptionsStruct cmdOptions,
                                          const std::string& contentFolderPath);
        
    private:
        static void RegisterInstallRequirementsCallback(std::shared_ptr<MainMenu> mainMenu, 
                                                       CommandLineOptionsStruct cmdOptions,
                                                       const std::string& contentFolderPath);
        
        static void RegisterOptionsCallback(std::shared_ptr<MainMenu> mainMenu, 
                                            CommandLineOptionsStruct cmdOptions);
        
        static void RegisterStartDocumentationCallback(std::shared_ptr<MainMenu> mainMenu,
                                                      const std::string& contentFolderPath);
        
        static void RegisterHelpCallback(std::shared_ptr<MainMenu> mainMenu);
        
        static void RegisterQuitCallback(std::shared_ptr<MainMenu> mainMenu);
    };
}
