#pragma once
/*
*
*
*/
#if defined(WIN32)
#include <windows.h>
#include <WinBase.h>

#endif

#include <string>
#include "mxkit.h"

//////////////////////////////////////////////////////////////////////////
#define _MX_CALL_TYPE __cdecl

#define _MX_DLL_FUNCTION_TYPE(func) _##func##_Type_

#define _MX_C_EXPORT extern "C" _declspec(dllexport)

_BEGIN_MX_KIT_NAME_SPACE_

#if defined(__cplusplus)

extern "C" {

#endif

    struct InterfaceInfo    //接口信息
    {
        const char* name = nullptr;     //
        const char* version = nullptr;  //版本
    };

    struct ExportInfo    //导出库信息
    {
        const char* version = nullptr;
        unsigned int interfaceCount = 0;
        const InterfaceInfo* interfaceInfo = nullptr;
    };


    //初始化
    typedef Result(_MX_CALL_TYPE* _MX_DLL_FUNCTION_TYPE(InitLibrary))();
    //卸载
    typedef Result(_MX_CALL_TYPE* _MX_DLL_FUNCTION_TYPE(UninitLibrary))();
    //获取所有接口信息
    typedef Result(_MX_CALL_TYPE* _MX_DLL_FUNCTION_TYPE(QueryExport))(ExportInfo**);
    //获取接口
    typedef Result(_MX_CALL_TYPE* _MX_DLL_FUNCTION_TYPE(QueryInterface))(const InterfaceInfo*, void**);

    struct MXDllObject
    {
        _MX_DLL_FUNCTION_TYPE(InitLibrary) dllInit = nullptr;
        _MX_DLL_FUNCTION_TYPE(UninitLibrary) dllUninit = nullptr;
        _MX_DLL_FUNCTION_TYPE(QueryExport) getExportInfo = nullptr;
        _MX_DLL_FUNCTION_TYPE(QueryInterface) getInterfaceInfo = nullptr;
    };


#if defined(__cplusplus)
} //extern "C" 

template<typename IImp>
class InterfaceImp
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

    const InterfaceInfo& GetInterfaceInfo()
    {
        return interfaceInfo;
    }
private:
    static InterfaceInfo interfaceInfo;
};

template<typename IImp>
InterfaceInfo InterfaceImp<IImp>::interfaceInfo;

#ifdef _MX_WIN_
#define MX_LOAD_LIBRARY_OBJECT(mxDllObj,dllModule)                                                                                          \
do                                                                                                                                          \
{                                                                                                                                           \
    if (!dllModule) break;                                                                                                                  \
    mxDllObj.dllInit = (mxkit::_MX_DLL_FUNCTION_TYPE(InitLibrary))GetProcAddress(dllModule, "InitLibrary");                                 \
    mxDllObj.dllUninit = (mxkit::_MX_DLL_FUNCTION_TYPE(UninitLibrary))GetProcAddress(dllModule, "UninitLibrary");                           \
    mxDllObj.getExportInfo = (mxkit::_MX_DLL_FUNCTION_TYPE(QueryExport))GetProcAddress(dllModule, "QueryExport");                           \
    mxDllObj.getInterfaceInfo = (mxkit::_MX_DLL_FUNCTION_TYPE(QueryInterface))GetProcAddress(dllModule, "QueryInterface");                  \
} while (0);

#else

#define MX_LOAD_LIBRARY_OBJECT(mxDllObj,dllModule)

#endif

template<
    typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
    = std::string
#endif
    , typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
    = typename Str::allocator_type::value_type
#endif
>
HModule LoadLibrary(MXDllObject& mxDllObj, const CharType* libName)
{
    HModule mxModule = 0;
#ifdef _MX_WIN_
    if (std::is_same<CharType, char>::value)
    {
        mxModule = ::LoadLibraryA((LPCSTR)libName);
    }
    else
    {
        mxModule = ::LoadLibraryW((LPCWSTR)libName);
    }
#endif

    MX_LOAD_LIBRARY_OBJECT(mxDllObj, mxModule);
    return mxModule;
}

template<
    bool b 
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
    = true
#endif
>
void FreeLibrary(HModule& md,bool def = b)
{
    ::FreeLibrary(md);
    md = 0;
}

#endif


_END_MX_KIT_NAME_SPACE_


