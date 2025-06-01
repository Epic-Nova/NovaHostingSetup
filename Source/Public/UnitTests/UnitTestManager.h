#ifndef UNIT_TEST_MANAGER_H
#define UNIT_TEST_MANAGER_H

#include "BaseUnitTest.h"
#include <vector>
#include <memory>
#include <string>

class UnitTestManager {
public:
    void Initialize(int argc, const char* argv[]);
    bool RunUnitTests();

private:
    bool runUnitTest = false;
    std::string unitTestId;
};

#endif // UNIT_TEST_MANAGER_H
