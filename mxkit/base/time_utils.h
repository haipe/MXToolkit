#pragma once

#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_

class TimeUtils
{
public:
    template<typename T = std::string>
    static T&& ToString(const char* format)
    {
        auto tNow = std::chrono::system_clock::now();
        auto timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::stringstream ss;
        if (format)
        {
            ss << std::put_time(std::localtime(&timeNow), format);
        }
        else
        {
            ss << std::put_time(std::localtime(&timeNow), "%Y-%m-%d %H:%M:%S");
            auto tMilli = std::chrono::duration_cast<std::chrono::milliseconds>(tNow.time_since_epoch());
            auto tSeconds = std::chrono::duration_cast<std::chrono::seconds>(tNow.time_since_epoch());
            auto ms = tMilli - tSeconds;
            ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        }

        return std::move(ss.str());
    }
};


_END_MX_KIT_NAME_SPACE_