#ifndef PIP_INSTALL_HELPER_TEST_H
#define PIP_INSTALL_HELPER_TEST_H

#include "BaseUnitTest.h"
#include "Helpers/PipInstallHelper/PipInstallHelper.h"
#include <iostream>

class PipInstallHelperTest : public BaseUnitTest {
public:
    std::string GetIdentifier() const override {
        return "com.epicnova.adi.fh.ds.unit_tests.pip_install_helper_test";
    }

    bool Run() override {
        try {
            auto pipHelper = Core::Helpers::PipInstallHelper::CreatePlatformSpecific();
            if (!pipHelper) {
                throw std::runtime_error("Failed to create PipInstallHelper.");
            }

            pipHelper->Initialize();
            if (!pipHelper->HasPipInstalled()) {
                throw std::runtime_error("Pip is not installed.");
            }

            std::cout << "Pip is installed and functional." << std::endl;
            return true; // Test passed
        } catch (const std::exception& e) {
            std::cerr << "PipInstallHelperTest failed with exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "PipInstallHelperTest failed with unknown exception." << std::endl;
            return false;
        }
    }
};

#endif // PIP_INSTALL_HELPER_TEST_H
