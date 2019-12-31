#pragma once
#include<thread>

struct StartupAppInfo
{
    std::string appName;
    std::string appFile;
    std::string appVersion;

    std::vector<std::string> appHosts;
};

struct AppFile
{
    std::string library;
    std::string libraryVersion;
    std::string fileMD5;
    std::string fileName;
};

typedef std::vector<AppFile> AppFileList;

struct AppRemoteFile
{
    std::string appVersion;
    std::string appName;
    AppFileList files;
};

typedef std::vector<AppRemoteFile> AppRemoteFileList;

class GetApp
{
public:
    ~GetApp();

    bool Start(const std::string& app);

protected:
    void GetAppProcessor();

protected:
    std::thread getAppThread_;

    static std::vector<std::string> hosts_;//下载地址集合
};

