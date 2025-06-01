#ifndef BREW_INSTALL_HELPER_TEST_H
#define BREW_INSTALL_HELPER_TEST_H

#include "BaseUnitTest.h"
#include "Helpers/BrewInstallHelper.h"
#include <iostream>

class BrewInstallHelperTest : public BaseUnitTest {
public:
    std::string GetIdentifier() const override {
        return "com.epicnova.adi.fh.ds.unit_tests.brew_install_helper_test";
    }

    bool Run() override {
        try {
            #ifdef __APPLE__
            auto brewHelper = Core::Helpers::BrewInstallHelper::CreatePlatformSpecific();
            if (!brewHelper) {
                throw std::runtime_error("Failed to create BrewInstallHelper.");
            }

            brewHelper->Initialize();
            if (!brewHelper->IsBrewInstalled()) {
                throw std::runtime_error("Homebrew is not installed.");
            }

            std::cout << "Homebrew is installed and functional." << std::endl;
            #else
            std::cout << "BrewInstallHelperTest skipped (not on macOS)." << std::endl;
            #endif

            return true; // Test passed
        } catch (const std::exception& e) {
            std::cerr << "BrewInstallHelperTest failed with exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "BrewInstallHelperTest failed with unknown exception." << std::endl;
            return false;
        }
    }
};

#endif // BREW_INSTALL_HELPER_TEST_H
