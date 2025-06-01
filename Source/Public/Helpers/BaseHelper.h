#pragma once

#include "NovaMinimal.h"

/**
 * Base class for helper utilities.
 */
namespace Core::Helpers
{
    class BaseHelper
    {
        public:
            BaseHelper() = default;

            virtual BaseHelper* GetHelperInstance() = 0;
            virtual void Initialize() = 0;
            virtual void Shutdown() = 0;

            bool IsInitialized() { return bIsInitialized; }
            std::string GetName() { return Name; }
            std::string GetDescription() { return Description; }

            virtual void Execute(std::function<bool()> callback) = 0;
            virtual void Reset() = 0;
            virtual void Abort() = 0;
            virtual bool HasMetRequirements() const { return true; } // Default implementation, can be overridden
            bool IsRunning() { return bIsRunning; }
            bool IsAborted() { return bIsAborted; } 

            // Add callbacks to the delegate
            void AddInstallCallback(std::function<void(std::string)> callback) {
                InstallCallbacks.push_back(callback);
            }

            // Set a single callback function (replaces all existing callbacks)
            void SetInstallCallbackFunction(std::function<void(const std::string&)> callback) {
                InstallCallbacks.clear();
                if (callback) {
                    InstallCallbacks.push_back(callback);
                }
            }

            // Fire all callbacks
            void FireInstallCallback(const std::string& message) {
                for (auto& callback : InstallCallbacks) {
                    if (callback) {
                        callback(message);
                    }
                }
            }

            // Execute command with progress tracking and blocking
            virtual bool ExecuteCommandBlocking(const std::string& command) {
                return RunCommandWithProgress(command);
            }

            // Wait for completion flag
            void WaitForCompletion() {
                while (bIsRunning) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }

            // This callback is used by the installation process itself
            std::vector<std::function<void(std::string)>> InstallCallbacks;

        protected:
            // Common command execution with progress
            virtual bool RunCommandWithProgress(const std::string& command) {
                FireInstallCallback("Executing: " + command);
                
                // Add output redirection for non-verbose mode
                std::string fullCommand = command;
                #ifndef VERBOSE_MODE
                fullCommand += " > /dev/null 2>&1";
                #endif
                
                FILE* pipe = popen(fullCommand.c_str(), "r");
                if (!pipe) {
                    FireInstallCallback("Failed to execute command");
                    return false;
                }

                char buffer[128];
                std::string result = "";
                int progressCounter = 0;
                
                while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    result += buffer;
                    if (++progressCounter % 20 == 0) {
                        FireInstallCallback("Command running... (" + std::to_string(progressCounter/20) + "s)");
                    }
                }

                int returnCode = pclose(pipe);
                
                if (returnCode == 0) {
                    FireInstallCallback("Command completed successfully");
                    return true;
                } else {
                    FireInstallCallback("Command failed with return code: " + std::to_string(returnCode));
                    return false;
                }
            }

            BaseHelper(std::string NewName, std::string NewDescription)
                : Name(std::move(NewName)), Description(std::move(NewDescription)) {};
            virtual ~BaseHelper() = default;

        private:

            std::string Name;
            std::string Description;

        protected:
            bool bIsInitialized = false;
            bool bIsRunning = false;
            bool bIsAborted = false;

            
    };  
}