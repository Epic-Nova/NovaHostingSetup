#include "NovaLog.h"

namespace Core
{
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
				Assert(false, Message);
				break;
			}
		}
	}

	void NovaLog::Log(const char* Message)
	{
		WriteLogToFile(Message);
		std::cout << Message << std::endl;
	}

	void NovaLog::Debug(const std::string& message)
	{
		WriteLogToFile(message);
		std::cout << "DEBUG: " << message << std::endl;
	}

	void NovaLog::Warning(const std::string& message)
	{
		WriteLogToFile(message);
		std::cout << "WARNING: " << message << std::endl;
	}

	void NovaLog::Error(const std::string& message)
	{
		WriteLogToFile(message);
		std::cout << "ERROR: " << message << std::endl;
	}

	void NovaLog::FatalError(const std::string& message)
	{
		WriteLogToFile(message);
		std::cout << "FATAL ERROR: " << message << std::endl;
		std::cin.get();
		exit(-1);
	}

	void NovaLog::Assert(bool condition, const std::string& message)
	{
		if (!condition)
		{
			std::cout << "ASSERTION FAILED: " << message << std::endl;
			std::cin.get();
			exit(-1);
		}
		WriteLogToFile(message);
	}
}