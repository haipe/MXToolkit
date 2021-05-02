#pragma once

#include <map>

#include "msiquery.h"
#include "mxkit.h"
#include "base/string_utils.h"

#pragma comment(lib,"Msi.lib")

_BEGIN_MX_KIT_NAME_SPACE_


class MsiUtils
{
public:
    template<typename Str, typename CharType = Str::allocator_type::value_type>
    static bool ProductVersion(const CharType* msiFile, CharType* version, unsigned int size)
    {
        MSIHANDLE hMsiHandle = NULL;
        UINT uiStatus = 0;
        if (std::is_same<CharType, wchar_t>::value)
            uiStatus = MsiOpenDatabaseW((LPCWSTR)msiFile, (LPCWSTR)MSIDBOPEN_READONLY, &hMsiHandle);
        else
            uiStatus = MsiOpenDatabaseA((LPCSTR)msiFile, (LPCSTR)MSIDBOPEN_READONLY, &hMsiHandle);

        if (ERROR_SUCCESS != uiStatus)
        {
            return false;
        }

        MSIHANDLE hView = NULL;
        MsiDatabaseOpenView(hMsiHandle, _T("SELECT * FROM Property WHERE Property='ProductVersion'"), &hView);
        MSIHANDLE hRecord = NULL;
        MsiViewExecute(hView, hRecord);
        while (MsiViewFetch(hView, &hRecord) == ERROR_SUCCESS)
        {
            if (std::is_same<CharType, wchar_t>::value)
            {
                DWORD  dwStringLen = MAX_PATH;
                //MsiRecordGetString(hRecord, 1, szValueBuf, &dwStringLen);
                dwStringLen = MAX_PATH;
                WCHAR Source[MAX_PATH] = { 0 };
                MsiRecordGetStringW(hRecord, 2, Source, &dwStringLen);
                //printf("%s\t:%s\r\n", szValueBuf, Source);
                wcscpy_s((wchar_t*)version, size, Source);
            }
            else
            {
                DWORD  dwStringLen = MAX_PATH;
                //MsiRecordGetString(hRecord, 1, szValueBuf, &dwStringLen);
                dwStringLen = MAX_PATH;
                CHAR Source[MAX_PATH] = { 0 };
                MsiRecordGetStringA(hRecord, 2, Source, &dwStringLen);
                //printf("%s\t:%s\r\n", szValueBuf, Source);
                strcpy_s((char*)version, size, Source);
            }
        }

        MsiCloseHandle(hView);
        MsiCloseHandle(hMsiHandle);

        return true;
    }


    template<typename Str, typename CharType = Str::allocator_type::value_type>
    static bool AllProperty(const CharType* msiFile, std::map<Str,Str>& all)
    {
        all.clear();
        MSIHANDLE hMsiHandle = NULL;
        UINT uiStatus = 0;
        if (std::is_same<CharType, wchar_t>::value)
            uiStatus = MsiOpenDatabaseW((LPCWSTR)msiFile, (LPCWSTR)MSIDBOPEN_READONLY, &hMsiHandle);
        else
            uiStatus = MsiOpenDatabaseA((LPCSTR)msiFile, (LPCSTR)MSIDBOPEN_READONLY, &hMsiHandle);

        if (ERROR_SUCCESS != uiStatus)
        {
            return false;
        }

        std::cout << std::endl;
        MSIHANDLE hView = NULL;
        MsiDatabaseOpenView(hMsiHandle, _T("SELECT * FROM Property"), &hView);
        MSIHANDLE hRecord = NULL;
        MsiViewExecute(hView, hRecord);
        while (MsiViewFetch(hView, &hRecord) == ERROR_SUCCESS)
        {
            if (std::is_same<CharType, wchar_t>::value)
            {
                DWORD  dwStringLen = MAX_PATH;
                //MsiRecordGetString(hRecord, 1, szValueBuf, &dwStringLen);
                dwStringLen = MAX_PATH;
                WCHAR key[MAX_PATH] = { 0 };
                MsiRecordGetStringW(hRecord, 1, key, &dwStringLen);
                dwStringLen = MAX_PATH;
                WCHAR value[MAX_PATH] = { 0 };
                MsiRecordGetStringW(hRecord, 2, value, &dwStringLen);
                all[(const CharType*)key] = (const CharType*)value;
                //wprintf(L"%s\t:%s\r\n", key, value);
            }
            else
            {
                DWORD  dwStringLen = MAX_PATH;
                dwStringLen = MAX_PATH;
                CHAR key[MAX_PATH] = { 0 };
                MsiRecordGetStringA(hRecord, 1, key, &dwStringLen);
                dwStringLen = MAX_PATH;
                CHAR value[MAX_PATH] = { 0 };
                MsiRecordGetStringA(hRecord, 2, value, &dwStringLen);
                //printf("%s\t:%s\r\n", key, value);
                all[(const CharType*)key] = (const CharType*)value;
            }
        }

        MsiCloseHandle(hView);
        MsiCloseHandle(hMsiHandle);

        return true;
    }
};


_END_MX_KIT_NAME_SPACE_