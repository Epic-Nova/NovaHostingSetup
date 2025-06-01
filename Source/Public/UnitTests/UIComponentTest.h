#ifndef UI_COMPONENT_TEST_H
#define UI_COMPONENT_TEST_H

#include "BaseUnitTest.h"
#include "Menus/MainMenu.h"
#include <iostream>

class UIComponentTest : public BaseUnitTest {
public:
    std::string GetIdentifier() const override {
        return "com.epicnova.adi.fh.ds.unit_tests.ui_component_test";
    }

    bool Run() override {
        try {
            auto mainMenu = Menus::MainMenu::InitializeMainMenu();
            if (!mainMenu) {
                throw std::runtime_error("Failed to initialize MainMenu.");
            }

            mainMenu->SetMenuActionCallback("TestButton", []() {
                std::cout << "TestButton clicked!" << std::endl;
            });

            std::cout << "MainMenu initialized successfully with TestButton." << std::endl;
            return true; // Test passed
        } catch (const std::exception& e) {
            std::cerr << "UIComponentTest failed with exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "UIComponentTest failed with unknown exception." << std::endl;
            return false;
        }
    }
};

#endif // UI_COMPONENT_TEST_H
