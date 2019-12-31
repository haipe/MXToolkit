#pragma once

#include <string>
#include "MXCommon.h"
#include "MXStringKit.h"
namespace mxtoolkit
{


    template<typename Str, typename CharType = Str::allocator_type::value_type>
    class Win32App
    {
    public:
        static const Str& CurrentDirectory()
        {
            if (appDirectory.empty())
            {
                if (std::is_same<CharType, wchar_t>::value)
                {
                    WCHAR path[MAX_PATH] = { 0 };
                    GetModuleFileNameW(NULL, path, MAX_PATH);
                    appDirectory = (CharType*)path;
                    appDirectory = appDirectory.substr(0, appDirectory.find_last_of('\\') + 1);
                }
                else
                {
                    CHAR path[MAX_PATH] = { 0 };
                    GetModuleFileNameA(NULL, path, MAX_PATH);
                    appDirectory = (CharType*)path;
                    appDirectory = appDirectory.substr(0, appDirectory.find_last_of('\\') + 1);
                }
            }

            return appDirectory;
        }

    private:
        static Str appDirectory;
    };

    template<typename Str, typename CharType = Str::allocator_type::value_type>
    Str Win32App<Str, CharType>::appDirectory;

}
