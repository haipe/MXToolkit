#include "stdafx.h"
#include "MainWindow.h"
#include "Win32Resource.h"
#include "Resource.h"

MainWindow::MainWindow()
    : MXDuiWnd(_T(""))
{
    void* xmlBuffer = nullptr;
    if (!mxtoolkit::LoadResource<int>(L"XML", IDR_XML_MAIN, &xmlBuffer, nullptr) && xmlBuffer)
        return;

    std::wstring xmlStr;
    if (mxtoolkit::WUtf8Convert<std::string, std::wstring>((const char*)xmlBuffer, &xmlStr).empty())
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
