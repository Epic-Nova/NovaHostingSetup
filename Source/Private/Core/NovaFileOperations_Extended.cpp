// Extended functionality for NovaFileOperations
// This file contains implementation of additional file system utilities

#include "NovaCore.h"
#include "NovaMinimal.h"
#include "NovaFileOperations.h"
#include "NovaLog.h"

namespace Core::FileOperations 
{
    std::vector<FileInfo> NovaFileOperations::ListFilesDetailed(
        const std::string& path, 
        bool recursive,
        const FileFilterOptions& filter) 
    {
        std::vector<FileInfo> result;
        try {
            if (!DirectoryExists(path)) {
                NOVA_LOG(("Directory does not exist for listing: " + path).c_str(), LogType::Warning);
                return result;
            }

            // Prepare regex pattern if provided
            std::optional<std::regex> nameRegex;
            if (filter.namePattern.has_value()) {
                nameRegex = WildcardToRegex(filter.namePattern.value());
            }

            if (recursive) {
                std::filesystem::recursive_directory_iterator recIterator(path);
                for (const auto& entry : recIterator) {
                    bool isDir = entry.is_directory();
                    
                    // Apply directory/file filtering
                    if ((isDir && !filter.includeDirectories) || (!isDir && !filter.includeFiles)) {
                        continue;
                    }
                    
                    // Skip hidden files if not included
                    if (!filter.includeHidden) {
                        auto filename = entry.path().filename().string();
                        if (!filename.empty() && filename[0] == '.') {
                            continue;
                        }
                    }
                    
                    // Get file information
                    FileInfo info;
                    info.path = entry.path().string();
                    info.name = entry.path().filename().string();
                    info.isDirectory = isDir;
                    
                    if (!isDir) {
                        info.extension = entry.path().extension().string();
                        info.size = entry.file_size();
                        info.lastModified = entry.last_write_time();
                        
                        // Apply extension filter
                        if (filter.extension.has_value() && info.extension != filter.extension.value()) {
                            continue;
                        }
                        
                        // Apply size filters
                        if (filter.minSize.has_value() && info.size < filter.minSize.value()) {
                            continue;
                        }
                        
                        if (filter.maxSize.has_value() && info.size > filter.maxSize.value()) {
                            continue;
                        }
                    } else {
                        info.extension = "";
                        info.size = 0;
                        info.lastModified = entry.last_write_time();
                    }
                    
                    // Apply modification time filters
                    if (filter.modifiedAfter.has_value() && info.lastModified < filter.modifiedAfter.value()) {
                        continue;
                    }
                    
                    if (filter.modifiedBefore.has_value() && info.lastModified > filter.modifiedBefore.value()) {
                        continue;
                    }
                    
                    // Apply name pattern filter
                    if (filter.namePattern.has_value()) {
                        if (!std::regex_match(info.name, nameRegex.value())) {
                            continue;
                        }
                    }
                    
                    // Apply regex pattern filter
                    if (filter.regexPattern.has_value() && 
                        !std::regex_match(info.name, filter.regexPattern.value())) {
                        continue;
                    }
                    
                    // If we got here, the entry passed all filters
                    result.push_back(info);
                }
            } else {
                std::filesystem::directory_iterator dirIterator(path);
                for (const auto& entry : dirIterator) {
                    bool isDir = entry.is_directory();
                
                // Apply directory/file filtering
                if ((isDir && !filter.includeDirectories) || (!isDir && !filter.includeFiles)) {
                    continue;
                }
                
                // Skip hidden files if not included
                if (!filter.includeHidden) {
                    auto filename = entry.path().filename().string();
                    if (!filename.empty() && filename[0] == '.') {
                        continue;
                    }
                }
                
                // Get file information
                FileInfo info;
                info.path = entry.path().string();
                info.name = entry.path().filename().string();
                info.isDirectory = isDir;
                
                if (!isDir) {
                    info.extension = entry.path().extension().string();
                    info.size = entry.file_size();
                    info.lastModified = entry.last_write_time();
                    
                    // Apply extension filter
                    if (filter.extension.has_value() && info.extension != filter.extension.value()) {
                        continue;
                    }
                    
                    // Apply size filters
                    if (filter.minSize.has_value() && info.size < filter.minSize.value()) {
                        continue;
                    }
                    
                    if (filter.maxSize.has_value() && info.size > filter.maxSize.value()) {
                        continue;
                    }
                } else {
                    info.extension = "";
                    info.size = 0;
                    info.lastModified = entry.last_write_time();
                }
                
                // Apply modification time filters
                if (filter.modifiedAfter.has_value() && info.lastModified < filter.modifiedAfter.value()) {
                    continue;
                }
                
                if (filter.modifiedBefore.has_value() && info.lastModified > filter.modifiedBefore.value()) {
                    continue;
                }
                
                // Apply name pattern filter
                if (filter.namePattern.has_value()) {
                    if (!std::regex_match(info.name, nameRegex.value())) {
                        continue;
                    }
                }
                
                // Apply regex pattern filter
                if (filter.regexPattern.has_value() && 
                    !std::regex_match(info.name, filter.regexPattern.value())) {
                    continue;
                }
                    
                    // Apply directory/file filtering
                    if ((isDir && !filter.includeDirectories) || (!isDir && !filter.includeFiles)) {
                        continue;
                    }
                    
                    // Skip hidden files if not included
                    if (!filter.includeHidden) {
                        auto filename = entry.path().filename().string();
                        if (!filename.empty() && filename[0] == '.') {
                            continue;
                        }
                    }
                    
                    // Get file information
                    FileInfo _info;
                    _info.path = entry.path().string();
                    _info.name = entry.path().filename().string();
                    _info.isDirectory = isDir;
                    
                    if (!isDir) {
                        _info.extension = entry.path().extension().string();
                        _info.size = entry.file_size();
                        _info.lastModified = entry.last_write_time();
                        
                        // Apply extension filter
                        if (filter.extension.has_value() && _info.extension != filter.extension.value()) {
                            continue;
                        }
                        
                        // Apply size filters
                        if (filter.minSize.has_value() && _info.size < filter.minSize.value()) {
                            continue;
                        }
                        
                        if (filter.maxSize.has_value() && _info.size > filter.maxSize.value()) {
                            continue;
                        }
                    } else {
                        _info.extension = "";
                        _info.size = 0;
                        _info.lastModified = entry.last_write_time();
                    }
                    
                    // Apply modification time filters
                    if (filter.modifiedAfter.has_value() && info.lastModified < filter.modifiedAfter.value()) {
                        continue;
                    }
                    
                    if (filter.modifiedBefore.has_value() && info.lastModified > filter.modifiedBefore.value()) {
                        continue;
                    }
                    
                    // Apply name pattern filter
                    if (filter.namePattern.has_value()) {
                        if (!std::regex_match(info.name, nameRegex.value())) {
                            continue;
                        }
                    }
                    
                    // Apply regex pattern filter
                    if (filter.regexPattern.has_value() && 
                        !std::regex_match(info.name, filter.regexPattern.value())) {
                        continue;
                    }
                    
                    // If we got here, the entry passed all filters
                    result.push_back(info);
                }
            }
            
            NOVA_LOG(("Listed " + std::to_string(result.size()) + " files with details in " + path).c_str(), LogType::Log);
            return result;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error listing files with details in directory: " + path + " - " + e.what()).c_str(), LogType::Error);
            return result;
        }
    }

    std::vector<std::string> NovaFileOperations::FindFiles(
        const std::string& path, 
        const std::string& pattern, 
        bool recursive) 
    {
        std::vector<std::string> result;
        try {
            if (!DirectoryExists(path)) {
                NOVA_LOG(("Directory does not exist for finding files: " + path).c_str(), LogType::Warning);
                return result;
            }
            
            auto regex = WildcardToRegex(pattern);
            
            if (recursive) {
                std::filesystem::recursive_directory_iterator recIterator(path);
                for (const auto& entry : recIterator) {
                    if (!entry.is_directory()) {
                        std::string filename = entry.path().filename().string();
                        if (std::regex_match(filename, regex)) {
                            result.push_back(entry.path().string());
                        }
                    }
                }
            } else {
                std::filesystem::directory_iterator dirIterator(path);
                for (const auto& entry : dirIterator) {
                    if (!entry.is_directory()) {
                        std::string filename = entry.path().filename().string();
                        if (std::regex_match(filename, regex)) {
                            result.push_back(entry.path().string());
                        }
                    }
                }
            }
            
            NOVA_LOG(("Found " + std::to_string(result.size()) + " files matching pattern '" + 
                    pattern + "' in " + path).c_str(), LogType::Log);
            return result;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error finding files in directory: " + path + " - " + e.what()).c_str(), LogType::Error);
            return result;
        }
    }

    std::optional<FileInfo> NovaFileOperations::GetFileInfo(const std::string& path) 
    {
        try {
            std::filesystem::path fsPath(path);
            if (!std::filesystem::exists(fsPath)) {
                NOVA_LOG(("File or directory does not exist: " + path).c_str(), LogType::Warning);
                return std::nullopt;
            }
            
            FileInfo info;
            info.path = fsPath.string();
            info.name = fsPath.filename().string();
            info.isDirectory = std::filesystem::is_directory(fsPath);
            
            if (!info.isDirectory) {
                info.extension = fsPath.extension().string();
                info.size = std::filesystem::file_size(fsPath);
            } else {
                info.extension = "";
                info.size = 0;
            }
            
            info.lastModified = std::filesystem::last_write_time(fsPath);
            
            return info;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error getting file info: " + path + " - " + e.what()).c_str(), LogType::Error);
            return std::nullopt;
        }
    }

    bool NovaFileOperations::MatchesPattern(const std::string& path, const std::string& pattern) 
    {
        try {
            std::filesystem::path fsPath(path);
            std::string filename = fsPath.filename().string();
            auto regex = WildcardToRegex(pattern);
            return std::regex_match(filename, regex);
        } catch (const std::exception& e) {
            NOVA_LOG(("Error matching pattern: " + path + " / " + pattern + " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    std::regex NovaFileOperations::WildcardToRegex(const std::string& pattern) 
    {
        // Convert wildcard pattern to regex
        // * matches any sequence of characters
        // ? matches any single character
        std::string regexPattern = "";
        for (char c : pattern) {
            switch (c) {
                case '*':
                    regexPattern += ".*";
                    break;
                case '?':
                    regexPattern += ".";
                    break;
                case '.':
                case '+':
                case '^':
                case '$':
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case '|':
                case '\\':
                    // Escape special regex characters
                    regexPattern += "\\" + std::string(1, c);
                    break;
                default:
                    regexPattern += c;
            }
        }
        return std::regex(regexPattern);
    }

    // Directory Watching Functions
    int NovaFileOperations::WatchDirectory(
        const std::string& path,
        std::function<void(const std::string&, const std::string&)> callback,
        unsigned int intervalMs,
        bool recursive) 
    {
        try {
            if (!DirectoryExists(path)) {
                NOVA_LOG(("Cannot watch non-existent directory: " + path).c_str(), LogType::Error);
                return -1;
            }
            
            std::lock_guard<std::mutex> lock(watchersMutex);
            
            int watcherId = nextWatcherId++;
            
            DirectoryWatcher watcher;
            watcher.path = path;
            watcher.recursive = recursive;
            watcher.intervalMs = intervalMs;
            watcher.callback = callback;
            watcher.active = true;
            
            // Initialize the file map with current state
            if (recursive) {
                std::filesystem::recursive_directory_iterator recIterator(path);
                for (const auto& entry : recIterator) {
                    watcher.lastModified[entry.path().string()] = entry.last_write_time();
                }
            } else {
                std::filesystem::directory_iterator dirIterator(path);
                for (const auto& entry : dirIterator) {
                    watcher.lastModified[entry.path().string()] = entry.last_write_time();
                }
            }
            
            // Start the watcher thread
            watcher.watchThread = std::thread(WatcherThreadFunc, watcherId);
            watcher.watchThread.detach();
            
            watchers[watcherId] = std::move(watcher);
            
            NOVA_LOG(("Started watching directory: " + path).c_str(), LogType::Log);
            return watcherId;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error setting up directory watch: " + path + " - " + e.what()).c_str(), LogType::Error);
            return -1;
        }
    }

    bool NovaFileOperations::StopWatching(int watchId) 
    {
        try {
            std::lock_guard<std::mutex> lock(watchersMutex);
            
            if (watchers.find(watchId) == watchers.end()) {
                NOVA_LOG(("No watcher found with ID: " + std::to_string(watchId)).c_str(), LogType::Warning);
                return false;
            }
            
            // Signal the thread to stop
            watchers[watchId].active = false;
            
            // Remove from map
            std::string path = watchers[watchId].path;
            watchers.erase(watchId);
            
            NOVA_LOG(("Stopped watching directory: " + path).c_str(), LogType::Log);
            return true;
        } catch (const std::exception& e) {
            NOVA_LOG(("Error stopping directory watch: " + std::to_string(watchId) + 
                     " - " + e.what()).c_str(), LogType::Error);
            return false;
        }
    }

    void NovaFileOperations::WatcherThreadFunc(int watcherId) 
    {
        try {
            DirectoryWatcher* watcher = nullptr;
            
            {
                std::lock_guard<std::mutex> lock(watchersMutex);
                if (watchers.find(watcherId) == watchers.end()) {
                    return;
                }
                watcher = &watchers[watcherId];
            }
            
            while (true) {
                // Check if we should stop
                {
                    std::lock_guard<std::mutex> lock(watchersMutex);
                    if (!watcher->active) {
                        break;
                    }
                }
                
                // Scan for changes
                try {
                    // Current files
                    std::map<std::string, std::filesystem::file_time_type> currentFiles;
                    
                    if (watcher->recursive) {
                        std::filesystem::recursive_directory_iterator recIterator(watcher->path);
                        for (const auto& entry : recIterator) {
                            currentFiles[entry.path().string()] = entry.last_write_time();
                        }
                    } else {
                        std::filesystem::directory_iterator dirIterator(watcher->path);
                        for (const auto& entry : dirIterator) {
                            currentFiles[entry.path().string()] = entry.last_write_time();
                        }
                    }
                    
                    // Compare with previous scan
                    for (const auto& [path, time] : currentFiles) {
                        // Check if file is new or modified
                        auto it = watcher->lastModified.find(path);
                        if (it == watcher->lastModified.end()) {
                            // New file
                            watcher->callback(path, "created");
                        } else if (time != it->second) {
                            // Modified file
                            watcher->callback(path, "modified");
                        }
                    }
                    
                    // Check for deleted files
                    for (const auto& [path, time] : watcher->lastModified) {
                        if (currentFiles.find(path) == currentFiles.end()) {
                            // Deleted file
                            watcher->callback(path, "deleted");
                        }
                    }
                    
                    // Update the file map
                    watcher->lastModified = currentFiles;
                } catch (const std::exception& e) {
                    NOVA_LOG(("Error in directory watcher: " + std::string(e.what())).c_str(), LogType::Error);
                }
                
                // Sleep for the specified interval
                std::this_thread::sleep_for(std::chrono::milliseconds(watcher->intervalMs));
            }
        } catch (const std::exception& e) {
            NOVA_LOG(("Watcher thread crashed: " + std::string(e.what())).c_str(), LogType::Error);
        }
    }

    // Path manipulation utilities
    std::string NovaFileOperations::GetParentDirectory(const std::string& path) 
    {
        try {
            std::filesystem::path fsPath(path);
            return fsPath.parent_path().string();
        } catch (const std::exception& e) {
            NOVA_LOG(("Error getting parent directory: " + path + " - " + e.what()).c_str(), LogType::Error);
            return path;
        }
    }

    std::string NovaFileOperations::GetFileName(const std::string& path, bool includeExtension) 
    {
        try {
            std::filesystem::path fsPath(path);
            if (includeExtension) {
                return fsPath.filename().string();
            } else {
                return fsPath.stem().string();
            }
        } catch (const std::exception& e) {
            NOVA_LOG(("Error getting filename: " + path + " - " + e.what()).c_str(), LogType::Error);
            return path;
        }
    }

    std::string NovaFileOperations::GetFileExtension(const std::string& path) 
    {
        try {
            std::filesystem::path fsPath(path);
            return fsPath.extension().string();
        } catch (const std::exception& e) {
            NOVA_LOG(("Error getting file extension: " + path + " - " + e.what()).c_str(), LogType::Error);
            return "";
        }
    }

    std::string NovaFileOperations::JoinPaths(const std::string& basePath, const std::string& relativePath) 
    {
        try {
            std::filesystem::path base(basePath);
            return (base / relativePath).string();
        } catch (const std::exception& e) {
            NOVA_LOG(("Error joining paths: " + basePath + " + " + relativePath + " - " + e.what()).c_str(), LogType::Error);
            return basePath + "/" + relativePath;
        }
    }

    std::string NovaFileOperations::GetTempFilePath(const std::string& prefix, const std::string& extension) 
    {
        try {
            auto tempDir = std::filesystem::temp_directory_path();
            
            // Generate a unique filename using time-based randomness
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
            auto value = now_ms.time_since_epoch().count();
            
            // Add some additional randomness from the current process ID and a counter
            static unsigned int counter = 0;
            counter++;
            
            std::string uniqueName = prefix + 
                                    std::to_string(value) + 
                                    "_" + 
                                    std::to_string(counter) + 
                                    extension;
            std::filesystem::path tempFile = tempDir / uniqueName;
            
            // Make sure it doesn't already exist
            while (std::filesystem::exists(tempFile)) {
                // Increment counter for uniqueness
                counter++;
                uniqueName = prefix + 
                           std::to_string(value) + 
                           "_" + 
                           std::to_string(counter) + 
                           extension;
                tempFile = tempDir / uniqueName;
            }
            
            return tempFile.string();
        } catch (const std::exception& e) {
            NOVA_LOG(("Error creating temp file path: " + std::string(e.what())).c_str(), LogType::Error);
            return "";
        }
    }

    std::string NovaFileOperations::GetRelativePath(const std::string& from, const std::string& to) 
    {
        try {
            std::filesystem::path fromPath(from);
            std::filesystem::path toPath(to);
            
            // Make sure both are absolute paths
            if (!fromPath.is_absolute()) {
                fromPath = std::filesystem::absolute(fromPath);
            }
            
            if (!toPath.is_absolute()) {
                toPath = std::filesystem::absolute(toPath);
            }
            
            return std::filesystem::relative(toPath, fromPath).string();
        } catch (const std::exception& e) {
            NOVA_LOG(("Error getting relative path: " + from + " to " + to + " - " + e.what()).c_str(), LogType::Error);
            return to;
        }
    }
}