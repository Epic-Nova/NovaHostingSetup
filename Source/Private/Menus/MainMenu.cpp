#include "MainMenu.h"
#include "NovaCore.h"

// Add these FTXUI includes at the top
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace Menus
{
    using namespace ftxui;
    using namespace Core;

    std::shared_ptr<MainMenu> MainMenu::InitializeMainMenu()
    {
        auto MainMenuSharedPtr = std::make_shared<MainMenu>();
        return MainMenuSharedPtr;
    }

    void MainMenu::Show()
    {
        NOVA_LOG("Showing main menu!", LogType::Log);

        auto InstallRequirementsButton = Button("Install Requirements", *GetMenuActionCallback("InstallRequirements"), ButtonOption::Animated(Color::Black, Color::Purple, Color::White, Color::MediumPurple));
        auto StartDocumentationWebpageButton = Button("Start Documentation Webpage", *GetMenuActionCallback("StartDocumentationWebpage"), ButtonOption::Animated(Color::Black, Color::Purple, Color::White, Color::MediumPurple));
        auto HelpButton = Button("Help", *GetMenuActionCallback("ShowHelp"), ButtonOption::Animated(Color::Black, Color::Purple, Color::White, Color::MediumPurple));
        auto QuitButton = Button("Quit", *GetMenuActionCallback("Quit"), ButtonOption::Animated(Color::Black, Color::Purple, Color::White, Color::MediumPurple));
        
        NOVA_LOG("Created main menu buttons!", LogType::Log);

        auto menu = Container::Vertical({
            InstallRequirementsButton,
            StartDocumentationWebpageButton,
            HelpButton,
            QuitButton
            });

            //i want this to be fill out the whole window space
            
        auto component = Renderer(menu, [&] {
            return vbox({
                vbox({
                    text("Nova Documentation Setup") | bold | center | color(Color::White) | bgcolor(Color::Black),
                    separator(),
                    text("Welcome to the Nova Documentation Setup!") | center,
                    separator(),
                    text("Please select an option below:") | center
                }) | border | center,
                separator(),
                menu->Render() | center
            }) | size(WIDTH, GREATER_THAN, 70) | center;
        });

        GetScreen().Loop(component);
    }

    void MainMenu::Hide()
    {
        NOVA_LOG("Hiding main menu!", LogType::Log);
        GetScreen().ExitLoopClosure()();
    }

    Component MainMenu::Wrap(std::string name, Component component)
    {
        return Renderer(component, [component, name] {
            return window(text(name), component->Render());
        });
    }
}