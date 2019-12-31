#include "stdafx.h"
#include "MainWindow.h"
#include "Win32Resource.h"
#include "MXStringConvert.h"
#include "Resource.h"
#include "GetApp.h"


MainWindow::MainWindow()
    : MXDuiWnd(_T(""))
{
    std::wstring xmlStr;
    if (!mxtoolkit::LoadResource<std::wstring>(L"XML", IDR_XML_MAIN, &xmlStr))
        return;

    m_xmlFile = xmlStr;
}

MainWindow::~MainWindow()
{
}


HRESULT MainWindow::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    switch (uMsg)
    {
    case GetApp::MSG_GET_APP:
    {
        *bHandled = true;

        DuiLib::CDuiString msg;
        switch (wParam)
        {
        case GetApp::MT_UNZIP_STARTUP_XML:   //解压
        {
            msg = L"正在解压 startup.xml";
        }break;
        case GetApp::MT_UNZIP_MXFILE_EXE:   //解压
        {
            msg = L"正在解压 MXFile";
        }break;
        case GetApp::MT_DOWNLOAD_APP_XML:    //下载app.xml
        {
            msg = L"正在下载 app.xml";
        }break;
        case GetApp::MT_DOWNLOAD_APP_EXE:    //下载app.exe
        {
            msg = L"正在下载 app";
        }break;
        case GetApp::MT_DOWNLOAD_APP_LIBRIRY:    //下载app.所需库
        {
            std::string * name = (std::string *)lParam;
            std::wstring wName;
            mxtoolkit::WAConvert<std::string, std::wstring>(name->c_str(), &wName);
            delete name;

            msg = L"正在安装运行库：";
            msg += wName.c_str();
        }break;
        case GetApp::MT_RUN_APP:             //运行App
        {
            msg = L"正在启动...";
        }break;
        default:
            break;
        }

        if (msgLabel_)
            msgLabel_->SetText(msg);

        if (wParam == GetApp::MT_RUN_APP)
        {
            getAppCompleted = true;

            Close();
            ::PostMessage(m_hWnd, WM_QUIT, 0, 0);
        }
            
        return 0;
    }break;
    default:
        break;
    }

    return MXDuiWnd::OnMessage(uMsg, wParam, lParam, bHandled);
}

void MainWindow::Notify(DuiLib::TNotifyUI& msg)
{
    if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
    {
        MXDuiWnd::InitWndAbilityManager();

        msgLabel_ = (DuiLib::CLabelUI*)m_pm.FindControl(L"Dialog_Body_Area_Progress");
    }
    else if (msg.sType == DUI_MSGTYPE_CLICK)
    {
        if (msg.pSender == m_close)
        {
            if (getAppCompleted)
            {
                Close();
                ::PostMessage(m_hWnd, WM_QUIT, 0, 0);
            }
        }
    }
}
