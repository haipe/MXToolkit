#include "mxcmdline.h"
#include <tchar.h>
#include "getopt.h"

#include "MXStringKit.h"

namespace mxtoolkit
{

    void GetStartupOptions(const std::string& flags, std::map<std::string, std::string>* result)
    {
#ifdef UNICODE
        result->clear();
        struct option_w longOptions[64] = { 0 };

        std::wstring wflags;
        mxtoolkit::WAConvert<std::string, std::wstring>(flags.c_str(), &wflags);

        do
        {
            int option = getopt_long_w(__argc, __wargv, wflags.c_str(), longOptions, NULL);
            if (option == -1)
                break;

            std::wstring wStr;
            wStr.append(1, (wchar_t)option);

            std::string key;
            mxtoolkit::WAConvert<std::wstring, std::string>(wStr.c_str(),&key);

            std::string value;
            mxtoolkit::WAConvert<std::wstring, std::string>(optarg_w, &value);
            result->insert(std::make_pair(key, value));

        } while (true);
#else
        result->clear();
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


    void GetStartupOptions(const std::wstring& flags, std::map<std::wstring, std::wstring>* result)
    {
#ifdef UNICODE
        result->clear();
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
        result->clear();
        struct option_a longOptions[64] = { 0 };
        std::string aflags;
        mxtoolkit::WAConvert<std::wstring, std::string>(flags.c_str(), &aflags);

        do
        {
            int option = getopt_long_a(__argc, __argv, aflags.c_str(), longOptions, NULL);
            if (option == -1)
                break;

            std::string aStr;
            aStr.append(1, (char)option);

            std::wstring key;
            mxtoolkit::WAConvert<std::string, std::wstring>(aStr.c_str(), &key);

            std::wstring value;
            mxtoolkit::WAConvert<std::string, std::wstring>(optarg_a, &value);
            result->insert(std::make_pair(key, value));

        } while (true);
#endif
    }

}