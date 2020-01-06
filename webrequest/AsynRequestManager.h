#pragma once

#include "RequestTask.h"
#include "RequestTaskManager.h"

namespace mxwebrequest
{

	typedef std::pair<RequestTask *, mxtoolkit::BaseNotify> _RequestTask_And_Notify;

	class AsynRequestManager :public RequestTaskManager<uint32, _RequestTask_And_Notify>
	{
	public:
		AsynRequestManager( void );
		virtual ~AsynRequestManager( void );

		void SetNotify( const mxtoolkit::BaseNotify &notify );

		uint32 AddWebRequestTask( const Request &requestParam );
		uint32 AddWebRequestTask( const Request &requestParam, const mxtoolkit::BaseNotify &notify );

		static void CALLBACK TaskCallBack(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2, mxtoolkit::autoBit userData);
	protected:
		uint32 CallBackHandle(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2);

	private:

        mxtoolkit::BaseNotify m_notifyDefaultOut;
        mxtoolkit::BaseNotify m_notifyIn;

	};
}