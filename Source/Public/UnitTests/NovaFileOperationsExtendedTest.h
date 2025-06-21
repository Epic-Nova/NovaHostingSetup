#ifndef NOVA_FILE_OPERATIONS_EXTENDED_TEST_H
#define NOVA_FILE_OPERATIONS_EXTENDED_TEST_H

#include "BaseUnitTest.h"
#include "NovaCore.h"
#include "NovaMinimal.h"
#include "Core/NovaFileOperations.h"
#include "Core/NovaLog.h"
#include <iostream>

using namespace Core::FileOperations;

class NovaFileOperationsExtendedTest : public BaseUnitTest {
public:
    std::string GetIdentifier() const override {
        return "com.epicnova.adi.fh.ds.unit_tests.nova_file_operations_extended_test";
    }
    
    bool Run() override {
        NOVA_LOG("Starting NovaFileOperations Extended Tests", Core::LogType::Log);
        
        try {
        NOVA_LOG("Starting NovaFileOperations Extended Tests", Core::LogType::Log);
        
        // Create a test directory structure
        std::string testBase = "TestExtendedFS";
        NovaFileOperations::CreateDirectory(testBase);
        
        std::string subdir1 = NovaFileOperations::JoinPaths(testBase, "subdir1");
        std::string subdir2 = NovaFileOperations::JoinPaths(testBase, "subdir2");
        
        NovaFileOperations::CreateDirectory(subdir1);
        NovaFileOperations::CreateDirectory(subdir2);
        
        // Create some test files
        NovaFileOperations::WriteTextFile(
            NovaFileOperations::JoinPaths(testBase, "test1.txt"), 
            "Test file 1 content"
        );
        
        NovaFileOperations::WriteTextFile(
            NovaFileOperations::JoinPaths(testBase, "test2.log"), 
            "Test file 2 content"
        );
        
        NovaFileOperations::WriteTextFile(
            NovaFileOperations::JoinPaths(subdir1, "subfile1.txt"), 
            "Subdir test file 1 content"
        );
        
        NovaFileOperations::WriteTextFile(
            NovaFileOperations::JoinPaths(subdir2, "subfile2.txt"), 
            "Subdir test file 2 content"
        );
        
        // Test FindFiles
        NOVA_LOG("Testing FindFiles", Core::LogType::Log);
        auto txtFiles = NovaFileOperations::FindFiles(testBase, "*.txt", true);
        NOVA_LOG(("Found " + std::to_string(txtFiles.size()) + " .txt files").c_str(), Core::LogType::Log);
        for (const auto& file : txtFiles) {
            NOVA_LOG(("  - " + file).c_str(), Core::LogType::Log);
        }
        
        // Test ListFilesDetailed with filter
        NOVA_LOG("Testing ListFilesDetailed with filter", Core::LogType::Log);
        FileFilterOptions filter;
        filter.extension = ".txt";
        filter.minSize = 5;  // Minimum size of 5 bytes
        
        auto detailedFiles = NovaFileOperations::ListFilesDetailed(testBase, true, filter);
        NOVA_LOG(("Found " + std::to_string(detailedFiles.size()) + " files matching filter criteria").c_str(), Core::LogType::Log);
        
        for (const auto& file : detailedFiles) {
            NOVA_LOG(("  - " + file.name + " (Size: " + std::to_string(file.size) + 
                      " bytes, Modified: " + file.GetFormattedTime() + ")").c_str(), Core::LogType::Log);
        }
        
        // Test path operations
        NOVA_LOG("Testing path operations", Core::LogType::Log);
        std::string testPath = NovaFileOperations::JoinPaths(subdir1, "subfile1.txt");
        
        NOVA_LOG(("File name with extension: " + NovaFileOperations::GetFileName(testPath)).c_str(), Core::LogType::Log);
        NOVA_LOG(("File name without extension: " + NovaFileOperations::GetFileName(testPath, false)).c_str(), Core::LogType::Log);
        NOVA_LOG(("File extension: " + NovaFileOperations::GetFileExtension(testPath)).c_str(), Core::LogType::Log);
        NOVA_LOG(("Parent directory: " + NovaFileOperations::GetParentDirectory(testPath)).c_str(), Core::LogType::Log);
        
        std::string relativePath = NovaFileOperations::GetRelativePath(testBase, testPath);
        NOVA_LOG(("Relative path from " + testBase + " to " + testPath + ": " + relativePath).c_str(), Core::LogType::Log);
        
        // Test file info
        NOVA_LOG("Testing file info", Core::LogType::Log);
        auto fileInfo = NovaFileOperations::GetFileInfo(testPath);
        if (fileInfo) {
            NOVA_LOG(("File info for " + testPath + ":").c_str(), Core::LogType::Log);
            NOVA_LOG(("  - Name: " + fileInfo->name).c_str(), Core::LogType::Log);
            NOVA_LOG(("  - Size: " + std::to_string(fileInfo->size) + " bytes").c_str(), Core::LogType::Log);
            NOVA_LOG(("  - Modified: " + fileInfo->GetFormattedTime()).c_str(), Core::LogType::Log);
            NOVA_LOG(("  - Is Directory: " + std::string(fileInfo->isDirectory ? "Yes" : "No")).c_str(), Core::LogType::Log);
        }
        
        // Test pattern matching
        NOVA_LOG("Testing pattern matching", Core::LogType::Log);
        bool matches = NovaFileOperations::MatchesPattern(testPath, "*.txt");
        NOVA_LOG(("Does " + testPath + " match *.txt? " + std::string(matches ? "Yes" : "No")).c_str(), Core::LogType::Log);
        
        matches = NovaFileOperations::MatchesPattern(testPath, "sub*.log");
        NOVA_LOG(("Does " + testPath + " match sub*.log? " + std::string(matches ? "Yes" : "No")).c_str(), Core::LogType::Log);
        
        // Test temp file path
        NOVA_LOG("Testing temp file path", Core::LogType::Log);
        std::string tempPath = NovaFileOperations::GetTempFilePath("nova_test_", ".tmp");
        NOVA_LOG(("Generated temp file path: " + tempPath).c_str(), Core::LogType::Log);
        
        // Test directory watching
        NOVA_LOG("Testing directory watching for 5 seconds...", Core::LogType::Log);
        int watchId = NovaFileOperations::WatchDirectory(
            testBase,
            [](const std::string& path, const std::string& event) {
                NOVA_LOG(("Watch event: " + event + " - " + path).c_str(), Core::LogType::Log);
            },
            500, // Check every 500ms
            true
        );
        
        // Make some changes to trigger the watcher
        std::this_thread::sleep_for(std::chrono::seconds(1));
        NovaFileOperations::WriteTextFile(NovaFileOperations::JoinPaths(testBase, "newfile.txt"), "New file content");
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        NovaFileOperations::WriteTextFile(NovaFileOperations::JoinPaths(testBase, "test1.txt"), "Modified content", false);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        NovaFileOperations::DeleteFile(NovaFileOperations::JoinPaths(testBase, "test2.log"));
        
        // Wait a bit more to see the changes
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Stop watching
        NovaFileOperations::StopWatching(watchId);
        NOVA_LOG("Stopped watching", Core::LogType::Log);
        
        // Clean up test directory
        NOVA_LOG("Cleaning up test directory", Core::LogType::Log);
        NovaFileOperations::DeleteDirectory(testBase);
        
            NOVA_LOG("NovaFileOperations Extended Tests completed", Core::LogType::Log);
            return true;
        } 
        catch (const std::exception& e) {
            NOVA_LOG(("NovaFileOperationsExtendedTest failed with exception: " + std::string(e.what())).c_str(), Core::LogType::Error);
            return false;
        } 
        catch (...) {
            NOVA_LOG("NovaFileOperationsExtendedTest failed with unknown exception", Core::LogType::Error);
            return false;
        }
    }
};

#endif // NOVA_FILE_OPERATIONS_EXTENDED_TEST_H
