#include "process_utils.h"
#include <Windows.h>

_BEGIN_MX_KIT_NAME_SPACE_


int CreateProcess(const std::string& filePath, const std::string& runParam /*= ""*/, unsigned int waitTimeout /*= -1*/)
{
    DWORD exitCode = 0;
    PROCESS_INFORMATION pInfo = { 0 };
    STARTUPINFOA        sInfo = { 0 };
    sInfo.cb = sizeof(STARTUPINFO);
    sInfo.wShowWindow = SW_SHOW;

    std::string cmd = filePath + " ";
    cmd += runParam;

    if (CreateProcessA(
        NULL,      //LPCTSTR lpApplicationName, // pointer to name of executable module
        (LPSTR)cmd.c_str(),   //LPTSTR lpCommandLine,  // pointer to command line string
        NULL,      //LPSECURITY_ATTRIBUTES lpProcessAttributes,  // process security attributes
        NULL,      //LPSECURITY_ATTRIBUTES lpThreadAttributes,   // thread security attributes
        FALSE,     //BOOL bInheritHandles,  // handle inheritance flag
        0,         //DWORD dwCreationFlags, // creation flags
        NULL,      //LPVOID lpEnvironment,  // pointer to new environment block
        NULL,      //LPCTSTR lpCurrentDirectory,   // pointer to current directory name
        &sInfo,    //LPSTARTUPINFO lpStartupInfo,  // pointer to STARTUPINFO
        &pInfo))    //LPPROCESS_INFORMATION lpProcessInformation  // pointer to PROCESS_INFORMATION
    {
        if (waitTimeout != 0)
        {
            // Wait until child process exits.
            WaitForSingleObject(pInfo.hProcess, waitTimeout);

            if (GetExitCodeProcess(pInfo.hProcess, &exitCode))
            {
                printf("Exit code = %d\n", exitCode);
            }
            else
            {
                printf("GetExitCodeProcess() failed: %ld\n", GetLastError());
            }
        }

        // Close process and thread handles. 
        CloseHandle(pInfo.hProcess);
        CloseHandle(pInfo.hThread);
    }
    else
    {
        printf("CreateProcess() failed: %ld\n", GetLastError());
    }

    return exitCode;
}

_END_MX_KIT_NAME_SPACE_