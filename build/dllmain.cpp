// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <vector>
#include "mxkit_library.h"
#include "base/auto_locker.h"
#include "base/time_utils.h"
#include "win32/file_utils.h"
#include "win32/win_path.h"

#include "spdlog/utils.h"

#include "MXInterprocessMessage.h"

HMODULE g_hModule;

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


//声明一个日志
namespace mxkit
{
    std::shared_ptr<spdlog::logger> static_spdlog = nullptr;
}

//导出接口对象接口定义
namespace mxkit
{
    static std::string DLL_VERSION = "202001061555";
    static std::recursive_mutex EXPORT_FUNCTION_MUTEX;

    static mxkit::ExportInfo DLL_EXPORT_INFO;
    static std::vector<mxkit::InterfaceInfo> EXPORT_INTERFACE_LIST;

    _MX_C_EXPORT mxkit::Result InitLibrary()
    {
        mxkit::AutoLocker aLock(EXPORT_FUNCTION_MUTEX);

        if (DLL_EXPORT_INFO.version && DLL_EXPORT_INFO.interfaceInfo)
            RETURN_RESULT(true);

        std::string fileDir(mxkit::Win32Path<std::string>::ModuleDirectory(g_hModule));
        fileDir += mxkit::TimeUtils::ToString<std::string>("\\log\\%Y-%m-%d\\");
        mxkit::FileSystem::CreateFolder<std::string>(fileDir);
        
        _MX_LOG_INIT_LOG(fileDir, "MXToolkit");

        MXInterprocessMessage::Instance()->InitInterface(DLL_VERSION.c_str());
        EXPORT_INTERFACE_LIST.emplace_back(MXInterprocessMessage::Instance()->Interface());

        DLL_EXPORT_INFO.interfaceCount = EXPORT_INTERFACE_LIST.size();
        DLL_EXPORT_INFO.version = DLL_VERSION.c_str();//当前时间戳
        DLL_EXPORT_INFO.interfaceInfo = &EXPORT_INTERFACE_LIST[0];

        RETURN_RESULT(true);
    }

    _MX_C_EXPORT mxkit::Result UninitLibrary()
    {
        mxkit::AutoLocker aLock(EXPORT_FUNCTION_MUTEX);

        //MX_RELEASE_LOG();
        RETURN_RESULT(true);
    }

    _MX_C_EXPORT mxkit::Result QueryExport(mxkit::ExportInfo **exp)
    {
        mxkit::AutoLocker aLock(EXPORT_FUNCTION_MUTEX);

        if (exp)
        {
            *exp = &DLL_EXPORT_INFO;
            RETURN_RESULT(true);
        }

        RETURN_RESULT(false);
    }

    _MX_C_EXPORT mxkit::Result QueryInterface(const mxkit::InterfaceInfo* info, void** it)
    {
        mxkit::AutoLocker aLock(EXPORT_FUNCTION_MUTEX);

        if (!info || !info->name || !info->version || !it)
            RETURN_RESULT(false);

        for (auto item : EXPORT_INTERFACE_LIST)
        {
            if (strcmp(item.name, info->name) == 0 && strcmp(item.version, info->version) == 0)
            {
                *it = (void*)dynamic_cast<IMXInterprocessMessage*>(MXInterprocessMessage::Instance());
                RETURN_RESULT(true);
            }
        }

        RETURN_RESULT(false);
    }
}