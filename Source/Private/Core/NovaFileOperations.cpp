#include "NovaCore.h"
#include "NovaMinimal.h"
#include "NovaFileOperations.h"
#include "NovaLog.h"
#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace Core::FileOperations 
{
    // Initialize static members
    std::map<int, NovaFileOperations::DirectoryWatcher> NovaFileOperations::watchers;
    std::mutex NovaFileOperations::watchersMutex;
    int NovaFileOperations::nextWatcherId = 1;
    
    // FileInfo::GetFormattedTime implementation
    std::string FileInfo::GetFormattedTime() const {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastModified - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        auto tt = std::chrono::system_clock::to_time_t(sctp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&tt), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    std::string NovaFileOperations::NormalizePath(const std::string& path)
    {
        try {
            std::filesystem::path fsPath(path);
            if (fsPath.is_absolute()) {
                return path;
            }
            return (std::filesystem::current_path() / fsPath).string();
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to normalize path: " + path + " - " + e.what()).c_str(), LogType::Error);
            return path;
        }
    }

    bool NovaFileOperations::CreateDirectory(const std::string& path) 
    {
        try {
            if (!std::filesystem::exists(path)) {
                std::filesystem::create_directory(path);
                NOVA_LOG(("Created directory: " + path).c_str(), LogType::Log);
            }
            return true;
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to create directory: " + path + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    bool NovaFileOperations::CreateDirectoryStructure(const std::string& path) 
    {
        try {
            std::string normalizedPath = NormalizePath(path);
            if (std::filesystem::exists(normalizedPath)) {
                return true;
            }
            if (std::filesystem::create_directories(normalizedPath)) {
                NOVA_LOG(("Created directory structure: " + normalizedPath).c_str(), LogType::Log);
                return true;
            }
            NOVA_LOG(("Failed to create directory structure: " + normalizedPath).c_str(), LogType::Error);
            return false;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error creating directory structure: " + path + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    bool NovaFileOperations::DeleteFile(const std::string& path) 
    {
        try {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove(path);
                NOVA_LOG(("Deleted file: " + path).c_str(), LogType::Log);
            }
            return true;
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to delete file: " + path + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    bool NovaFileOperations::DeleteDirectory(const std::string& path) 
    {
        try {
            if (std::filesystem::exists(path)) {
                std::filesystem::remove_all(path);
                NOVA_LOG(("Deleted directory: " + path).c_str(), LogType::Log);
            }
            return true;
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to delete directory: " + path + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    bool NovaFileOperations::FileExists(const std::string& path) 
    {
        try {
            return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to check file existence: " + path + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    bool NovaFileOperations::DirectoryExists(const std::string& path) 
    {
        try {
            return std::filesystem::exists(path) && std::filesystem::is_directory(path);
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to check directory existence: " + path + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    bool NovaFileOperations::CopyFile(const std::string& source, const std::string& destination) 
    {
        try {
            std::string normalizedSource = NormalizePath(source);
            std::string normalizedDestination = NormalizePath(destination);
            std::filesystem::copy(normalizedSource, normalizedDestination, std::filesystem::copy_options::overwrite_existing);
            NOVA_LOG(("Copied file: " + normalizedSource + " to " + normalizedDestination).c_str(), LogType::Log);
            return true;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error copying file: " + source + " to " + destination + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    bool NovaFileOperations::MoveFile(const std::string& source, const std::string& destination) 
    {
        try {
            std::string normalizedSource = NormalizePath(source);
            std::string normalizedDestination = NormalizePath(destination);
            std::filesystem::rename(normalizedSource, normalizedDestination);
            NOVA_LOG(("Moved file: " + normalizedSource + " to " + normalizedDestination).c_str(), LogType::Log);
            return true;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error moving file: " + source + " to " + destination + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    std::string NovaFileOperations::ReadTextFile(const std::string& path) 
    {
        try {
            std::ifstream file(path);
            if (!file.is_open()) {
                NOVA_LOG(("Failed to open file for reading: " + path).c_str(), LogType::Error);
                return "";
            }
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            return content;
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to read file: " + path + " - " + e.what()).c_str(), LogType::Error);
            return "";
        }
    }

    bool NovaFileOperations::WriteTextFile(const std::string& path, const std::string& content, bool append) 
    {
        try {
            std::ofstream file(path, append ? std::ios::app : std::ios::out);
            if (!file.is_open()) {
                NOVA_LOG(("Failed to open file for writing: " + path).c_str(), LogType::Error);
                return false;
            }
            file << content;
            file.close();
            NOVA_LOG(("Wrote file: " + path).c_str(), LogType::Log);
            return true;
        } catch (const std::exception& e) {
            NOVA_LOG(("Failed to write to file: " + path + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    std::vector<std::string> NovaFileOperations::ListFiles(const std::string& path, bool recursive) 
    {
        std::vector<std::string> files;
        try {
            if (!DirectoryExists(path)) {
                NOVA_LOG(("Directory does not exist for listing: " + path).c_str(), LogType::Warning);
                return files;
            }
            if (recursive) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                    if (!entry.is_directory()) {
                        files.push_back(entry.path().string());
                    }
                }
            } else {
                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                    if (!entry.is_directory()) {
                        files.push_back(entry.path().string());
                    }
                }
            }
            NOVA_LOG(("Listed " + std::to_string(files.size()) + " files in " + path).c_str(), LogType::Log);
            return files;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error listing files in directory: " + path + " - " + e.what()).c_str(), LogType::Error);
            return files;
        }
    }
}
