#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "Logger.h"
#include "Constants.h"

Logger::Logger(const std::string& logFileName, LogLevel logLevel)
{
	m_logFilePath = fs::current_path().append(logFileName);
	m_logLevel = logLevel;
}

void Logger::SetFileName(const std::string& newLogFileName)
{
	m_logFilePath = fs::current_path().append(newLogFileName);
}

void Logger::SetLogLevel(LogLevel newLogLevel)
{
	m_logLevel = newLogLevel;
}

void Logger::Write(LogLevel logLevel, const std::string& text) const
{
	if (m_logLevel == LogLevel::LOG_NONE || m_logLevel > logLevel)
	{
		return;
	}

	std::ofstream logFile(m_logFilePath, std::ofstream::out | std::ofstream::app);
	if (logFile)
	{
		std::time_t t = std::time(nullptr);
		logFile << fmt::format("[{:%H:%M:%S}] [{}] {}", *std::localtime(&t), _logLevelPrefixes[ static_cast<int>(logLevel) ], text) << "\n";
	}
}

void Logger::Clear() const
{
	std::ofstream logFile(m_logFilePath, std::ofstream::out | std::ofstream::trunc);
}

Logger ScriptLog(Constants::ModName + ".log", LogLevel::LOG_INFO);