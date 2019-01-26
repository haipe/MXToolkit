#include "stdafx.h"
#include <time.h>
#include <vector>
#include "MXString.h"

namespace mxtoolkit
{
#ifdef WIN32
#include <WinSock.h>
    int gettimeofday(struct timeval *tp, void *tzp)	
    {
        time_t clock;
        struct tm tm;
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);
        tm.tm_year = wtm.wYear - 1900;
        tm.tm_mon = wtm.wMonth - 1;
        tm.tm_mday = wtm.wDay;
        tm.tm_hour = wtm.wHour;
        tm.tm_min = wtm.wMinute;
        tm.tm_sec = wtm.wSecond;
        tm.tm_isdst = -1;
        clock = mktime(&tm);
        tp->tv_sec = clock;
        tp->tv_usec = wtm.wMilliseconds * 1000;
        return (0);
    }
#endif


    const WString& WStringFromCurrentTime()
    {
        static WString ret;

#ifdef _FS_OS_WIN
        SYSTEMTIME time;
        GetLocalTime(&time);
        WCHAR date[128] = { 0 };
        swprintf(
            date, 
            L"%d-%02d-%02d %02d:%02d:%02d", 
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

        ret = date;
#endif

        return ret;
    }
    
    const AString& AStringFromCurrentTime()
    {
        static AString ret;

#ifdef _FS_OS_WIN
        SYSTEMTIME time;
        GetLocalTime(&time);
        CHAR date[128] = { 0 };
        sprintf(
            date,
            "%d-%02d-%02d %02d:%02d:%02d",
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

        ret = date;

#endif

        return ret;
    }



    const WString& WStringFromCurrentTime2()
    {
        static WString ret;

#ifdef _FS_OS_WIN
        SYSTEMTIME time;
        GetLocalTime(&time);
        WCHAR date[128] = { 0 };
        swprintf(
            date,
            L"%d-%02d-%02d %02d:%02d:%02d:%04d",
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

        ret = date;
#endif

        return ret;
    }


    const AString& AStringFromCurrentTime2()
    {
        static AString ret;

#ifdef _FS_OS_WIN
        SYSTEMTIME time;
        GetLocalTime(&time);
        CHAR date[128] = { 0 };
        sprintf(
            date,
            "%d-%02d-%02d %02d:%02d:%02d:%04d",
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

        ret = date;

#endif

        return ret;
    }

    const WString& WStringFromRect(const RECT& rc)
	{
		WCHAR cache[64] = { 0 };
		_snwprintf( 
			cache, 
			sizeof( cache )*sizeof( WCHAR ),
			L"{l:%d,t:%d,r:%d,b:%d,w:%d,h:%d}", 
			rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top );

		static WString ret;
		ret = cache;

		return ret;
	}

	const AString& AStringFromRect( const RECT& rc )
	{
		CHAR cache[64] = { 0 };
		snprintf( cache,
			sizeof( cache )*sizeof( CHAR ), 
			"{l:%d,t:%d,r:%d,b:%d,w:%d,h:%d}",
			rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top );

		static AString ret;
		ret = cache;

		return ret;
	}
    
	int ReplaceAll( AString* str, const AString& pattern, const AString& newpat )
	{
		int count = 0;
		const size_t nsize = newpat.size();
		const size_t psize = pattern.size();

		for ( size_t pos = str->find( pattern, 0 ); pos != AString::npos; pos = str->find( pattern, pos + nsize ) )
		{
			str->replace( pos, psize, newpat );
			count++;
		}

		return count;
	}


	
    AString NewGuidStringA()
    {
        GUID guid;
        CoCreateGuid(&guid);

        char buf[64] = { 0 };

        snprintf(
            buf,
            sizeof(buf),
            "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);

        return buf;
    }

    WString NewGuidStringW()
    {
        GUID guid;
        CoCreateGuid(&guid);

        wchar_t buf[64] = { 0 };

        swprintf(
            buf,
            sizeof(buf),
            L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);

        return buf;
    }

    void GUID2String(const GUID &param, AString *out)
	{
		char szGuid[128] = { 0 };
		//{6FA683C9-EAAC-4522-B10E-1C3C167FD3ED}
		sprintf( szGuid, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", param.Data1, param.Data2, param.Data3,
			param.Data4[0], param.Data4[1], param.Data4[2], param.Data4[3],
			param.Data4[4], param.Data4[5], param.Data4[6], param.Data4[7] );

		*out = szGuid;
	}

	void GUID2String( const GUID &param, WString *out )
	{
		WCHAR wszGuid[128] = { 0 };
		swprintf( wszGuid, L"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", param.Data1, param.Data2, param.Data3,
			param.Data4[0], param.Data4[1], param.Data4[2], param.Data4[3],
			param.Data4[4], param.Data4[5], param.Data4[6], param.Data4[7] );

		*out = wszGuid;
	}

	void String2GUID( const AString &param, GUID *out )
	{
		if ( !param.empty() )
		{
			std::vector<AString> vc;
			SplitString<AString>( param, ",", &vc );

			if ( vc.size() == 11 )
			{
				//sscanf(param.c_str(), "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",&(out->Data1),&(out->Data2),&(out->Data3),&(out->Data4[0]),&(out->Data4[1]),&(out->Data4[2]),&(out->Data4[3]),&(out->Data4[4]),&(out->Data4[5]),&(out->Data4[6]),&(out->Data4[7]));
				long temp = 0;
				sscanf( vc[0].c_str(), "%x", &temp );
				out->Data1 = ( uint32 ) temp;

				temp = 0;
				sscanf( vc[1].c_str(), "%x", &temp );
				out->Data2 = ( uint16 ) temp;
				temp = 0;
				sscanf( vc[2].c_str(), "%x", &temp );
				out->Data3 = ( uint16 ) temp;

				temp = 0;
				sscanf( vc[3].c_str(), "%x", &temp );
				out->Data4[0] = ( uint8 ) temp;
				temp = 0;
				sscanf( vc[4].c_str(), "%x", &temp );
				out->Data4[1] = ( uint8 ) temp;
				temp = 0;
				sscanf( vc[5].c_str(), "%x", &temp );
				out->Data4[2] = ( uint8 ) temp;
				temp = 0;
				sscanf( vc[6].c_str(), "%x", &temp );
				out->Data4[3] = ( uint8 ) temp;
				temp = 0;
				sscanf( vc[7].c_str(), "%x", &temp );
				out->Data4[4] = ( uint8 ) temp;
				temp = 0;
				sscanf( vc[8].c_str(), "%x", &temp );
				out->Data4[5] = ( uint8 ) temp;
				temp = 0;
				sscanf( vc[9].c_str(), "%x", &temp );
				out->Data4[6] = ( uint8 ) temp;
				temp = 0;
				sscanf( vc[10].c_str(), "%x", &temp );
				out->Data4[7] = ( uint8 ) temp;
			}
		}
	}
}