#pragma once
#include<thread>

class GetApp
{
public:
    bool Start(const std::string& app);

protected:
    static std::vector<std::string> hosts_;//下载地址集合
};

