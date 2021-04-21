#pragma once

//#ifndef _USE_CONSOLE 
//#define _USE_EASY_LOGGIN_

#ifdef _MX_USE_CONSOLE_
#include <iostream>
//使用控制台
#define LOG_UTILS(log_level, log_stream)   std::cout(log_stream)

#elif defined(_MX_USE_EASY_LOGGING_)
#include "easylogging++.h"

//使用文件
#define LOG_UTILS(log_level, log_stream)   LOG(log_level) << log_stream


#else

#define LOG_UTILS(log_level, log_stream)


#endif