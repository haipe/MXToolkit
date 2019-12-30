#pragma once
#include <Windows.h>
#include <fstream>
#include "MXStringKit.h"
#include "MXAnonymousPip.h"

namespace mxtoolkit
{
    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    bool FileExist(const T& path) 
    {
        if (path.empty())
            return false;

        //if (std::is_same<StrType, wchar_t>::value)
        {
            std::wfstream f(path, std::ios::in);
            if (f.is_open())
            {
                f.close();
                return true;
            }
        }

        return false;
    }

    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    T GetMD5(const std::string& file)
    {
    doAgain:
        std::string cmd = "certutil -hashfile ";
        cmd += file;
        cmd += " MD5";

        std::string res;
        MXAnonymousPip::CreateCmd<std::string>(cmd, [&res](const std::string & result) {
            res += result;
        });

        std::vector<std::string> resSplit;
        mxtoolkit::SplitString<std::string>(res, "\r\n", &resSplit);

        if (resSplit.size() >= 2 && resSplit[1].length() == 32)
        {
            printf("file MD5:%s.\n", resSplit[1].c_str());
            if (std::is_same<StrType, wchar_t>::value)
            {
                T r;
                mxtoolkit::WAConvert<std::string, std::wstring>(resSplit[1].c_str(), (std::wstring*)&r);
                return r;
            }

            return resSplit[1];
        }

        printf("Get MD5 Again.\n");
        goto doAgain;
        return "";
    }
}