#pragma once

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_


template<class T_CHAR>
class EnvironmentUtils
{
public:
    typedef T_CHAR* TCHAR_PTR;
    typedef const T_CHAR* CONST_TCHAR_PTR;
    typedef std::basic_string<T_CHAR, std::char_traits<T_CHAR>, std::allocator<T_CHAR>> StdTString;

public:

    static void SetEnvironment(const T_CHAR* name, const T_CHAR* value)
    {
        if (std::is_same<T_CHAR, char>::value == true)
            SetEnvironmentVariableA((LPCSTR)name, (LPCSTR)value);
        else
            SetEnvironmentVariableW((LPCWSTR)name, (LPCWSTR)value);
    }

    static bool GetEnvironment(const T_CHAR* name, T_CHAR* value, UINT& nLen)
    {
        DWORD dwLen = 0;
        if (std::is_same<T_CHAR, char>::value == true)
            dwLen = GetEnvironmentVariableA((LPCSTR)name, (LPSTR)value, nLen);
        else
            dwLen = GetEnvironmentVariableW((LPCWSTR)name, (LPWSTR)value, nLen);

        if (dwLen == 0 || dwLen > nLen)
        {
            nLen = dwLen;
            return false;
        }

        return true;
    }

    static bool GetEnvironment(const T_CHAR* name, StdTString& value)
    {
        UINT uLen = mxkit::_MAX_STRING_BUFFER;
        std::vector<T_CHAR> buf(uLen, (T_CHAR)'\0');
        if (GetEnvironment(name, &buf[0], uLen))
        {
            value = (T_CHAR*)&buf[0];
            return true;
        }

        if (uLen <= 0)
            return false;

        buf.resize(uLen + 1);
        if (!GetEnvironment(name, &buf[0], uLen))
            return false;

        value = (T_CHAR*)&buf[0];
        return true;
    }

    static mxkit::uint32 GetAllString(std::vector<StdTString>& all)
    {
        T_CHAR* envStrings;//LPCTSTR
        T_CHAR* envStringsTmp;
        if (std::is_same<T_CHAR, char>::value == true)
            envStrings = (T_CHAR*)GetEnvironmentStrings();
        else
            envStrings = (T_CHAR*)GetEnvironmentStringsW();

        envStringsTmp = envStrings;

        do
        {
            if (envStringsTmp[0] == 0)
                break;

            all.push_back(envStringsTmp);
            envStringsTmp = envStringsTmp + strlen(envStringsTmp) + 1;

        } while (true);

        if (std::is_same<T_CHAR, char>::value == true)
            FreeEnvironmentStringsA((LPCH)envStrings);
        else
            FreeEnvironmentStringsW((LPWCH)envStrings);

        return all.size();
    }
};





_END_MX_KIT_NAME_SPACE_