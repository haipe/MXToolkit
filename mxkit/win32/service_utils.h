#pragma once
#include <string>
#include <map>

#include <xstring>
#include <vector>

#include <windows.h>

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_


class ServiceUtils
{
public:
    static bool Start(const std::string& name);
    static bool Stop(const std::string& name);

    static bool Config(const std::string& name, const std::string& cfg);

    typedef std::map<std::string, std::string> StatusMap;
    static bool Status(const std::string& name, StatusMap& status);
};

#if _MX_VS_VER_ > 2010

template<class T_CHAR>
class ServiceManager
{
public:
    typedef const T_CHAR*   TCAHR_PTR;
    typedef std::basic_string<T_CHAR, std::char_traits<T_CHAR>, std::allocator<T_CHAR>> String;

    struct Info
    {
        DWORD status;//服务状态
        DWORD startType;//启动类型

        String name;//服务名称
        String displayName;//显示名称
        String binPath;//所在路径
        String desc;//描述信息
        String dependService;//依赖关系
    };

    struct SCHandle
    {
        SCHandle(SC_HANDLE h = 0):h(h){}
        ~SCHandle()
        {
            if (h)
            {
                CloseServiceHandle(h);
                h = 0;
            }
        }

        operator bool() { return h != 0; }
        operator SC_HANDLE() { return h; }

        SC_HANDLE h;
    };

    ServiceManager() {}

    ~ServiceManager() {}

    bool Init()
    {
        if (sc == 0)
        {
            sc = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
            DWORD e = GetLastError();
            return sc ? true : false;
        }

        return true;
    }

    bool Open(const String& name, SCHandle& hSCService)
    {
        if (!Init())
            return false;

        if (std::is_same<T_CHAR, char>::value)
            hSCService = OpenServiceA(sc.h, (LPCSTR)name.c_str(), SERVICE_ALL_ACCESS);
        else
            hSCService = OpenServiceW(sc.h, (LPCWSTR)name.c_str(), SERVICE_ALL_ACCESS);

        DWORD e = GetLastError(); 

        //            LPTSTR lpBuffer = NULL;
        //             FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, //标志位，决定如何说明lpSource参数，dwFlags的低位指定如何处理换行功能在输出缓冲区，也决定最大宽度的格式化输出行,可选参数。
        //                 NULL,//根据dwFlags标志而定。
        //                 e,//请求的消息的标识符。当dwFlags标志为FORMAT_MESSAGE_FROM_STRING时会被忽略。
        //                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//请求的消息的语言标识符。
        //                 (LPTSTR)&lpBuffer,//接收错误信息描述的缓冲区指针。
        //                 0,//如果FORMAT_MESSAGE_ALLOCATE_BUFFER标志没有被指定，这个参数必须指定为输出缓冲区的大小，如果指定值为0，这个参数指定为分配给输出缓冲区的最小数。
        //                 NULL//保存格式化信息中的插入值的一个数组。
        //                 
        return hSCService;
    }

    bool Install(const String& name, const String& displayName, DWORD startType, const String& binPath)
    {
        if (!Init())
            return false;

        SCHandle hSCRet;

        if (std::is_same<T_CHAR, char>::value)
            hSCRet = ::CreateServiceA(sc, (LPCSTR)name.c_str(), (LPCSTR)displayName.c_str(), SC_MANAGER_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, startType, 0, (LPCSTR)binPath.c_str(), NULL, NULL, NULL, NULL, NULL);
        else
            hSCRet = ::CreateServiceW(sc, (LPCWSTR)name.c_str(), (LPCWSTR)displayName.c_str(), SC_MANAGER_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, startType, 0, (LPCWSTR)binPath.c_str(), NULL, NULL, NULL, NULL, NULL);

        return hSCRet;
    }

    bool ServiceStatus(const String &name, DWORD &status)
    {
        SCHandle hSCService;
        if (!Open(name, hSCService))
            return false;

        SERVICE_STATUS ss = { 0 };
        if (!QueryServiceStatus(hSCService, &ss))
        {
            return FALSE;
        }

        status = ss.dwCurrentState;
        return TRUE;
    }

    bool ServiceStartType(const String &name, DWORD &startType)
    {
        SCHandle hSCService;
        if (!Open(name, hSCService))
            return false;

        std::vector<unsigned wchar_t> buffer;
        LPQUERY_SERVICE_CONFIGW lpServiceInfo = NULL;
        DWORD dwBufferSize = 0;
        if (!QueryServiceConfigW(hSCService, lpServiceInfo, dwBufferSize, &dwBufferSize))
        {
            if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
            {
                buffer.resize(dwBufferSize);
                lpServiceInfo = (LPQUERY_SERVICE_CONFIGW)&buffer[0];
                if (!QueryServiceConfigW(hSCService, lpServiceInfo, dwBufferSize, &dwBufferSize))
                    return false;
            }
        }

        if (!lpServiceInfo)
            return false;

        startType = lpServiceInfo->dwStartType;

        return true;
    }

    bool ControlService(const String& name, DWORD dwNewStatus)
    {
        SCHandle hSCService;
        if (!Open(name, hSCService))
            return false;

        SERVICE_STATUS sts = { 0 };
        if (!QueryServiceStatus(hSCService, &sts))
            return false;

        if (sts.dwCurrentState == dwNewStatus)//当前状态与修改状态一致
        {
            return TRUE;
        }
        else if (SERVICE_START_PENDING == sts.dwCurrentState ||
            SERVICE_STOP_PENDING == sts.dwCurrentState ||
            SERVICE_CONTINUE_PENDING == sts.dwCurrentState ||
            SERVICE_PAUSE_PENDING == sts.dwCurrentState)
        {
            return FALSE;
        }
        else if (SERVICE_STOPPED == sts.dwCurrentState && SERVICE_START == dwNewStatus)//当前状态为停止，需要启动服务
        {
            return ::StartService(hSCService, NULL, NULL) ? true : false;
        }
        else if ((SERVICE_RUNNING == sts.dwCurrentState || SERVICE_PAUSED == sts.dwCurrentState) && SERVICE_STOP == dwNewStatus)//当前状态为已启动或者暂停，需要停止服务
        {
            return ::ControlService(hSCService, SERVICE_CONTROL_STOP, &sts) ? true : false;
        }
        else if (SERVICE_PAUSED == sts.dwCurrentState && SERVICE_START == dwNewStatus)//当前状态为暂停，需要继续运行服务
        {
            return ::ControlService(hSCService, SERVICE_CONTROL_CONTINUE, &sts) ? true : false;
        }
        else if (SERVICE_RUNNING == sts.dwCurrentState && SERVICE_PAUSED == dwNewStatus)//当前状态为已启动，需要修改为暂停
        {
            return ::ControlService(hSCService, SERVICE_CONTROL_PAUSE, &sts) ? true : false;
        }

        return FALSE;
    }

    bool ChangeServiceStartType(const String& name, DWORD dwNewStartType)
    {
        SCHandle hSCService;
        if (!Open(name, hSCService))
            return false;

        return ChangeServiceConfig(hSCService, SERVICE_NO_CHANGE, dwNewStartType, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL) ? true : false;
    }

    bool DeleteService(const String &name)
    {
        DWORD dwCurrentStatus = 0;
        if (!ServiceStatus(name, dwCurrentStatus))
            return FALSE;

        if (SERVICE_STOP != dwCurrentStatus) //如果服务不为停止状态，则停止服务
            ControlService(name, SERVICE_STOP);

        SCHandle hSCService;
        if (!Open(name, hSCService))
            return false;

        return ::DeleteService(hSCService) ? true : false;
    }

protected:
    SCHandle sc = 0;
};



template<>
ServiceManager<char>::ServiceManager()
{
}


template<>
ServiceManager<wchar_t>::ServiceManager()
{
}

#endif

_END_MX_KIT_NAME_SPACE_