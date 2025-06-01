#pragma once

#include "BaseMenu.h"

namespace Menus
{
    using namespace ftxui;

    class HelpMenu : public BaseMenu
    {
    public:
        static std::shared_ptr<HelpMenu> Create();
        
        void Show() override;
        void Hide() override;
        virtual ~HelpMenu() = default;

    private:
        HelpMenu() = default;
    };
}
