#pragma once

#include "mxkit.h"

#if _MX_CPP_VER_ >= 11
#include <functional>
#include <algorithm>
#endif

#include <list>
#include <vector>
#include <iostream>
#include <string>

#include <windows.h>


_BEGIN_MX_KIT_NAME_SPACE_

struct ICmdNotify
{
    virtual void OnOutput(const std::string& output, bool& willBreak) {}

    virtual void OnAllOutput(const std::string& result){}
};

class CmdWithAnonymousPip
{
public:
    struct CmdProcess
    {
        DWORD  dwProcessID;
        HANDLE hProcess;
        HANDLE hRead;
        HANDLE hWrite;
    };

    static bool IsInvalid(const CmdProcess& process)
    {
        return process.hProcess && process.hRead && process.hWrite;
    }

    static bool WriteToCmdProcess(std::string cmd, const CmdProcess& process)
    {
        if (!IsInvalid(process))
            return false;

        DWORD writeLen = 0;
        BOOL r = WriteFile(process.hWrite, cmd.c_str(), (DWORD)cmd.size(), &writeLen, NULL);

        return r ? true : false;
    }

#if _MX_CPP_VER_ >= 11
    static bool ReadFromCmdProcess(const CmdProcess& process, std::function<void(const std::string& result, bool& willBreak)> onResult)
#else
    static bool ReadFromCmdProcess(const CmdProcess& process, ICmdNotify* onResult)
#endif
    {
        if (!process.hProcess || !process.hRead)
            return false;

        static const int maxBufferSize = 4096;
        char buffer[maxBufferSize] = { 0 };
        DWORD bytesRead = 0;

        while (true)
        {
            if (ReadFile(process.hRead, buffer, maxBufferSize, &bytesRead, NULL) == NULL) //读取管道
                break;

            buffer[bytesRead] = '\0';

#ifdef _DEBUG
            std::cout << buffer;
#endif

            std::string result = buffer;
            bool willBreak = false;
#if _MX_CPP_VER_ >= 11
            onResult(result, willBreak);
#else
            onResult->OnOutput(result, willBreak);
#endif
            if (willBreak)
                break;
        }

        return true;
    }

    static bool ReadFromCmdProcess(const CmdProcess& process, std::string& result)
    {
        result.clear();

#if _MX_CPP_VER_ >= 11
        return ReadFromCmdProcess(process, [&result](const std::string& output, bool& willBreak) {
            result += output;
        });
#else
        struct OutPutProcess : public ICmdNotify
        {
            OutPutProcess(std::string& r) :result(r){}

            virtual void OnOutput(const std::string& output, bool& willBreak)
            {
                result += output;
            }

            std::string& result;
        };

        OutPutProcess output(result);
        return ReadFromCmdProcess(process, &output);
#endif
    }

    static void CloseCmdWrite(CmdProcess& process)
    {
        if (process.hWrite)
        {
            CloseHandle(process.hWrite);
            process.hWrite = 0;
        }
    }

    static void CloseCmdProcess(CmdProcess& process)
    {
        if (process.hProcess)
            ::TerminateProcess(process.hProcess, 0);

        if (process.hWrite)
            CloseHandle(process.hWrite);
        if (process.hRead)
            CloseHandle(process.hRead);
        if (process.hProcess)
            CloseHandle(process.hProcess);
        
        process.dwProcessID = 0;
        process.hWrite = 0;
        process.hRead = 0;
        process.hProcess = 0;
    }

#if _MX_CPP_VER_ >= 11
    //创建匿名管道，执行xmd，并返回结果
    static bool CreateCmd(const std::string& cmd, bool auto_close, CmdProcess* out, std::function<void(const std::string& result)> onResult)
#else
    static bool CreateCmd(const std::string& cmd, bool auto_close, CmdProcess* out, ICmdNotify* onResult)
#endif
    {
        SECURITY_ATTRIBUTES sa;
        CmdProcess process;

        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL; //使用系统默认的安全描述符 
        sa.bInheritHandle = TRUE; //创建的进程继承句柄

        if (!CreatePipe(&process.hRead, &process.hWrite, &sa, 0)) //创建匿名管道
        {
            return false;
        }

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        GetStartupInfoA(&si);
        si.hStdError = process.hWrite;
        si.hStdOutput = process.hWrite; //新创建进程的标准输出连在写管道一端
        si.wShowWindow = SW_HIDE; //隐藏窗口 
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        
        if (!CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) //创建子进程
        {
            DWORD e = GetLastError();
            return false;
        }

        process.hProcess = pi.hProcess;
        process.dwProcessID = pi.dwProcessId;

        if (auto_close || !out)
        {
            CloseCmdWrite(process);

            //如果自动关闭，则直接返回结果
            static const int maxBufferSize = 4096;
            char buffer[maxBufferSize] = { 0 };
            DWORD bytesRead;
            while (true)
            {
                if (ReadFile(process.hRead, buffer, maxBufferSize, &bytesRead, NULL) == NULL) //读取管道
                    break;

                buffer[bytesRead] = '\0';

#ifdef _DEBUG
                std::cout << buffer;
#endif
                std::string result = buffer;
#if _MX_CPP_VER_ >= 11
                onResult(result);
#else
                onResult->OnAllOutput(result);
#endif
            }

            CloseCmdProcess(process);
        }
        else if (out)
        {
            //如果不自动关闭，则需要自己处理返回结果。
            *out = process;
        }

        return true;
    }

    static bool CreateCmd(const std::string& cmd, bool auto_close, CmdProcess* out, std::string& result)
    {
        result.clear();

#if _MX_CPP_VER_ >= 11
        return CreateCmd(cmd, auto_close, out, [&result](const std::string& res) {
            result += res;
        });

#else
        struct OutPutProcess : public ICmdNotify
        {
            OutPutProcess(std::string& r) :result(r){}

            virtual void OnAllOutput(const std::string& output)
            {
                result += output;
            }

            std::string& result;
        };

        OutPutProcess output(result);
        return CreateCmd(cmd, auto_close, out, &output);

#endif
    }


};

class RunCommond
{
public:
    static bool Run(const std::string& cmd, std::string* result = nullptr, bool* completed = nullptr)
    {
        if (completed)
            *completed = false;

        do
        {
            CmdWithAnonymousPip::CmdProcess process;
            if (!CmdWithAnonymousPip::CreateCmd(cmd, false, &process, nullptr))
                break;

            //关闭
            CmdWithAnonymousPip::CloseCmdWrite(process);

            std::string r;
            CmdWithAnonymousPip::ReadFromCmdProcess(process, r);
            if (result)
                *result = r;

            CmdWithAnonymousPip::CloseCmdProcess(process);

            if (completed)
                *completed = true;

            return true;
        } while (0);

        return false;
    }

};

_END_MX_KIT_NAME_SPACE_