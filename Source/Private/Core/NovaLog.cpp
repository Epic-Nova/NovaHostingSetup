#include "NovaLog.h"

namespace Core
{
	void NovaLog::Log(const char* Message)
	{
		std::cout << Message << std::endl;
	}

	void NovaLog::Debug(const std::string& message)
	{
		std::cout << "DEBUG: " << message << std::endl;
	}

	void NovaLog::Warning(const std::string& message)
	{
		std::cout << "WARNING: " << message << std::endl;
	}

	void NovaLog::Error(const std::string& message)
	{
		std::cout << "ERROR: " << message << std::endl;
	}

	void NovaLog::FatalError(const std::string& message)
	{
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
	}
}