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