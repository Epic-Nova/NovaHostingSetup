#pragma once

#include "NovaMinimal.h"
#include "MenuMinimal.h"

namespace Menus
{
    /**
     * Base class for all menus.
     */
    class BaseMenu
    {
    public:
        BaseMenu() = default;
        virtual ~BaseMenu() = default;
        
        /**
         * Display the menu and handle input.
         */
        virtual void Show() = 0;
        
        /**
         * Hide the menu.
         */
        virtual void Hide() = 0;
        
        /**
         * Get the FTXUI screen used for this menu.
         */
        ftxui::ScreenInteractive& GetScreen()
        {
            static ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::FitComponent();
            if (!screen_) {
                screen_ = &screen;
                screen_->SetCursor(ftxui::ScreenInteractive::Cursor{1, 1, ftxui::ScreenInteractive::Cursor::Shape::Bar}); // 1 = bar cursor
            }
            return *screen_;
        }
        
        std::shared_ptr<std::function<void()>> SetMenuActionCallback(std::string MenuAction, std::function<void()> callback);

        virtual std::shared_ptr<std::function<void()>> GetMenuActionCallback(const std::string& actionName);
        
        static std::map<std::string, std::shared_ptr<std::function<void()>>>* MenuActionsReference;

    protected:
        ftxui::Component CreateSizedComponent(ftxui::Component component);

    private:
        ftxui::ScreenInteractive* screen_;
    };
}