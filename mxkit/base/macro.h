#pragma once

#include "mxkit.h"

#ifdef _MX_WIN_


#define _MX_DECLARE_STD_CALL_(ret,name) typedef ret(_stdcall* name)
#define _MX_DECLARE_CDECL_CALL_(ret,name) typedef ret(_cdecl* name)

#else
#endif