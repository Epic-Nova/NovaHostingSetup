#pragma once

#include "NovaMinimal.h"

namespace Core
{
    enum class LogType
    {
        Log,
        Debug,
        Warning,
        Error,
        FatalError,
        Assert
    };


    #define NOVA_LOG(LogMessage, LogType) NovaLog::Type(LogMessage, LogType)
    
    class NovaLog
    {
        public:

            static void Type(const char* Message, LogType Type);

            static void StartLogFile()
            {
                //if a log file is present, rename it to the current date and then create a new log file
                std::string logFileName = "nova.log";
                std::filesystem::path savedDir = std::filesystem::current_path() / "Saved";
                std::filesystem::path logFilePath = savedDir / logFileName;
                
                // Create the Saved directory if it doesn't exist
                if (!std::filesystem::exists(savedDir)) {
                    std::filesystem::create_directories(savedDir);
                }
                
                if (std::filesystem::exists(logFilePath)) {
                    auto now = std::chrono::system_clock::now();
                    auto time_t = std::chrono::system_clock::to_time_t(now);
                    auto tm = *std::localtime(&time_t);
                    
                    std::string newLogFileName = "nova_" + std::to_string(tm.tm_year + 1900) + "-" +
                                                std::to_string(tm.tm_mon + 1) + "-" +
                                                std::to_string(tm.tm_mday) + ".log";
                    
                    std::filesystem::rename(logFilePath, savedDir / newLogFileName);
                }
            }

        private:

            static void Log(const char* Message);
            static void Debug(const std::string& message);
            static void Warning(const std::string& message);
            static void Error(const std::string& message);
            static void FatalError(const std::string& message);
            static void Assert(bool condition, const std::string& message);

            static void WriteLogToFile(const std::string& message)
            {
                //Check if a log file at the pwd exists, if not create it at current working directory, open it and write the message with the date in it
                std::string logFileName = "nova.log";
                std::filesystem::path savedDir = std::filesystem::current_path() / "Saved";
                std::filesystem::path logFilePath = savedDir / logFileName;
                
                // Create the Saved directory if it doesn't exist
                if (!std::filesystem::exists(savedDir)) {
                    std::filesystem::create_directories(savedDir);
                }
                
                std::ofstream logFile;
                
                // Check if file exists, if not create it
                if (!std::filesystem::exists(logFilePath)) {
                    logFile.open(logFilePath, std::ios::out);
                } else {
                    logFile.open(logFilePath, std::ios::app); // Append to existing file
                }
                
                if (logFile.is_open()) {
                    // Get current time
                    auto now = std::chrono::system_clock::now();
                    auto time_t = std::chrono::system_clock::to_time_t(now);
                    auto tm = *std::localtime(&time_t);
                    
                    // Format: [YYYY-MM-DD HH:MM:SS] message
                    logFile << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;
                    logFile.close();
                }
            }
    };
}