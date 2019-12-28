// MXStartup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MXStartup.h"

#include "MainWindow.h"

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


bool LoadRc(LPCTSTR sourceName, LPCTSTR sourceType, void** buffer, unsigned int* size)
{
    HRSRC hResourceFile = FindResource(hInst, sourceName, sourceType);
    if (!hResourceFile)
        return false;

    if (size)
    {
        *size = SizeofResource(hInst, hResourceFile);
    }

    if (buffer)
    {
        HANDLE hglob = LoadResource(hInst, hResourceFile);
        *buffer = LockResource(hglob);
    }

    return true;
}

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
    std::string appDir = path;
    appDir = appDir.substr(0, appDir.find_last_of('\\') + 1);
    
    //先判断原有文件是否存在，一样的
    unsigned int fileSize = 0;
    bool needUnzip = mxtoolkit::LoadResource(L"E", IDR_E_FILEMANAGER, NULL, &fileSize);
    std::string mxFilePath = appDir + "MXFile.exe";
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
    std::wstring dirPath;
    mxtoolkit::WAConvert<std::string, std::wstring>(appDir.c_str(), &dirPath);
    DuiLib::CPaintManagerUI::SetResourcePath(dirPath.c_str());

    MainWindow wnd;
    wnd.SetNotify(dynamic_cast<DuiLib::INotifyUI*>(&wnd));
    if (wnd.Create(NULL, szWindowClass, UI_WNDSTYLE_DIALOG | WS_MINIMIZEBOX, WS_EX_WINDOWEDGE))
    {
        wnd.CenterWindow();
        wnd.ModalWindow();
    }

    return 0;
}
