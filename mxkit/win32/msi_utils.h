#pragma once

#include "msiquery.h"
#include "mxkit.h"


#pragma comment(lib,"Msi.lib")

_BEGIN_MX_KIT_NAME_SPACE_


template<typename T>
class MsgUtils
{
public:
    static bool GetMsiVersion(const TCHAR* msiFile, TCHAR* version, unsigned int size)
    {
        MSIHANDLE hMsiHandle = NULL;
        UINT uiStatus = MsiOpenDatabase(msiFile, MSIDBOPEN_READONLY, &hMsiHandle);
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
            //TCHAR szValueBuf[MAX_PATH] = { 0 };
            DWORD  dwStringLen = MAX_PATH;
            //MsiRecordGetString(hRecord, 1, szValueBuf, &dwStringLen);
            dwStringLen = MAX_PATH;
            TCHAR Source[MAX_PATH] = { 0 };
            MsiRecordGetString(hRecord, 2, Source, &dwStringLen);
            //printf("%s\t:%s\r\n", szValueBuf, Source);
            _tcscpy_s(version, size, Source);
        }

        MsiCloseHandle(hView);
        MsiCloseHandle(hMsiHandle);

        return true;
    }
};


_END_MX_KIT_NAME_SPACE_