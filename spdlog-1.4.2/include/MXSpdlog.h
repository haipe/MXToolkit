#pragma once

#include "MXTimeDate.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging


namespace mxtoolkit
{
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
            std::string fileName = mxtoolkit::CurrentTimeString<std::string>(timeFormat.c_str());
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



}

#define MX_INIT_LOG(path, name) mxtoolkit::MXSpdlog::CreateLogger(path, name);
#define MX_RELEASE_LOG()        mxtoolkit::MXSpdlog::ReleaseLogger();

#define MX_TRACE_EX(logger, fmt, ...) { if(logger)logger->trace(fmt, ## __VA_ARGS__);}
#define MX_DEBUG_EX(logger, fmt, ...) { if(logger)logger->debug(fmt, ## __VA_ARGS__);}
#define MX_INFO_EX(logger, fmt, ...) { if(logger)logger->info(fmt, ## __VA_ARGS__);}
#define MX_WARN_EX(logger, fmt, ...) { if(logger)logger->warn(fmt, ## __VA_ARGS__);}
#define MX_ERROR_EX(logger, fmt, ...) { if(logger)logger->error(fmt, ## __VA_ARGS__);}
#define MX_CRITICAL_EX(logger, fmt, ...) { if(logger)logger->critical(fmt, ## __VA_ARGS__);}


// #define MX_TRACE(fmt, ...) MX_TRACE_EX(mxtoolkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_DEBUG(fmt, ...) MX_TRACE_EX(mxtoolkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_INFO(fmt, ...) MX_TRACE_EX(mxtoolkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_WARN(fmt, ...) MX_TRACE_EX(mxtoolkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_ERROR(fmt, ...) MX_TRACE_EX(mxtoolkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)
// #define MX_CRITICAL(fmt, ...) MX_TRACE_EX(mxtoolkit::MXSpdlog::static_spdlog,fmt, ## __VA_ARGS__)


// #define MX_TRACE(fmt, ...) if(mxtoolkit::MXSpdlog::static_spdlog)mxtoolkit::MXSpdlog::static_spdlog->trace(fmt, ##__VA_ARGS__);
// #define MX_DEBUG(fmt, ...) if(mxtoolkit::MXSpdlog::static_spdlog)mxtoolkit::MXSpdlog::static_spdlog->debug(fmt, ##__VA_ARGS__);
// #define MX_INFO(fmt, ...) if(mxtoolkit::MXSpdlog::static_spdlog)mxtoolkit::MXSpdlog::static_spdlog->info(fmt, ##__VA_ARGS__);
// #define MX_WARN(fmt, ...) if(mxtoolkit::MXSpdlog::static_spdlog)mxtoolkit::MXSpdlog::static_spdlog->warn(fmt, ##__VA_ARGS__);
// #define MX_ERROR(fmt, ...) if(mxtoolkit::MXSpdlog::static_spdlog)mxtoolkit::MXSpdlog::static_spdlog->error(fmt, ##__VA_ARGS__);
// #define MX_CRITICAL(fmt, ...) if(mxtoolkit::MXSpdlog::static_spdlog)mxtoolkit::MXSpdlog::static_spdlog->critical(fmt, ##__VA_ARGS__);


#define MX_TRACE    mxtoolkit::MXSpdlog::Trace
#define MX_DEBUG    mxtoolkit::MXSpdlog::Debug
#define MX_INFO     mxtoolkit::MXSpdlog::Info
#define MX_WARN     mxtoolkit::MXSpdlog::Warn
#define MX_ERROR    mxtoolkit::MXSpdlog::Error
#define MX_CRITICAL mxtoolkit::MXSpdlog::Critical
