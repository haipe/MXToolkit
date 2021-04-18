#pragma once

#include "MXCommon.h"
#include "MXBaseNotify.h"
#include "WebRequestDef.h"

namespace mxwebrequest
{
    //----------------------------------------------------------------------------
    // IWebRequest,
    //----------------------------------------------------------------------------
    struct IWebRequest
    {
        //初始化
        virtual mxtoolkit::Result Initialize(IRespondNotify *pNotify)= 0;

        //设置代理
        virtual mxtoolkit::Result SetProxy(const Proxy *pWebRequestProxy)= 0;

        //设置域名指向IP
        virtual mxtoolkit::Result SetHostResolve(const char* host, unsigned int port, const char* ip)= 0;

        //设置默认表头
        virtual mxtoolkit::Result SetDefaultHeader(RequestHeader *pRequestHeader)= 0;

        //同步
        virtual mxtoolkit::uint32 SynRequest(Request *pRequest, Respond *pRespond)= 0;

        //异步
        virtual mxtoolkit::uint32 AsynRequest(Request *pRequest)= 0;

        //异步
        virtual mxtoolkit::uint32 AsynRequest(Request *pRequest, IRespondNotify *pNotify)= 0;

        //异步
        virtual mxtoolkit::uint32 AsynRequest(Request *pRequest, mxtoolkit::BaseNotify *pNotify)= 0;

        //卸载
        virtual mxtoolkit::Result Uninstall()= 0;
    };

}

