#pragma once
#include <functional>
#include <string>
#include <list>


class AnonymousPip
{
public:
    AnonymousPip();
    ~AnonymousPip();

    static bool CreateCmd(const std::string& cmd, std::function<void(const std::string& result)> onResult);

    template<typename T>
    static bool CreateCmd(const std::string& cmd, typename T& result)
    {
        return CreateCmd(cmd, [&result](const std::string& res) {
            result.push_back(res);
        });
    }
};

