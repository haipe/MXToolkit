// MXFileManager.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MXFileManager.h"
#include <string>
#include <curl/curl.h>

#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"libcurl.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Usp10.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"DbgHelp.lib")
#pragma comment(lib,"openssl.lib")
#pragma comment(lib,"zlib.lib")


#include <tchar.h>

#include "fcntl.h"
#include "io.h"

#include "OperateUnzip.h"
#include "MXStringKit.h"

void InitConsoleWindow()
{
    int nCrt = 0;
    AllocConsole();

    FILE* fp = freopen("CONOUT$", "w", stdout);

    //nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    //fp = _fdopen(nCrt, "w");
    //*stdout = *fp;
    //setvbuf(stdout, NULL, _IONBF, 0);
}

struct OperateFileInfo
{
    std::string download_url;   //下载地址
    std::string download_file;  //下载文件
    std::string unzip_file;     //解压文件

    FILE *download_stream = nullptr;
};

int DownloadFile(OperateFileInfo& info);
int UnzipFile(OperateFileInfo& info);

// 全局变量:
HINSTANCE hInst;                                // 当前实例
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //MessageBoxW(NULL, lpCmdLine, lpCmdLine, MB_OK);

    DWORD startTick = GetCurrentTime();

    // TODO: 在此处放置代码。    
    std::string cmdUrl;
    mxtoolkit::WUtf8Convert<std::wstring, std::string>(lpCmdLine, &cmdUrl);

#ifdef _DEBUG
    if (cmdUrl.empty())
    {
        //cmdUrl = "https://moooxin.gitee.io/MXApps/qt/5.13.2/D3Dcompiler_47.dll";
        cmdUrl = "https://moooxin.gitee.io/MXApps/miniblink/node.dll";
    }
        
#endif

    if (cmdUrl.empty())
        return 0;

#ifdef _DEBUG
    bool bDebug = true;

#else
    bool bDebug = false;

    std::vector<std::string> cmds;
    if (mxtoolkit::SplitString<std::string>(cmdUrl, " ", &cmds) <= 0)
        return 0;

    for (auto item : cmds)
    {
        bDebug = (item == "debug");
        if (bDebug)
            break;
    }

    cmdUrl = cmds[0];
#endif

    if(bDebug)
        InitConsoleWindow();
    
    OperateFileInfo info;
    info.download_url = cmdUrl;

    DownloadFile(info);
    
    printf("download completed!\n");

    UnzipFile(info);

    printf("unzip completed!\n");

    DWORD endTick = GetCurrentTime();

    if (bDebug)
    {
        DWORD runTime = endTick - startTick;
        std::string useTime = "一共用时 [";
        useTime += std::to_string(runTime / 1000);
        useTime += "] 秒.";

        printf("%s\n",useTime.c_str());
        MessageBoxA(NULL, useTime.c_str(), "MXFileManager", MB_OK);
    }
    else
    {
        DeleteFileA(info.download_file.c_str());
    }
    
    return (int) 0;
}

int UnzipFile(OperateFileInfo& info)
{
    info.unzip_file = info.download_file.substr(0, info.download_file.find_last_of("\\") + 1);
    OperateUnzip::Unzip(info.download_file.c_str(), info.unzip_file.c_str(), "");
    info.unzip_file += "\\";
    info.unzip_file += info.download_url.substr(info.download_url.find_last_of("/") + 1);

    return 0;
}

size_t WriteToFile(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct OperateFileInfo *out = (struct OperateFileInfo *)stream;
    if (out && !out->download_stream)
    {
        /* open file for writing */
        out->download_stream = fopen(out->download_file.c_str(), "wb");
        if (!out->download_stream)
            return -1; /* failure, can‘t open file to write */
    }

    return fwrite(buffer, size, nmemb, out->download_stream);
}

int DownloadFile(OperateFileInfo& info)
{
    CURL *curl;
    CURLcode res;

    CHAR exePath[256] = { 0 };
    GetModuleFileNameA(nullptr, exePath, sizeof(exePath));
    std::string filePath = exePath;
    filePath = filePath.substr(0, filePath.find_last_of('\\'));

    DWORD tick = GetCurrentTime();
    filePath += "\\_temp[";
    filePath += std::to_string(tick);
    filePath += "].mx";

    info.download_file = filePath;
    info.download_stream = nullptr;

    printf("download url:%s.\n", info.download_url.c_str());
    printf("download file:%s.\n", info.download_file.c_str());

    curl_global_init(CURL_GLOBAL_ALL);
    /* get a curl handle */
    curl = curl_easy_init();
    if (!curl)
    {
        return -1;
    }

    /*设置easy handle属性*/
    /* specify URL */
    curl_easy_setopt(curl, CURLOPT_URL, info.download_url.c_str());
    /* Define our callback to get called when there‘s data to be written */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFile);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &info);

    /* set commomoption */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 20);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    /* get verbose debug output please */

    /*执行数据请求*/
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    // 释放资源
    if (info.download_stream)
    {
        fclose(info.download_stream); /* close the local file */
        info.download_stream = nullptr;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}