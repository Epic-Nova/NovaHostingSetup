/// @file NovaLog.h
/// @brief Declares logging behavior.

#pragma once

#include "NovaCore.h"
#include "NovaMinimal.h"

#include <string>
#include <vector>
#include <mutex>
#include "ftxui/component/component.hpp" // Include the ftxui component header

#define NOVA_LOG(message, type) Core::NovaLog::Type(message, type)
#define NOVA_LOG_VERBOSE(message, type) Core::NovaLog::TypeVerbose(message, type)

namespace Core {
	enum class LogType
	{
		Log,
		Debug,
		Warning,
		Error,
		FatalError,
		Assert
	};

	class NovaLog
	{
	public:
		static void Type(const char* Message, LogType Type);
		static void TypeVerbose(const char* Message, LogType Type);
		static void SetVerbose(bool verbose);
		static void StartLogFile();
		static void* CreateScrollableLog();
		static void* RenderLog();
		
	//private:
		static void Log(const char* Message);
		static void Debug(const std::string& message);
		static void Warning(const std::string& message);
		static void Error(const std::string& message);
		static void FatalError(const std::string& message);
		static void Assert(bool condition, const std::string& message);
		static void AddToGuiLog(const std::string& message);
		static void WriteLogToFile(const std::string& message);
		static void CreateRequiredDirectories(); // New function to create directories
		
		static bool verboseEnabled;
	};
}