#ifndef NOVA_LOG_TEST_H
#define NOVA_LOG_TEST_H

#include "NovaCore.h"
#include "NovaMinimal.h"

#include "BaseUnitTest.h"
#include "NovaLog.h"
#include <iostream>

class NovaLogTest : public BaseUnitTest {
public:
    std::string GetIdentifier() const override {
        return "com.epicnova.adi.fh.ds.unit_tests.nova_log_test";
    }

    bool Run() override {
        try {
            Core::NovaLog::SetVerbose(true);
            Core::NovaLog::Type("Test message", Core::LogType::Log);
            Core::NovaLog::Type("Debug message", Core::LogType::Debug);
            Core::NovaLog::Type("Warning message", Core::LogType::Warning);
            Core::NovaLog::Type("Error message", Core::LogType::Error);

            // Simulate assertion
            try {
                Core::NovaLog::Assert(false, "Assertion test");
            } catch (...) {
                std::cout << "Assertion test passed." << std::endl;
            }

            return true; // Test passed
        } catch (const std::exception& e) {
            std::cerr << "NovaLogTest failed with exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "NovaLogTest failed with unknown exception." << std::endl;
            return false;
        }
    }
};

#endif // NOVA_LOG_TEST_H
