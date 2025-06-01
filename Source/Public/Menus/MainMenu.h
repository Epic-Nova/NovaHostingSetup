#pragma once

#include "BaseMenu.h"

namespace Menus
{
    using namespace ftxui;

    class MainMenu : public BaseMenu
    {
        public:
            static std::shared_ptr<MainMenu> InitializeMainMenu();            
            void Show() override;
            void Hide() override;
            virtual ~MainMenu() = default;

        Component Wrap(std::string name, Component component);
     };
}