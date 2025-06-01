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
        auto QuitButton = Button("Quit", *GetMenuActionCallback("Quit"), ButtonOption::Animated(Color::Black, Color::Purple, Color::White, Color::MediumPurple));
        
        NOVA_LOG("Created main menu buttons!", LogType::Log);

        auto menu = Container::Vertical({
            InstallRequirementsButton,
            StartDocumentationWebpageButton,
            QuitButton
            });

        auto component = Renderer(menu, [&] {
            return vbox({
                menu->Render(),
            }) | xflex | size(WIDTH, GREATER_THAN, 40) | border;
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