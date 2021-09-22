#pragma once
#include "ILogger.h"


#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>

/*
 * Наследуемся от интерфейса логгера
 * 
 * 
 * 
 * 
 * */
namespace log
{
    class Logger : public ILogger
	{
	public:
    	Logger() = default;

    	Logger(const char * const& filename, bool overwrite = false){
    		overwrite ? filestream.open(filename, std::ofstream::trunc) : filestream.open(filename);
    	}

    	RC log(RC code, Level level, const char* const& srcfile, const char* const& function, int line) override;
    	RC log(RC code, Level level) override;

    	 ~Logger(){
    		if (filestream)
    			filestream.close();
    	 }

	private:
    	 std::pair<std::string, std::string> findMsg(RC code, Level level){
    		 return {EMSG.count(code) ? EMSG.at(code) : "ERROR_CODE", LMSG.count(level) ? LMSG.at(level) : "ERROR_LEVEL"};
    	 }

    	const std::unordered_map<ILogger::Level, std::string>              LMSG = {
        		{ILogger::Level::INFO,    "INFO"},
        		{ILogger::Level::SEVER,   "SEVER"},
        		{ILogger::Level::WARNING, "WARNING"}
        };

    	const std::unordered_map <RC, std::string> EMSG = {
        		{RC::SUCCESS,                "SUCCESS"},
        		{RC::MISMATCHING_DIMENSIONS, "MISMATCHING_DIMENSIONS"},
        		{RC::ALLOCATION_ERROR,       "ALLOCATION_ERROR"},
        		{RC::INFINITY_OVERFLOW,      "INFINITY_OVERFLOW"},
        		{RC::INDEX_OUT_OF_BOUND,     "INDEX_OUT_OF_BOUND"},
        		{RC::NULLPTR_ERROR,          "NULLPTR_ERROR"},
        		{RC::VECTOR_NOT_FOUND,       "VECTOR_NOT_FOUND"},
        		{RC::FILE_NOT_FOUND,         "FILE_NOT_FOUND"},
        		{RC::INVALID_ARGUMENT,       "INVALID_ARGUMENT"},
        		{RC::NOT_NUMBER,             "NOT_NUMBER"},
        		{RC::UNKNOWN,                "UNKNOWN"},
        		{RC::MEMORY_INTERSECTION,    "MEMORY_INTERSECTION"},
        		{RC::AMOUNT,                 "AMOUNT"},
        		{RC::IO_ERROR,               "IO_ERROR"}
    	};

    	std::ofstream filestream;
	};
}
