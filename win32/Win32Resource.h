#pragma once
#include <Windows.h>

namespace mxtoolkit
{

    template<typename T = int>
    bool LoadResource(LPCTSTR sourceType, DWORD sourceID, void** buffer, T* size = nullptr)
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
        }

        return true;
    }
}