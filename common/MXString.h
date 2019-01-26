#pragma once

#include <string>
#include "MXCommon.h"

namespace mxtoolkit
{
    const WString& WStringFromCurrentTime();
    const AString& AStringFromCurrentTime();

    const WString& WStringFromCurrentTime2();
    const AString& AStringFromCurrentTime2();

	const WString& WStringFromRect( const RECT& rc );
	const AString& AStringFromRect( const RECT& rc );
    	
	int ReplaceAll( AString* in, const AString& pattern, const AString& newpat );

	template<typename T = AString,typename Tout = std::vector<T>>
	unsigned int SplitString( const T& in, const T& sp, Tout* out )
	{
		if ( in.empty() || out == nullptr )return 0;

		out->clear();
		size_t bpos = 0;
		size_t pos = in.find( sp );

		T x = in.substr( bpos, pos );
		out->push_back( x );
		if ( pos == T::npos )return out->size();

		while ( true )
		{
			bpos = pos + 1;
			pos = in.find( sp, bpos );

			x = in.substr( bpos, pos - bpos );
			out->push_back( x );

			if ( pos == T::npos )
				break;
		}

		return out->size();
	}

#ifdef UNICODE
#define NewGuidString NewGuidStringW

#else
#define NewGuidString NewGuidStringA

#endif

    AString NewGuidStringA();
    WString NewGuidStringW();

	void GUID2String( const GUID &param, AString *out );
	void GUID2String( const GUID &param, WString *out );
	void String2GUID( const AString &param, GUID *out );

}