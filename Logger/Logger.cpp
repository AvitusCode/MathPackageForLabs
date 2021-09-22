#include "LoggerImpl.h"


ILogger* ILogger::createLogger()
{
	return new log::Logger();
}

ILogger* ILogger::createLogger(const char* const& filename, bool overwrite)
{
	return new log::Logger(filename, overwrite);
}

ILogger::~ILogger() noexcept = default;


