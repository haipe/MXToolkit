#pragma once
#include <map>
#include <string>

namespace mxtoolkit
{
    void GetStartupOptions(const std::string& flags, std::map<std::string, std::string>* result);

    void GetStartupOptions(const std::wstring& flags, std::map<std::wstring, std::wstring>* result);
}

