/********************************************************************************
*                                                                               *
* NTVersion.h : Get Windows version and information. (Version 1.1)              *
*                                                                               *
* Copyright (C) 2020 - 2021 Coral Studio. All Rights Reserved.                  *
*                                                                               *
********************************************************************************/

#pragma once

#pragma warning(push)
#pragma warning(disable: 4996)

#include <Windows.h>
#include <string>

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_

class WinVersion
{
public:
    enum Version
    {
        WIN_UNKNOW,
        WIN_NT,
        WIN_2000,
        WIN_XP,
        WIN_SERVER_2003,
        WIN_SERVER_2003_R2,
        WIN_VISTA,
        WIN_SERVER_2008,
        WIN_SERVER_2008_R2,
        WIN_7,
        WIN_8,
        WIN_SERVER_2012,
        WIN_8_1,
        WIN_SERVER_2012_R2,
        WIN_10,
        WIN_SERVER_2016,
        WIN_SERVER_2019,
    };

    struct NTVersionInfo                   // for GetWindowsNTVersionEx()
    {
		NTVersionInfo():version(WIN_UNKNOW){}

        bool is64BitSystem;                // 64-bit(true) 32-bit(false)
        bool isWindowsServer;              // Server(true) Workstation(false)

        DWORD majorVersion;                // Major NT version
        DWORD minorVersion;                // Minor NT version
        DWORD buildNumber;                 // NT build number

        Version version;

        char osName[256];                  // OS short name
        char osFullName[256];              // OS full name
    };

    struct WindowsVersion
    {
        Version     ver;
        DWORD       buildNUmber;
        char        osName[256];
    };

    inline bool IsWindowsNT()
    {
        OSVERSIONINFOEX os;
        os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO*)&os);
        return os.dwPlatformId == VER_PLATFORM_WIN32_NT;
    }

    inline bool Is64bitSystem()            // Only supports Windows XP / Server 2003 and higher
    {
        SYSTEM_INFO si;
        GetNativeSystemInfo(&si);
        return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
            si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64;
    }

    inline bool IsWindowsServer()          // Only supports Windows 2000 and higher
    {
        OSVERSIONINFOEXW osvi
            = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0, 0, VER_NT_WORKSTATION };
        const DWORDLONG dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);
        return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
    }

    BOOL GetWindowsVersionInfo(RTL_OSVERSIONINFOW& osVer)
    {
        typedef LONG(__stdcall* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
        fnRtlGetVersion pRtlGetVersion;
        HMODULE hNtdll;
        LONG ntStatus;

        RTL_OSVERSIONINFOW versionInformation = { 0 };

#ifdef UNICODE
        hNtdll = GetModuleHandle(L"ntdll.dll");
#else
        hNtdll = GetModuleHandle("ntdll.dll");
#endif

        if (hNtdll == NULL)
            return FALSE;

        pRtlGetVersion = (fnRtlGetVersion)GetProcAddress(hNtdll, "RtlGetVersion");
        if (pRtlGetVersion == NULL)
            return FALSE;

        versionInformation.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
        ntStatus = pRtlGetVersion(&versionInformation);

        if (ntStatus != 0)
            return FALSE;

        osVer = versionInformation;
        return TRUE;
    }

    const char* GetWindowsFullName(int majorVersion, int buildNumber, bool isServer)      // Only supports Windows 2000 and higher
    {
        if (majorVersion <= 0 || buildNumber <= 0)
        {
            RTL_OSVERSIONINFOW versionInformation = { 0 };
            GetWindowsVersionInfo(versionInformation);

            majorVersion = versionInformation.dwMajorVersion;
            buildNumber = versionInformation.dwBuildNumber;
            isServer = IsWindowsServer();
        }
        // NT 3.1
        if (buildNumber < 528)
            return "Windows NT 3.1 Beta";
        else if (buildNumber == 528)
            return "Windows NT 3.1";

        // NT 3.51
        else if (buildNumber < 807)
            return "Windows NT 3.51 Beta";
        else if (buildNumber == 807)
            return "Windows NT 3.51";

        // NT 4.0
        else if (buildNumber < 1381)
            return "Windows NT 4.0 Beta";
        else if (buildNumber == 1381)
            return "Windows NT 4.0";

        // NT 5.0
        else if (buildNumber < 1946)
            return "Windows NT 5.0 Beta";
        else if (buildNumber < 2195)
            return "Windows 2000 Beta";
        else if (buildNumber == 2195)
            return "Windows 2000";

        // NT 5.1
        else if (buildNumber < 2464)
            if (!isServer)
                return "Windows Codename Whistler Beta";
            else
                return "Windows Codename Whistler Server Beta";
        else if (buildNumber < 2600)
            if (!isServer)
                return "Windows XP Beta";
            else
                return "Windows Server 2003 Beta";
        else if (buildNumber == 2600)
            return "Windows XP";

        // NT 5.2 - 6.0 (Pre-Reset)
        else if (buildNumber < 3790)
            if (!isServer)
            {
                if (majorVersion == 5)
                    return "Windows XP Beta";
                else
                    return "Windows Codename Longhorn Beta";
            }
            else
            {
                if (majorVersion == 5)
                    return "Windows Server 2003 Beta";
                else
                    return "Windows Codename Longhorn Server Beta";
            }
        else if (buildNumber == 3790)
            if (!isServer)
                return "Windows XP";
            else
            {
                if (GetSystemMetrics(SM_SERVERR2) == 0)
                    return "Windows Server 2003";
                else
                    return "Windows Server 2003 R2";
            }
        else if (buildNumber < 5000)
            if (!isServer)
                return "Windows Codename Longhorn Beta";
            else
                return "Windows Codename Longhorn Server Beta";

        // NT 6.0 (Post-Reset)
        else if (buildNumber < 6000)
            if (!isServer)
                return "Windows Vista Beta";
            else
                return "Windows Server 2008 Beta";
        else if (buildNumber == 6000)
            if (!isServer)
                return "Windows Vista";
            else
                return "Windows Server 2008";
        else if (buildNumber == 6001)
            if (!isServer)
                return "Windows Vista SP1";
            else
                return "Windows Server 2008 SP1";
        else if (buildNumber == 6002)
            if (!isServer)
                return "Windows Vista SP2";
            else
                return "Windows Server 2008 SP2";

        // NT 6.1
        else if (buildNumber < 7600)
            if (!isServer)
                return "Windows 7 Beta";
            else
                return "Windows Server 2008 R2 Beta";
        else if (buildNumber == 7600)
            if (!isServer)
                return "Windows 7";
            else
                return "Windows Server 2008 R2";
        else if (buildNumber == 7601)
            if (!isServer)
                return "Windows 7 SP1";
            else
                return "Windows Server 2008 R2 SP1";

        // NT 6.2
        else if (buildNumber < 9200)
            if (!isServer)
                return "Windows 8 Beta";
            else
                return "Windows Server 2012 Beta";
        else if (buildNumber == 9200)
            if (!isServer)
                return "Windows 8";
            else
                return "Windows Server 2012";

        // NT 6.3
        else if (buildNumber < 9600)
            if (!isServer)
                return "Windows 8.1 Preview";
            else
                return "Windows Server 2012 R2 Preview";
        else if (buildNumber == 9600)
            if (!isServer)
                return "Windows 8.1";
            else
                return "Windows Server 2012 R2";

        // NT 6.4 - 10.0.10240
        else if (buildNumber < 10074)
            if (!isServer)
                return "Windows 10 Technical Preview";
            else
                return "Windows Server 2016 Technical Preview";
        else if (buildNumber < 10240)
            if (!isServer)
                return "Windows 10 Insider Preview (TH1)";
            else
                return "Windows Server 2016 Technical Preview";
        else if (buildNumber == 10240)
            if (!isServer)
                return "Windows 10 (Version 1507)";
            else
                return "Windows Server 2016 Technical Preview";

        // NT 10.0.10586
        else if (buildNumber < 10586)
            if (!isServer)
                return "Windows 10 Insider Preview (TH2)";
            else
                return "Windows Server 2016 Technical Preview";
        else if (buildNumber == 10586)
            if (!isServer)
                return "Windows 10 (Version 1511)";
            else
                return "Windows Server 2016 Technical Preview";

        // NT 10.0.14393
        else if (buildNumber < 14393)
            if (!isServer)
                return "Windows 10 Insider Preview (RS1)";
            else
                return "Windows Server 2016 Technical Preview";
        else if (buildNumber == 14393)
            if (!isServer)
                return "Windows 10 (Version 1607)";
            else
                return "Windows Server 2016";

        // NT 10.0.15063
        else if (buildNumber < 15063)
            if (!isServer)
                return "Windows 10 Insider Preview (RS2)";
            else
                return "Windows Server 1703 Insider Preview";
        else if (buildNumber == 15063)
            if (!isServer)
                return "Windows 10 (Version 1703)";
            else
                return "Windows Server 1703";

        // NT 10.0.16299
        else if (buildNumber < 16299)
            if (!isServer)
                return "Windows 10 Insider Preview (RS3)";
            else
                return "Windows Server 1709 Insider Preview";
        else if (buildNumber == 16299)
            if (!isServer)
                return "Windows 10 (Version 1709)";
            else
                return "Windows Server 1709";

        // NT 10.0.17134
        else if (buildNumber < 17134)
            if (!isServer)
                return "Windows 10 Insider Preview (RS4)";
            else
                return "Windows Server 1803 Insider Preview";
        else if (buildNumber == 17134)
            if (!isServer)
                return "Windows 10 (Version 1803)";
            else
                return "Windows Server 1803";

        // NT 10.0.17763
        else if (buildNumber < 17763)
            if (!isServer)
                return "Windows 10 Insider Preview (RS5)";
            else
                return "Windows Server 2019 Insider Preview";
        else if (buildNumber == 17763)
            if (!isServer)
                return "Windows 10 (Version 1809)";
            else
                return "Windows Server 2019";

        // NT 10.0.18362
        else if (buildNumber < 18362)
            if (!isServer)
                return "Windows 10 Insider Preview (19H1)";
            else
                return "Windows Server 1903 Insider Preview";
        else if (buildNumber == 18362)
            if (!isServer)
                return "Windows 10 (Version 1903)";
            else
                return "Windows Server 1903";

        // NT 10.0.18363
        else if (buildNumber == 18363)
            if (!isServer)
                return "Windows 10 (Version 1909)";
            else
                return "Windows Server 1909";

        // NT 10.0.19041
        else if (buildNumber < 19041)
            if (!isServer)
                return "Windows 10 Insider Preview (20H1)";
            else
                return "Windows Server 2004 Insider Preview";
        else if (buildNumber == 19041)
            if (!isServer)
                return "Windows 10 (Version 2004)";
            else
                return "Windows Server 2004";

        // NT 10.0.19042
        else if (buildNumber == 19042)
            if (!isServer)
                return "Windows 10 (Version 20H2)";
            else
                return "Windows Server 20H2";

        // NT 10.0.19043
        else if (buildNumber == 19043)
            if (!isServer)
                return "Windows 10 (Version 21H1)";
            else
                return "Windows Server 21H1";

        // Unknown Version
        else if (buildNumber > 19043)
            if (!isServer)
                return "Windows 10 (Unknown Version)";
            else
                return "Windows Server (Unknown Version)";
        else
            return "Unknown Version";
    }

    BOOL GetWindowsVersion(NTVersionInfo& info)  // Only supports Windows XP / Server 2003 and higher
    {
        RTL_OSVERSIONINFOW versionInformation = { 0 };
        if (!GetWindowsVersionInfo(versionInformation))
            return FALSE;

        info.majorVersion = versionInformation.dwMajorVersion;
        info.minorVersion = versionInformation.dwMinorVersion;
        info.buildNumber = versionInformation.dwBuildNumber;
        info.isWindowsServer = IsWindowsServer();
        info.is64BitSystem = Is64bitSystem();

        std::string OSName;
        OSName = "";

        if (info.buildNumber <= 528)
        {
            info.version = WIN_NT;
            OSName = "Windows NT 3.1";
        }
        else if (info.buildNumber <= 807)
        {
            info.version = WIN_NT;
            OSName = "Windows NT 3.51";
        }
        else if (info.buildNumber <= 1381)
        {
            info.version = WIN_NT;
            OSName = "Windows NT 4.0";
        }
        else if (info.buildNumber <= 2195)
        {
            info.version = WIN_2000;
            OSName = "Windows 2000";
        }
        else if (info.buildNumber <= 6002)
        {
            if (info.majorVersion == 5)
            {
                bool notR2 = GetSystemMetrics(SM_SERVERR2) == 0;
                info.version = !info.isWindowsServer ?
                    WIN_XP : notR2 ?
                    WIN_SERVER_2003 : WIN_SERVER_2003_R2;
                OSName = !info.isWindowsServer ?
                    "Windows XP" : notR2 ?
                    "Windows Server 2003" : "Windows Server 2003 R2";
            }
            else
            {
                info.version = !info.isWindowsServer ? WIN_VISTA : WIN_SERVER_2008;
                OSName = !info.isWindowsServer ? "Windows Vista" : "Windows Server 2008";
            }
        }
        else if (info.buildNumber <= 7601)
        {
            info.version = !info.isWindowsServer ? WIN_7 : WIN_SERVER_2008_R2;
            OSName = !info.isWindowsServer ? "Windows 7" : "Windows Server 2008 R2";
        }
        else if (info.buildNumber <= 9200)
        {
            info.version = !info.isWindowsServer ? WIN_8 : WIN_SERVER_2012;
            OSName = !info.isWindowsServer ? "Windows 8" : "Windows Server 2012";
        }
        else if (info.buildNumber <= 9600)
        {
            info.version = !info.isWindowsServer ? WIN_8_1 : WIN_SERVER_2012_R2;
            OSName = !info.isWindowsServer ? "Windows 8.1" : "Windows Server 2012 R2";
        }
        else
        {
            info.version = !info.isWindowsServer ? WIN_10 : info.buildNumber <= 14393 ? WIN_SERVER_2016 : WIN_SERVER_2019;
            OSName = !info.isWindowsServer ? "Windows 10" : info.buildNumber <= 14393 ? "Windows Server 2016" : info.buildNumber <= 17763 ? "Windows Server 2019" : "Windows Server Unknown Version";
        }

        strcpy(info.osName, OSName.c_str());
        strcpy(info.osFullName, GetWindowsFullName(info.majorVersion, info.buildNumber, info.isWindowsServer));

        return TRUE;
    }

    static Result Compare(Version ver, Version* selfVersion = nullptr)
    {
        static NTVersionInfo info;
        if (info.version == WIN_UNKNOW)
        {
            WinVersion wv;
            if (!wv.GetWindowsVersion(info))
                info.version = WIN_UNKNOW;
        }

        if (selfVersion)
            *selfVersion = info.version;

        return (info.version < ver) ? -1 :
            (info.version == ver) ? 0 : 1;
    }

};

_END_MX_KIT_NAME_SPACE_

#pragma warning(pop)