#include "LoggerImpl.h"


RC log::Logger::log(RC code, Level level, const char* const& srcfile, const char* const& function, int line)
{
    std::string l_msg, e_msg;
    std::tie(l_msg, e_msg) = findMsg(code, level);
    
    if (filestream){
    	filestream << l_msg << ' ' << e_msg << ' ' << srcfile << ' ' << function << ' ' << line;
    }

    return RC::SUCCESS;
}

RC log::Logger::log(RC code, Level level)
{
     std::string l_msg, e_msg;
     
     std::tie(l_msg, e_msg) = findMsg(code, level);
    	if (filestream){
    		filestream << l_msg << ' ' << e_msg;
    	}
    	
    return RC::SUCCESS;
}



