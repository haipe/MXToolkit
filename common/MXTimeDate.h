#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>


namespace mxtoolkit
{
    template<typename T>
    T CurrentTimeString(const char* format)
    {
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::stringstream ss;
        if(format)
        ss << std::put_time(std::localtime(&t), format);
        else
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
        T sss = ss.str();
        std::cout << sss;

        return sss;
    }
}
