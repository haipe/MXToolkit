#pragma once

#if defined(_WIN64)
#define _MX_WIN_ 64
#define _MX_64_  1
#define _MX_32_  0

#elif defined(_WIN32)
#define _MX_WIN_ 32
#define _MX_64_  0
#define _MX_32_  1

#endif

#if defined(_MX_WIN_)
#include <windows.h>
#include <tchar.h>
#include <string>

#if defined(UNICODE)
#define _MX_UNICODE_
#endif

#pragma warning(disable:4996)
#else
#include <sys/types.h>

#endif


#ifdef _MX_WIN_ //begin win32 =================

#if _MSC_VER == 1928    //(Visual Studio 2019) //begin vs =================
#define _MX_VS_VER_ 2019
#define _MX_CPP_VER_ 11

#elif _MSC_VER == 1900    //(Visual Studio 2015)
#define _MX_VS_VER_ 2015
#define _MX_CPP_VER_ 11

#elif _MSC_VER == 1800    //(Visual Studio 2013)
#define _MX_VS_VER_ 2013
#define _MX_CPP_VER_ 11

#elif _MSC_VER == 1700  //(Visual Studio 2012)
#define _MX_VS_VER_ 2012
#define _MX_CPP_VER_ 11

#elif _MSC_VER == 1600  //(Visual Studio 2010)
#define _MX_VS_VER_ 2010

#elif _MSC_VER == 1500  //(Visual Studio 2008)
#define _MX_VS_VER_ 2008


#elif _MSC_VER == 1400  //(Visual Studio 2005)
#define _MX_VS_VER_ 2005


#elif _MSC_VER == 1310  //(Visual Studio 2003) 

#define _MX_VS_VER_ 2003

#else

#endif  //end vs -------------------------

#define _MX_DEFAULT_TEMPLATE_ARGUMENTS_ (_MX_VS_VER_ > 2010)
#if _MX_VS_VER_ > 2010 //begin  =================
#define _STD_THREAD_
#define _STD_TEMPLATE_DEFAULT_

#define _STD_NAME_SPACE_ std

#elif _MX_VS_VER_ == 2010 
#define _STD_NAME_SPACE_ std::tr1

#else
#undef _MX_CPP_VER_

#endif   //end -------------------------

#ifdef _DEBUG
#define _MX_DEBUG_ 1
#endif

#else // not def  win32

#define _MX_CPP_VER_
#define _STD_NAME_SPACE_ std::

#ifdef _DEBUG
#define _MX_DEBUG_ 1
#endif

#endif //end win32 -------------------------

#if _MX_VS_VER_ >= 2010
#include <functional>

#define _STD_PLACE_HOLDERS_     _STD_NAME_SPACE_::placeholders
#define _STD_FUNCTION_          _STD_NAME_SPACE_::function

#endif






#define _MX_KIT_NAME_SPACE_ mxkit

#define _BEGIN_MX_KIT_NAME_SPACE_ namespace _MX_KIT_NAME_SPACE_ {

#define _END_MX_KIT_NAME_SPACE_ }


_BEGIN_MX_KIT_NAME_SPACE_

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;

#ifdef _MX_WIN_
typedef __int64				int64;
typedef unsigned __int64	uint64;

typedef HANDLE  Handle;
typedef HWND    Hwnd;
typedef HMODULE HModule;

#else
typedef int64_t				int64;
typedef u_int64_t			uint64;

typedef uint32  Handle;
typedef uint32  Hwnd;

#endif

#if _MX_64_
typedef uint64 uint;

#else
typedef uint32 uint;

#endif

typedef void* VoidPoint;


////////////////////////////////// 边界
enum
{
    _MX_LITTLE_BUFFER = 256,
    _MAX_CACHE_BUFFER = 1024, //

    _MAX_STRING_BUFFER = 512, //

    _MAX_PATH_LENGTH = 1024, //路径长度
    _MAX_FILE_NAME = 256, //文件名长度
    _MAX_EX_NAME = 16, //后缀名长度

    _MAX_URL = 512,	//URL长度
};

typedef uint32 Result;

class ResultUtils
{
public:
    enum
    {
        _SUCCESS_ = 0,
        _FAIL_ = 1,
        _ERROR_ = -1
    };

    static bool Success(const Result& rt) { return rt == 0 ? true : false; }
    static bool Fail(const Result& rt) { return rt > 0 ? true : false; }
    static bool Error(const Result& rt) { return rt < 0 ? true : false; }

    template<typename T>
    static Result Value(const T& v) { return (Result)v; }

    template<>
    static Result Value(const bool& b) { return b ? 0 : 1; }

};

#define RETURN_RESULT(r) return mxkit::ResultUtils::Value(r)


_END_MX_KIT_NAME_SPACE_