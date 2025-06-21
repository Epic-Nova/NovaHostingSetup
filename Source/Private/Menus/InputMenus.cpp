#include "InputMenus.h"
#include "NovaMinimal.h"
#include "NovaLog.h"

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include <chrono>

namespace Menus 
{
    using namespace ftxui;
    using namespace Core;
    
    std::pair<std::string, std::string> CredentialsInputMenu::ShowCredentialsInput(const std::string& title) 
    {
        auto screen = ScreenInteractive::Fullscreen();
        std::string username;
        std::string token;
        bool confirmed = false;
        bool canceled = false;
        bool showPassword = false;
        
        // Create input fields
        auto usernameInput = Input(&username, "GitHub Username");
        auto tokenInput = Input(&token, "Personal Access Token");
        
        // Toggle for password visibility
        auto togglePasswordButton = Button("👁 Toggle Token Visibility", [&] {
            showPassword = !showPassword;
        });
        
        // Buttons
        auto confirmButton = Button("✅ Confirm", [&] {
            if (!username.empty() && !token.empty()) {
                confirmed = true;
                screen.ExitLoopClosure()();
            }
        });
        
        auto cancelButton = Button("❌ Cancel", [&] {
            canceled = true;
            screen.ExitLoopClosure()();
        });
        
        auto layout = Container::Vertical({
            usernameInput,
            tokenInput,
            togglePasswordButton,
            Container::Horizontal({
                confirmButton,
                cancelButton,
            }),
        });
        
        auto start_time = std::chrono::steady_clock::now();
        
        // Main component
        auto component = Renderer(layout, [&] {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
            
            return RenderCredentialsUI(title, username, token, showPassword, elapsed);
        });
        
        // Run the UI
        screen.Loop(component);
        
        // Return the input values
        return canceled ? std::make_pair("", "") : std::make_pair(username, token);
    }
    
    Element CredentialsInputMenu::RenderCredentialsUI(
        const std::string& title,
        const std::string& username,
        const std::string& token,
        bool showPassword,
        int elapsed) 
    {
        // Create animated rainbow colors
        auto getRainbowColor = [&](int offset) -> Color {
            int hue = (elapsed / 100 + offset) % 360;
            if      (hue < 60)  return Color::Red;
            else if (hue < 120) return Color::Yellow;
            else if (hue < 180) return Color::Green;
            else if (hue < 240) return Color::Cyan;
            else if (hue < 300) return Color::Blue;
            else                return Color::Magenta;
        };
        
        // Status bar
        auto createStatusBar = [&]() -> Element {
            Elements bar;
            for (int i = 0; i < 36; ++i) {
                int idx = (elapsed / 120 + i) % 8;
                std::vector<std::string> bars{"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
                bar.push_back(text(bars[idx]) | color(getRainbowColor(i * 10)));
            }
            return hbox(bar) | center;
        };
        
        // Masked password display
        std::string displayToken = showPassword ? token : std::string(token.length(), '*');
        
        return vbox({
            text(title) | ftxui::bold | center | color(Color::Yellow),
            text("") | center,
            hbox({
                text("Username: ") | color(Color::Cyan),
                text(!username.empty() ? username : "Not set") | color(Color::White),
            }) | center,
            hbox({
                text("Token: ") | color(Color::Cyan),
                text(!token.empty() ? displayToken : "Not set") | color(Color::White),
            }) | center,
            text("") | center,
            text("Enter your GitHub credentials for repository access.") | color(Color::GrayLight) | center,
            text("") | center,
            text("") | center,
            createStatusBar(),
        }) 
        | size(WIDTH, EQUAL, 80) 
        | size(HEIGHT, EQUAL, 20) 
        | border 
        | center;
    }
    
    std::string PathInputMenu::ShowPathInput(const std::string& title, const std::string& defaultPath, const std::string& subtitle) 
    {
        auto screen = ScreenInteractive::Fullscreen();
        std::string path = defaultPath;
        bool confirmed = false;
        bool canceled = false;
        
        // Create input field
        auto pathInput = Input(&path, "Path");
        
        // Buttons
        auto confirmButton = Button("✅ Confirm", [&] {
            if (!path.empty()) {
                confirmed = true;
                screen.ExitLoopClosure()();
            }
        });
        
        auto cancelButton = Button("❌ Cancel", [&] {
            canceled = true;
            screen.ExitLoopClosure()();
        });
        
        auto layout = Container::Vertical({
            pathInput,
            Container::Horizontal({
                confirmButton,
                cancelButton,
            }),
        });
        
        auto start_time = std::chrono::steady_clock::now();
        
        // Main component
        auto component = Renderer(layout, [&] {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
            
            return RenderPathInputUI(title, subtitle, path, elapsed);
        });
        
        // Run the UI
        screen.Loop(component);
        
        // Return the input value
        return canceled ? "" : path;
    }
    
    Element PathInputMenu::RenderPathInputUI(
        const std::string& title,
        const std::string& subtitle,
        const std::string& path,
        int elapsed) 
    {
        // Create animated rainbow colors
        auto getRainbowColor = [&](int offset) -> Color {
            int hue = (elapsed / 100 + offset) % 360;
            if      (hue < 60)  return Color::Red;
            else if (hue < 120) return Color::Yellow;
            else if (hue < 180) return Color::Green;
            else if (hue < 240) return Color::Cyan;
            else if (hue < 300) return Color::Blue;
            else                return Color::Magenta;
        };
        
        // Status bar
        auto createStatusBar = [&]() -> Element {
            Elements bar;
            for (int i = 0; i < 36; ++i) {
                int idx = (elapsed / 120 + i) % 8;
                std::vector<std::string> bars{"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
                bar.push_back(text(bars[idx]) | color(getRainbowColor(i * 10)));
            }
            return hbox(bar) | center;
        };
        
        return vbox({
            text(title) | ftxui::bold | center | color(Color::Yellow),
            text("") | center,
            text(subtitle) | center | color(Color::GrayLight),
            text("") | center,
            hbox({
                text("Path: ") | color(Color::Cyan),
                text(!path.empty() ? path : "Not set") | color(Color::White),
            }) | center,
            text("") | center,
            text("") | center,
            createStatusBar(),
        }) 
        | size(WIDTH, EQUAL, 80) 
        | size(HEIGHT, EQUAL, 20) 
        | border 
        | center;
    }
}
