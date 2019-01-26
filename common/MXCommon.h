#pragma once

#include <windows.h>
#include <tchar.h>
#include <string>

#ifndef WIN32
#include <sys/types.h>
#else
#pragma warning(disable:4996)
#endif




/////////////////////////////////位操作
#ifndef WIN32

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


#ifdef WIN32
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
#ifdef WIN32
    typedef __int64				int64;
    typedef unsigned __int64	uint64;
#else
#include <sys/types.h>
    typedef int64_t				int64;
    typedef u_int64_t			uint64;
#endif

    typedef std::wstring WString;
    typedef std::string AString;

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

