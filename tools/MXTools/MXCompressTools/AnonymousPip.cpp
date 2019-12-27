﻿#include "AnonymousPip.h"
#include <windows.h>


AnonymousPip::AnonymousPip()
{
}


AnonymousPip::~AnonymousPip()
{
}

bool AnonymousPip::CreateCmd(const std::string& cmd, std::function<void(const std::string& result)> onResult)
{
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL; //使用系统默认的安全描述符 
    sa.bInheritHandle = TRUE; //创建的进程继承句柄

    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) //创建匿名管道
    {
        return false;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfoA(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite; //新创建进程的标准输出连在写管道一端
    si.wShowWindow = SW_HIDE; //隐藏窗口 
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    if (!CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) //创建子进程
    {
        return false;
    }

    CloseHandle(hWrite); //关闭管道句柄

    static const int maxBufferSize = 4096;
    char buffer[maxBufferSize] = { 0 };
    DWORD bytesRead;
    while (true)
    {
        if (ReadFile(hRead, buffer, maxBufferSize, &bytesRead, NULL) == NULL) //读取管道
            break;

        std::string result = buffer;
        onResult(result);
    }

    CloseHandle(hRead);
    return true;
}

