#pragma once
#include <list>
#include "win32/cmd_utils.h"


_BEGIN_MX_KIT_NAME_SPACE_

class Thread;

class PowerShellUtils
{
public:
    PowerShellUtils();
    ~PowerShellUtils();

    struct FileInfo
    {
        bool appx;
        std::string path;

        struct Feature
        {
            std::string type;
            std::string source;
            std::string hash;
        };

        Feature feature;

        struct Product
        {
            std::string publisher;
            std::string product;
            std::string name;
            std::string version;
        };

        Product info;
    };

    bool IsCompleted(){ return m_completed; }
    bool Result(){ return m_result; }

    std::string GetFileInfoCmd(bool ps, const std::string& file);
    bool GetFileInfo(const std::string& file, FileInfo* info);
    bool GetFileInfoImplement(const std::string& file, FileInfo* info);

    //需要管理员权限
    std::string SetAppLockerPolicyCmd(bool ps, const std::list<std::string>& file, bool merge = true);
    bool SetAppLockerPolicy(const std::string& file, bool merge = true);
    bool SetAppLockerPolicyImplement(const std::string& file, bool merge = true);

    bool RunCmd(const std::string& cmd, std::string* result);

protected:
    bool                            m_completed;
    bool                            m_result;
    Thread*                         m_thread;
    CmdWithAnonymousPip::CmdProcess m_process;
};

_END_MX_KIT_NAME_SPACE_