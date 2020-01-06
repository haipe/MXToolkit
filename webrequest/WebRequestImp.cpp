// WebRequestImp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "WebRequestImp.h"
#include "RequestTask.h"
#include "HostResolveManager.h"
#ifdef _MX_WIN
#include <curl.h>
#endif

namespace mxwebrequest
{

    WebRequestImp::WebRequestImp()
    {
        m_defaultRequestHeader.headers = nullptr;
        m_defaultRequestHeader.header_count = 0;
    }

    WebRequestImp::~WebRequestImp()
    {
        HostResolveManager::GetInstance()->Release();

        CleanTagRequestHeader(&m_defaultRequestHeader);

        CleanTagProxy(&m_proxy);
    }

    mxtoolkit::Result WebRequestImp::Initialize(IRespondNotify *pNotify)
    {
        if (!m_bInited)
        {
#if defined(_FS_OS_WIN)
            /*
            CURL_GLOBAL_ALL                //初始化所有的可能的调用。
            CURL_GLOBAL_SSL               //初始化支持 安全套接字层。
            CURL_GLOBAL_WIN32            //初始化win32套接字库。
            CURL_GLOBAL_NOTHING         //没有额外的初始化。
            */
            curl_global_init(CURL_GLOBAL_ALL);
#endif
            m_bInited = true;
        }

        if (m_synRequestManager == nullptr)
            m_synRequestManager = new SynRequestManager;

        if (m_asynRequestManager == nullptr)
            m_asynRequestManager = new AsynRequestManager;

        if (m_asynRequestExManager == nullptr)
        {
            m_asynRequestExManager = new AsynRequestExManager;

            m_asynRequestExManager->SetNotify(pNotify);
        }

        return true;
    }

    mxtoolkit::Result WebRequestImp::SetProxy(const Proxy *pWebRequestProxy)
    {
        if (m_bInited == false)
            return S_FALSE;

        if (pWebRequestProxy == nullptr)
            return S_FALSE;

        if (pWebRequestProxy->proxy_host == nullptr)
            return S_FALSE;

        HRESULT hr = S_FALSE;
        do
        {
            // ------------------- host
            int size = strlen(pWebRequestProxy->proxy_host) + 1;
            if (size == 1)
                break;

            m_proxy.proxy_host = new char[size];

            if (m_proxy.proxy_host == nullptr)
                break;

            memcpy(m_proxy.proxy_host, pWebRequestProxy->proxy_host, size);

            // ------------------- username 
            if (pWebRequestProxy->proxy_username == nullptr)
                break;

            size = strlen(pWebRequestProxy->proxy_username) + 1;

            if (size > 1)
            {
                m_proxy.proxy_username = new char[size];

                if (m_proxy.proxy_username != nullptr)
                    memcpy(m_proxy.proxy_username, pWebRequestProxy->proxy_username, size);
            }
            // ------------------- passwod

            if (pWebRequestProxy->proxy_password == nullptr)
                break;

            size = strlen(pWebRequestProxy->proxy_password) + 1;

            if (size > 1)
            {

                m_proxy.proxy_password = new char[size];

                if (m_proxy.proxy_password != nullptr)
                {
                    memcpy(m_proxy.proxy_password, pWebRequestProxy->proxy_password, size);
                }
            }
            m_proxy.proxy_type = pWebRequestProxy->proxy_type;

            m_proxy.proxy_port = pWebRequestProxy->proxy_port;

            hr = S_OK;

        } while (0);

        if (hr == S_FALSE)
        {
            CleanTagProxy(&m_proxy);
        }
        return hr;
    }

    mxtoolkit::Result WebRequestImp::SetHostResolve(const char* host, unsigned int port, const char* ip)
    {
        //if (host && ip)
        //    ;// ("WebRequestImp::SetHostResolve host:%s resolve to:%s:%d.\n", host, ip, port);

        HostResolveManager::GetInstance()->AddHostResolve(host, port, ip);

        return true;
    }

    mxtoolkit::Result WebRequestImp::SetDefaultHeader(RequestHeader *pRequestHeader)
    {
        if (pRequestHeader == nullptr)
            return false;

        CleanTagRequestHeader(&m_defaultRequestHeader);

        RequestHeaderItem *pSourceHeaders = pRequestHeader->headers;
        RequestHeaderItem *pDestHeaders = new RequestHeaderItem[pRequestHeader->header_count];

        for (int i = 0; i < pRequestHeader->header_count; i++)
        {
            RequestHeaderItem *pDestHeader = &pDestHeaders[i];
            RequestHeaderItem *pSourceHeader = &pSourceHeaders[i];

            if (pDestHeader && pSourceHeader && pSourceHeader->len)
            {
#ifdef _DEBUG_CURL
                FMC_LOG("header %d:%s\n", i, pSourceHeader->data);
#endif

                pDestHeader->data = new char[pSourceHeader->len + 1];

                memcpy(pDestHeader->data, pSourceHeader->data, pSourceHeader->len + 1);

                pDestHeader->len = pSourceHeader->len;
            }
        }

        m_defaultRequestHeader.headers = pDestHeaders;
        m_defaultRequestHeader.header_count = pRequestHeader->header_count;

        return true;
    }

    mxtoolkit::uint32 WebRequestImp::SynRequest(Request *pRequest, Respond *pRespond)
    {
        if (pRequest == nullptr || pRespond == nullptr)
            return 0;

        if (pRequest->request_headers == nullptr)
            pRequest->request_headers = &m_defaultRequestHeader;

        if (pRequest->request_proxy == nullptr && m_proxy.proxy_host != nullptr)
        {
            pRequest->request_proxy = &m_proxy;
        }

        return m_synRequestManager->AddWebRequestTask(*pRequest, pRespond);
    }

    mxtoolkit::uint32 WebRequestImp::AsynRequest(Request *pRequest)
    {
        if (pRequest == nullptr)
            return 0;

        if (pRequest->request_headers == nullptr)
            pRequest->request_headers = &m_defaultRequestHeader;

        if (pRequest->request_proxy == nullptr && m_proxy.proxy_host != nullptr)
        {
            pRequest->request_proxy = &m_proxy;
        }

        return m_asynRequestExManager->AddWebRequestTask(*pRequest);
    }

    mxtoolkit::uint32 WebRequestImp::AsynRequest(Request *pRequest, IRespondNotify *pNotify)
    {
        if (pRequest == nullptr || pNotify == nullptr)
            return 0;

        if (pRequest->request_headers == nullptr)
            pRequest->request_headers = &m_defaultRequestHeader;

        if (pRequest->request_proxy == nullptr && m_proxy.proxy_host != nullptr)
        {
            pRequest->request_proxy = &m_proxy;
        }

        return m_asynRequestExManager->AddWebRequestTask(*pRequest, pNotify);
    }

    mxtoolkit::uint32 WebRequestImp::AsynRequest(Request *pRequest, mxtoolkit::BaseNotify *pNotify)
    {
        if (pRequest == nullptr || pNotify == nullptr)
            return 0;

        if (pRequest->request_headers == nullptr)
            pRequest->request_headers = &m_defaultRequestHeader;

        if (pRequest->request_proxy == nullptr && m_proxy.proxy_host != nullptr)
        {
            pRequest->request_proxy = &m_proxy;
        }

        return m_asynRequestManager->AddWebRequestTask(*pRequest, *pNotify);
    }


    mxtoolkit::Result WebRequestImp::Uninstall()
    {
        if (m_bInited)
        {
            CleanTagRequestHeader(&m_defaultRequestHeader);

            CleanTagProxy(&m_proxy);

            if (m_synRequestManager)
            {
                m_synRequestManager->StopThread();

                delete m_synRequestManager;

                m_synRequestManager = nullptr;
            }

            if (m_asynRequestManager)
            {
                m_asynRequestManager->StopThread();

                delete m_asynRequestManager;
                m_asynRequestManager = nullptr;
            }

            if (m_asynRequestExManager)
            {
                m_asynRequestExManager->StopThread();

                delete m_asynRequestExManager;
                m_asynRequestExManager = nullptr;
            }

#if defined(_FS_OS_WIN)
            curl_global_cleanup();
#endif
            m_bInited = false;

            return true;
        }

        return true;
    }

    void WebRequestImp::CleanTagRequestHeader(RequestHeader* requestHeader)
    {
        if (requestHeader->headers)
        {
            for (int i = 0; i < requestHeader->header_count; i++)
            {
                RequestHeaderItem *pSourceHeader = &requestHeader->headers[i];

                if (pSourceHeader)
                {
                    delete[] pSourceHeader->data;
                }
            }

            delete[] requestHeader->headers;
        }
    }

    void WebRequestImp::CleanTagProxy(Proxy* proxy)
    {
        if (proxy->proxy_host != nullptr)
        {
            delete[] proxy->proxy_host;

            proxy->proxy_host = nullptr;
        }

        if (proxy->proxy_username != nullptr)
        {
            delete[] proxy->proxy_username;

            proxy->proxy_username = nullptr;
        }

        if (proxy->proxy_password != nullptr)
        {
            delete[] proxy->proxy_password;

            proxy->proxy_password = nullptr;
        }
    }

}