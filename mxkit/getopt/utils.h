#pragma once

#ifdef _MX_USE_GETOPT_

#include <map>

#include "mxkit.h"

#include "getopt/getopt.h"
#include "base/string_convert.h"


_BEGIN_MX_KIT_NAME_SPACE_

class GetoptUtils
{
public:
    static void Options(const std::string& flags, std::map<std::string, std::string>* result)
    {
        result->clear();
#ifdef UNICODE
        struct option_w longOptions[64] = { 0 };

        std::wstring wflags;
        mxkit::Win32StringConvert::AnsiiToUnicode(flags.c_str(), wflags);

        do
        {
            int option = getopt_long_w(__argc, __wargv, wflags.c_str(), longOptions, NULL);
            if (option == -1)
                break;

            std::wstring wStr;
            wStr.append(1, (wchar_t)option);

            std::string key;
            mxkit::Win32StringConvert::UnicodeToAnsii(wStr.c_str(), key);

            std::string value;
            mxkit::Win32StringConvert::UnicodeToAnsii(optarg_w, value);
            result->insert(std::make_pair(key, value));

        } while (true);
#else

        struct option_a longOptions[64] = { 0 };
        do
        {
            int option = getopt_long_a(__argc, __argv, flags.c_str(), longOptions, NULL);
            if (option == -1)
                break;

            std::string k;
            k.append(1, (char)option);
            result->insert(std::make_pair(k, optarg_a));

        } while (true);
#endif
    }

    static void Options(const std::wstring& flags, std::map<std::wstring, std::wstring>* result)
    {
        result->clear();
#if defined(UNICODE)
        struct option_w longOptions[64] = { 0 };
        do
        {
            int option = getopt_long_w(__argc, __wargv, flags.c_str(), longOptions, NULL);
            if (option == -1)
                break;

            std::wstring k;
            k.append(1, (wchar_t)option);
            result->insert(std::make_pair(k, optarg_w));

        } while (true);
#else

        struct option_a longOptions[64] = { 0 };
        do
        {
            int option = getopt_long_a(__argc, __argv, "", longOptions, NULL);
            if (option == -1)
                break;

            std::string k;
            k.append(1, (char)option);

            std::wstring key;
            mxkit::Win32StringConvert::AnsiiToUnicode(k.c_str(), key);

            std::wstring value;
            mxkit::Win32StringConvert::AnsiiToUnicode(optarg_a, value);
            result->insert(std::make_pair(key, value));

        } while (true);
#endif
    }

};

_END_MX_KIT_NAME_SPACE_

#endif