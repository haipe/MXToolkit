
#pragma once


#include "IWebRequest.h"
#include "AsynRequestManager.h"
#include "AsynRequestExManager.h"
#include "SynRequestManager.h"
#include "MXSingleObject.h"

#include <string>
#include <list>

namespace mxwebrequest
{

    //http请求

    class WebRequestImp 
        : public IWebRequest
        , public mxtoolkit::MXSingleObject<WebRequestImp>
    {
    public:
        WebRequestImp();
        ~WebRequestImp();

        //初始化
        mxtoolkit::Result Initialize(IRespondNotify *pNotify) override;
        //设置代理
        mxtoolkit::Result SetProxy(const Proxy *pWebRequestProxy) override;
        //设置域名指向IP
        mxtoolkit::Result SetHostResolve(const char* host, unsigned int port, const char* ip) override;
        //设置默认表头
        mxtoolkit::Result SetDefaultHeader(RequestHeader *pRequestHeader) override;

        mxtoolkit::uint32 SynRequest(Request *pRequest, Respond *pRespond) override;

        mxtoolkit::uint32 AsynRequest(Request *pRequest) override;

        mxtoolkit::uint32 AsynRequest(Request *pRequest, IRespondNotify *pNotify) override;

        mxtoolkit::uint32 AsynRequest(Request *pRequest, mxtoolkit::BaseNotify *pNotify) override;

        //卸载
        mxtoolkit::Result Uninstall() override;

    protected:
        void CleanTagRequestHeader(RequestHeader* requestHeader);
        void CleanTagProxy(Proxy* proxy);

    protected:

        SynRequestManager    *m_synRequestManager = nullptr;
        AsynRequestManager   *m_asynRequestManager = nullptr;
        AsynRequestExManager *m_asynRequestExManager = nullptr;

        RequestHeader   m_defaultRequestHeader;

        Proxy m_proxy;

    private:
        bool m_bInited = false;

    };

}