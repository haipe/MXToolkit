#pragma once
#include <string>
#include <strstream>
#include <iostream>
#include <windows.h>

#include "mxkit.h"

#include "win32/system_utils.h"

_BEGIN_MX_KIT_NAME_SPACE_


class Win32Tasks
{
public:
    bool Execute(const std::string& cmd)
    {
        std::cout << "Win32Tasks cmd:" << cmd.c_str() << std::endl;

        SHELLEXECUTEINFOA shExecInfo = { 0 };
        shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
        shExecInfo.fMask = SEE_MASK_DEFAULT;
        shExecInfo.hwnd = NULL;
        shExecInfo.lpVerb = "runas";
        shExecInfo.lpFile = "schtasks.exe";
        //ShExecInfo.lpFile = "C:\\Windows\\SysWOW64\\schtasks.exe"; //can be a file as well

        shExecInfo.lpParameters = cmd.c_str();
        shExecInfo.lpDirectory = NULL;
        shExecInfo.nShow = SW_HIDE;
        shExecInfo.hInstApp = NULL;
        BOOL b = ShellExecuteExA(&shExecInfo);

        int i = GetLastError();

        return b ? true : false;
    }
    
    bool Create(const std::string& taskName, const std::string& cmd, const char* user, bool force = false, bool autuDel = false)
    {
        static std::string userName;
        std::strstream ss;
        ss
            << "/create "
            << "/tn \"" << taskName.c_str() << "\" "
            << "/ru \"" << (user ? user : Win32System::GetLoginUseName<std::string>(userName)) << "\" "
            //<< "/ru \"" << GetLoginUser().c_str() << "\" "
            //<< "/ru \"KSVDI-VDI-000C\\Administrator\" "
            << "/sc ONLOGON "
            << "/tr \"" << cmd.c_str() << "\" "
            << "/rl HIGHEST" << " "
            << (force ? "/f " : "")
            //<< (autuDel ? "/z " : "")
            << '\0';

        return Execute(ss.str());
    }

    bool Run(const std::string& taskName)
    {
        std::strstream ss;
        ss
            << "/run " << "/i /tn \"" << taskName.c_str() << "\" "
            << '\0';

        return Execute(ss.str());
    }

    bool Delete(const std::string& taskName)
    {
        std::strstream ss;
        ss
            << "/delete " << "/tn \"" << taskName.c_str() << "\" /f "
            << '\0';

        return Execute(ss.str());
    }

};


_END_MX_KIT_NAME_SPACE_