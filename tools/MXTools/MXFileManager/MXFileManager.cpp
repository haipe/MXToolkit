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

#include "MXCmdline.h"

#include "OperateUnzip.h"
#include "MXStringKit.h"

#include "Win32FileCertUtil.h"

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
    bool wasCompress = false;   //是否压缩
    std::string downloadUrl;    //下载地址
    std::string fileMD5;        //文件MD5校验
    std::string downloadFile;   //下载文件
    std::string unzipFile;      //解压文件

    FILE *downloadStream = nullptr;
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
    int returnValue = 999;
    bool bDebug = true;
    OperateFileInfo info;

goEnd:
    if (returnValue != 999)
    {
        if (bDebug)
        {
            DWORD endTick = GetCurrentTime();
            DWORD runTime = endTick - startTick;
            std::string useTime = "一共用时 [";
            useTime += std::to_string(runTime / 1000);
            useTime += "] 秒.";

            printf("%s\n", useTime.c_str());
            MessageBoxA(NULL, useTime.c_str(), "MXFileManager", MB_OK);
        }
        else
        {
            if(info.wasCompress)
                DeleteFileA(info.downloadFile.c_str());
        }

        return returnValue;
    }
        
#ifdef _DEBUG    
    //"-u https://moooxin.gitee.io/MXApps/miniblink/node.dll -d true -m dsadada";

#endif

    std::map<std::string, std::string> startupParam;
    mxtoolkit::GetStartupOptions("u:m:d:c:", &startupParam);

    bDebug = startupParam["d"].empty() ? false : true;    

    info.wasCompress = startupParam["c"].empty() ? false : true;
    info.downloadUrl = startupParam["u"];
    info.fileMD5 = startupParam["m"];

    if (bDebug)
        InitConsoleWindow();
    
    if (info.downloadUrl.empty())
    {
        printf("not have file param.\n");
        returnValue = -1;
        goto goEnd;
    }

    wprintf(L"cmd :%s\n", lpCmdLine);
    
    int ret = DownloadFile(info);
    if(ret != 0)
    {
        printf("download fail :%d\n", ret);
        returnValue = -2;
        goto goEnd;
    }

    printf("download completed!\n");

    if (!info.fileMD5.empty())
    {
        std::string md5 = mxtoolkit::GetMD5(info.downloadFile.c_str());
        if (md5 != info.fileMD5)
        {
            printf("file cert fail!\n");
            returnValue = -3;
            goto goEnd;
        }
    }

    if (info.wasCompress)
    {
        ret = UnzipFile(info);
        if (ret != 0)
        {
            printf("unzip fail :%d\n", ret);
            returnValue = -4;
            goto goEnd;
        }

        printf("unzip completed!\n");
    }

    returnValue = 0;
    goto goEnd;
}

int UnzipFile(OperateFileInfo& info)
{
    info.unzipFile = info.downloadFile.substr(0, info.downloadFile.find_last_of("\\") + 1);
    if (!OperateUnzip::Unzip(info.downloadFile.c_str(), info.unzipFile.c_str(), ""))
    {
        info.unzipFile.clear();
        return -1;
    }

    info.unzipFile += "\\";
    info.unzipFile += info.downloadUrl.substr(info.downloadUrl.find_last_of("/") + 1);
    return 0;
}

size_t WriteToFile(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct OperateFileInfo *out = (struct OperateFileInfo *)stream;
    if (out && !out->downloadStream)
    {
        /* open file for writing */
        out->downloadStream = fopen(out->downloadFile.c_str(), "wb");
        if (!out->downloadStream)
            return -1; /* failure, can‘t open file to write */
    }

    return fwrite(buffer, size, nmemb, out->downloadStream);
}

int DownloadFile(OperateFileInfo& info)
{
    CURL *curl;
    CURLcode res;

    CHAR exePath[256] = { 0 };
    GetModuleFileNameA(nullptr, exePath, sizeof(exePath));
    std::string filePath = exePath;
    filePath = filePath.substr(0, filePath.find_last_of('\\') + 1);

    if (info.wasCompress)
    {
        DWORD tick = GetCurrentTime();
        filePath += "_temp[";
        filePath += std::to_string(tick);
        filePath += "].mx";
    }
    else
    {
        filePath += info.downloadUrl.substr(info.downloadUrl.find_last_of('//') + 1);
    }

    info.downloadFile = filePath;
    info.downloadStream = nullptr;

    printf("download url:%s.\n", info.downloadUrl.c_str());
    printf("download file:%s.\n", info.downloadFile.c_str());

    curl_global_init(CURL_GLOBAL_ALL);
    /* get a curl handle */
    curl = curl_easy_init();
    if (!curl)
    {
        return -1;
    }

    /*设置easy handle属性*/
    /* specify URL */
    curl_easy_setopt(curl, CURLOPT_URL, info.downloadUrl.c_str());
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
    if (info.downloadStream)
    {
        fclose(info.downloadStream); /* close the local file */
        info.downloadStream = nullptr;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}