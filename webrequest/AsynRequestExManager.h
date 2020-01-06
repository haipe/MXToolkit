#pragma once

#include "RequestTask.h"
#include "RequestTaskManager.h"
#include "WebRequestDef.h"

namespace mxwebrequest
{

    typedef std::pair<RequestTask *, IRespondNotify *> _Request_And_CallBack_;

    class AsynRequestExManager : public RequestTaskManager<uint32, _Request_And_CallBack_>
    {
    public:
        AsynRequestExManager(void);
        virtual ~AsynRequestExManager(void);

        void SetNotify(IRespondNotify *pNotify);

        uint32 AddWebRequestTask(const Request &requestParam);
        uint32 AddWebRequestTask(const Request &requestParam, IRespondNotify *pNotify);

        static void CALLBACK TaskCallBack(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2, mxtoolkit::autoBit userData);
    protected:
        uint32 CallBackHandle(uint32 msg, uint32 param, mxtoolkit::autoBit reserver1, mxtoolkit::autoBit reserve2);

        mxtoolkit::BaseNotify GetNotify(const uint32 &key);

    private:

        IRespondNotify *m_notifyDefaultOut;
        mxtoolkit::BaseNotify m_notifyIn;
    };

}