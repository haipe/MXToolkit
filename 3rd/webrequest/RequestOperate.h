//
//  CRequestOprate.hpp
//  WebRequestForIos
//
//  Created by Loki-mac on 16/4/19.
//  Copyright © 2016年 com.inpor. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <vector>
#include "WebRequestDef.h"
#include "RequestTask.h"

#define RESPOND_BUFF_DEFAULT_SIZE_ 1024

namespace mxwebrequest
{

	class RequestOperate
	{
	public:
		friend class RequestTask;

		RequestOperate( RequestTask* pTask )
			:m_pRequestTask( pTask )
			, m_nTaskID( pTask->m_nTaskID )
			, m_pRequstParam( pTask->GetRequestInfo() )
			, m_respondBufferSize( 0 )
		{
			m_respondBuffer.reserve( RESPOND_BUFF_DEFAULT_SIZE_ );
			m_pError = nullptr;
		}

		virtual bool InitOprate( const Request *pHeader = nullptr ) = 0;
		virtual void SetProxy() = 0;
		virtual void SetHeader() = 0;
		virtual void SetRequestOpt() = 0;
		virtual void SetRequestParam() = 0;

		virtual unsigned int Excute() = 0;
		virtual void Done() = 0;

	protected:
		RequestTask *m_pRequestTask;
		const unsigned int m_nTaskID;

		const Request *m_pRequstParam;

        unsigned int m_respondBufferSize;
		std::vector<char> m_respondBuffer;

		CHAR *m_pError;
	};





}