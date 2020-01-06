#pragma once

#include "..\RequestOperate.h"
#include "..\WebRequestDef.h"
#include <curl.h>

namespace mxwebrequest
{

	class RequestOperateImp :public RequestOperate
	{
	public:
		RequestOperateImp( RequestTask* pTask );
		virtual ~RequestOperateImp( void );

		virtual bool InitOprate( const Request *pRequest = nullptr );
		virtual void SetProxy();
		virtual void SetHeader();
		virtual void SetRequestOpt();
		virtual void SetRequestParam();

		virtual unsigned int Excute();
		virtual void Done();

		void Header_Respond_Data( const char  *ptr, size_t size );
		void Respond_Data( const char  *ptr, size_t size );
	private:

		CURL *m_pCurl;
		CHAR m_szErrorBuffer[CURL_ERROR_SIZE];

#ifdef _DEBUG_CURL
		std::string m_szbuffer;
		std::string m_szheader_buffer;
#endif
	};

}