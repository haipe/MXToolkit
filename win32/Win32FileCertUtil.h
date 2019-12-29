#pragma once
#include <Windows.h>
#include "MXStringKit.h"
#include "AnonymousPip.h"

namespace mxtoolkit
{
    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    T GetMD5(const std::string& file)
    {
    doAgain:
        std::string cmd = "certutil -hashfile ";
        cmd += file;
        cmd += " MD5";

        std::string res;
        AnonymousPip::CreateCmd(cmd, [&res](const std::string & result) {
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