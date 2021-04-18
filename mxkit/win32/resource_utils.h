#pragma once
#include <Windows.h>

#include "base/string.h"

_BEGIN_MX_KIT_NAME_SPACE_

class ResourceUtils
{
public:
    template<typename T = uint32>
    bool GetBuffer(LPCTSTR sourceType, DWORD sourceID, void** buffer, T* size)
    {
        HINSTANCE hInst = GetModuleHandle(0);
        HRSRC hResourceFile = FindResource(hInst, MAKEINTRESOURCE(sourceID), sourceType);
        if (!hResourceFile)
            return false;

        if (size)
        {
            *size = SizeofResource(hInst, hResourceFile);
        }

        if (buffer)
        {
            HANDLE hglob = LoadResource(hInst, hResourceFile);
            *buffer = LockResource(hglob);

            FreeResource(hglob);
        }

        return true;
    }

    template<typename Str = std::string>
    bool GetString(LPCTSTR sourceType, DWORD sourceID, Str* strResult, bool fromUtf8 = true)
    {
        typedef typename Str::allocator_type::value_type CharType;
        HINSTANCE hInst = GetModuleHandle(0);
        HRSRC hResourceFile = FindResource(hInst, MAKEINTRESOURCE(sourceID), sourceType);
        if (!hResourceFile)
            return false;

        if (strResult)
        {
            HANDLE hglob = LoadResource(hInst, hResourceFile);
            void* buffer = LockResource(hglob);

            //w
            if (std::is_same<CharType, wchar_t>::value)
            {
                if (fromUtf8)
                    WUtf8Convert<std::string, std::wstring>((const char*)buffer, (std::wstring*)strResult);
                else
                    WAConvert<std::string, std::wstring>((const char*)buffer, (std::wstring*)strResult);
            }
            else if (std::is_same<CharType, char>::value)
            {
                *strResult = (const StrType*)buffer;
            }

            FreeResource(hglob);
        }
        return true;
    }

};

_END_MX_KIT_NAME_SPACE_