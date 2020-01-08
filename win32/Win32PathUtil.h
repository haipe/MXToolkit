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
        static const Str& CurrentDirectory(HMODULE module = NULL)
        {
            if (appDirectory.empty())
            {
                if (std::is_same<CharType, wchar_t>::value)
                {
                    WCHAR path[MAX_PATH] = { 0 };
                    GetModuleFileNameW(module, path, MAX_PATH);
                    appDirectory = (CharType*)path;
                    appDirectory = appDirectory.substr(0, appDirectory.find_last_of('\\') + 1);
                }
                else
                {
                    CHAR path[MAX_PATH] = { 0 };
                    GetModuleFileNameA(module, path, MAX_PATH);
                    appDirectory = (CharType*)path;
                    appDirectory = appDirectory.substr(0, appDirectory.find_last_of('\\') + 1);
                }
            }

            return appDirectory;
        }

        static const Str& GetModuleDirectory(HMODULE module)
        {
            if (module == NULL)
            {
                _ASSERT(!module);
                static Str s;
                return s;
            }

            if (moduleDirectory.empty())
            {
                if (std::is_same<CharType, wchar_t>::value)
                {
                    WCHAR path[MAX_PATH] = { 0 };
                    GetModuleFileNameW(module, path, MAX_PATH);
                    moduleDirectory = (CharType*)path;
                    moduleDirectory = moduleDirectory.substr(0, moduleDirectory.find_last_of('\\') + 1);
                }
                else
                {
                    CHAR path[MAX_PATH] = { 0 };
                    GetModuleFileNameA(module, path, MAX_PATH);
                    moduleDirectory = (CharType*)path;
                    moduleDirectory = moduleDirectory.substr(0, moduleDirectory.find_last_of('\\') + 1);
                }
            }

            return moduleDirectory;
        }

        //全路径
        static void CreateDirectory(const Str& path)
        {
        }
        
    private:
        static Str appDirectory;
        static Str moduleDirectory;
    };

    template<typename Str, typename CharType = Str::allocator_type::value_type>
    Str Win32App<Str, CharType>::appDirectory;

    template<typename Str, typename CharType = Str::allocator_type::value_type>
    Str Win32App<Str, CharType>::moduleDirectory;

    template<>
    static void Win32App<std::string>::CreateDirectory(const std::string& path)
    {
        std::vector<std::string> dirs;
        SplitString<std::string>(path, "\\", &dirs);

        if (dirs.size() <= 1)
            return;

        auto it = dirs.begin();
        std::string dirStr = *it;
        for (++it; it != dirs.end(); it++)
        {
            if(it->empty())
                continue;

            dirStr += "\\";
            dirStr += *it;
            ::CreateDirectoryA(dirStr.c_str(), NULL);
        }
    }

    template<>
    static void Win32App<std::wstring>::CreateDirectory(const std::wstring& path)
    {
        std::vector<std::wstring> dirs;
        SplitString<std::wstring>(path, L"\\", &dirs);

        if (dirs.size() <= 1)
            return;

        auto it = dirs.begin();
        std::wstring dirStr = *it;
        for (++it; it != dirs.end(); it++)
        {
            if (it->empty())
                continue;

            dirStr += L"\\";
            dirStr += *it;
            ::CreateDirectoryW(dirStr.c_str(), NULL);
        }
    }
}
