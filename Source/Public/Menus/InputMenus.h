#pragma once

#include "NovaCore.h"
#include "ftxui/component/component.hpp"
#include <string>

namespace Menus 
{
    class CredentialsInputMenu 
    {
    public:
        static std::pair<std::string, std::string> ShowCredentialsInput(const std::string& title = "Enter Git Credentials");
        
    private:
        static ftxui::Element RenderCredentialsUI(
            const std::string& title,
            const std::string& username,
            const std::string& token,
            bool showPassword,
            int elapsed
        );
    };
    
    class PathInputMenu 
    {
    public:
        static std::string ShowPathInput(const std::string& title = "Enter Path", 
                                        const std::string& defaultPath = "",
                                        const std::string& subtitle = "");
        
    private:
        static ftxui::Element RenderPathInputUI(
            const std::string& title,
            const std::string& subtitle,
            const std::string& path,
            int elapsed
        );
    };
}
