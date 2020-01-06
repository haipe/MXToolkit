#pragma once

#include "RequestTask.h"
#include "RequestTaskManager.h"
#include "WebRequestDef.h"

namespace mxwebrequest
{

	typedef std::pair<RequestTask *, Respond *> _UrlTask_And_Respond;

	class SynRequestManager : public RequestTaskManager<uint32, _UrlTask_And_Respond>
	{
	public:
		SynRequestManager( void );
		virtual ~SynRequestManager( void );

		uint32 AddWebRequestTask( const Request &requestParam, Respond *pRespond );

		static void CALLBACK TaskCallBack(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2, mxtoolkit::autoBit userData);
	protected:
		uint32 CallBackHandle(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2);

	};

}