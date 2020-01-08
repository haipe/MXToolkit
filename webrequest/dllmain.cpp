// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "MXDllExportDefine.h"
#include "WebRequestImp.h"
#include "MXLock.h"

#include "Win32PathUtil.h"

#include "MXSpdlog.h"

#include <curl.h>
#pragma comment(lib,"libcurl.lib")

HMODULE g_hModule;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//声明一个日志
namespace mxtoolkit
{
    std::shared_ptr<spdlog::logger> static_spdlog = nullptr;
}

//导出接口对象接口定义
namespace mxwebrequest
{
    static std::string DLL_VERSION = "202001061555";
    static std::recursive_mutex EXPORT_FUNCTION_MUTEX;

    static mxtoolkit::MXDllExportInfo DLL_EXPORT_INFO;
    static std::vector<mxtoolkit::MXInterfaceInfo> EXPORT_INTERFACE_LIST;

    MX_C_EXPORT mxtoolkit::Result mxDllInit()
    {
        mxtoolkit::MXAutoLock aLock(EXPORT_FUNCTION_MUTEX);

        if (DLL_EXPORT_INFO.version && DLL_EXPORT_INFO.interfaceInfo)
            RETURN_RESULT(true);

        std::string fileDir(mxtoolkit::Win32App<std::string>::GetModuleDirectory(g_hModule));
        fileDir += mxtoolkit::MXTimeDate::ToString<std::string>("\\log\\%Y-%m-%d\\");
        mxtoolkit::Win32App<std::string>::CreateDirectory(fileDir);


        MX_INIT_LOG(fileDir, "MXWebRequest");

        mxtoolkit::MXInterfaceInfo info;
        WebRequestImp::GetInstance()->GetExportInterfaceInfo(&info);
        EXPORT_INTERFACE_LIST.emplace_back(info);

        DLL_EXPORT_INFO.interfaceCount = EXPORT_INTERFACE_LIST.size();
        DLL_EXPORT_INFO.version = DLL_VERSION.c_str();//当前时间戳
        DLL_EXPORT_INFO.interfaceInfo = &EXPORT_INTERFACE_LIST[0];

        RETURN_RESULT(true);
    }

    MX_C_EXPORT mxtoolkit::Result mxDllUninit()
    {
        mxtoolkit::MXAutoLock aLock(EXPORT_FUNCTION_MUTEX);

        WebRequestImp::GetInstance()->Uninstall();
        WebRequestImp::DestroyInstance();

        MX_RELEASE_LOG();
        RETURN_RESULT(true);
    }

    MX_C_EXPORT mxtoolkit::Result mxGetExportInfo(mxtoolkit::MXDllExportInfo **exp)
    {
        mxtoolkit::MXAutoLock aLock(EXPORT_FUNCTION_MUTEX);

        if (exp)
        {
            *exp = &DLL_EXPORT_INFO;
            RETURN_RESULT(true);
        }

        RETURN_RESULT(false);
    }

    MX_C_EXPORT mxtoolkit::Result mxGetInterfaceInfo(const mxtoolkit::MXInterfaceInfo* info, void** it)
    {
        mxtoolkit::MXAutoLock aLock(EXPORT_FUNCTION_MUTEX);

        if (!info || !info->name || !info->version || !it)
            RETURN_RESULT(false);

        for (auto item : EXPORT_INTERFACE_LIST)
        {
            if (strcmp(item.name, info->name) == 0 && strcmp(item.version, info->version) == 0)
            {
                *it = (void*)dynamic_cast<IWebRequest*>(WebRequestImp::GetInstance());
                RETURN_RESULT(true);
            }
        }

        RETURN_RESULT(false);
    }
}