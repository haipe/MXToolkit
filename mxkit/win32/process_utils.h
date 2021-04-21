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
    static PROCESSENTRY32* GetProcessEntry(LPCTSTR szExeName, PROCESSENTRY32* pProc)
    {
        if (!pProc)
            return NULL;

        PROCESSENTRY32 pc = { sizeof(PROCESSENTRY32) };
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
        if (Process32First(hSnapshot, &pc))
        {
            do{

                if (!_tcsicmp(pc.szExeFile, szExeName))
                {
                    *pProc = pc;
                    return pProc;
                    //返回explorer.exe进程的PID
                    //printf("explorer's PID=%d\n", Pc.th32ProcessID);
                    //return OpenProcess(PROCESS_ALL_ACCESS, TRUE, pc.th32ProcessID);
                }

            } while (Process32Next(hSnapshot, &pc));
        }

        return NULL;
    }

    static const TCHAR* GetProcessUserName(DWORD dwID, TCHAR* nameBuffer, DWORD nBufferSize, TCHAR* domainBuffer = NULL, DWORD dBufferSize = 0) // 进程ID 
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwID);
        if (hProcess == NULL)
            return NULL;

        HANDLE hToken = NULL;
        BOOL bResult = FALSE;
        DWORD dwSize = 0;

        TCHAR szUserName[MAX_PATH] = { 0 };
        TCHAR szDomain[MAX_PATH] = { 0 };
        DWORD dwDomainSize = MAX_PATH;
        DWORD dwNameSize = MAX_PATH;

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

            if (LookupAccountSid(NULL, pTokenUser->User.Sid, szUserName, &dwNameSize, szDomain, &dwDomainSize, &SNU) != 0)
            {
                if (nameBuffer)
                    _tcscpy_s(nameBuffer, nBufferSize, szUserName);

                if (domainBuffer)
                    _tcscpy_s(domainBuffer, dBufferSize, szDomain);

                return nameBuffer;
            }
        }
        __finally
        {
            if (pTokenUser != NULL)
                free(pTokenUser);
        }

        return NULL;
    }

    static DWORD GetProcessToken(LPCWSTR processName, OUT PHANDLE phExplorerToken)
    {
        DWORD dwStatus = ERROR_FILE_NOT_FOUND;
        BOOL bRet = FALSE;
        HANDLE hProcess = NULL;
        HANDLE hProcessSnap = NULL;
        TCHAR szExplorerPath[MAX_PATH] = { 0 };
        TCHAR FileName[MAX_PATH] = { 0 };
        PROCESSENTRY32 pe32 = { 0 };

        __try
        {
            hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hProcessSnap == INVALID_HANDLE_VALUE)
            {
                dwStatus = GetLastError();
                __leave;
            }
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (!Process32First(hProcessSnap, &pe32))
            {
                dwStatus = GetLastError();
                __leave;
            }

            do {
                if (_wcsicmp(pe32.szExeFile, processName) == 0)
                {
                    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
                    if (NULL != hProcess)
                    {
                        HANDLE hToken;
                        if (OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken))
                        {
                            HANDLE hNewToken = NULL;
                            DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hNewToken);
                            *phExplorerToken = hNewToken;
                            dwStatus = 0;
                            CloseHandle(hToken);
                        }

                        CloseHandle(hProcess);
                        hProcess = NULL;

                        break;
                    }
                }

            } while (Process32Next(hProcessSnap, &pe32));
        }
        __finally
        {
            if (NULL != hProcess)
            {
                CloseHandle(hProcess);
            }
            if (NULL != hProcessSnap)
            {
                CloseHandle(hProcessSnap);
            }
        }

        return dwStatus;
    }

    static void ExecWithoutAdmin(const char* cmd, PROCESS_INFORMATION* ppi)
    {
        HANDLE hPtoken = 0;
        GetProcessToken(L"explorer.exe", &hPtoken);
        if (hPtoken)
            CreateProcessWithToken(hPtoken, cmd, ppi);
    }

    static BOOL CreateProcessWithToken(HANDLE hPtoken, const char* cmd, PROCESS_INFORMATION* ppi)
    {
        typedef BOOL(WINAPI* WIN32_API_CreateProcessWithTokenW)(
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

        static WIN32_API_CreateProcessWithTokenW f_CreateProcessWithTokenW =
            (WIN32_API_CreateProcessWithTokenW)GetProcAddress(GetModuleHandleA("Advapi32"), "CreateProcessWithTokenW");

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::wstring wCmd;
        Win32StringConvert::AnsiiToUnicode(cmd, wCmd);

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

    static BOOL CreateWin32Process(const char* app, const char* param, PROCESS_INFORMATION* ppi =  NULL, bool show = false, DWORD waitTime = -1)
    {
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::wstring wApp;
        Win32StringConvert::AnsiiToUnicode(app, wApp);

        std::wstring wParam;
        Win32StringConvert::AnsiiToUnicode(param, wParam);

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
        //说明进程将以隐藏的方式在后台执行    
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
        if (!GetProcessEntry(_T("explorer.exe"), &pc))
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