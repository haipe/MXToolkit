#include "stdio.h"
#include "windows.h"
#include "tchar.h"

#include <process.h>
#include <Tlhelp32.h>
#include <psapi.h>

#include <tlhelp32.h> 
#pragma  comment (lib,"psapi")

#include "base/string_convert.h"

_BEGIN_MX_KIT_NAME_SPACE_


class ProcessUtils
{
public:

    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
        static bool FindProcess(const typename CharType* processName, std::function<void(const PROCESSENTRY32&, bool& bBreak)> func)
    {
#ifdef UNICODE

        if (!processName)
            return false;
        
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

        PROCESSENTRY32 pc = { sizeof(PROCESSENTRY32) };
        if (!Process32First(hProcessSnap, &pc))
        {
            if (NULL != hProcessSnap)
                CloseHandle(hProcessSnap);

            return false;
        }

        std::wstring ws;
        if (std::is_same<CharType, char>::value)
            Win32StringConvert::AnsiiToUnicode(processName, ws);

        bool bFind = false;
        do {

            bool bSame = false;
            if (std::is_same<CharType, char>::value)
                bSame = (wcsicmp(pc.szExeFile, ws.c_str()) == 0);
            else
                bSame = (wcsicmp(pc.szExeFile, (const wchar_t*)processName) == 0);

            if (!bSame)
                continue;

            if (!func)
            {
                bFind = true;
                break;
            }

            bool bBreak = false;
            func(pc, bBreak);
            if (bBreak)
            {
                bFind = bFind | false;
                break;
            }

            bFind = true;
        } while (Process32Next(hProcessSnap, &pc));

        if (NULL != hProcessSnap)
            CloseHandle(hProcessSnap);

        return bFind;
#endif

    }

    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
    static PROCESSENTRY32* FindProcess(const typename CharType* processName, PROCESSENTRY32& refProc)
    {
        bool bFind = false;
        FindProcess<Str, CharType>(processName, [&](const PROCESSENTRY32& pro, bool& bBreak)
        {
            refProc = pro;
            bFind = true;
            bBreak = true;
        });

        return bFind ? &refProc : nullptr;
    }

    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
        static uint32 FindProcess(const typename CharType* processName, std::vector<PROCESSENTRY32> procList)
    {
        procList.clear();
        FindProcess<Str, CharType>(processName, [&](const PROCESSENTRY32& pro, bool& bBreak)
        {
            procList.push_back(pro);
        });

        return procList.size();
    }

    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
    static bool ProcessToken(const typename CharType* processName, HANDLE& phExplorerToken)
    {
        phExplorerToken = 0;
        procList.clear();
        FindProcess<Str, CharType>(processName, [&](const PROCESSENTRY32& pro)
        {
            HANDLE hToken;
            HANDLE hProcess = NULL;
            if (OpenProcessToken(pro.th32ProcessID, TOKEN_DUPLICATE, &hToken))
            {
                HANDLE hNewToken = NULL;
                if(DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hNewToken))
                    phExplorerToken = hNewToken;

                CloseHandle(hToken);
            }

            CloseHandle(hProcess);

            return phExplorerToken != 0 ? false : true;
        });

        return phExplorerToken != 0;
    }

    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
    static const CharType* ProcessUser(DWORD processID, Str& userName, Str* domainName) // 进程ID 
    {
        userName.clear();
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
        if (hProcess == NULL)
            return NULL;

        HANDLE hToken = NULL;
        BOOL bResult = FALSE;
        DWORD dwSize = 0;

        SID_NAME_USE    SNU;
        PTOKEN_USER pTokenUser = NULL;
        __try
        {
            if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
            {
                bResult = FALSE;
                __leave;
            }

            if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize))
            {
                if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                {
                    bResult = FALSE;
                    __leave;
                }
            }

            pTokenUser = NULL;
            pTokenUser = (PTOKEN_USER)malloc(dwSize);
            if (pTokenUser == NULL)
            {
                bResult = FALSE;
                __leave;
            }

            if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize))
            {
                bResult = FALSE;
                __leave;
            }

            CharType szUserName[MAX_PATH] = { 0 };
            CharType szDomain[MAX_PATH] = { 0 };
            DWORD dwDomainSize = MAX_PATH;
            DWORD dwNameSize = MAX_PATH;
            
            BOOL success = FALSE;
            if (std::is_same<CharType, char>::value)
                success = LookupAccountSidA(NULL, pTokenUser->User.Sid, (LPSTR)szUserName, &dwNameSize, (LPSTR)szDomain, &dwDomainSize, &SNU);
            else
                success = LookupAccountSidW(NULL, pTokenUser->User.Sid, (LPWSTR)szUserName, &dwNameSize, (LPWSTR)szDomain, &dwDomainSize, &SNU);
            
            if (success != FALSE)
            {
                userName = szUserName;

                if (domainName)
                    domainName = szDomain;
            }
        }
        __finally
        {
        }
        
        if (pTokenUser != NULL)
            free(pTokenUser);

        return userName.empty() ? NULL : userName.c_str();
    }


    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
    static BOOL ExecWithToken(HANDLE hPtoken, const CharType* cmd, PROCESS_INFORMATION* ppi = nullptr)
    {
        typedef BOOL(WINAPI* API_CreateProcessWithTokenW)(
            __in         HANDLE hToken,
            __in         DWORD dwLogonFlags,
            __in_opt     LPCWSTR lpApplicationName,
            __inout_opt  LPWSTR lpCommandLine,
            __in         DWORD dwCreationFlags,
            __in_opt     LPVOID lpEnvironment,
            __in_opt     LPCWSTR lpCurrentDirectory,
            __in         LPSTARTUPINFOW lpStartupInfo,
            __out        LPPROCESS_INFORMATION lpProcessInfo
            );

        static API_CreateProcessWithTokenW f_CreateProcessWithTokenW = nullptr;
        if(!f_CreateProcessWithTokenW)
            f_CreateProcessWithTokenW = (API_CreateProcessWithTokenW)GetProcAddress(GetModuleHandleA("Advapi32"), "CreateProcessWithTokenW");

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::wstring wCmd;
        if (std::is_same<CharType, char>::value)
            Win32StringConvert::AnsiiToUnicode((const char*)cmd, wCmd);
        else
            wCmd = cmd;

        BOOL br = FALSE;
        DWORD lastError = 0;
        if (hPtoken && f_CreateProcessWithTokenW)
        {
            //Rely on Windows Secondary Logon service
            STARTUPINFOW si = { sizeof(STARTUPINFO), NULL, L"", NULL, 0, 0, 0, 0, 0, 0, 0, STARTF_USESHOWWINDOW, 0, 0, NULL, 0, 0, 0 };
            si.wShowWindow = SW_HIDE;
            si.lpDesktop = NULL;
            br = f_CreateProcessWithTokenW(hPtoken, LOGON_WITH_PROFILE, NULL, (LPWSTR)wCmd.c_str(), NULL, NULL, NULL, &si, &pi);
            lastError = GetLastError();
        }

        if (!br)
        {
            //After the startup fails, try the CreateProcess method again
            ZeroMemory(&pi, sizeof(pi));
            STARTUPINFOW si = { sizeof(STARTUPINFO), NULL, L"", NULL, 0, 0, 0, 0, 0, 0, 0, STARTF_USESHOWWINDOW, 0, 0, NULL, 0, 0, 0 };
            si.wShowWindow = SW_HIDE;
            si.lpDesktop = NULL;
            br = ::CreateProcessW(NULL, (LPWSTR)wCmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
            lastError = GetLastError();
        }

        if (br && ppi)
            *ppi = pi;

        return br;
    }

    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
    static void ExecWithoutAdmin(const CharType* cmd, PROCESS_INFORMATION* ppi)
    {
        HANDLE hPtoken = 0;
        if(std::is_same<CharType, char >::value)
            ProcessToken<std::string>("explorer.exe", hPtoken);
        else
            ProcessToken<std::wstring>(L"explorer.exe", hPtoken);

        if (!hPtoken)
            return;

        ExecWithToken<Str>(hPtoken, cmd, ppi);
    }

    template<
        typename Str
        , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = typename Str::allocator_type::value_type
#endif
    >
    static BOOL Exec(const typename CharType* app, const typename CharType* param = nullptr, PROCESS_INFORMATION* ppi = nullptr, bool show = false, DWORD waitTime = -1)
    {
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::wstring wApp;
        if (app)
        {
            if (std::is_same<CharType, char >::value)
                Win32StringConvert::AnsiiToUnicode((const char*)app, wApp);
            else
                wApp = (const wchar_t*)app;
        }

        std::wstring wParam;
        if (param)
        {
            if (std::is_same<CharType, char >::value)
                Win32StringConvert::AnsiiToUnicode((const char*)param, wParam);
            else
                wParam = (const wchar_t*)param;
        }

        BOOL br = FALSE;
        DWORD lastError = 0;

        STARTUPINFOW si;
        GetStartupInfoW(&si);
        si.lpReserved = NULL;
        si.lpDesktop = NULL;
        si.lpTitle = NULL;
        si.dwX = 0;
        si.dwY = 0;
        si.dwXSize = 0;
        si.dwYSize = 0;
        si.dwXCountChars = 9999;
        si.dwYCountChars = 9999;
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = show ? SW_SHOWNORMAL : SW_HIDE;
        si.cbReserved2 = 0;
        si.lpReserved2 = NULL;
        si.hStdInput = stdin;
        si.hStdOutput = stdout;
        si.hStdError = stderr;
        
        br = ::CreateProcessW(wApp.empty() ? NULL : (LPWSTR)wApp.c_str(), (LPWSTR)wParam.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
        lastError = GetLastError();

        if (br && ppi)
            *ppi = pi;

        if (waitTime > 0)
        {
            ::WaitForSingleObject(pi.hProcess, waitTime);		
            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);
        }

        return br;
    }

    static BOOL EnableDebugPrivilege()
    {
        HANDLE hToken;
        LUID sedebugnameValue;
        TOKEN_PRIVILEGES tkp;

        if (!OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {

            return FALSE;
        }

        if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
        {

            CloseHandle(hToken);
            return FALSE;
        }

        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = sedebugnameValue;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
        {

            return FALSE;
        }
        CloseHandle(hToken);
        return TRUE;
    }

    static int ImpUser(TCHAR cmdline[256])
    {
        PROCESSENTRY32 pc = { sizeof(PROCESSENTRY32) };
        HANDLE hExp = 0;
        if (!FindProcess<std::string>("explorer.exe", pc))
            return FALSE;

        hExp = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pc.th32ProcessID);
        if (hExp == NULL)
            return FALSE;

        HANDLE hToken;
        OpenProcessToken(hExp, TOKEN_ALL_ACCESS, &hToken);
        if (hToken == NULL)
            return FALSE;

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        si.lpDesktop = (LPWSTR)_T("winsta0\\default");
        si.wShowWindow = SW_SHOW;
        si.dwFlags = STARTF_USESHOWWINDOW;

        TCHAR   szParameter[256] = _T("/c ");
        lstrcat(szParameter, cmdline);
        //printf("szParameter=%s\n", szParameter);

        TCHAR path[MAX_PATH];
        GetSystemWindowsDirectory(path, MAX_PATH);
        _tcscat_s(path, MAX_PATH, _T("\\system32\\cmd.exe"));

        if (CreateProcessAsUser(hToken,
            (LPCTSTR)path,
            szParameter,
            NULL,
            NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi))   //以administrator用户身份执行程序,CREATE_NO_WINDOW,CREATE_NEW_CONSOLE,CREATE_DEFAULT_ERROR_MODE
        {
            printf("CreateProcessAsUser sucessed!%d\n", GetLastError());
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

        DWORD er = GetLastError();
        return 0;
    }

};

_END_MX_KIT_NAME_SPACE_