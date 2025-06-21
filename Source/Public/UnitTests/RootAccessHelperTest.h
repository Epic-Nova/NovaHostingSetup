#ifndef ROOT_ACCESS_HELPER_TEST_H
#define ROOT_ACCESS_HELPER_TEST_H

#include "BaseUnitTest.h"
#include "Helpers/RootAccessHelper/RootAccessHelper.h"
#include <iostream>

class RootAccessHelperTest : public BaseUnitTest {
public:
    std::string GetIdentifier() const override {
        return "com.epicnova.adi.fh.ds.unit_tests.root_access_helper_test";
    }

    bool Run() override {
        try {
            auto rootHelper = Core::Helpers::RootAccessHelper::CreatePlatformSpecific();
            if (!rootHelper) {
                throw std::runtime_error("Failed to create RootAccessHelper.");
            }

            rootHelper->Initialize();
            std::cout << "RootAccessHelper initialized successfully." << std::endl;

            rootHelper->Execute([]() {
                std::cout << "Executing command with elevated privileges." << std::endl;
                // Simulate a command that requires root access
                return true; // Simulate success
            });
            if (!rootHelper->HasRootAccess()) {
                throw std::runtime_error("Root access was not granted.");
            }
            else {
                std::cout << "Root access granted successfully." << std::endl;
            }
            rootHelper->Shutdown();
            std::cout << "RootAccessHelper shutdown successfully." << std::endl;
            rootHelper->Reset();
            std::cout << "RootAccessHelper reset successfully." << std::endl;
            rootHelper->Abort();
            std::cout << "RootAccessHelper aborted successfully." << std::endl;
            if (rootHelper->IsRunning()) {
                throw std::runtime_error("RootAccessHelper is still running or aborted state is not cleared.");
            }
            std::cout << "RootAccessHelper test completed successfully." << std::endl;

            return true; // Test passed
        } catch (const std::exception& e) {
            std::cerr << "RootAccessHelperTest failed with exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "RootAccessHelperTest failed with unknown exception." << std::endl;
            return false;
        }
    }
};

#endif // ROOT_ACCESS_HELPER_TEST_H
