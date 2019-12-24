#ifndef STRINGS_UTIL_H
#define STRINGS_UTIL_H

#ifdef _MSC_VER
#define strcasecmp stricmp
#define strncasecmp  strnicmp

#else
#include <strings.h>

#endif


#endif // STRINGS_UTIL_H
