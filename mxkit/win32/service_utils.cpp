#include "service_utils.h"
#include <list>
#include <vector>

#include "win32/cmd_utils.h"
#include "base/string_utils.h"

_BEGIN_MX_KIT_NAME_SPACE_

bool ServiceUtils::Start(const std::string& name)
{
    std::string cmd = "sc start " + name;
    std::string result;
    RunCommond::Run(cmd, &result);

    std::list<std::string> res;
    if (SplitString<std::string, std::list<std::string>>(result, "\n", &res, true) > 0)
    {
        return true;
    }

    return false;
}

bool ServiceUtils::Stop(const std::string& name)
{
    std::string cmd = "sc stop " + name;
    std::string result;
    RunCommond::Run(cmd, &result);

    std::list<std::string> res;
    if (SplitString<std::string, std::list<std::string>>(result, "\n", &res, true) > 0)
    {
        return true;
    }

    return false;
}

bool ServiceUtils::Config(const std::string& name, const std::string& cfg)
{
    std::string cmd = "sc config " + name + " " + cfg;
    std::string result;
    RunCommond::Run(cmd, &result);

    return true;
}

bool ServiceUtils::Status(const std::string& name, StatusMap& status)
{
    std::string cmd = "sc query " + name;
    std::string result;
    RunCommond::Run(cmd, &result);

    std::list<std::string> res;
    if (SplitString<std::string, std::list<std::string>>(result, "\r\n", &res, true) > 0)
    {
        std::list<std::string>::iterator it = res.begin();
        for (; it != res.end(); it++)
        {
            std::vector<std::string> s;
            if (SplitString<std::string, std::vector<std::string>>(*it, ":", &s, true) > 0)
            {
                std::string key = ReplaceString<std::string>(s[0], " ", "");
                if (s.size() > 1)
                    status[key] = s[1];
                else
                    status[key] = "";
            }
        }

        return true;
    }

    return false;
}

_END_MX_KIT_NAME_SPACE_