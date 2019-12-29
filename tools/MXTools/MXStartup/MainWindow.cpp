#include "stdafx.h"
#include "MainWindow.h"
#include "Win32Resource.h"
#include "Resource.h"

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

void MainWindow::Notify(DuiLib::TNotifyUI& msg)
{
    if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
    {
        MXDuiWnd::InitWndAbilityManager();
    }
    else if (msg.sType == DUI_MSGTYPE_CLICK)
    {
        if (msg.pSender == m_close)
        {
            Close();
        }
    }
}
