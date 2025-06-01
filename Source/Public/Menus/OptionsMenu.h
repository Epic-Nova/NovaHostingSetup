#pragma once

#include "BaseMenu.h"
#include "Utils/CommandLineParsing.h"
#include "Utils/CommandLineOptions.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace Menus
{
    using namespace ftxui;
    using Utils::CommandLineOptions;
    using Utils::CommandLineOptionsStruct;

    class OptionsMenu : public BaseMenu
    {
    public:
        static std::shared_ptr<OptionsMenu> Create(CommandLineOptionsStruct& options);

        static OptionsMenu* GetSingletonInstance() { return SingletonPtr; }
        
        void Show() override;
        void Hide() override;
        virtual ~OptionsMenu() = default;

        void LoadConfig();
        void SaveConfig();

        void RegisterCommandLineOptions();
        void ApplyCommandLineOptions(const CommandLineOptionsStruct& cmdOptions);
        const std::unordered_map<std::string, std::string*>& GetCommandLineMapping() const { return commandLineMapping_; }

    private:
        CommandLineOptionsStruct& options_;
        std::unordered_map<std::string, std::string*> commandLineMapping_;
        
        OptionsMenu(CommandLineOptionsStruct& options) : options_(options) {}

        CommandLineOptions* optionManager_ = Utils::CommandLineOptions::GetSingletonInstance();

        static OptionsMenu* SingletonPtr;
    };
}
