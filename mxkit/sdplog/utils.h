#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging

#include "base/time_utils.h"

_BEGIN_MX_KIT_NAME_SPACE_

extern std::shared_ptr<spdlog::logger> static_spdlog;

class MXSpdlog
{
public:
    static std::shared_ptr<spdlog::logger> CreateLogger(const std::string& logPath, const std::string& name)
    {
        if (name.empty())
        {
            return nullptr;
        }

        std::string filePath = logPath + "\\";
        std::string timeFormat = name + " %H-%M-%S.log";
        std::string fileName = mxkit::TimeUtils::ToString<std::string>(timeFormat.c_str());
        filePath += fileName;

        return static_spdlog = spdlog::rotating_logger_mt<spdlog::async_factory>(name.c_str(), filePath.c_str(), 1024 * 1024 * 10, 10);
    }

    static void ReleaseLogger()
    {
        spdlog::drop_all();
        spdlog::shutdown();
    }

    template<typename ...Args>
    static void Trace(Args... args)
    {
        if (static_spdlog)
            static_spdlog->trace(args...);
    }

    template<typename ...Args>
    static void Debug(Args... args)
    {
        if (static_spdlog)
            static_spdlog->debug(args...);
    }

    template<typename ...Args>
    static void Info(Args... args)
    {
        if (static_spdlog)
            static_spdlog->info(args...);
    }

    template<typename ...Args>
    static void Warn(Args... args)
    {
        if (static_spdlog)
            static_spdlog->warn(args...);
    }

    template<typename ...Args>
    static void Error(Args... args)
    {
        if (static_spdlog)
            static_spdlog->error(args...);
    }

    template<typename ...Args>
    static void Critical(Args... args)
    {
        if (static_spdlog)
            static_spdlog->critical(args...);
    }

};


_END_MX_KIT_NAME_SPACE_

#define _MX_LOG_INIT_LOG(path, name) mxkit::MXSpdlog::CreateLogger(path, name);
#define _MX_LOG_RELEASE_LOG()        mxkit::MXSpdlog::ReleaseLogger();

#define _MX_LOG_TRACE_EX(logger, fmt, ...) { if(logger)logger->trace(fmt, ## __VA_ARGS__);}
#define _MX_LOG_DEBUG_EX(logger, fmt, ...) { if(logger)logger->debug(fmt, ## __VA_ARGS__);}
#define _MX_LOG_INFO_EX(logger, fmt, ...) { if(logger)logger->info(fmt, ## __VA_ARGS__);}
#define _MX_LOG_WARN_EX(logger, fmt, ...) { if(logger)logger->warn(fmt, ## __VA_ARGS__);}
#define _MX_LOG_ERROR_EX(logger, fmt, ...) { if(logger)logger->error(fmt, ## __VA_ARGS__);}
#define _MX_LOG_CRITICAL_EX(logger, fmt, ...) { if(logger)logger->critical(fmt, ## __VA_ARGS__);}


// #define MX_TRACE(fmt, ...) MX_TRACE_EX(mxkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_DEBUG(fmt, ...) MX_TRACE_EX(mxkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_INFO(fmt, ...) MX_TRACE_EX(mxkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_WARN(fmt, ...) MX_TRACE_EX(mxkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_ERROR(fmt, ...) MX_TRACE_EX(mxkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_CRITICAL(fmt, ...) MX_TRACE_EX(mxkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)


// #define MX_TRACE(fmt, ...) if(mxkit::MXSpdlog::static_spdlog)mxkit::MXSpdlog::static_spdlog->trace(fmt, ##__VA_ARGS__);
// #define MX_DEBUG(fmt, ...) if(mxkit::MXSpdlog::static_spdlog)mxkit::MXSpdlog::static_spdlog->debug(fmt, ##__VA_ARGS__);
// #define MX_INFO(fmt, ...) if(mxkit::MXSpdlog::static_spdlog)mxkit::MXSpdlog::static_spdlog->info(fmt, ##__VA_ARGS__);
// #define MX_WARN(fmt, ...) if(mxkit::MXSpdlog::static_spdlog)mxkit::MXSpdlog::static_spdlog->warn(fmt, ##__VA_ARGS__);
// #define MX_ERROR(fmt, ...) if(mxkit::MXSpdlog::static_spdlog)mxkit::MXSpdlog::static_spdlog->error(fmt, ##__VA_ARGS__);
// #define MX_CRITICAL(fmt, ...) if(mxkit::MXSpdlog::static_spdlog)mxkit::MXSpdlog::static_spdlog->critical(fmt, ##__VA_ARGS__);


#define _MX_LOG_TRACE    mxkit::MXSpdlog::Trace
#define _MX_LOG_DEBUG    mxkit::MXSpdlog::Debug
#define _MX_LOG_INFO     mxkit::MXSpdlog::Info
#define _MX_LOG_WARN     mxkit::MXSpdlog::Warn
#define _MX_LOG_ERROR    mxkit::MXSpdlog::Error
#define _MX_LOG_CRITICAL mxkit::MXSpdlog::Critical
