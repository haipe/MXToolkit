#pragma once
#include <Windows.h>
#include "MXStringKit.h"

namespace mxtoolkit
{

    template<typename T = int>
    bool LoadResource(LPCTSTR sourceType, DWORD sourceID, void** buffer, T* size)
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


    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    bool LoadResource(LPCTSTR sourceType, DWORD sourceID, T* strResult, bool fromUtf8 = true)
    {
        HINSTANCE hInst = GetModuleHandle(0);
        HRSRC hResourceFile = FindResource(hInst, MAKEINTRESOURCE(sourceID), sourceType);
        if (!hResourceFile)
            return false;

        if (strResult)
        {
            HANDLE hglob = LoadResource(hInst, hResourceFile);
            void* buffer = LockResource(hglob);

            //w
            if (std::is_same<StrType, wchar_t>::value)
            {
                if (fromUtf8)
                    mxtoolkit::WUtf8Convert<std::string, std::wstring>((const char*)buffer, (std::wstring*)strResult);
                else
                    mxtoolkit::WAConvert<std::string, std::wstring>((const char*)buffer, (std::wstring*)strResult);
            }
            else if(std::is_same<StrType, char>::value)
            {
                *strResult = (const StrType*)buffer;
            }

            FreeResource(hglob);
        }
        return true;
    }
}