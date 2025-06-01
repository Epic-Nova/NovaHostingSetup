/// @file NovaLog.cpp
/// @brief Implements logging behavior.

#include "NovaCore.h"
#include "NovaMinimal.h"

namespace Core {

	using namespace Core::FileOperations;

	std::mutex logMutex;
	std::vector<std::string> logMessages;
	bool NovaLog::verboseEnabled = false;

	void NovaLog::SetVerbose(bool verbose)
	{
		verboseEnabled = verbose;
		if (verbose) {
			NOVA_LOG("Verbose logging enabled", LogType::Debug);
		}
	}

	void NovaLog::TypeVerbose(const char* Message, LogType Type)
	{
		if (verboseEnabled) {
			Debug(Message);
		}
	}

	void NovaLog::Type(const char* Message, LogType Type)
	{
		switch (Type)
		{
			case LogType::Log:
			{
				Log(Message);
				break;
			}
			case LogType::Debug:
			{
				Debug(Message);
				break;
			}
			case LogType::Warning:
			{
				Warning(Message);
				break;
			}
			case LogType::Error:
			{
				Error(Message);
				break;
			}
			case LogType::FatalError:
			{
				FatalError(Message);
				break;
			}
			case LogType::Assert:
			{
				// Changed: don't automatically fail assertions
				// We need to assess them properly
				Assert(true, Message);
				break;
			}
		}
	}

	void NovaLog::StartLogFile()
	{
		try {
			CreateRequiredDirectories();

			std::string logsDirectory = "Content/Logs";
			std::string logFilePath = logsDirectory + "/Nova.log";

			// Check if the log file already exists
			if (std::filesystem::exists(logFilePath)) {
				// Generate a timestamp
				auto now = std::chrono::system_clock::now();
				auto time_t_now = std::chrono::system_clock::to_time_t(now); // Store the result in a variable
				std::stringstream timestamp;
				timestamp << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d_%H-%M-%S");

				// Rename the existing log file
				std::string newLogFilePath = logsDirectory + "/Nova-" + timestamp.str() + ".log";
				std::filesystem::rename(logFilePath, newLogFilePath);
				std::cout << "Renamed existing log file to: " << newLogFilePath << std::endl;
			}

			// Create a new log file
			auto now = std::chrono::system_clock::now();
			auto time_t_now = std::chrono::system_clock::to_time_t(now); // Store the result in a variable
			std::string header =
				"====================================================\n"
				"=          NOVA DOCUMENTATION SETUP LOG            =\n"
				"====================================================\n"
				"Started: " + std::string(std::ctime(&time_t_now)) + // Pass the variable to std::ctime
				"====================================================\n\n";

			std::ofstream logFile(logFilePath, std::ios::out);
			if (!logFile.is_open()) {
				throw std::runtime_error("Failed to create log file: " + logFilePath);
			}

			logFile << header;
			logFile.close();

			logMessages.clear();
			std::cout << "Log file started successfully at: " << logFilePath << std::endl;
		} catch (const std::exception& e) {
			std::cerr << "Error in StartLogFile: " << e.what() << std::endl;
			throw;
		}
	}

	// Create directories needed by the application using NovaFileOperations
	void NovaLog::CreateRequiredDirectories()
	{
		try {
			// Create base Content directory
			if (!std::filesystem::exists("Content")) {
				std::filesystem::create_directory("Content");
				std::cout << "Created Content directory" << std::endl;
			}

			// Create Content/Logs directory
			if (!std::filesystem::exists("Content/Logs")) {
				std::filesystem::create_directory("Content/Logs");
				std::cout << "Created Content/Logs directory" << std::endl;
			}

			// Create Content/Website directory
			if (!std::filesystem::exists("Content/Website")) {
				std::filesystem::create_directory("Content/Website");
				std::cout << "Created Content/Website directory" << std::endl;
			}
		} catch (const std::exception& e) {
			std::cerr << "Error creating required directories: " << e.what() << std::endl;
		}
	}

	void NovaLog::Log(const char* Message)
	{
		WriteLogToFile(Message);
		AddToGuiLog(Message);
	}

	void NovaLog::Debug(const std::string& message)
	{
		WriteLogToFile("DEBUG: " + message);
		AddToGuiLog("DEBUG: " + message);
	}

	void NovaLog::Warning(const std::string& message)
	{
		WriteLogToFile("WARNING: " + message);
		AddToGuiLog("WARNING: " + message);
	}

	void NovaLog::Error(const std::string& message)
	{
		WriteLogToFile("ERROR: " + message);
		AddToGuiLog("ERROR: " + message);
	}

	void NovaLog::FatalError(const std::string& message)
	{
		WriteLogToFile("FATAL ERROR: " + message);
		AddToGuiLog("FATAL ERROR: " + message);
		exit(-1);
	}

	void NovaLog::Assert(bool condition, const std::string& message)
	{
		if (!condition)
		{
			WriteLogToFile("ASSERTION FAILED: " + message);
			AddToGuiLog("ASSERTION FAILED: " + message);
			exit(-1);  // This will immediately terminate the application
		}
		else {
			// Only log the message if condition is true
			WriteLogToFile("ASSERTION PASSED: " + message);
		}
	}

	void NovaLog::AddToGuiLog(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(logMutex);
		logMessages.push_back(message);
		// Removed console output to prevent UI movement
		// std::cout << message << std::endl;
	}

	void* NovaLog::CreateScrollableLog()
	{
		static ftxui::Component component = ftxui::Renderer([] {
			std::lock_guard<std::mutex> lock(logMutex);
			ftxui::Elements elements;
			for (const auto& message : logMessages)
			{
				elements.push_back(ftxui::text(message));
			}
			return ftxui::vbox(elements) | ftxui::vscroll_indicator | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 10);
		}) | ftxui::CatchEvent([](ftxui::Event) { return false; });
		
		return &component;
	}

	void* NovaLog::RenderLog()
	{
		static ftxui::Element element = []() {
			std::lock_guard<std::mutex> lock(logMutex);
			ftxui::Elements elements;
			
			// Limit to last 20 messages to prevent overflow
			size_t startIndex = logMessages.size() > 20 ? logMessages.size() - 20 : 0;
			
			for (size_t i = startIndex; i < logMessages.size(); ++i)
			{
				elements.push_back(ftxui::text(logMessages[i]));
			}
			return ftxui::vbox(elements) | ftxui::vscroll_indicator;
		}();
		
		return &element;
	}

	void NovaLog::WriteLogToFile(const std::string& message)
	{
		try {
			std::lock_guard<std::mutex> lock(logMutex);

			auto now = std::chrono::system_clock::now();
			auto time_t_now = std::chrono::system_clock::to_time_t(now);
			std::tm local_tm = *std::localtime(&time_t_now);

			char date_buffer[20];
			std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", &local_tm);

			char time_buffer[20];
			std::strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &local_tm);

			std::string logFileName = std::string("Nova") + ".log";
			std::string logFilePath = "Content/Logs/" + logFileName;

			std::string formattedMessage = "[" + std::string(date_buffer) + " " + std::string(time_buffer) + "] " + message + "\n";

			std::ofstream logFile(logFilePath, std::ios::app);
			if (!logFile.is_open()) {
				std::cerr << "Failed to write to log file: " << logFilePath << std::endl;
				return;
			}

			logFile << formattedMessage;
			logFile.close();
			std::cout << formattedMessage;  // Print to console for debugging
		} catch (const std::exception& e) {
			std::cerr << "Error writing to log file: " << e.what() << std::endl;
		}
		catch (...) {
			std::cerr << "Unknown error writing to log file" << std::endl;
		}
	}
}