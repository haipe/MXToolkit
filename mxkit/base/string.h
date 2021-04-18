#pragma once

#include <string>

#include "mxkit.h"


#ifdef _MX_WIN_

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

#define _MX_DOT_CHAR_A   '.'
#define _MX_DOT_STRING_A "."
#define _MX_DOT_CHAR_W   L'.'
#define _MX_DOT_STRING_W L"."

_BEGIN_MX_KIT_NAME_SPACE_

typedef std::wstring WString;
typedef std::string AString;

#ifdef _MX_UNICODE_
typedef WString TString;

#define _MX_DIR_CHAR_	_MX_DIR_CHAR_W
#define _MX_DIR_STRING_	_MX_DIR_STRING_W

#define _MX_DOT_CHAR_   _MX_DOT_CHAR_W
#define _MX_DOT_STRING_ _MX_DOT_STRING_W

#else
typedef AString TString;

#define _MX_DIR_CHAR_	_MX_DIR_CHAR_A
#define _MX_DIR_STRING_	_MX_DIR_STRING_A

#define _MX_DOT_CHAR_   _MX_DOT_CHAR_A
#define _MX_DOT_STRING_ _MX_DOT_STRING_A

#endif


_END_MX_KIT_NAME_SPACE_


