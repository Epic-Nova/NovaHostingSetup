#pragma once

#include "NovaMinimal.h"
#include <filesystem>
#include <vector>
#include <string>

namespace Core::FileOperations 
{
    /**
     * Utility class for Nova file operations
     */
    class NovaFileOperations 
    {
    public:
        /**
         * Creates a directory if it doesn't exist
         * @param path The path to create
         * @return True if operation was successful or directory already exists
         */
        static bool CreateDirectory(const std::string& path);
        
        /**
         * Creates a directory structure with multiple nested levels
         * @param path The full path to create
         * @return True if operation was successful
         */
        static bool CreateDirectoryStructure(const std::string& path);
        
        /**
         * Delete a file
         * @param path Path to the file
         * @return True if operation was successful
         */
        static bool DeleteFile(const std::string& path);
        
        /**
         * Delete a directory and all its contents
         * @param path Path to the directory
         * @return True if operation was successful
         */
        static bool DeleteDirectory(const std::string& path);
        
        /**
         * Check if a file exists
         * @param path Path to the file
         * @return True if file exists
         */
        static bool FileExists(const std::string& path);
        
        /**
         * Check if a directory exists
         * @param path Path to the directory
         * @return True if directory exists
         */
        static bool DirectoryExists(const std::string& path);
        
        /**
         * Copy a file from source to destination
         * @param source Source file path
         * @param destination Destination file path
         * @return True if operation was successful
         */
        static bool CopyFile(const std::string& source, const std::string& destination);
        
        /**
         * Move a file from source to destination
         * @param source Source file path
         * @param destination Destination file path
         * @return True if operation was successful
         */
        static bool MoveFile(const std::string& source, const std::string& destination);
        
        /**
         * Read the contents of a text file
         * @param path Path to the file
         * @return File contents as string, empty if failed
         */
        static std::string ReadTextFile(const std::string& path);
        
        /**
         * Write content to a text file
         * @param path Path to the file
         * @param content Content to write
         * @param append Whether to append or overwrite
         * @return True if operation was successful
         */
        static bool WriteTextFile(const std::string& path, const std::string& content, bool append = false);
        
        /**
         * List all files in a directory
         * @param path Directory path
         * @param recursive Whether to include subdirectories
         * @return Vector of file paths
         */
        static std::vector<std::string> ListFiles(const std::string& path, bool recursive = false);
        
        /**
         * Normalizes a path, converting relative paths to absolute using current working directory
         * @param path Path to normalize
         * @return Normalized (absolute) path
         */
        static std::string NormalizePath(const std::string& path);
    };
}
