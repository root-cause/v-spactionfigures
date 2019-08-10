#pragma once
#include "fmt/format.h"
#include "fmt/time.h"
#include "Header.h"

enum class LogLevel
{
	LOG_NONE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_ERROR
};

static const std::string _logLevelPrefixes[4] = { "", "DEBUG", "INFO", "ERROR" };

class Logger
{
private:
	fs::path m_logFilePath;
	LogLevel m_logLevel = LogLevel::LOG_NONE;

public:
	Logger(const std::string& logFileName, LogLevel logLevel);
	void SetFileName(const std::string& newLogFileName);
	void SetLogLevel(LogLevel newLogLevel);
	void Write(LogLevel logLevel, const std::string& text) const;
	void Clear() const;
};

extern Logger ScriptLog;