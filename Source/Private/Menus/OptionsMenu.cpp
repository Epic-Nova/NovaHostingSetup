#include "NovaCore.h"
#include "NovaMinimal.h"
#include "Utils/ScrollableLog.h"

//@TODO: Implement the options from the struct in the functionality of the program
namespace Menus
{
    using namespace ftxui;
    using namespace Core;

    OptionsMenu* OptionsMenu::SingletonPtr = nullptr;
    
    std::shared_ptr<OptionsMenu> OptionsMenu::Create(CommandLineOptionsStruct& options)
    {
        auto instance = std::shared_ptr<OptionsMenu>(new OptionsMenu(options));
        instance->LoadConfig(); // Load configuration upon creation
        SingletonPtr = instance.get(); // Set the singleton pointer
        return instance;
    }

    void OptionsMenu::LoadConfig()
    {
        NOVA_LOG("Loading configuration...", LogType::Log);
        try {
            std::string configPath = std::filesystem::current_path().string() + "/Configs/app_config.json";
            //Log the path for debugging
            if (std::filesystem::exists(configPath)) {
                std::ifstream configFile(configPath);
                nlohmann::json configJson;
                configFile >> configJson;

                options_.clearContent = configJson.value("clearContent", false);
                options_.noRoot = configJson.value("noRoot", false);
                options_.verbose = configJson.value("verbose", false);
                options_.requestRootForBrew = configJson.value("requestRootForBrew", false);
                options_.requestRootForPip = configJson.value("requestRootForPip", false);
                options_.requestRootForVenv = configJson.value("requestRootForVenv", false);
                options_.scrollableLogAlwaysVisible = configJson.value("scrollableLogAlwaysVisible", false);
                options_.mkdocsProjectPath = configJson.value("mkdocsProjectPath", "");

                NOVA_LOG("Configuration loaded successfully.", LogType::Log);
            } else {
                NOVA_LOG("Configuration file not found. Using default settings.", LogType::Warning);
            }
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to load configuration: " + std::string(e.what())).c_str(), LogType::Error);
        }
    }

    void OptionsMenu::SaveConfig()
    {
        NOVA_LOG("Saving configuration...", LogType::Log);
        try {
            std::string configPath = std::filesystem::current_path().string() + "/Content/config.json";
            nlohmann::json configJson;

            configJson["clearContent"] = options_.clearContent;
            configJson["noRoot"] = options_.noRoot;
            configJson["verbose"] = options_.verbose;
            configJson["requestRootForBrew"] = options_.requestRootForBrew;
            configJson["requestRootForPip"] = options_.requestRootForPip;
            configJson["requestRootForVenv"] = options_.requestRootForVenv;
            configJson["scrollableLogAlwaysVisible"] = options_.scrollableLogAlwaysVisible;
            configJson["mkdocsProjectPath"] = options_.mkdocsProjectPath;

            std::ofstream configFile(configPath);
            configFile << configJson.dump(4);

            NOVA_LOG("Configuration saved successfully.", LogType::Log);
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to save configuration: " + std::string(e.what())).c_str(), LogType::Error);
        }
    }

    void OptionsMenu::Show()
    {
        using namespace ftxui;
        NOVA_LOG("Showing enhanced options menu!", LogType::Log);

        auto start_time = std::chrono::steady_clock::now();
        bool show_particles = true;

        // Create styled checkbox
        auto makeStyledCheckbox = [&](const std::string& label, bool* value) {
            auto checkbox = Checkbox(label, value);
            return Renderer(checkbox, [=]() {
                return hbox({
                    text(" ") | size(WIDTH, EQUAL, 2),
                    text(label) | ftxui::bold | color(Color::Yellow),
                    text(" ") | size(WIDTH, EQUAL, 2),
                }) | center;
            });
        };

        // Create styled text box
        auto makeStyledTextBox = [&](const std::string& label, std::string* value) {
            auto input = Input(value);
            return Renderer(input, [=]() {
                return hbox({
                    text(label + ": ") | ftxui::bold | color(Color::Yellow),
                    input->Render() | size(WIDTH, GREATER_THAN, 35),
                }) | center;
            });
        };

        // Create checkboxes for boolean options
        auto clearContentCheckbox = makeStyledCheckbox("Clear Content", &options_.clearContent);
        auto noRootCheckbox = makeStyledCheckbox("No Root Access", &options_.noRoot);
        auto verboseCheckbox = makeStyledCheckbox("Verbose Mode", &options_.verbose);
        auto requestRootForBrewCheckbox = makeStyledCheckbox("Request Root for Homebrew", &options_.requestRootForBrew);
        auto requestRootForPipCheckbox = makeStyledCheckbox("Request Root for Pip", &options_.requestRootForPip);
        auto requestRootForVenvCheckbox = makeStyledCheckbox("Request Root for Virtual Environment", &options_.requestRootForVenv);
        auto scrollableLogAlwaysVisibleCheckbox = makeStyledCheckbox("Scrollable Log Always Visible", &options_.scrollableLogAlwaysVisible);

        // Create text box for string options
        auto mkdocsProjectPathTextBox = makeStyledTextBox("MkDocs Project Path", &options_.mkdocsProjectPath);

        // Back button
        auto BackButton = Button("🔙 Back", [&]() { GetScreen().ExitLoopClosure()(); });

        // Menu container
        auto menu = Container::Vertical({
            clearContentCheckbox,
            noRootCheckbox,
            verboseCheckbox,
            requestRootForBrewCheckbox,
            requestRootForPipCheckbox,
            requestRootForVenvCheckbox,
            scrollableLogAlwaysVisibleCheckbox,
            mkdocsProjectPathTextBox,
            BackButton,
        });

        // Main component with animations
        auto component = Renderer(menu, [=]() -> Element {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

            // Animated title
            std::vector<std::string> ascii_title {
                "  ██████╗ ███╗   ███╗ ██████╗ ████████╗██╗ ██████╗ ██╗   ██╗  ",
                "  ██╔══██╗████╗ ████║██╔═══██╗╚══██╔══╝██║██╔═══██╗██║   ██║  ",
                "  ██████╔╝██╔████╔██║██║   ██║   ██║   ██║██║   ██║██║   ██║  ",
                "  ██╔═══╝ ██║╚██╔╝██║██║   ██║   ██║   ██║██║   ██║╚██╗ ██╔╝  ",
                "  ██║     ██║ ╚═╝ ██║╚██████╔╝   ██║   ██║╚██████╔╝ ╚████╔╝   ",
                "  ╚═╝     ╚═╝     ╚═╝ ╚═════╝    ╚═╝   ╚═╝ ╚═════╝   ╚═══╝    "
            };
            Elements title_lines;
            for (size_t i = 0; i < ascii_title.size(); ++i) {
                auto line_color = Color::Yellow;
                title_lines.push_back(text(ascii_title[i]) | color(line_color) | center);
            }

            // Particle effect
            auto createParticles = [&]() -> Element {
                if (!show_particles) return text("");
                Elements rows;
                for (int y = 0; y < 2; ++y) {
                    Elements row;
                    for (int x = 0; x < 40; ++x) {
                        auto val = (elapsed + x * 91 + y * 211) / 70 % 6;
                        std::vector<std::string> stars{"✦", "✧", "❂", "❉", "✼", "⛧"};
                        row.push_back(text(stars[val]) | color(Color::Yellow));
                    }
                    rows.push_back(hbox(row));
                }
                return vbox(rows) | center | dim;
            };

            return vbox({
                createParticles(),
                vbox(title_lines),
                text("") | center,
                text("OPTIONS MENU") | ftxui::bold | color(Color::White) | center,
                text("") | center,
                menu->Render() | center,
            })
            | size(WIDTH, EQUAL, 80)
            | size(HEIGHT, EQUAL, 40)
            | border
            | center;
        });

        GetScreen().Loop(component);
    }

    void OptionsMenu::Hide()
    {
        NOVA_LOG("Hiding options menu!", LogType::Log);
        GetScreen().ExitLoopClosure()();
    }

    void OptionsMenu::RegisterCommandLineOptions()
    {
        NOVA_LOG("Registering command-line options dynamically...", LogType::Log);

        commandLineMapping_ = {
            {"clearContent", options_.clearContent ? new std::string("true") : new std::string("false")},
            {"noRoot", options_.noRoot ? new std::string("true") : new std::string("false")},
            {"verbose", options_.verbose ? new std::string("true") : new std::string("false")},
            {"requestRootForBrew", options_.requestRootForBrew ? new std::string("true") : new std::string("false")},
            {"requestRootForPip", options_.requestRootForPip ? new std::string("true") : new std::string("false")},
            {"requestRootForVenv", options_.requestRootForVenv ? new std::string("true") : new std::string("false")},
            {"scrollableLogAlwaysVisible", options_.scrollableLogAlwaysVisible ? new std::string("true") : new std::string("false")},
            {"mkdocsProjectPath", &options_.mkdocsProjectPath} // Keep pointer
        };

       // Register each option with its associated behavior
        optionManager_->RegisterOption("clearContent", options_.clearContent ? new std::string("true") : new std::string("false"), []() {
            std::string contentFolderPath = "Content";
            if (Core::FileOperations::NovaFileOperations::DirectoryExists(contentFolderPath)) {
                Core::FileOperations::NovaFileOperations::DeleteDirectory(contentFolderPath);
                NOVA_LOG("Content directory cleared", LogType::Log);
                Core::FileOperations::NovaFileOperations::CreateDirectory(contentFolderPath);
                Core::FileOperations::NovaFileOperations::CreateDirectory(contentFolderPath + "/Logs");
                Core::FileOperations::NovaFileOperations::CreateDirectory(contentFolderPath + "/Website");
                NOVA_LOG("Content directories recreated", LogType::Log);
            }
        });

        optionManager_->RegisterOption("noRoot", options_.noRoot ? new std::string("true") : new std::string("false"), []() {
            NOVA_LOG("No root access mode enabled", LogType::Log);
        });

        optionManager_->RegisterOption("verbose", options_.verbose ? new std::string("true") : new std::string("false"), []() {
            NOVA_LOG("Verbose mode enabled", LogType::Log);
        });

        optionManager_->RegisterOption("requestRootForBrew", options_.requestRootForBrew ? new std::string("true") : new std::string("false"), []() {
            NOVA_LOG("Requesting root access for Homebrew", LogType::Log);
        });

        optionManager_->RegisterOption("requestRootForPip", options_.requestRootForPip ? new std::string("true") : new std::string("false"), []() {
            NOVA_LOG("Requesting root access for Pip", LogType::Log);
        });

        optionManager_->RegisterOption("requestRootForVenv", options_.requestRootForVenv ? new std::string("true") : new std::string("false"), []() {
            NOVA_LOG("Requesting root access for Virtual Environment", LogType::Log);
        });

        optionManager_->RegisterOption("scrollableLogAlwaysVisible", options_.scrollableLogAlwaysVisible ? new std::string("true") : new std::string("false"), []() {
            auto scrollableLog = Utils::ScrollableLog::Create();
            scrollableLog->SetAlwaysVisible(true);
            NOVA_LOG("Scrollable log visibility set to always visible", LogType::Log);
        });

        optionManager_->RegisterOption("mkdocsProjectPath", &options_.mkdocsProjectPath, [this]() {
            NOVA_LOG(("MkDocs project path set to: " + options_.mkdocsProjectPath).c_str(), LogType::Log);
        });

        NOVA_LOG("Command-line options registered successfully.", LogType::Log);
    }

    void OptionsMenu::ApplyCommandLineOptions(const CommandLineOptionsStruct& cmdOptions)
    {
        NOVA_LOG("Applying command-line options dynamically...", LogType::Log);

        for (const auto& [key, value] : commandLineMapping_)
        {
            if (optionManager_->IsOptionEnabled(key)) // Check if the command-line argument is set
            {
                *value = true; // Override the corresponding menu option
                NOVA_LOG(("Command-line option '" + key + "' applied: " + (optionManager_->IsOptionEnabled(key) ? "Enabled" : "Disabled")).c_str(), LogType::Log);
            }
        }

        NOVA_LOG("Command-line options applied successfully.", LogType::Log);
    }
}
