#include "stdafx.h"
#include "RequestTask.h"
#include "RequestOperate.h"

#include "RequestOperateImp.h"

#include "HostResolveManager.h"

#include "MXLock.h"

#define REQUEST_PROTOCOL_HTTPS  "https"
#define REQUEST_PROTOCOL_HTTP   "http"


#define FAIL_LOOP_NUMBER  3 //不再自动重试3次

namespace
{
    static uint32 g_nTaskID = 1;
}

namespace mxwebrequest
{

    RequestTask::RequestTask(const Request &requestInfo) :m_nTaskID(g_nTaskID++)

        , m_retCode(0)
    {
        DeepCopyParam(requestInfo);

        m_pRequestOperate = new RequestOperateImp(this);
    }

    RequestTask::~RequestTask(void)
    {
        ReleaseParam();
    }

    void RequestTask::DeepCopyParam(const Request &req)
    {
        m_requestInfo.request_type = req.request_type;

        if (req.request_proxy)
        {
            //m_requestInfo.request_proxy
        }

        if (req.request_headers)
        {
            m_requestInfo.request_headers = new RequestHeader;
            m_requestInfo.request_headers->headers = new RequestHeaderItem[req.request_headers->header_count];
            m_requestInfo.request_headers->header_count = req.request_headers->header_count;

            for (int i = 0; i < m_requestInfo.request_headers->header_count; i++)
            {
                if (req.request_headers->headers[i].data && req.request_headers->headers[i].len > 0)
                {
                    m_requestInfo.request_headers->headers[i].len = req.request_headers->headers[i].len;

                    m_requestInfo.request_headers->headers[i].data =
                        new char[m_requestInfo.request_headers->headers[i].len + 1];

                    memcpy(
                        m_requestInfo.request_headers->headers[i].data,
                        req.request_headers->headers[i].data,
                        req.request_headers->headers[i].len);

                    m_requestInfo.request_headers->headers[i].data[m_requestInfo.request_headers->headers[i].len] = '\0';
                }
            }
        }

        if (req.timeout < 0 || req.timeout > 600)
            m_requestInfo.timeout = DEFAULT_TIME_OUT;
        else
            m_requestInfo.timeout = req.timeout;

        if (req.request_host)
        {
            size_t nLen = strlen(req.request_host);
            m_requestInfo.request_host = new char[nLen + 1];

            memcpy(m_requestInfo.request_host, req.request_host, nLen);
            m_requestInfo.request_host[nLen] = '\0';
        }
        else
        {
            m_requestInfo.request_host = nullptr;
        }

        if (req.request_param)
        {
            size_t nLen = strlen(req.request_param);
            m_requestInfo.request_param = new char[nLen + 1];

            memcpy(m_requestInfo.request_param, req.request_param, nLen);
            m_requestInfo.request_param[nLen] = '\0';
        }
        else
        {
            m_requestInfo.request_param = nullptr;
        }
    }

    void RequestTask::ReleaseParam()
    {
        if (m_requestInfo.request_headers)
        {
            for (int i = 0; i < m_requestInfo.request_headers->header_count; i++)
            {
                if (m_requestInfo.request_headers->headers[i].data && m_requestInfo.request_headers->headers[i].len > 0)
                {
                    delete[] m_requestInfo.request_headers->headers[i].data;
                    m_requestInfo.request_headers->headers[i].len = 0;
                }
            }

            delete[] m_requestInfo.request_headers->headers;

            delete m_requestInfo.request_headers;
        }

        if (m_requestInfo.request_host)
        {
            delete[] m_requestInfo.request_host;
        }

        if (m_requestInfo.request_param)
        {
            delete[] m_requestInfo.request_param;
        }
    }

    RequestTask * RequestTask::CreateTask(const Request &requestInfo)
    {
        static std::recursive_mutex s_mutex;

        mxtoolkit::MXAutoLock autoLock(s_mutex);

        RequestTask *pTask = nullptr;
        if (strstr(requestInfo.request_host, REQUEST_PROTOCOL_HTTPS) == requestInfo.request_host)
            pTask = new HttpsRequestTask(requestInfo);
        else if (strstr(requestInfo.request_host, REQUEST_PROTOCOL_HTTP) == requestInfo.request_host)
            pTask = new HttpRequestTask(requestInfo);

        return pTask;
    }

    void RequestTask::ReleaseTask(RequestTask *pTask)
    {
        delete pTask;
        pTask = nullptr;
    }

    void RequestTask::SetNotify(const mxtoolkit::BaseNotify &notify)
    {
        m_notify = notify;
    }

    void RequestTask::SetMsg(mxtoolkit::MXElementAllocator<mxtoolkit::BaseMsg> *pMsgAllocator, mxtoolkit::MXMsgQueue<mxtoolkit::BaseMsg> *pMsgQueue)
    {
        m_pMsgAllocator = pMsgAllocator; m_pMsgQueue = pMsgQueue;
    }

    void RequestTask::Header_Respond_Data(const char *ptr, size_t size)
    {
        if (m_notify.notifyMode == mxtoolkit::BaseNotify::MODE_NONE)
            return;

        if (m_notify.notifyMode == mxtoolkit::BaseNotify::MODE_CALLBACK)
        {
            if (m_notify.CallbackMode.callback)
            {
                ((WEBREQUSET_CALLBACK)m_notify.CallbackMode.callback)(
                    m_notify.CallbackMode.msgID,
                    MAKEWPARAM(REQUEST_HEADER_RESPOND_NOTIFY, m_nTaskID),
                    (LPARAM)ptr,
                    size,
                    m_notify.CallbackMode.userData);
            }
        }
    }

    void RequestTask::Respond_Data(const char *ptr, size_t size)
    {
        if (m_notify.notifyMode == mxtoolkit::BaseNotify::MODE_CALLBACK)
        {
            if (m_notify.CallbackMode.callback)
            {
                ((WEBREQUSET_CALLBACK)m_notify.CallbackMode.callback)(
                    m_notify.CallbackMode.msgID,
                    MAKEWPARAM(REQUEST_DATA_RESPOND_NOTIFY, m_nTaskID),
                    (LPARAM)ptr,
                    size,
                    m_notify.CallbackMode.userData);
            }
        }
    }

    uint32 RequestTask::ThreadProcEx()
    {
        do
        {
            VerifyUrlResolveUtil verifyUtil(m_requestInfo.request_host);

            int loopNumber = 0;
        doItAgain:
            loopNumber++;

            if (m_isStop)
                break;
            if (m_pRequestOperate && m_pRequestOperate->InitOprate())
            {
                m_pRequestOperate->SetProxy();
                m_pRequestOperate->SetHeader();
                m_pRequestOperate->SetRequestOpt();
                m_pRequestOperate->SetRequestParam();

                m_retCode = m_pRequestOperate->Excute();

                if (m_isStop)
                    break;
#if defined(_FS_OS_WIN)
                //失败后重试，直到遍历完定向IP
                if (m_retCode != 0 && verifyUtil.HasResolveLink()) //loopNumber <= FAIL_LOOP_NUMBER && 
#else
                //ios mac 未实现定向IP
                if (m_retCode != 0 && loopNumber <= FAIL_LOOP_NUMBER)
#endif
                {
                    ;// ("Rquest[%d] Failed! Failed Count:%d! ErrorInfo:%s.\n", m_nTaskID, loopNumber, m_pRequestOperate->m_pError);
                    printf("Rquest[%d] Failed! Failed Count:%d! ErrorInfo:%s.\n", m_nTaskID, loopNumber, m_pRequestOperate->m_pError ? "1" : "2");

                    m_pRequestOperate->Done();

                    goto doItAgain;
                }

                //失败了重置定向IP
                verifyUtil.ResetUrlResolve();

                ;// ("Request[%d] retCode:%d.\n", m_nTaskID, m_retCode);
                printf("Request[%d] host:%s. param:%s. retCode:%d.\n",
                    m_nTaskID,
                    m_requestInfo.request_host, m_requestInfo.request_param ? m_requestInfo.request_param : nullptr, m_retCode);
#if defined(_FS_OS_IOS) || defined(_FS_OS_MAC)
                if (m_retCode == 9999)
                {
                    //exception error
                    static const char *s = "HTTP/1.1 403 Forbidden";
                    static const int   l = strlen(s) + 1;

                    Header_Respond_Data(s, l);
            }
#endif

                if (m_isStop)
                    break;

                if (m_notify.notifyMode == mxtoolkit::BaseNotify::MODE_CALLBACK)
                {
                    if (m_notify.CallbackMode.callback)
                    {
                        if (m_pRequestOperate->m_respondBufferSize && m_pRequestOperate->m_respondBuffer.size() != 0)
                        {
                            const char *pRespondBuff = &m_pRequestOperate->m_respondBuffer[0];

                            ((WEBREQUSET_CALLBACK)m_notify.CallbackMode.callback)(
                                m_notify.CallbackMode.msgID,
                                MAKEWPARAM(REQUEST_COMPLETE_NOTIFY, m_nTaskID),
                                (LPARAM)pRespondBuff,
                                m_pRequestOperate->m_respondBufferSize,
                                m_notify.CallbackMode.userData);
                        }
                        else
                        {
                            ((WEBREQUSET_CALLBACK)m_notify.CallbackMode.callback)(
                                m_notify.CallbackMode.msgID,
                                MAKEWPARAM(REQUEST_COMPLETE_NOTIFY, m_nTaskID), 0, 0, m_notify.CallbackMode.userData);
                        }
                    }
                }

                m_pRequestOperate->Done();
        }

            if (m_isStop)
                break;

            if (m_pMsgAllocator && m_pMsgQueue)
            {
                mxtoolkit::BaseMsg *pMsg = m_pMsgAllocator->Alloc();

                if (pMsg)
                {
                    pMsg->message = REQUEST_COMPLETE_NOTIFY;
                    pMsg->param = m_nTaskID;

                    m_pMsgQueue->PushMsg(pMsg);
                }
            }

            ;// ("Request[%d] Over!\n", m_nTaskID);

    } while (0);

    return 0;
}//NOLINT(readability/fn_size)

    const Request * RequestTask::GetRequestInfo() const
    {
        return &m_requestInfo;
    }

    uint32 RequestTask::GetRespondData(char **ppRespond, uint32 *nSize)
    {
        if (ppRespond == nullptr ||
            m_pRequestOperate->m_respondBufferSize == 0 || m_pRequestOperate->m_respondBuffer.size() == 0)
        {
            *nSize = 0;
            return m_retCode;
        }

        *ppRespond = (char *)&m_pRequestOperate->m_respondBuffer[0];
        *nSize = m_pRequestOperate->m_respondBufferSize;

        return m_retCode;
    }

    const char * RequestTask::GetRespondErrorString() const
    {
        return m_pRequestOperate->m_pError;
    }


    HttpsRequestTask::HttpsRequestTask(const Request &requestInfo) :RequestTask(requestInfo)
    {
        m_requestInfo.request_protocol = REQUEST_PROTOCOLTYPE_HTTPS;
    }

    HttpsRequestTask::~HttpsRequestTask()
    {
    }

    HttpRequestTask::HttpRequestTask(const Request &requestInfo) :RequestTask(requestInfo)
    {
        m_requestInfo.request_protocol = REQUEST_PROTOCOLTYPE_HTTP;
    }

    HttpRequestTask::~HttpRequestTask()
    {

    }

}