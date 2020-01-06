#include "stdafx.h"
#include "RequestOperateImp.h"
#include <string>
#include <atomic>

#include "MXCommon.h"

#include "HostResolveManager.h"

namespace mxwebrequest
{

    namespace
    {
        size_t Respond_Data_Callback(const char *ptr, size_t size, size_t nmemb, RequestOperateImp *pTask)
        {
            size_t len = size * nmemb;

            if (pTask)
                pTask->Respond_Data(ptr, len);

            return len;
        }

        size_t Header_Callback(const char  *ptr, size_t size, size_t nmemb, RequestOperateImp *pTask)
        {
            size_t len = size * nmemb;

            if (pTask)
                pTask->Header_Respond_Data(ptr, len);

            return len;
        }


        std::atomic<uint32> g_nTaskID = 1;

        static std::string s_defaultDNSIP =
            "119.29.29.29,"
            "182.254.116.116,"
            "114.114.114.114,"
            "114.114.114.115,"
            "8.8.8.8,"
            "8.8.4.4,"
            "1.1.1.1,"
            "1.0.0.1,"
            "223.5.5.5,"
            "223.6.6.6";

        struct UrlResolveUtil
        {
            struct curl_slist *hostResolve;//NOLINT()

            UrlResolveUtil(CURL* curl, const std::string& url, uint32 taskID)
            {
                hostResolve = nullptr;

                std::string resolve;
                if (curl &&
                    !url.empty() &&
                    HostResolveManager::GetInstance()->CheckUrlResolve(url, resolve) && !resolve.empty())
                {
                    //("Request[%d] Need Resolve:%s to -> %s\n", taskID, url.c_str(), resolve.c_str());
                    hostResolve = curl_slist_append(nullptr, resolve.c_str());
                    curl_easy_setopt(curl, CURLOPT_RESOLVE, hostResolve);
                }
            }

            ~UrlResolveUtil()
            {
                if (hostResolve)
                {
                    curl_slist_free_all(hostResolve);
                    hostResolve = nullptr;
                }
            }
        };


    }


    RequestOperateImp::RequestOperateImp(RequestTask* pTask) :RequestOperate(pTask)
    {
        memset(m_szErrorBuffer, 0, CURL_ERROR_SIZE * sizeof(CHAR));

        m_pError = &m_szErrorBuffer[0];
    }

    RequestOperateImp::~RequestOperateImp(void)
    {
    }

    bool RequestOperateImp::InitOprate(const Request *pRequest /*= nullptr*/)
    {
        if (m_pRequstParam == nullptr && pRequest == nullptr)
            return false;

        if (pRequest != nullptr)
            m_pRequstParam = pRequest;

        m_pCurl = curl_easy_init();

#ifdef _DEBUG_CURL
        if (m_pCurl)
            curl_easy_setopt(m_pCurl, CURLOPT_VERBOSE, 1L);//调试信息打开  
#endif
        return m_pCurl != nullptr;
    }

    void RequestOperateImp::SetProxy()
    {
        if (m_pRequstParam == nullptr
            || m_pRequstParam->request_proxy == nullptr
            || m_pRequstParam->request_proxy->proxy_host == nullptr)
            return;

        switch (m_pRequstParam->request_proxy->proxy_type)
        {
        case PROXY_TYPE_HTTP:
            curl_easy_setopt(m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
            break;
        case PROXY_TYPE_SOCK4:
            curl_easy_setopt(m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
            break;
        case PROXY_TYPE_SOCK5:
            curl_easy_setopt(m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
            break;
        default:
            return;
        }

        curl_easy_setopt(m_pCurl, CURLOPT_PROXY, m_pRequstParam->request_proxy->proxy_host);
        curl_easy_setopt(m_pCurl, CURLOPT_PROXYPORT, m_pRequstParam->request_proxy->proxy_port);
        curl_easy_setopt(m_pCurl, CURLOPT_PROXY_TRANSFER_MODE, 1L);

        if (m_pRequstParam->request_proxy->proxy_username != nullptr &&
            m_pRequstParam->request_proxy->proxy_password != nullptr)
        {
            CHAR szTemp[MAX_PATH] = { 0 };
            snprintf(
                szTemp,
                MAX_PATH,
                "%s:%s", m_pRequstParam->request_proxy->proxy_username, m_pRequstParam->request_proxy->proxy_password);

            curl_easy_setopt(m_pCurl, CURLOPT_PROXYUSERPWD, szTemp);
        }
    }

    void RequestOperateImp::SetHeader()
    {
        if (m_pRequstParam == nullptr)
            return;

        struct curl_slist *headers = nullptr; /* init to nullptr is important */ //NOLINT()

        if (m_pRequstParam->request_headers->headers)
        {
            for (int i = 0; i < m_pRequstParam->request_headers->header_count; i++)
            {
                ;// ("Request[%d] Header Param:%s\n", m_nTaskID, m_pRequstParam->request_headers->headers[i].data);

                headers = curl_slist_append(headers, m_pRequstParam->request_headers->headers[i].data);
            }
        }

        //请求头...
        if (headers)
            curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, headers);
    }

    void RequestOperateImp::SetRequestOpt()
    {
        if (m_pRequstParam == nullptr)
            return;

        if (m_pRequstParam->request_host != nullptr)
        {
            //("Request[%d] Host Param:%s\n", m_nTaskID, m_pRequstParam->request_host);

            curl_easy_setopt(m_pCurl, CURLOPT_URL, m_pRequstParam->request_host);
        }

        //增加预设的 DNS解析地址，避免DNS无法解析域名 loki 2018-11-20
        curl_easy_setopt(m_pCurl, CURLOPT_DNS_SERVERS, s_defaultDNSIP.c_str());

        //("Request[%d] Type Param:%d\n", m_nTaskID, m_pRequstParam->request_type);
        if (m_pRequstParam->request_param != 0)
            ;// ("Request[%d] Post Param:%s\n", m_nTaskID, m_pRequstParam->request_param);
        else
            ;// ("Request[%d] Post Param:null\n", m_nTaskID);

        switch (m_pRequstParam->request_type)
        {
        case REQUEST_TYPE_PUT:
        {
            /* PUT 数据 */
            curl_easy_setopt(m_pCurl, CURLOPT_CUSTOMREQUEST, "PUT");
        }
        case REQUEST_TYPE_POST:
        {
            /* POST 数据 */
            if (m_pRequstParam->request_param == nullptr)
            {
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, "");
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, 0);
            }
            else
            {
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, m_pRequstParam->request_param);
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, strlen(m_pRequstParam->request_param));
            }
            break;
        }
        case REQUEST_TYPE_DELETE:
        {
            /* DELETE 数据 */
            curl_easy_setopt(m_pCurl, CURLOPT_PUT, 0L); //关闭PUT请求设置
            curl_easy_setopt(m_pCurl, CURLOPT_CUSTOMREQUEST, "DELETE");
            if (m_pRequstParam->request_param == nullptr)
            {
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, "");
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, 0);
            }
            else
            {
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, m_pRequstParam->request_param);
                curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, strlen(m_pRequstParam->request_param));
            }
        }
        break;
        default:
        {
            /* GET 数据 */
            break;
        }
        }
    }

    void RequestOperateImp::SetRequestParam()
    {
        if (m_pRequstParam == nullptr)
            return;

        static std::string useragent = "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1";

        if (m_pRequstParam->timeout != 0)
        {
            ;// ("Request[%d] Param TimeOut:%d\n", m_nTaskID, m_pRequstParam->timeout);
            curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, m_pRequstParam->timeout);
            //curl_easy_setopt( m_pCurl, CURLOPT_TIMEOUT_MS, m_pRequstParam->timeout );   //毫秒
            //curl_easy_setopt( m_pCurl, CURLOPT_CONNECTTIMEOUT, m_pRequstParam->timeout );
        }

        static CURLSH *s_pShare_handler = nullptr;
        if (!s_pShare_handler)
        {
            s_pShare_handler = curl_share_init();
            curl_share_setopt(s_pShare_handler, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        }
        curl_easy_setopt(m_pCurl, CURLOPT_SHARE, s_pShare_handler);

        curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, useragent.c_str());
        // 官方下载的DLL并不支持GZIP，Accept-Encoding:deflate, gzip  
        //curl_easy_setopt(m_pCurl, CURLOPT_ENCODING, "gzip, deflate");    

        //发送cookie值给服务器  
        //curl_easy_setopt(m_pCurl, CURLOPT_COOKIE, "name1=var1; name2=var2;");   
        /* 与服务器通信交互cookie，默认在内存中，可以是不存在磁盘中的文件或留空 */
        //curl_easy_setopt(m_pCurl, CURLOPT_COOKIEFILE, "./cookie.txt");   
        /* 与多个CURL或浏览器交互cookie，会在释放内存后写入磁盘文件 */
        //curl_easy_setopt(m_pCurl, CURLOPT_COOKIEJAR, "./cookie.txt");   

        //设置重定向的最大次数  
        curl_easy_setopt(m_pCurl, CURLOPT_MAXREDIRS, 10);
        //设置301、302跳转跟随location  
        curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);
        //抓取内容后，回调函数  
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, Respond_Data_Callback);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);
        //抓取头信息，回调函数  
        curl_easy_setopt(m_pCurl, CURLOPT_HEADERFUNCTION, Header_Callback);
        curl_easy_setopt(m_pCurl, CURLOPT_HEADERDATA, this);

        if (m_pRequstParam->request_protocol == REQUEST_PROTOCOLTYPE_HTTPS)
        {
            //https 访问专用：start
            //跳过服务器SSL验证，不使用CA证书  
            curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
            //如果不跳过SSL验证，则可指定一个CA证书目录  
            //curl_easy_setopt(m_pCurl, CURLOPT_CAPATH, "this is ca ceat");  
            //验证服务器端发送的证书，默认是 2(高)，1（中），0（禁用）  
            curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
            //https 访问专用：end  
        }
    }

    uint32 RequestOperateImp::Excute()
    {
        if (m_pRequstParam == nullptr)
            return -1;

        if (m_pCurl)
        {
            //增加定向IP设置， loki 2019-05-06
            UrlResolveUtil util(m_pCurl, m_pRequstParam->request_host, m_nTaskID);

            uint32 retCode = curl_easy_perform(m_pCurl);

            ;// ("Request[%d] retCode:%ld.\n", m_nTaskID, retCode);
            /*
            CURLE_OK    任务完成一切都好
            CURLE_UNSUPPORTED_PROTOCOL  不支持的协议，由URL的头部指定
            CURLE_COULDNT_CONNECT   不能连接到remote 主机或者代理
            CURLE_REMOTE_ACCESS_DENIED  访问被拒绝
            CURLE_HTTP_RETURNED_ERROR   Http返回错误
            CURLE_READ_ERROR    读本地文件错误
            CURLE_SSL_CACERT    访问HTTPS时需要CA证书路径
            */

            if (CURLE_OK == retCode)
            {
                m_respondBuffer.push_back('\0');
#ifdef _DEBUG_CURL
                double val;

                /* check for bytes downloaded */
                retCode = curl_easy_getinfo(m_pCurl, CURLINFO_SIZE_DOWNLOAD, &val);
                if ((CURLE_OK == retCode) && (val > 0))
                    FMC_LOG("Data downloaded: %0.0f bytes.\n", val);

                /* check for total download time */
                retCode = curl_easy_getinfo(m_pCurl, CURLINFO_TOTAL_TIME, &val);
                if ((CURLE_OK == retCode) && (val > 0))
                    FMC_LOG("Total download time: %0.3f sec.\n", val);

                /* check for average download speed */
                retCode = curl_easy_getinfo(m_pCurl, CURLINFO_SPEED_DOWNLOAD, &val);
                if ((CURLE_OK == retCode) && (val > 0))
                    FMC_LOG("Average download speed: %0.3f kbyte/sec.\n", val / 1024);
                FMC_LOG("Request[%d] Success :%s\n", m_nTaskID, m_szbuffer.c_str());
#endif
                ;//("Request[%d] Success :%s\n", m_nTaskID, &m_respondBuffer[0]);
            }

            return retCode;
        }

        return -1;
    }

    void RequestOperateImp::Done()
    {
        /* 释放内存 */
        curl_easy_cleanup(m_pCurl);
    }

    void RequestOperateImp::Header_Respond_Data(const char *ptr, size_t size)
    {
#ifdef _DEBUG_CURL
        m_szheader_buffer.append(ptr, size);
#endif

        if (m_pRequestTask)
            m_pRequestTask->Header_Respond_Data(ptr, size);
    }

    void RequestOperateImp::Respond_Data(const char *ptr, size_t size)
    {
#ifdef _DEBUG_CURL
        m_szbuffer.append(ptr, size);
#endif

        if (m_respondBufferSize + size > m_respondBuffer.capacity())
        {
            m_respondBuffer.resize(m_respondBufferSize * 2);
        }

        m_respondBuffer.insert(m_respondBuffer.begin() + m_respondBufferSize, ptr, ptr + size);
        //m_respondBuffer.assign(ptr,ptr + size); 
        m_respondBufferSize += size;

        if (m_pRequestTask)
            m_pRequestTask->Respond_Data(ptr, size);
    }

}