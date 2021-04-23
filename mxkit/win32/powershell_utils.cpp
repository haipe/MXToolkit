
#include "powershell_utils.h"

#include <strstream>

#include <io.h>

#include <process.h>

#include "base/thread.h"

#include "base/string_utils.h"
#include "base/string_convert.h"
#include "base/path_utils.h"

#include "win32/file_utils.h"

#define MX_PRINT(x) std::cout << x

#define MX_DEBUG(x) ;//MX_PRINT(x)

#define MX_STR_PATH "Path"
#define MX_STR_PUBLISHER "Publisher"
#define MX_STR_HASH "Hash"
#define MX_STR_APPX "AppX"

_BEGIN_MX_KIT_NAME_SPACE_

PowerShellUtils::PowerShellUtils()
    : m_thread(nullptr)
    , m_completed(true)
{
}

PowerShellUtils::~PowerShellUtils()
{
    if (m_thread)
    {
        m_thread->Wait();
        delete m_thread;
        m_thread = nullptr;
    }

    CmdWithAnonymousPip::CloseCmdProcess(m_process);
}

std::string PowerShellUtils::GetFileInfoCmd(bool ps, const std::string& file)
{
    std::strstream ss;
    ss
        << (ps ? "powershell " : "")
        << "Import-Module AppLocker ; " << (ps ? "" : "\r\n")
        << "Get-AppLockerFileInformation \"'" << file << "' | Format-List\";" << (ps ? "" : "\r\n")
        //<< "exit;"
        << '\0';

    return ss.str();
}

bool PowerShellUtils::GetFileInfo(const std::string& file, FileInfo* info)
{
    if (m_thread || file.empty())
        return false;

    if (::GetFileAttributesA(file.c_str()) == -1)
        return false;

    m_completed = false;
    m_result = false;

    struct GetFileInfoThread : public Thread
    {
        virtual uint32 Process() override
        {
            if (ps && info)
                ps->GetFileInfoImplement(file, info);

            return 0;
        }

        PowerShellUtils* ps;
        std::string file;
        PowerShellUtils::FileInfo* info;
    };

    GetFileInfoThread* g = new GetFileInfoThread;
    m_thread = g;

    g->ps = this;
    g->file = file;
    g->info = info;

    m_thread->Start();
    return true;
}

bool PowerShellUtils::GetFileInfoImplement(const std::string& file, FileInfo* info)
{
    m_result = false;

    bool can_import = false;
    bool can_get = false;
    bool get_completed = false;

    bool wait_write = false;
    do
    {
        std::string cmd = GetFileInfoCmd(true, file);
        MX_DEBUG(cmd << std::endl);

        std::string result;
        RunCommond::Run(cmd, &result, &m_completed);

        std::string pathStr, publisherStr, hashStr, appXStr;
        std::vector<std::string> resultList;

        if (!result.empty())
        {
            MX_DEBUG(result << std::endl);

            pathStr.clear();
            publisherStr.clear();
            hashStr.clear();
            appXStr.clear();

            StringUtils::Split<std::string>(result, "\r\n", &resultList, true);
            if (resultList.empty())
                return false;

            //遍历，如果不是 : 间隔的，把后面的加到前面取
            auto next = resultList.begin();
            auto current = next++;
            std::string eStr;
            for (; next != resultList.end(); current++, next++)
            {
                if (eStr.empty())
                {
                    size_t pos = current->find(" : ");
                    if (pos != std::string::npos)
                    {
                        pos += strlen(" : ");
                        eStr = std::string(pos, ' ');

                        MX_DEBUG("eStr :" << eStr << ", pos :" << pos << std::endl);
                    }
                }

                std::vector<std::string> vec;
                StringUtils::Split<std::string>(*next, " : ", &vec);
                if (vec.empty())
                    continue;

                //if (vec[0].find("Path") != 0 && vec[0].find("Publisher") != 0 && vec[0].find("Hash") != 0 && vec[0].find("AppX") != 0)
                size_t pos = vec[0].find(eStr);
                MX_DEBUG("find eStr :" << pos << std::endl);
                if (pos == 0)
                {
                    StringUtils::Replace<std::string>(*next, eStr, "");

                    std::string s = *current;
                    *current = s + *next;
                    MX_DEBUG("replace current :" << *current << std::endl);

                    current--;
                    auto er = next--;
                    resultList.erase(er);
                }
            }

            for (size_t i = 0; i < resultList.size(); i++)
            {
                std::string& s = resultList[i];
                if (s.empty())
                    continue;

                MX_DEBUG("line ->" << s << std::endl);

                std::vector<std::string> vec;
                StringUtils::Split<std::string>(s, " : ", &vec);
                if (vec.empty() || vec.size() > 2)
                    continue;

                if (pathStr.empty() && vec[0].find(MX_STR_PATH) == 0)
                {
                    MX_DEBUG("Path str :" << s << std::endl);
                    if (vec.size() == 2)
                        pathStr = vec[1];
                }
                else if (publisherStr.empty() && vec[0].find(MX_STR_PUBLISHER) == 0)
                {
                    MX_DEBUG("Publisher str :" << s << std::endl);
                    if (vec.size() == 2)
                        publisherStr = vec[1];
                }
                else if (hashStr.empty() && vec[0].find(MX_STR_HASH) == 0)
                {
                    MX_DEBUG("Hash str :" << s << std::endl);
                    if (vec.size() == 2)
                        hashStr = vec[1];
                }
                else if (appXStr.empty() && vec[0].find(MX_STR_APPX) == 0)
                {
                    MX_DEBUG("AppX str :" << s << std::endl);
                    if (vec.size() == 2)
                        appXStr = vec[1];
                }
            }
        }

        MX_PRINT(std::endl);

        if (!pathStr.empty())
        {
            MX_DEBUG("pathStr :" << pathStr << std::endl);
            MX_PRINT("path :" << pathStr << std::endl);

            if (info)
                info->path = pathStr;
        }

        if (!publisherStr.empty())
        {
            MX_DEBUG("publisherStr :" << publisherStr << std::endl);
            std::vector<std::string> publisher;
            StringUtils::Split<std::string>(publisherStr, "\\", &publisher);
            if (publisher.size() == 3)
            {
                std::vector<std::string> fileInfo;
                StringUtils::Split<std::string>(publisher[2], ",", &fileInfo);
                if (fileInfo.size() == 2)
                {
                    MX_PRINT("publisher : " << publisher[0] << std::endl <<
                        "product : " << publisher[1] << std::endl <<
                        "file name : " << fileInfo[0] << std::endl <<
                        "file version : " << fileInfo[1] << std::endl);

                    if (info)
                    {
                        info->info.publisher = publisher[0];
                        info->info.product = publisher[1];
                        info->info.name = fileInfo[0];
                        info->info.version = fileInfo[1];
                    }
                }
            }
        }

        if (!hashStr.empty())
        {
            MX_DEBUG("hashStr :" << hashStr << std::endl);

            std::vector<std::string> ve;
            StringUtils::Split<std::string>(hashStr, "SHA256", &ve);

            if (ve.size() == 2)
            {
                m_result = true;

                std::string hs = ve[1];
                StringUtils::Replace<std::string>(hs, " ", "");
                MX_PRINT("hash : " << hs << std::endl);

                if (info)
                {
                    info->feature.type = "SHA256";
                    PathUtils::FileInfo<std::string>(file, nullptr, &info->feature.source);
                    info->feature.hash = hs;
                }
            }
        }

        if (!appXStr.empty())
        {
            MX_DEBUG("appXStr :" << appXStr << std::endl);

            bool isAppX = appXStr.find("False") == std::string::npos;
            MX_PRINT("appx : " << isAppX << std::endl);
            if (info)
                info->appx = isAppX;
        }

        CmdWithAnonymousPip::CloseCmdProcess(m_process);

    } while (0);

    m_completed = true;
    return m_result;
}

std::string PowerShellUtils::SetAppLockerPolicyCmd(bool ps, const std::list<std::string>& file, bool merge /*= true*/)
{
    std::strstream ss;
    ss
        << (ps ? "powershell " : "")
        << "Import-Module AppLocker ; " << (ps ? "" : "\r\n");

    for (std::list<std::string>::const_iterator it = file.begin(); it != file.end(); it++)
        ss << "Set-AppLockerPolicy " << (merge ? "-Merge " : "") << "-XmlPolicy '" << *it << "';" << (ps ? "" : "\r\n");

    ss << '\0';

    return ss.str();
}

bool PowerShellUtils::SetAppLockerPolicy(const std::string& file, bool merge)
{
    if (m_thread || file.empty())
        return false;

    if (::GetFileAttributesA(file.c_str()) == -1)
        return false;

    m_completed = false;
    m_result = false;

    struct SetALPThread : public Thread
    {
        virtual uint32 Process() override
        {
            if (ps)
                ps->SetAppLockerPolicyImplement(file, merge);

            return 0;
        }

        PowerShellUtils* ps;
        std::string file;
        bool merge;
    };

    SetALPThread* g = new SetALPThread;
    m_thread = g;

    g->ps = this;
    g->file = file;
    g->merge = merge;

    m_thread->Start();
    return true;
}

bool PowerShellUtils::SetAppLockerPolicyImplement(const std::string& file, bool merge /*= true*/)
{
    m_result = false;

    if (::GetFileAttributesA(file.c_str()) == -1)
        return false;

    std::list<std::string> ls(1, file);
    std::string cmd = SetAppLockerPolicyCmd(true, ls, merge);
    std::string result;

    RunCommond::Run(cmd, &result, &m_completed);
    m_result = result.empty();

    m_completed = true;
    return m_result;
}

bool PowerShellUtils::RunCmd(const std::string& cmd, std::string* result)
{
    if (m_thread || cmd.empty())
        return false;

    m_completed = false;
    m_result = false;

    struct RunCmdThread : public Thread
    {
        virtual uint32 Process() override
        {
            RunCommond::Run(cmd, result, &ps->m_completed);
            return 0;
        }

        PowerShellUtils* ps;
        const std::string& cmd;
        std::string* result;

        RunCmdThread(const std::string& c, std::string* r)
            : cmd(c)
            , result(r) {}
    };

    RunCmdThread* g = new RunCmdThread(cmd, result);
    m_thread = g;

    m_thread->Start();
    return true;
}

_END_MX_KIT_NAME_SPACE_