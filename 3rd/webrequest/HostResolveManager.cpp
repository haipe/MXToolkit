#include "stdafx.h"
#include "HostResolveManager.h"
#include <algorithm>

using mxtoolkit::uint32;

namespace mxwebrequest
{

    HostResolveManager::HostResolveManager(void)
    {
    }

    HostResolveManager::~HostResolveManager(void)
    {
    }

    HostResolveManager* HostResolveManager::GetInstance()
    {
        static HostResolveManager s_manager;

        return &s_manager;
    }

    bool HostResolveManager::Init()
    {
        m_hostResolve.clear();
        return true;
    }

    void HostResolveManager::Release()
    {
        m_hostResolve.clear();
    }

    bool HostResolveManager::AddHostResolve(const std::string& host, uint32 port, const std::string& ip)
    {
        if (host.empty() || port == 0 || ip.empty())
            return false;

        std::string realHost;
        uint32 po;
        if (!GetHost(host, realHost, po))
            return false;

        HostResolveMap::iterator findHost = m_hostResolve.find(realHost);
        if (findHost == m_hostResolve.end())
        {
            findHost = m_hostResolve.insert(std::make_pair(realHost, ResolveLinkMap())).first;
            if (findHost == m_hostResolve.end())
                return false;
        }

        ResolveLinkMap& resolveLinks = findHost->second;
        ResolveLinkMap::iterator findResolve = resolveLinks.find(port);
        if (findResolve == resolveLinks.end())
        {
            std::set<std::string> ips;
            ips.insert(ip);
            resolveLinks.insert(std::make_pair(port, ips));
            return true;
        }

        findResolve->second.insert(ip);
        return true;
    }

    void HostResolveManager::RemoveHostResolve(const std::string& host, uint32 port, const std::string& ip)
    {
        if (host.empty())
            return;

        std::string realHost;
        uint32 po;
        if (!GetHost(host, realHost, po))
            return;

        HostResolveMap::iterator findHost = m_hostResolve.find(realHost);
        if (findHost == m_hostResolve.end())
            return;

        if (port == 0)
        {
            //删除所有
            m_hostResolve.erase(findHost);
            return;
        }

        ResolveLinkMap& resolveLinks = findHost->second;
        ResolveLinkMap::iterator findResolve = resolveLinks.find(port);
        if (findResolve == resolveLinks.end())
        {
            return;
        }

        if (ip.empty())
        {
            //删除所有
            findHost->second.erase(findResolve);
            return;
        }

        //删除单个IP
        findResolve->second.erase(ip);
    }

    uint32 HostResolveManager::GetHostResolve(const std::string& host, ResolveLinkMap& resolve)
    {
        if (host.empty())
            return 0;

        HostResolveMap::iterator findHost = m_hostResolve.find(host);
        if (findHost == m_hostResolve.end())
            return 0;

        resolve = findHost->second;
        return resolve.size();
    }

    uint32 HostResolveManager::GetHostResolve(const std::string& host, uint32 port, StringSet& ips)
    {
        if (host.empty())
            return 0;

        HostResolveMap::iterator findHost = m_hostResolve.find(host);
        if (findHost == m_hostResolve.end())
            return 0;

        ResolveLinkMap& resolveLinks = findHost->second;
        ResolveLinkMap::iterator findResolve = resolveLinks.find(port);
        if (findResolve == resolveLinks.end())
            return 0;

        ips = findResolve->second;
        return ips.size();
    }

    uint32 HostResolveManager::GetUrlResolve(
        const std::string& url, std::string& host, uint32& port, StringSet& ips)
    {
        if (url.empty())
            return 0;

        if (!GetHost(url, host, port))
            return 0;

        GetHostResolve(host, port, ips);

        return ips.size();
    }

    bool HostResolveManager::MakeResolveLink(
        const std::string& host, uint32& port, const std::string& ip, std::string& resolve)
    {
        resolve.clear();
        if (host.empty() || port == 0 || ip.empty())
            return false;

        resolve += host;
        resolve += ":";
		
		std::string port_str = std::to_string(port);
		resolve += port_str;

        resolve += ":";
        resolve += ip;

        return true;
    }

    bool HostResolveManager::MakeResolveLink(
        const std::string& host, uint32& port, const StringSet& ips, std::string& resolve)
    {
        resolve.clear();
        if (host.empty() || port == 0 || ips.empty())
            return false;

        resolve += host;
		resolve += ":";

		std::string port_str = std::to_string(port);
		resolve += port_str;

        resolve += ":";

        StringSet::const_iterator it = ips.begin();
        resolve += *it;
        while (1)
        {
            it++;
            if (it == ips.end())
                break;

            resolve += ",";
            resolve += *it;
        }

        return true;
    }

    void HostResolveManager::SetUrlResolve(const std::string& url, const std::string& resolve)
    {
        if (url.empty())
            return;

        std::string host;
        uint32 port;
        if (!GetHost(url, host, port))
            return;

        HostResolveLinkMap::iterator findHost = m_needResolveHost.find(host);
        if (findHost == m_needResolveHost.end())
        {
            findHost = m_needResolveHost.insert(std::make_pair(host, PortAndResolveLink())).first;
            if (findHost == m_needResolveHost.end())
                return;
        }

        PortAndResolveLink& resolveLinks = findHost->second;
        PortAndResolveLink::iterator findResolve = resolveLinks.find(port);
        if (findResolve == resolveLinks.end())
            resolveLinks.insert(std::make_pair(port, resolve));
        else
            findResolve->second = resolve;
    }

    bool HostResolveManager::CheckUrlResolve(const std::string& url, std::string& resolve)
    {
        std::string host;
        uint32 port;
        if (!GetHost(url, host, port))
            return false;

        HostResolveLinkMap::iterator findHost = m_needResolveHost.find(host);
        if (findHost == m_needResolveHost.end())
        {
            return false;
        }

        PortAndResolveLink::iterator findLink = findHost->second.find(port);
        if (findLink == findHost->second.end() || findLink->second.empty())
            return false;

        resolve = findLink->second;
        return true;
    }

    bool HostResolveManager::GetHost(const std::string& url, std::string& host, uint32& port)
    {
        if (url.empty())
            return false;

        //转成小写
        std::string lowUrl = url;
        std::transform(lowUrl.begin(), lowUrl.end(), lowUrl.begin(), ::tolower);

        int beginPos = lowUrl.find("//");
        bool isHttps = lowUrl.find("https") != std::string::npos;

        if (beginPos == std::string::npos)
            beginPos = 0;
        else
            beginPos += 2;

        int endPos = lowUrl.find('/', beginPos);
        if (endPos == std::string::npos)
            endPos = lowUrl.length();

        host = lowUrl.substr(beginPos, endPos - beginPos);
        port = isHttps ? 443 : 80;

        return !host.empty();
    }

    bool VerifyUrlResolveUtil::supportMultiResolve = false;

    VerifyUrlResolveUtil::VerifyUrlResolveUtil(const std::string& url)
        : port(0)
        , strUrl(url)
    {
        resolveLinksIterrator = resolveLinks.end();
    }

    void VerifyUrlResolveUtil::BuildResolveLinks()
    {
        if (!resolveLinks.empty())
            return;

        HostResolveManager::GetInstance()->GetUrlResolve(strUrl, host, port, resolveLinks);
        resolveLinksIterrator = resolveLinks.begin();
    }

    bool VerifyUrlResolveUtil::HasResolveLink()
    {
        BuildResolveLinks();

        if (!resolveLinks.empty() && resolveLinksIterrator != resolveLinks.end())
        {
            std::string resolveLink;	//定向字符串
            if (VerifyUrlResolveUtil::supportMultiResolve)
            {
                HostResolveManager::GetInstance()->MakeResolveLink(host, port, resolveLinks, resolveLink);
                resolveLinksIterrator = resolveLinks.end();
            }
            else
            {
                HostResolveManager::GetInstance()->MakeResolveLink(host, port, *resolveLinksIterrator, resolveLink);
                resolveLinksIterrator++;
            }

            HostResolveManager::GetInstance()->SetUrlResolve(strUrl, resolveLink);
            return true;
        }

        return false;
    }

    void VerifyUrlResolveUtil::ResetUrlResolve()
    {
        HostResolveManager::GetInstance()->SetUrlResolve(strUrl, "");
    }


}