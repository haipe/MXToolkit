// MXStartup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MXStartup.h"
#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "Win32ConsoleWindow.h"
#include "Win32PathUtil.h"

#pragma comment(lib,"libxml.lib")

#ifdef _DEBUG
#pragma comment(lib,"DuiLib_StaticD.lib")

#else
#pragma comment(lib,"DuiLib_Static.lib")

#endif

#include "MainWindow.h"
#include "GetApp.h"

#define MAX_LOADSTRING 100
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名


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
    
    //显示UI =============================
    DuiLib::CPaintManagerUI::SetInstance(hInstance);
    DuiLib::CPaintManagerUI::SetResourceDll(hInstance);

    DuiLib::CPaintManagerUI::SetResourcePath(mxtoolkit::Win32App<std::wstring>::CurrentDirectory().c_str());
    MainWindow wnd;
    wnd.SetNotify(dynamic_cast<DuiLib::INotifyUI*>(&wnd));
    if (wnd.Create(NULL, szWindowClass, UI_WNDSTYLE_DIALOG | WS_MINIMIZEBOX, WS_EX_WINDOWEDGE))
    {
        GetApp getApp;
        getApp.Start("mxwasai");

        wnd.CenterWindow();
        wnd.ShowModal();
    }

    //消息
//     MSG msg = { 0 };
//     while (::GetMessage(&msg, NULL, 0, 0))
//     {
//         ::TranslateMessage(&msg);
//         ::DispatchMessage(&msg);
// 
//         if (msg.message == WM_QUIT)
//             break;
//     }

    return 0;
}
