#pragma once
/*
*
*
*/
#if defined(WIN32)
#include <windows.h>
#include <WinBase.h>

#endif

#include "MXCommon.h"

//////////////////////////////////////////////////////////////////////////
#define MX_CALL_TYPE __cdecl

#define MX_DLL_FUNCTION_TYPE(func) _##func##_Type_

#define MX_C_EXPORT extern "C" _declspec(dllexport)

#if defined(__cplusplus)
namespace mxtoolkit {

    extern "C" {
#endif
        

struct MXInterfaceInfo
{
    const char* name = nullptr;     //
    const char* version = nullptr;  //版本
};

struct MXDllExportInfo
{
	const char* version = nullptr;
	unsigned int interfaceCount = 0;
	const MXInterfaceInfo* interfaceInfo = nullptr;
};


//初始化
typedef Result(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mxDllInit))();
//卸载
typedef Result(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mxDllUninit))();
//获取所有接口信息
typedef Result(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mxGetExportInfo))(MXDllExportInfo **);
//获取接口
typedef Result(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mxGetInterfaceInfo))(const MXInterfaceInfo*, void**);

struct MXDllObject
{
    MX_DLL_FUNCTION_TYPE(mxDllInit) dllInit = nullptr;
    MX_DLL_FUNCTION_TYPE(mxDllUninit) dllUninit = nullptr;
    MX_DLL_FUNCTION_TYPE(mxGetExportInfo) getExportInfo = nullptr;
    MX_DLL_FUNCTION_TYPE(mxGetInterfaceInfo) getInterfaceInfo = nullptr;
};


#define LOAD_MX_DLLOBJECT(mxDllObj,dllModule)                                                                                               \
do                                                                                                                                          \
{                                                                                                                                           \
    if (!dllModule)break;                                                                                                                   \
    mxDllObj.dllInit = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mxDllInit))GetProcAddress(dllModule, "mxDllInit");                                    \
    mxDllObj.dllUninit = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mxDllUninit))GetProcAddress(dllModule, "mxDllUninit");                              \
    mxDllObj.getExportInfo = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mxGetExportInfo))GetProcAddress(dllModule, "mxGetExportInfo");                  \
    mxDllObj.getInterfaceInfo = (mxtoolkit::MX_DLL_FUNCTION_TYPE(mxGetInterfaceInfo))GetProcAddress(dllModule, "mxGetInterfaceInfo");         \
} while (0);

#if defined(__cplusplus)
    } //extern "C" 
    
    template<typename IImp>
    class MXInterfaceImp
    {
    public:
        void InitInterface(const char* version) 
        {
            if (!interfaceInfo.name)
            {
                if (interfaceInfo.name = strrchr(typeid(IImp).name(), ':'))
                    interfaceInfo.name++;
                else
                    interfaceInfo.name = typeid(IImp).name();
            }

            if (!interfaceInfo.version)
                interfaceInfo.version = version;
        }

        const MXInterfaceInfo& GetInterfaceInfo()
        {
            return interfaceInfo;
        }
    private:
        static MXInterfaceInfo interfaceInfo;
    };

    template<typename IImp>
    MXInterfaceInfo MXInterfaceImp<IImp>::interfaceInfo;

    template<typename Str = std::string>
    HMODULE MXInitDll(MXDllObject& mxDllObj, const typename Str::allocator_type::value_type* dllName)
    {
        if (std::is_same<typename Str::allocator_type::value_type, char>::value)
        {
            HMODULE mxDllModule = LoadLibraryA(dllName);
            LOAD_MX_DLLOBJECT(mxDllObj, mxDllModule);
            return mxDllModule;
        }

        return nullptr;
    }
    
} //namespace mxtoolkit 


#endif
