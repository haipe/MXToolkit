#pragma once

#include <vector>
#include <string>
#include <iostream>


#include "MXMsgQueue.h"
#include "MXThread.h"
#include "MXBaseNotify.h"
#include "MXBaseNotify.h"

#include "WebRequestDef.h"



namespace mxwebrequest
{

    class RequestOperate;

    class RequestTask :public mxtoolkit::MXThread
    {
    public:
        RequestTask(const Request &requestInfo);
        virtual ~RequestTask(void);

    public:
        static RequestTask *CreateTask(const Request &requestInfo);
        static void      ReleaseTask(RequestTask *pTask);

        void SetNotify(const mxtoolkit::BaseNotify &notify);
        void SetMsg(mxtoolkit::MXElementAllocator<mxtoolkit::BaseMsg> *pMsgAllocator, mxtoolkit::MXMsgQueue<mxtoolkit::BaseMsg> *pMsgQueue);
        virtual uint32 ThreadProcEx();

        virtual void Header_Respond_Data(const char  *ptr, size_t size);
        virtual void Respond_Data(const char  *ptr, size_t size);

        const Request *GetRequestInfo() const;
        uint32   GetRespondData(char **ppRespond, uint32 *nSize);
        const char *GetRespondErrorString() const;
    public:
        const uint32 m_nTaskID;

    protected:

        void DeepCopyParam(const Request &req);
        void ReleaseParam();

        Request m_requestInfo;

        RequestOperate *m_pRequestOperate;

        uint32 m_retCode;
        char *m_pErrorBuffer;


        mxtoolkit::BaseNotify m_notify;

        mxtoolkit::MXElementAllocator<mxtoolkit::BaseMsg> *m_pMsgAllocator;
        mxtoolkit::MXMsgQueue<mxtoolkit::BaseMsg>         *m_pMsgQueue;
    };



    class HttpsRequestTask : public RequestTask
    {
    public:
        HttpsRequestTask(const Request &requestInfo);
        virtual ~HttpsRequestTask();


    };


    class HttpRequestTask : public RequestTask
    {

    public:
        HttpRequestTask(const Request &requestInfo);
        virtual ~HttpRequestTask();

    };

}