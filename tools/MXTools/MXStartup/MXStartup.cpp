// MXStartup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MXStartup.h"
#include <fstream>
#include <iostream>

#include "libxml/tree.h"

#include "MainWindow.h"

#pragma comment(lib,"libxml.lib")

#ifdef _DEBUG
#pragma comment(lib,"DuiLib_StaticD.lib")

#else
#pragma comment(lib,"DuiLib_Static.lib")

#endif

#define MAX_LOADSTRING 100
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
std::string g_appDir_a;
std::wstring g_appDir_w;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    // 初始化全局字符串
    hInst = hInstance;
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MXSTARTUP, szWindowClass, MAX_LOADSTRING);
        
    CHAR path[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, path, MAX_PATH);
    g_appDir_a = path;
    g_appDir_a = g_appDir_a.substr(0, g_appDir_a.find_last_of('\\') + 1);
    mxtoolkit::WAConvert<std::string, std::wstring>(g_appDir_a.c_str(), &g_appDir_w);
    
    //判断是否有Wasai app
    if (true)
    {
        std::string wasaiXml = g_appDir_a + "\\wasai.xml";
        std::fstream f(wasaiXml.c_str(), std::ios::in);
        if (!f.is_open())
        {
            //从资源里面解压
            std::string xmlStr;
            mxtoolkit::LoadResource(L"XML", IDR_XML_WASAI, &xmlStr);

            f = std::fstream(wasaiXml.c_str(), std::ios::binary | std::ios::out);
            f.write(xmlStr.c_str(), xmlStr.length());
            f.close();
        }

        f.close();
    }

    //先判断原有文件是否存在，一样的
    unsigned int fileSize = 0;
    bool needUnzip = mxtoolkit::LoadResource(L"E", IDR_E_FILEMANAGER, NULL, &fileSize);
    std::string mxFilePath = g_appDir_a + "MXFile.exe";
    do
    {
        if(!needUnzip)
            break;

        std::fstream fileMgr(mxFilePath.c_str(), std::ios::binary | std::ios::in);
        if (!fileMgr.is_open())
            break;

        fileMgr.seekp(0, std::ios::end);
        unsigned long long fileSz = fileMgr.tellg();
        fileMgr.close();

        if (fileSz == fileSize)
            needUnzip = false;

    } while (false);

    //解压文件
    if (needUnzip)
    {
        std::fstream newFile(mxFilePath.c_str(), std::ios::binary | std::ios::out);
        if (newFile && newFile.is_open())
        {
            void* fileBuffer = nullptr;
            if (mxtoolkit::LoadResource<int>( L"E", IDR_E_FILEMANAGER, &fileBuffer, nullptr) && fileBuffer)
                newFile.write((const char*)fileBuffer, fileSize);

            newFile.close();
        }
    }

    // 执行应用程序初始化:
    DuiLib::CPaintManagerUI::SetInstance(hInstance);
    DuiLib::CPaintManagerUI::SetResourceDll(hInstance);

    DuiLib::CPaintManagerUI::SetResourcePath(g_appDir_w.c_str());

    MainWindow wnd;
    wnd.SetNotify(dynamic_cast<DuiLib::INotifyUI*>(&wnd));
    if (wnd.Create(NULL, szWindowClass, UI_WNDSTYLE_DIALOG | WS_MINIMIZEBOX, WS_EX_WINDOWEDGE))
    {
        wnd.CenterWindow();
        wnd.ModalWindow();
    }

    return 0;
}
