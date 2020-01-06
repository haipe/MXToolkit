#pragma once
/*
*
*
*/
#if defined(WIN32)
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////////
#define MX_CALL_TYPE __cdecl

#define MX_DECLEAR_STRUCT(st_name) struct st_name

#define MX_DECLEAR_INTERFACE(ret_type, it_name) virtual ret_type MX_CALL_TYPE it_name

#define MX_IMPLEMENT_INTERFACE(ret_type, it_name) virtual ret_type MX_CALL_TYPE it_name

#define MX_DLL_FUNCTION_TYPE(fc) fc##_function

#define MX_C_EXPORT extern "C" _declspec(dllexport)

#if defined(__cplusplus)
namespace mxtoolkit {
    extern "C" {
#endif

typedef unsigned int mx_call_result;			//接口返回
const mx_call_result mx_call_successed = 0;		//返回成功
const mx_call_result mx_call_fail = 1;			//调用失败


struct mx_export_interface_info
{
    const char* name = nullptr;     //
    const char* version = nullptr;  //版本
};

struct mx_dll_export_info
{
	const char* version = nullptr;
	unsigned int interface_count = 0;
	const mx_export_interface_info* interface_info = nullptr;
};


//初始化
typedef mx_call_result(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mx_dll_init))();
//卸载
typedef void(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mx_dll_uninit))();
//获取所有接口信息
typedef mx_call_result(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mx_dll_all_export))(mx_dll_export_info **);
//获取接口
typedef mx_call_result(MX_CALL_TYPE *MX_DLL_FUNCTION_TYPE(mx_dll_get_interface))(const mx_export_interface_info*, void**);


#if defined(__cplusplus)
    } //extern "C" 
} //namespace mxtoolkit 

#endif