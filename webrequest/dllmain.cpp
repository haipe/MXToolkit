// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "MXDllExportDefine.h"
#include "WebRequestImp.h"
#include "MXLock.h"

#include <curl.h>

#pragma comment(lib,"libcurl.lib")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


//导出接口对象接口定义
namespace mxwebrequest
{
    static std::string dll_version = "202001061555";
    static std::recursive_mutex export_function_mutex;

    static mxtoolkit::mx_dll_export_info dll_export_info;
    static std::vector<mxtoolkit::mx_export_interface_info> export_interface_list;

    MX_C_EXPORT mxtoolkit::mx_call_result mx_dll_init()
    {
        mxtoolkit::MXAutoLock aLock(export_function_mutex);

        mxtoolkit::mx_export_interface_info info;
        WebRequestImp::GetInstance()->GetExportInterfaceInfo(&info);
        export_interface_list.push_back(info);

        dll_export_info.interface_count = export_interface_list.size();
        dll_export_info.version = dll_version.c_str();//当前时间戳
        dll_export_info.interface_info = &export_interface_list[0];

        return mxtoolkit::mx_call_successed;
    }

    MX_C_EXPORT void mx_dll_uninit()
    {
        mxtoolkit::MXAutoLock aLock(export_function_mutex);

        WebRequestImp::GetInstance()->Uninstall();
    }

    MX_C_EXPORT unsigned int mx_dll_all_export(mxtoolkit::mx_dll_export_info **exp)
    {
        mxtoolkit::MXAutoLock aLock(export_function_mutex);

        if (exp)
        {
            *exp = &dll_export_info;
            return mxtoolkit::mx_call_successed;
        }

        return mxtoolkit::mx_call_fail;
    }

    MX_C_EXPORT unsigned int mx_dll_get_interface(const mxtoolkit::mx_export_interface_info* info, void** it)
    {
        mxtoolkit::MXAutoLock aLock(export_function_mutex);

        if (!info || !info->name || !info->version || !it)
            return mxtoolkit::mx_call_fail;


        for (auto item : export_interface_list)
        {
            if (strcmp(item.name, info->name) == 0 && strcmp(item.version, info->version) == 0)
            {
                *it = (void*)dynamic_cast<IWebRequest*>(WebRequestImp::GetInstance());
                return mxtoolkit::mx_call_successed;
            }
        }

        return mxtoolkit::mx_call_fail;
    }
}