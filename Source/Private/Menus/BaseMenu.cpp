#include "BaseMenu.h"
#include "NovaCore.h"

namespace Menus
{

    // Static member initialization
    std::map<std::string, std::shared_ptr<std::function<void()>>>* BaseMenu::MenuActionsReference = new std::map<std::string, std::shared_ptr<std::function<void()>>>();

    using namespace Core;

    std::shared_ptr<std::function<void()>> BaseMenu::SetMenuActionCallback(std::string MenuAction, std::function<void()> callback)
    {
        if (MenuActionsReference->find(MenuAction) == MenuActionsReference->end()) {
            if (!callback) callback = [](){};
            MenuActionsReference->insert({ MenuAction, std::make_shared<std::function<void()>>(callback) });
            NOVA_LOG(("Menu action: " + MenuAction + " set!").c_str(), LogType::Log);
        } 
        else 
        {
            if (!callback) callback = [](){};
            // Update the function INSIDE the existing shared_ptr, don't replace the shared_ptr
            *(*MenuActionsReference)[MenuAction] = callback;
            NOVA_LOG(("Menu action: " + MenuAction + " updated!").c_str(), LogType::Log);
        }

        return MenuActionsReference->at(MenuAction);
    }

    std::shared_ptr<std::function<void()>> BaseMenu::GetMenuActionCallback(const std::string& MenuAction)
    {
        if (MenuActionsReference->find(MenuAction) != MenuActionsReference->end())
        {
            return MenuActionsReference->at(MenuAction);
        }
        else
        {
            NOVA_LOG(("Menu action: " + MenuAction + " not found!").c_str(), LogType::Error);
            // Always return a valid no-op callback
            return std::make_shared<std::function<void()>>([](){});
        }
    }
}