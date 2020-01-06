#pragma once
#include <string>
#include <map>
#include <set>

#include "MXCommon.h"

namespace mxwebrequest
{
    using mxtoolkit::uint32;

    typedef std::set<std::string>					StringSet;
    typedef std::map<mxtoolkit::uint32, StringSet>  ResolveLinkMap;
    typedef std::map<std::string, ResolveLinkMap>	HostResolveMap;
         

    class HostResolveManager
    {
    public:
        HostResolveManager(void);
        ~HostResolveManager(void);

        static HostResolveManager* GetInstance();

    public:
        bool Init();
        void Release();

        bool AddHostResolve(const std::string& host, uint32 port, const std::string& ip);
        void RemoveHostResolve(const std::string& host, uint32 port, const std::string& ip);

        uint32 GetHostResolve(const std::string& host, ResolveLinkMap& resolve);
        uint32 GetHostResolve(const std::string& host, uint32 port, StringSet& ips);

        uint32 GetUrlResolve(const std::string& url, std::string& host, uint32& port, StringSet& ips);

        bool MakeResolveLink(const std::string& host, uint32& port, const std::string& ip, std::string& resolve);
        bool MakeResolveLink(const std::string& host, uint32& port, const StringSet& ips, std::string& resolve);

        void SetUrlResolve(const std::string& url, const std::string& resolve);
        bool CheckUrlResolve(const std::string& url, std::string& resolve);

        bool GetHost(const std::string& url, std::string& host, uint32& port);

    private:

        HostResolveMap				m_hostResolve;	//全部的指定IP

        typedef						std::map<uint32, std::string> PortAndResolveLink;
        typedef						std::map<std::string, PortAndResolveLink> HostResolveLinkMap;
        HostResolveLinkMap			m_needResolveHost;	//验证需要指定IP的host集合
    };


    class VerifyUrlResolveUtil
    {
    public:
        VerifyUrlResolveUtil(const std::string& url);

        void BuildResolveLinks();

        bool HasResolveLink();

        void ResetUrlResolve();

        static bool supportMultiResolve;	//支持一次设置多个定向IP，curl 7.59后的版本支持

    private:
        std::string strUrl;			//请求url

        std::string host;			//url的域名
        uint32 port;					//定向端口	
        StringSet resolveLinks;		//请求URL拥有的定向IP集合
        StringSet::iterator resolveLinksIterrator;	//当前使用IP集合中的迭代器

    };



}