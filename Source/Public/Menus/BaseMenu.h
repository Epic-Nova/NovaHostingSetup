#pragma once

#include "NovaMinimal.h"
#include "MenuMinimal.h"

namespace Menus
{
    using namespace ftxui;

    class BaseMenu
    {
        public:
            virtual void Show() = 0;
            virtual void Hide() = 0;

            ScreenInteractive& GetScreen()
            {
                return Screen;
            }

            // Helper functions to manage menu action callbacks
            std::shared_ptr<std::function<void()>> SetMenuActionCallback(std::string MenuAction, std::function<void()> callback);
            std::shared_ptr<std::function<void()>> GetMenuActionCallback(const std::string& MenuAction);

            protected:
            
            ScreenInteractive Screen = ScreenInteractive::Fullscreen();

            static std::map<std::string, std::shared_ptr<std::function<void()>>>* MenuActionsReference;
    };
}