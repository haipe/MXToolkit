// MXStartup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MXStartup.h"
#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "MXLibxml.h"
#include "Win32FileCertUtil.h"
#include "Win32ConsoleWindow.h"

#pragma comment(lib,"libxml.lib")

#ifdef _DEBUG
#pragma comment(lib,"DuiLib_StaticD.lib")

#else
#pragma comment(lib,"DuiLib_Static.lib")

#endif

#include "MainWindow.h"


#define MAX_LOADSTRING 100
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
std::string g_appDir_a;
std::wstring g_appDir_w;

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


int Execute(const std::string& filePath, const std::string& runParam = "", DWORD waitTimeout = INFINITE);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    // 初始化全局字符串 =======================================================================
#ifdef _DEBUG
    mxtoolkit::InitConsoleWindow();
#endif
    hInst = hInstance;
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MXSTARTUP, szWindowClass, MAX_LOADSTRING);
        
    CHAR path[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, path, MAX_PATH);
    g_appDir_a = path;
    g_appDir_a = g_appDir_a.substr(0, g_appDir_a.find_last_of('\\') + 1);
    mxtoolkit::WAConvert<std::string, std::wstring>(g_appDir_a.c_str(), &g_appDir_w);

    //显示UI =============================
    DuiLib::CPaintManagerUI::SetInstance(hInstance);
    DuiLib::CPaintManagerUI::SetResourceDll(hInstance);

    DuiLib::CPaintManagerUI::SetResourcePath(g_appDir_w.c_str());
    MainWindow wnd;
    wnd.SetNotify(dynamic_cast<DuiLib::INotifyUI*>(&wnd));
    if (wnd.Create(NULL, szWindowClass, UI_WNDSTYLE_DIALOG | WS_MINIMIZEBOX, WS_EX_WINDOWEDGE))
    {
        wnd.CenterWindow();
        wnd.ShowWindow();
    }

    //判断是否有 startup.xml，没有就解压 =======================================================================
    std::string startupXml = g_appDir_a + "\\startup.xml";
    if(!mxtoolkit::FileExist(startupXml))
    {
        //从资源里面解压
        void* xmlBuffer;
        int xmlSize = 0;
        if (mxtoolkit::LoadResource(L"XML", IDR_XML_STARTUP, &xmlBuffer, &xmlSize))
        {
            std::fstream f = std::fstream(startupXml.c_str(), std::ios::binary | std::ios::out);
            f.write((const char*)xmlBuffer, xmlSize);
            f.close();
        }
    }

    //加载startup.xml 获取要启动的应用信息=======================================================================
    StartupAppInfo appInfo;
    do
    {
        /*****************打开xml文档********************/
        //必须加上，防止程序把元素前后的空白文本符号当作一个node
        xmlKeepBlanksDefault(0);
        xmlDocPtr xmlDoc = xmlReadFile(startupXml.c_str(), "UTF-8", XML_PARSE_RECOVER);
        if (xmlDoc == NULL)
        {
            printf("error:can't open :%s!\n", startupXml.c_str());
            break;
        }

        /*****************获取xml文档对象的根节对象********************/
        xmlNodePtr rootNode = NULL;
        rootNode = xmlDocGetRootElement(xmlDoc);
        if (rootNode == NULL)
        {
            printf("error: file is empty!\n");
            break;
        }

        xmlChar *xpath = BAD_CAST("//Host"); //xpath语句
        mxtoolkit::LoadResult(xmlDoc, xpath, [&](xmlNodePtr node)
        {
            std::string host = (const char*)XML_GET_CONTENT(node->xmlChildrenNode);
            appInfo.appHosts.emplace_back(host);
            return true;//找多个
        });
        
        xpath = BAD_CAST("//App"); //xpath语句
        mxtoolkit::LoadResult(xmlDoc, xpath, [&](xmlNodePtr node)
        {
            appInfo.appName = (const char*)XML_GET_CONTENT(node->xmlChildrenNode);

            xmlChar* attrValue = NULL;
            attrValue = xmlGetProp(node, BAD_CAST("file"));
            if (attrValue)
            {
                appInfo.appFile = (const char*)attrValue;
                xmlFree(attrValue);
            }

            attrValue = xmlGetProp(node, BAD_CAST("version"));
            if (attrValue)
            {
                appInfo.appVersion = (const char*)attrValue;
                xmlFree(attrValue);
            }
            return false;//只找一个
        });

        /*****************释放资源********************/
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
    } while (0);
    
    //判断应用是否存在，如果存在，则直接启动 =======================================================================
    std::string appPath = g_appDir_a + appInfo.appName;
    appPath += "\\";
    appPath += appInfo.appVersion;
    appPath += "\\";
    appPath += appInfo.appFile;
    bool isAppExist = mxtoolkit::FileExist(appPath);
    if (isAppExist)
{
        int ret = Execute(appPath, "", 1000);//5秒
        //if (ret == 0)
            return 0;
    }

    //判断 MXFile.exe 文件是否存在，一样的 =======================================================================
    bool needUnzip = false;
    std::string mxFilePath = g_appDir_a + "MXFile.exe";
    unsigned int fileSize = 0;
    bool mxFileExist = mxtoolkit::FileExist(mxFilePath, &fileSize);
    unsigned int resourceFileSize = 0;
    void* resourceFileBuffer = nullptr;
    //解压成功，并且文件大小不一样，则解压覆盖
    if (mxtoolkit::LoadResource(L"E", IDR_E_FILEMANAGER, &resourceFileBuffer, &resourceFileSize) && resourceFileSize != fileSize)
    {
        std::fstream newFile(mxFilePath.c_str(), std::ios::binary | std::ios::out);
        if (newFile && newFile.is_open())
        {
            newFile.write((const char*)resourceFileBuffer, resourceFileSize);
            newFile.close();
        }
    }
    
    //下载app.xml 文件 =======================================================================
    std::string appXmlPath = g_appDir_a + appInfo.appName + "\\app.xml";
    std::string mxFileParam;// = "-d true";
    mxFileParam += (" -u " + appInfo.appHosts[0] + appInfo.appName + "/app.xml");
    mxFileParam += (" -l " + appXmlPath);

#ifdef _DEBUG
    if (!mxtoolkit::FileExist(appXmlPath))  //Debug的时候 ，不存在才下载
#endif
    {
        Execute(mxFilePath, mxFileParam);
    }
    //解析app.xml下载所有文件
    AppRemoteFileList appRemoteFileList;
    do 
    {
        xmlKeepBlanksDefault(0);
        xmlDocPtr xmlDoc = xmlReadFile(appXmlPath.c_str(), "UTF-8", XML_PARSE_RECOVER);
        if (xmlDoc == NULL)
        {
            printf("error:can't open :%s!\n", startupXml.c_str());
            break;
        }

        /*****************获取xml文档对象的根节对象********************/
        xmlNodePtr rootNode = NULL;
        rootNode = xmlDocGetRootElement(xmlDoc);
        if (rootNode == NULL)
        {
            printf("error: file is empty!\n");
            break;
        }

        //xmlChar *xpath = BAD_CAST("//App[@name=\"MXWasai.exe\" and @version=\"1.0.0.0\"]//File[text()='node.dll']"); //xpath语句    and File[text()='node.dll']
        xmlChar *xpath = BAD_CAST("//App"); //xpath语句    and File[text()='node.dll']
        mxtoolkit::LoadResult(xmlDoc, xpath, [&](xmlNodePtr appNode)
        {
            AppRemoteFile appRemoteFile;

            xmlChar* attrValue = NULL;
            attrValue = xmlGetProp(appNode, BAD_CAST("version"));
            if (attrValue)
            {
                appRemoteFile.appVersion = (const char*)attrValue;
                xmlFree(attrValue);
            }

            attrValue = xmlGetProp(appNode, BAD_CAST("name"));
            if (attrValue)
            {
                appRemoteFile.appName = (const char*)attrValue;
                xmlFree(attrValue);
            }
            
            xmlNodePtr fileNode = appNode->xmlChildrenNode;
            while (fileNode)
            {
                if (xmlStrcmp(fileNode->name, (const xmlChar *)"File") == 0)
                {
                    AppFile file;
                    const char* content = (const char*)XML_GET_CONTENT(fileNode->xmlChildrenNode);
                    if (content)
                        file.fileName = content;

                    attrValue = xmlGetProp(fileNode, BAD_CAST("library"));
                    if (attrValue)
                    {
                        file.library = (const char*)attrValue;
                        xmlFree(attrValue);
                    }

                    attrValue = xmlGetProp(fileNode, BAD_CAST("version"));
                    if (attrValue)
                    {
                        file.libraryVersion = (const char*)attrValue;
                        xmlFree(attrValue);
                    }

                    attrValue = xmlGetProp(fileNode, BAD_CAST("md5"));
                    if (attrValue)
                    {
                        file.fileMD5 = (const char*)attrValue;
                        xmlFree(attrValue);
                    }

                    appRemoteFile.files.emplace_back(file);
                }

                fileNode = fileNode->next;
            }

            appRemoteFileList.emplace_back(appRemoteFile);
            return true;
        });

        
        /*****************释放资源********************/
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
    } while (0);

    // 下载最后一个
    while (!appRemoteFileList.empty())
    {
        AppRemoteFile& appRemoteFile = appRemoteFileList[appRemoteFileList.size() - 1];

        std::string baseDir = g_appDir_a + appInfo.appName + "\\" + appRemoteFile.appVersion + "\\";

        std::string appDownloadPath = baseDir + appInfo.appFile;

        std::string fileParam;
        //fileParam += "-d true";
        //fileParam += " -c true";
        fileParam += (" -u " + appInfo.appHosts[0] + appInfo.appName + "/" + appInfo.appVersion + "/" + appInfo.appFile);
        //fileParam += (" -m " + item.fileMD5);
        fileParam += (" -l " + appDownloadPath);

        Execute(mxFilePath, fileParam);

        for (auto item : appRemoteFile.files)
        {
            std::string fileDownloadPath = baseDir + item.fileName;
            
            fileParam.clear();
            //fileParam += "-d true";
            fileParam += " -c true";
            fileParam += (" -u " + appInfo.appHosts[0] + item.library + "/" + item.libraryVersion + "/" + item.fileName);
            //fileParam += (" -m " + item.fileMD5);
            fileParam += (" -l " + fileDownloadPath);
            
            Execute(mxFilePath, fileParam);
        }
        
        Execute(appDownloadPath, "", 0);
        break;
    }
    //消息
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        if (msg.message == WM_QUIT)
            break;
    }

    return 0;
}

int Execute(const std::string& filePath, const std::string& runParam, DWORD waitTimeout)
{
    DWORD exitCode = 0;
    PROCESS_INFORMATION pInfo = { 0 };
    STARTUPINFOA        sInfo = { 0 };
    sInfo.cb = sizeof(STARTUPINFO);
    sInfo.wShowWindow = SW_SHOW;

    std::string cmd = filePath + " ";
    cmd += runParam;

    if (CreateProcessA(
        NULL,      //LPCTSTR lpApplicationName, // pointer to name of executable module
        (LPSTR)cmd.c_str(),   //LPTSTR lpCommandLine,  // pointer to command line string
        NULL,      //LPSECURITY_ATTRIBUTES lpProcessAttributes,  // process security attributes
        NULL,      //LPSECURITY_ATTRIBUTES lpThreadAttributes,   // thread security attributes
        FALSE,     //BOOL bInheritHandles,  // handle inheritance flag
        0,         //DWORD dwCreationFlags, // creation flags
        NULL,      //LPVOID lpEnvironment,  // pointer to new environment block
        NULL,      //LPCTSTR lpCurrentDirectory,   // pointer to current directory name
        &sInfo,    //LPSTARTUPINFO lpStartupInfo,  // pointer to STARTUPINFO
        &pInfo))    //LPPROCESS_INFORMATION lpProcessInformation  // pointer to PROCESS_INFORMATION
    {
        if (waitTimeout != 0)
        {
            // Wait until child process exits.
            WaitForSingleObject(pInfo.hProcess, waitTimeout);

            if (GetExitCodeProcess(pInfo.hProcess, &exitCode))
            {
                printf("Exit code = %d/n", exitCode);
            }
            else
            {
                printf("GetExitCodeProcess() failed: %ld/n", GetLastError());
                ASSERT(0);
            }
        }

        // Close process and thread handles. 
        CloseHandle(pInfo.hProcess);
        CloseHandle(pInfo.hThread);
    }
    else
    {
        printf("CreateProcess() failed: %ld/n", GetLastError());
        ASSERT(0);
    }

    return exitCode;
}