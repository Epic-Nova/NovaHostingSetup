#ifndef NOVA_FILE_OPERATIONS_TEST_H
#define NOVA_FILE_OPERATIONS_TEST_H

#include "BaseUnitTest.h"
#include "NovaFileOperations.h"
#include <iostream>

class NovaFileOperationsTest : public BaseUnitTest {
public:
    std::string GetIdentifier() const override {
        return "com.epicnova.adi.fh.ds.unit_tests.nova_file_operations_test";
    }

    bool Run() override {
        try {
            std::string testDir = "TestDir";
            std::string testFile = testDir + "/test.txt";

            // Test directory creation
            Core::FileOperations::NovaFileOperations::CreateDirectory(testDir);
            if (!Core::FileOperations::NovaFileOperations::DirectoryExists(testDir)) {
                throw std::runtime_error("Directory creation failed.");
            }

            // Test file creation and writing
            Core::FileOperations::NovaFileOperations::WriteTextFile(testFile, "Hello, World!");
            if (!Core::FileOperations::NovaFileOperations::FileExists(testFile)) {
                throw std::runtime_error("File creation failed.");
            }

            // Test file reading
            std::string content = Core::FileOperations::NovaFileOperations::ReadTextFile(testFile);
            if (content != "Hello, World!") {
                throw std::runtime_error("File content mismatch.");
            }

            // Test file deletion
            Core::FileOperations::NovaFileOperations::DeleteFile(testFile);
            if (Core::FileOperations::NovaFileOperations::FileExists(testFile)) {
                throw std::runtime_error("File deletion failed.");
            }

            // Test directory deletion
            Core::FileOperations::NovaFileOperations::DeleteDirectory(testDir);
            if (Core::FileOperations::NovaFileOperations::DirectoryExists(testDir)) {
                throw std::runtime_error("Directory deletion failed.");
            }

            return true; // Test passed
        } catch (const std::exception& e) {
            std::cerr << "NovaFileOperationsTest failed with exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "NovaFileOperationsTest failed with unknown exception." << std::endl;
            return false;
        }
    }
};

#endif // NOVA_FILE_OPERATIONS_TEST_H
