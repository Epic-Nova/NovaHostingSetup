#include "NovaCore.h"
#include "NovaMinimal.h"
#include "MainMenu.h"
#include "Utils/CommandLineOptions.h"

namespace Menus
{
    using namespace ftxui;
    using namespace Core;

    std::shared_ptr<HelpMenu> HelpMenu::Create()
    {
        return std::shared_ptr<HelpMenu>(new HelpMenu());
    }

    void HelpMenu::Show()
    {
        NOVA_LOG("Showing enhanced help menu!", LogType::Log);

        auto start_time = std::chrono::steady_clock::now();
        bool show_particles = true;

        // Access the command list dynamically
        auto commands = OptionsMenu::GetSingletonInstance()->GetCommandLineMapping();

        Elements commandButtons;
        for (const auto& [command, description] : commands) {
            commandButtons.push_back(
                hbox({
                    text("▶ " + command + " : ") | ftxui::color(Color::Yellow) | ftxui::bold,
                    text(*description) | ftxui::color(Color::White)
                }) | center | size(WIDTH, GREATER_THAN, 60)
            );
        }

        auto BackButton = Button("🔙 Back", [&]() { GetScreen().ExitLoopClosure()(); });

        auto menu = Container::Vertical({
            Renderer([&]() { return vbox(commandButtons); }),
            BackButton
        });

        auto component = Renderer(menu, [=]() mutable -> Element {
            // Removed unused variable 'elapsed'
            auto createTitle = [&]() -> Element {
                std::vector<std::string> title_lines{
                    "██╗  ██╗███████╗██╗     ██████╗ ",
                    "██║  ██║██╔════╝██║     ██╔══██╗",
                    "███████║█████╗  ██║     ██████╔╝",
                    "██╔══██║██╔══╝  ██║     ██╔═══╝ ",
                    "██║  ██║███████╗███████╗██║     ",
                    "╚═╝  ╚═╝╚══════╝╚══════╝╚═╝     "
                };

                Elements title;
                for (size_t i = 0; i < title_lines.size(); ++i) {
                    title.push_back(text(title_lines[i]) | ftxui::color(Color::Yellow) | center);
                }
                return vbox(title);
            };

            return vbox({
                createTitle(),
                text("HELP MENU") | ftxui::bold | center | ftxui::color(Color::White),
                menu->Render() | center,
            }) | border | center;
        });

        auto enhanced_component = CatchEvent(component, [&](Event event) {
            if (event == Event::Character('p') || event == Event::Character('P')) {
                show_particles = !show_particles;
                return true;
            }
            return false;
        });

        GetScreen().Loop(enhanced_component);
    }

    void HelpMenu::Hide()
    {
        NOVA_LOG("Hiding help menu!", LogType::Log);
        GetScreen().ExitLoopClosure()();
    }
}
