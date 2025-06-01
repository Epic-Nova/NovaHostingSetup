/// @file Interactables.cpp
/// @brief Implements user-interaction callbacks for menus.

#include "NovaCore.h"
#include "NovaMinimal.h"

#include "Menus/MainMenu.h"
#include "Menus/InstallationMenu.h"
#include "Menus/OptionsMenu.h"
#include "Menus/HelpMenu.h"
#include "Utils/ConfigManager.h"

#ifdef __APPLE__
#include "Helpers/BrewInstallHelper.h"
#endif

#include "Helpers/PipInstallHelper/PipInstallHelper.h"
#include "Helpers/RootAccessHelper/RootAccessHelper.h"
#include <filesystem>

namespace Utils
{
    using namespace Core;
    using namespace Core::Helpers;

    /**
     * Registers all main menu callbacks for user interaction.
     */
    void Interactables::RegisterMenuCallbacks(std::shared_ptr<MainMenu> mainMenu, 
                                              CommandLineOptionsStruct cmdOptions,
                                              const std::string& contentFolderPath) // Changed NovaFileHandle to std::string
    {
        RegisterOptionsCallback(mainMenu, cmdOptions);
        RegisterInstallRequirementsCallback(mainMenu, cmdOptions, contentFolderPath);
        RegisterStartDocumentationCallback(mainMenu, contentFolderPath);
        RegisterHelpCallback(mainMenu);
        RegisterQuitCallback(mainMenu);
    }

    /**
     * Registers the callback for the Options menu.
     */
    void Interactables::RegisterOptionsCallback(std::shared_ptr<MainMenu> mainMenu, 
                                                CommandLineOptionsStruct cmdOptions)
    {
        // Use the cmdOptions directly since it's already a copy
        mainMenu->SetMenuActionCallback("Options", [cmdOptions]() mutable {
            auto optionsMenu = OptionsMenu::Create(cmdOptions);
            optionsMenu->Show();
            
            // Save configuration after options menu closes
            CommandLineOptionsStruct config;
            config.clearContent = cmdOptions.clearContent;
            config.noRoot = cmdOptions.noRoot;
            config.verbose = cmdOptions.verbose;
            config.requestRootForBrew = cmdOptions.requestRootForBrew;
            config.requestRootForPip = cmdOptions.requestRootForPip;
            config.requestRootForVenv = cmdOptions.requestRootForVenv;
            config.mkdocsProjectPath = cmdOptions.mkdocsProjectPath;
            config.scrollableLogAlwaysVisible = cmdOptions.scrollableLogAlwaysVisible;
            NOVA_LOG("Saving configuration after options menu closed", LogType::Log);
            ConfigManager::SaveConfig(config);
        });
    }

    /**
     * Registers the callback for installing requirements.
     */
    void Interactables::RegisterInstallRequirementsCallback(std::shared_ptr<MainMenu> mainMenu, 
                                                            CommandLineOptionsStruct cmdOptions,
                                                            const std::string& contentFolderPath) // Changed NovaFileHandle to std::string
    {
        // Use cmdOptions directly as it's already passed by value
        mainMenu->SetMenuActionCallback("InstallRequirements", [cmdOptions, contentFolderPath]() 
        {
            NOVA_LOG("Starting installation process...", LogType::Debug);
            NOVA_LOG_VERBOSE("RegisterInstallRequirementsCallback: callback invoked", LogType::Debug);

            // Create and configure installation menu
            NOVA_LOG_VERBOSE("Creating installation menu instance", LogType::Debug);
            auto installMenu = InstallationMenu::Create();
            NOVA_LOG_VERBOSE("Installation menu instance created", LogType::Debug);
            
            // Add all installation steps with substeps
            installMenu->AddProgressStep("Check Homebrew");
            installMenu->AddSubStep("Check Homebrew", "Initialize Homebrew Helper");
            installMenu->AddSubStep("Check Homebrew", "Verify Installation");
            
            installMenu->AddProgressStep("Install/Verify Homebrew");
            installMenu->AddSubStep("Install/Verify Homebrew", "Request Root Access");
            installMenu->AddSubStep("Install/Verify Homebrew", "Download Installation Script");
            installMenu->AddSubStep("Install/Verify Homebrew", "Execute Installation");
            installMenu->AddSubStep("Install/Verify Homebrew", "Verify Installation");
            
            installMenu->AddProgressStep("Check Python/Pip");
            installMenu->AddSubStep("Check Python/Pip", "Initialize Pip Helper");
            installMenu->AddSubStep("Check Python/Pip", "Check Requirements");
            installMenu->AddSubStep("Check Python/Pip", "Verify Pip Installation");
            
            installMenu->AddProgressStep("Install/Verify Python/Pip");
            installMenu->AddSubStep("Install/Verify Python/Pip", "Install Python via Homebrew");
            installMenu->AddSubStep("Install/Verify Python/Pip", "Link Python Installation");
            installMenu->AddSubStep("Install/Verify Python/Pip", "Verify Pip Access");
            
            installMenu->AddProgressStep("Install Virtual Environment");
            installMenu->AddSubStep("Install Virtual Environment", "Install virtualenv package");
            
            installMenu->AddProgressStep("Create Virtual Environment");
            installMenu->AddSubStep("Create Virtual Environment", "Request Root Access");
            installMenu->AddSubStep("Create Virtual Environment", "Create venv directory");
            installMenu->AddSubStep("Create Virtual Environment", "Initialize virtual environment");
            
            installMenu->AddProgressStep("Install Python Packages");
            installMenu->AddSubStep("Install Python Packages", "Install MkDocs");
            installMenu->AddSubStep("Install Python Packages", "Install MkDocs Material");
            installMenu->AddSubStep("Install Python Packages", "Verify Installation");
            
            // Start installation in separate thread
            NOVA_LOG_VERBOSE("Creating installation thread", LogType::Debug);
            std::thread installThread([cmdOptions, contentFolderPath, installMenu]() {
                NOVA_LOG_VERBOSE("Installation thread started", LogType::Debug);
                
                // Store helper pointers for proper cleanup
                BrewInstallHelper* BrewInstallHelperPtr = nullptr;
                PipInstallHelper* PipInstallHelperPtr = nullptr;
                RootAccessHelper* RootAccessHelperPtr = nullptr;
                RootAccessHelper* RootAccessHelperPtr2 = nullptr;
                
                try {
                    std::string contentPath = contentFolderPath;
                    NOVA_LOG_VERBOSE(("Installation thread: content path = " + contentPath).c_str(), LogType::Debug);
                    
                    // Step 1: Check Homebrew
                    NOVA_LOG_VERBOSE("Installation thread: Step 1 - Check Homebrew", LogType::Debug);
                    installMenu->SetCurrentStep("Check Homebrew");
                    installMenu->SetCurrentSubStep("Check Homebrew", "Initialize Homebrew Helper");
                    
                    #ifdef __APPLE__
                    NOVA_LOG_VERBOSE("Creating BrewInstallHelper", LogType::Debug);
                    BrewInstallHelperPtr = BrewInstallHelper::CreatePlatformSpecific();
                    if (!BrewInstallHelperPtr) {
                        NOVA_LOG_VERBOSE("Failed to create BrewInstallHelper", LogType::Error);
                        throw std::runtime_error("Failed to create BrewInstallHelper");
                    }
                    NOVA_LOG_VERBOSE("BrewInstallHelper created successfully", LogType::Debug);
                    
                    BrewInstallHelperPtr->SetInstallCallbackFunction([installMenu](const std::string& msg) {
                        NOVA_LOG_VERBOSE(("Brew callback: " + msg).c_str(), LogType::Debug);
                        installMenu->UpdateProgress("Homebrew: " + msg);
                    });
                    
                    NOVA_LOG_VERBOSE("Initializing BrewInstallHelper", LogType::Debug);
                    BrewInstallHelperPtr->Initialize();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Check Homebrew", "Verify Installation");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    installMenu->CompleteCurrentStep();
                    
                    // Step 2: Install/Verify Homebrew
                    installMenu->SetCurrentStep("Install/Verify Homebrew");
                    installMenu->SetCurrentSubStep("Install/Verify Homebrew", "Request Root Access");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Install/Verify Homebrew", "Download Installation Script");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Install/Verify Homebrew", "Execute Installation");
                    if (!cmdOptions.noRoot) {
                        BrewInstallHelperPtr->Execute([&]() {
                            return BrewInstallHelperPtr->IsBrewInstalled();
                        });
                    } else {
                        installMenu->UpdateProgress("Skipping root access (no-root mode)");
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Install/Verify Homebrew", "Verify Installation");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    installMenu->CompleteCurrentStep();
                    
                    // Step 3: Check Python/Pip
                    installMenu->SetCurrentStep("Check Python/Pip");
                    installMenu->SetCurrentSubStep("Check Python/Pip", "Initialize Pip Helper");
                    
                    PipInstallHelper* PipInstallHelperPtr = PipInstallHelper::CreatePlatformSpecific();
                    PipInstallHelperPtr->SetInstallCallbackFunction([installMenu](const std::string& msg) {
                        installMenu->UpdateProgress("Pip: " + msg);
                    });
                    
                    PipInstallHelperPtr->Initialize();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Check Python/Pip", "Check Requirements");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Check Python/Pip", "Verify Pip Installation");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    installMenu->CompleteCurrentStep();
                    
                    // Step 4: Install/Verify Python/Pip
                    installMenu->SetCurrentStep("Install/Verify Python/Pip");
                    installMenu->SetCurrentSubStep("Install/Verify Python/Pip", "Install Python via Homebrew");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Install/Verify Python/Pip", "Link Python Installation");
                    if (!cmdOptions.noRoot) {
                        PipInstallHelperPtr->Execute([&]() {
                            return PipInstallHelperPtr->HasPipInstalled();
                        });
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    
                    installMenu->SetCurrentSubStep("Install/Verify Python/Pip", "Verify Pip Access");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    installMenu->CompleteCurrentSubStep();
                    installMenu->CompleteCurrentStep();

                    // Step 5: Install Virtual Environment
                    installMenu->SetCurrentStep("Install Virtual Environment");
                    installMenu->SetCurrentSubStep("Install Virtual Environment", "Install virtualenv package");
                    
                    PipInstallHelperPtr->InstallPackage("virtualenv", [&](std::string result) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        installMenu->CompleteCurrentSubStep();
                        installMenu->CompleteCurrentStep();
                        
                        // Step 6: Create Virtual Environment
                        installMenu->SetCurrentStep("Create Virtual Environment");
                        installMenu->SetCurrentSubStep("Create Virtual Environment", "Request Root Access");
                        std::this_thread::sleep_for(std::chrono::milliseconds(300));
                        installMenu->CompleteCurrentSubStep();
                        
                        if (!cmdOptions.noRoot) {
                            RootAccessHelper* RootAccessHelperPtr = RootAccessHelper::CreatePlatformSpecific();
                            RootAccessHelperPtr->SetInstallCallbackFunction([installMenu](const std::string& msg) {
                                installMenu->UpdateProgress("Root: " + msg);
                            });
                            RootAccessHelperPtr->Initialize();
                            
                            installMenu->SetCurrentSubStep("Create Virtual Environment", "Create venv directory");
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                            installMenu->CompleteCurrentSubStep();
                            
                            RootAccessHelper* RootAccessHelperPtr2 = RootAccessHelper::CreatePlatformSpecific();
                            RootAccessHelperPtr2->Execute([&]() {
                                std::string venvPath = contentPath + "/pyenv";
                                std::string venvCommand = "python3 -m venv " + venvPath;
                                
                                installMenu->SetCurrentSubStep("Create Virtual Environment", "Initialize virtual environment");
                                
                                if (RootAccessHelperPtr2->RunCommandWithElevatedPrivileges(venvCommand)) {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                                    installMenu->CompleteCurrentSubStep();
                                    installMenu->CompleteCurrentStep();
                                    
                                    // Step 7: Install Python Packages
                                    installMenu->SetCurrentStep("Install Python Packages");
                                    installMenu->SetCurrentSubStep("Install Python Packages", "Install MkDocs");
                                    
                                    std::string pipPath = venvPath + "/bin/pip";
                                    std::string mkdocsInstallCommand = pipPath + " install mkdocs mkdocs-material";
                                    
                                    if (RootAccessHelperPtr2->RunCommandWithElevatedPrivileges(mkdocsInstallCommand)) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                                        installMenu->CompleteCurrentSubStep();
                                        installMenu->SetCurrentSubStep("Install Python Packages", "Install MkDocs Material");
                                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                                        installMenu->CompleteCurrentSubStep();
                                        installMenu->SetCurrentSubStep("Install Python Packages", "Verify Installation");
                                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                                        installMenu->CompleteCurrentSubStep();
                                        installMenu->CompleteCurrentStep();
                                        installMenu->SetComplete();
                                    } else {
                                        installMenu->SetError("Failed to install Python packages");
                                    }
                                } else {
                                    installMenu->SetError("Failed to create virtual environment");
                                }
                                
                                return true;
                            });
                        } else {
                            installMenu->SetCurrentSubStep("Create Virtual Environment", "Create venv directory");
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                            installMenu->CompleteCurrentSubStep();
                            installMenu->SetCurrentSubStep("Create Virtual Environment", "Initialize virtual environment");
                            installMenu->UpdateProgress("Skipping virtual environment creation (no-root mode)");
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                            installMenu->CompleteCurrentSubStep();
                            installMenu->CompleteCurrentStep();
                            installMenu->SetComplete();
                        }
                    });
                    #endif
                    
                } catch (const std::exception& e) {
                    NOVA_LOG_VERBOSE(("Installation thread exception: " + std::string(e.what())).c_str(), LogType::Error);
                    
                    // Cleanup on exception
                    if (BrewInstallHelperPtr) delete BrewInstallHelperPtr;
                    if (PipInstallHelperPtr) delete PipInstallHelperPtr;
                    if (RootAccessHelperPtr) delete RootAccessHelperPtr;
                    if (RootAccessHelperPtr2) delete RootAccessHelperPtr2;
                    
                    installMenu->SetError(std::string("Installation failed: ") + e.what());
                }
                NOVA_LOG_VERBOSE("Installation thread ending", LogType::Debug);
            });

            NOVA_LOG_VERBOSE("Starting installation menu show", LogType::Debug);
            // Show installation menu (this will block until installation is complete)
            installMenu->Show();
            NOVA_LOG_VERBOSE("Installation menu show completed", LogType::Debug);
            
            // Wait for installation thread to complete
            NOVA_LOG_VERBOSE("Waiting for installation thread to join", LogType::Debug);
            if (installThread.joinable()) {
                installThread.join();
                NOVA_LOG_VERBOSE("Installation thread joined successfully", LogType::Debug);
            } else {
                NOVA_LOG_VERBOSE("Installation thread was not joinable", LogType::Warning);
            }
            
            NOVA_LOG("Installation process completed!", LogType::Log);
            NOVA_LOG_VERBOSE("InstallRequirements callback completed", LogType::Debug);
        });
    }

    /**
     * Registers the callback for starting the documentation webpage.
     */
    void Interactables::RegisterStartDocumentationCallback(std::shared_ptr<MainMenu> mainMenu,
                                                           const std::string& contentFolderPath) // Changed NovaFileHandle to std::string
    {
        mainMenu->SetMenuActionCallback("StartDocumentationWebpage", [&contentFolderPath]() {
            NOVA_LOG("Starting documentation webpage...", LogType::Debug);
            
            std::string contentPath = contentFolderPath;
            std::string venvPath = contentPath + "/pyenv";
            
            #ifdef _WIN32
            std::string mkdocsPath = venvPath + "/Scripts/mkdocs";
            #else
            std::string mkdocsPath = venvPath + "/bin/mkdocs";
            #endif
            
            std::string serveCommand = mkdocsPath + " serve";
            NOVA_LOG(("Starting MkDocs server with command: " + serveCommand).c_str(), LogType::Debug);
            
            NOVA_LOG("Documentation webpage started successfully!", LogType::Log);
            NOVA_LOG("You can access the documentation at: http://localhost:8000", LogType::Log);
        });
    }

    /**
     * Registers the callback for showing help.
     */
    void Interactables::RegisterHelpCallback(std::shared_ptr<MainMenu> mainMenu)
    {
        mainMenu->SetMenuActionCallback("ShowHelp", []() {
            NOVA_LOG("Showing help menu...", LogType::Debug);
            auto helpMenu = HelpMenu::Create();
            helpMenu->Show();
        });
    }

    /**
     * Registers the callback for quitting the application.
     */
    void Interactables::RegisterQuitCallback(std::shared_ptr<MainMenu> mainMenu)
    {
        mainMenu->SetMenuActionCallback("Quit", [mainMenu]() {
            NOVA_LOG("Quitting application...", LogType::Debug);
            // First exit the screen loop
            exit(0);
            mainMenu->GetScreen().ExitLoopClosure();

            // Then exit the application
            #ifdef _WIN32
            PostQuitMessage(0);
            #else
            std::exit(0);
            #endif
        });
    }
}
