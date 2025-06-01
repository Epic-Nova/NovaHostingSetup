#ifndef BASE_UNIT_TEST_H
#define BASE_UNIT_TEST_H

#include <string>
#include <iostream>

class BaseUnitTest {
public:
    virtual ~BaseUnitTest() = default;

    // Returns the unique identifier for the unit test
    virtual std::string GetIdentifier() const = 0;

    // Executes the unit test
    virtual bool Run() = 0;

    // Logs the result of the test
    void LogResult(bool success) const {
        std::cout << "Unit Test [" << GetIdentifier() << "] "
                  << (success ? "PASSED" : "FAILED") << std::endl;
    }
};

#endif // BASE_UNIT_TEST_H
