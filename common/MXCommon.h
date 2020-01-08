#pragma once

#if defined(_WIN64)
#define _MX_WIN 64
#define _MX_64  1

#elif defined(_WIN32)
#define _MX_WIN 32
#define _MX_64  0

#endif

#if defined(_MX_WIN)
#include <windows.h>
#include <tchar.h>
#include <string>

#pragma warning(disable:4996)
#else
#include <sys/types.h>

#endif




/////////////////////////////////位操作
#ifndef _MX_WIN

#define DWORD_PTR DWORD

#define MAKEWORD(a, b) \
      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b) \
     ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))

#define MAKEWPARAM(l, h)      ((WPARAM)(DWORD)MAKELONG(l, h))
#define MAKELPARAM(l, h)      ((LPARAM)(DWORD)MAKELONG(l, h))
#define MAKELRESULT(l, h)     ((LRESULT)(DWORD)MAKELONG(l, h))

#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#endif



////////////////////////////////文件系统相关


#ifdef _MX_WIN
#define _MX_DIR_CHAR_A		'\\'
#define _MX_DIR_CHAR_W		L'\\'

#define _MX_DIR_STRING_A	"\\"
#define _MX_DIR_STRING_W	L"\\"

#else
#define _MX_DIR_CHAR_A		'/'
#define _MX_DIR_CHAR_W		L'/'

#define _MX_DIR_STRING_A	"/"
#define _MX_DIR_STRING_W	L"/"

#endif

namespace mxtoolkit
{
    typedef char				int8;
    typedef unsigned char		uint8;
    typedef short				int16;
    typedef unsigned short		uint16;
    typedef int					int32;
    typedef unsigned int		uint32;

#ifdef _MX_WIN
    typedef __int64				int64;
    typedef unsigned __int64	uint64;
#else
    typedef int64_t				int64;
    typedef u_int64_t			uint64;
#endif
    typedef std::wstring WString;
    typedef std::string AString;

#if _MX_64
    typedef uint64 autoBit;

#else
    typedef uint32 autoBit;

#endif


#ifdef UNICODE
    typedef WString TString;

#define _MX_DIR_CHAR_	_MX_DIR_CHAR_W
#define _MX_DIR_STRING_	_MX_DIR_STRING_W

#else
    typedef AString TString;

#define _MX_DIR_CHAR_	_MX_DIR_CHAR_A
#define _MX_DIR_STRING_	_MX_DIR_STRING_A

#endif


}

////////////////////////////////// 边界
#define _MX_MAX_STRING_BUFFER	512		//
#define _MX_MAX_PATH            1024	//路径长度
#define _MX_MAX_FILE_NAME		256		//文件名长度
#define _MX_MAX_EX_NAME			16		//后缀名长度
#define _MX_MAX_URL				256		//URL长度
#define _MX_MAX_SERVER_ADDR		512		//服务器地址长度


namespace mxtoolkit
{
    enum
    {
        MX_SUCCESS = 0,
        MX_FAIL = 1,
        MX_ERROR = -1
    };

    struct Result
    {
        int value;  //默认失败

#if __cplusplus
        operator bool() const { return value == MX_SUCCESS ? true : false; }

#endif
    };

#if __cplusplus
#define RETURN_RESULT(r) return mxtoolkit::ToResult(r)

    template<typename TYPE>
    Result ToResult(TYPE r)
    {
        if(std::is_same<typename std::decay<TYPE>::type, bool>::value)
            return { r ? MX_SUCCESS : MX_FAIL };

        return { r == 0 ? MX_SUCCESS : r };
    }
    
#endif
}