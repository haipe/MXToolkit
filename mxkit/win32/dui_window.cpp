// App.cpp : Defines the entry point for the application.
//

#ifdef _MX_USE_DUILIB_

#include "dui_window.h"
#include "display_utils.h"

#include <shobjidl_core.h>

_BEGIN_MX_KIT_NAME_SPACE_

class WindowsTaskbarHelper
{
public:
    WindowsTaskbarHelper()
    {
        HRESULT hr =
            CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (void**)&m_pTaskbar);
        if (FAILED(hr))
            return;

        m_pTaskbar->HrInit();
    }

    ~WindowsTaskbarHelper()
    {
        if (m_pTaskbar)
            m_pTaskbar->Release();
    }

    void Show(HWND wnd, bool show)
    {
        if (!m_pTaskbar)
            return;

        if (show)
        {
            m_pTaskbar->AddTab(wnd);
        }
        else
        {
            m_pTaskbar->DeleteTab(wnd);
        }
    }
protected:
    ITaskbarList* m_pTaskbar = nullptr;

};


HICON DuiWindow::s_smalIcon = 0;
HICON DuiWindow::s_bigIcon = 0;

DuiWindow::DuiWindow(const TCHAR* xml)
    : m_xmlFile(xml)
{
}

DuiWindow::~DuiWindow()
{
}

void DuiWindow::ShowInTask(HWND wnd, bool show)
{
    static WindowsTaskbarHelper helper;

    helper.Show(wnd, show);
}

bool DuiWindow::CheckWindowStyle(HWND wnd, DWORD style)
{
    DWORD sty = ::GetWindowLong(wnd, GWL_STYLE);

    return (sty & style) == style;
}

void DuiWindow::SetNotify(DuiLib::INotifyUI* notify)
{
    if (notify == NULL)
        return;

    //未创建
    if (!::IsWindow(m_hWnd))
    {
        m_duiNotify = notify;
        return;
    }

    m_pm.AddNotifier(notify);
    m_pm.RemoveNotifier(m_duiNotify);
    m_duiNotify = notify;
}

void DuiWindow::SetBuilderCallback(DuiLib::IDialogBuilderCallback* callback)
{
    if (callback == NULL)
        return;

    m_duiBuilderCallback = callback;
}

HWND DuiWindow::Create(
    HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu /*= NULL*/)
{
    return CWindowWnd::Create(hwndParent, pstrName, dwStyle, dwExStyle, rc, hMenu);
}

HWND DuiWindow::Create(
    HWND hwndParent,
    LPCTSTR pstrName,
    DWORD dwStyle,
    DWORD dwExStyle,
    int x /*= CW_USEDEFAULT*/,
    int y /*= CW_USEDEFAULT*/, int cx /*= CW_USEDEFAULT*/, int cy /*= CW_USEDEFAULT*/, HMENU hMenu /*= NULL*/)
{
    return CWindowWnd::Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy, hMenu);
}

void DuiWindow::ShowShadow(bool bShow)
{
    m_pm.GetShadow()->ShowShadow(bShow);
    m_pm.GetShadow()->DisableShadow(!bShow);
}


void DuiWindow::CenterWindow(int width, int height)
{
    POINT pt = { 0,0 };
    //如果父窗口隐藏了
    if (::IsIconic(::GetParent(m_hWnd)))
    {
        CDuiRect rc;
        DisplayUtils::GetDualDisplayRect(::GetParent(m_hWnd), &rc);

        if (width == 0 || height == 0)
        {
            SIZE sz = m_pm.GetClientSize();
            pt.x = (rc.GetWidth() - sz.cx) / 2;
            pt.y = (rc.GetHeight() - sz.cy) / 2;

            ::SetWindowPos(m_hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE);
        }
        else
        {
            pt.x = (rc.GetWidth() - width) / 2;
            pt.y = (rc.GetHeight() - height) / 2;

            ::SetWindowPos(m_hWnd, NULL, pt.x, pt.y, width, height, SWP_NOZORDER);
        }
    }
    else
    {
        if (width != 0 && height != 0)
            ::SetWindowPos(m_hWnd, NULL, pt.x, pt.y, width, height, SWP_NOMOVE);

        __super::CenterWindow();
    }
}

void DuiWindow::Minimize()
{
    ::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void DuiWindow::Maxmize()
{
    if (m_restore)m_restore->SetVisible(true);
    if (m_max)m_max->SetVisible(false);
    ::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}

void DuiWindow::Restore()
{
    if (m_restore)m_restore->SetVisible(false);
    if (m_max)m_max->SetVisible(true);
    ::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
}

void DuiWindow::SetWindowText(LPCTSTR text)
{
    if (!text || _tcslen(text) == 0)
        return;

    ::SetWindowText(m_hWnd, text);
}

void DuiWindow::PopWindow(DWORD addFlag, DWORD removeFlag)
{
    int   nStyleOffset = GWL_STYLE;
    DWORD dwStyle = ::GetWindowLong(m_hWnd, nStyleOffset);
    DWORD dwNewStyle = (dwStyle & ~removeFlag) | addFlag;
    if (dwStyle != dwNewStyle)
        ::SetWindowLong(m_hWnd, nStyleOffset, dwNewStyle);

    //SetParent( m_hWnd, NULL );		
    //CenterWindow();

    ShowWindow();
    ShowShadow(true);
    m_pm.Invalidate();
}

DWORD DuiWindow::ModalWindow(DWORD addFlag, DWORD removeFlag)
{
    int   nStyleOffset = GWL_STYLE;
    DWORD dwStyle = ::GetWindowLong(m_hWnd, nStyleOffset);
    DWORD dwNewStyle = (dwStyle & ~removeFlag) | addFlag;
    if (dwStyle != dwNewStyle)
        ::SetWindowLong(m_hWnd, nStyleOffset, dwNewStyle);

    DuiLib::CWindowWnd::CenterWindow();

    DuiWindow::ShowShadow(true);
    return DuiLib::CWindowWnd::ShowModal();
}

void DuiWindow::TileWindow(DWORD addFlag, DWORD removeFlag)
{
    int   nStyleOffset = GWL_STYLE;
    DWORD dwStyle = ::GetWindowLong(m_hWnd, nStyleOffset);
    DWORD dwNewStyle = (dwStyle & ~removeFlag) | addFlag;
    if (dwStyle != dwNewStyle)
        ::SetWindowLong(m_hWnd, nStyleOffset, dwNewStyle);

    ShowWindow(true, false);
    ShowShadow(false);

    ::InvalidateRect(m_hWnd, NULL, TRUE);
}

void DuiWindow::InitWndAbilityManager()
{
    //根据默认名称查找
    if (!m_logo) m_logo = m_pm.FindControl(L"Dialog_Title_Area_Logo");
    if (!m_logoName) m_logoName = m_pm.FindControl(L"Dialog_Title_Area_Logo_Name");
    if (!m_min) m_min = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(_T("Dialog_Title_Area_Min")));
    if (!m_max) m_max = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(_T("Dialog_Title_Area_Max")));
    if (!m_restore) m_restore = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(_T("Dialog_Title_Area_Restore")));//NOLINT()
    if (!m_close) m_close = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(_T("Dialog_Title_Area_Close")));
    if (!m_titleArea) m_titleArea = m_pm.FindControl(L"Dialog_Title_Area_Layout");
    if (!m_bodyArea) m_bodyArea = m_pm.FindControl(L"Dialog_Body_Area_Layout");

    if (m_logoName)
        SetWindowText(m_logoName->GetText().GetData());

    m_uiCompleted = true;
}

LRESULT DuiWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
    if (m_xmlFile == _T("FLTaskBarIcon.xml"))
    {
        CHAR ss[MAX_PATH];
        sprintf(ss, "wnd:%ud,[msg:%x, w:%d, l:%d]-----------------------.\n", (DWORD)m_hWnd, uMsg, wParam, lParam);
        OutputDebugStringA(ss);
    }
#endif

    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    switch (uMsg)
    {
    case WM_TIMER:			lRes = OnTimer(wParam, lParam, &bHandled); break;
    case WM_CREATE:			lRes = OnCreate(wParam, lParam, &bHandled); break;
    case WM_CLOSE:			lRes = OnClose(wParam, lParam, &bHandled); break;
    case WM_DESTROY:		lRes = OnDestroy(wParam, lParam, &bHandled); break;
    case WM_NCACTIVATE:		lRes = OnNcActivate(wParam, lParam, &bHandled); break;
    case WM_NCCALCSIZE:		lRes = OnNcCalcSize(wParam, lParam, &bHandled); break;
    case WM_NCPAINT:		lRes = OnNcPaint(wParam, lParam, &bHandled); break;
    case WM_NCHITTEST:		lRes = OnNcHitTest(wParam, lParam, &bHandled); break;
    case WM_NCLBUTTONDBLCLK:lRes = OnNCLButtonDBClick(wParam, lParam, &bHandled); break;
    case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(wParam, lParam, &bHandled); break;
    case WM_SIZE:			lRes = OnSize(wParam, lParam, &bHandled); break;
    case WM_SYSCOMMAND:		lRes = OnSysCommand(wParam, lParam, &bHandled); break;
    case WM_KEYDOWN:        lRes = OnKeyDown(wParam, lParam, &bHandled); break;

    default:
    {
        bHandled = FALSE;
        lRes = OnMessage(uMsg, wParam, lParam, &bHandled);
    }
    }
    if (bHandled) return lRes;

    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;

    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT DuiWindow::OnTimer(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    *bHandled = FALSE;
    return 0;
}

LRESULT DuiWindow::OnCreate(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    if (m_xmlFile.empty())
        return 0;

    m_pm.Init(m_hWnd);
    DuiLib::CDialogBuilder builder;
    DuiLib::CControlUI* pRoot = builder.Create(m_xmlFile.c_str(), (UINT)0, m_duiBuilderCallback, &m_pm);
    ASSERT(pRoot && "Failed to parse XML");
    m_pm.AttachDialog(pRoot);

    if (m_duiNotify)
        m_pm.AddNotifier(m_duiNotify);

    if (s_smalIcon)
        ::SendMessage(m_hWnd, WM_SETICON, 0, (LPARAM)s_smalIcon);

    if (s_bigIcon)
        ::SendMessage(m_hWnd, WM_SETICON, 1, (LPARAM)s_bigIcon);

    return 0;
}

LRESULT DuiWindow::OnClose(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    *bHandled = FALSE;
    return 0;
}

LRESULT DuiWindow::OnDestroy(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    *bHandled = FALSE;
    return 0;
}

LRESULT DuiWindow::OnNcActivate(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    if (::IsIconic(*this))
        *bHandled = FALSE;
    return (wParam == 0) ? TRUE : FALSE;
}

LRESULT DuiWindow::OnNcCalcSize(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    LPRECT pRect = NULL;
    if (wParam == TRUE)
    {
        LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
        pRect = &pParam->rgrc[0];
    }
    else
    {
        pRect = (LPRECT)lParam;
    }

    if (::IsZoomed(m_hWnd))
    {
        // 最大化时，计算当前显示器最适合宽高度
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
        CDuiRect rcWork = oMonitor.rcWork;
        CDuiRect rcMonitor = oMonitor.rcMonitor;
        rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

        pRect->left = rcMonitor.left;
        pRect->right = pRect->left + rcWork.GetWidth();
        pRect->top = rcMonitor.top;
        pRect->bottom = pRect->top + rcWork.GetHeight();
        //return WVR_REDRAW;
    }

    return 0;
}

LRESULT DuiWindow::OnNcPaint(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    return 0;
}

LRESULT DuiWindow::OnNcHitTest(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(*this, &pt);
    RECT rcClient;
    ::GetClientRect(*this, &rcClient);

    if (!::IsZoomed(*this))
    {
        RECT rcSizeBox = m_pm.GetSizeBox();
        if (pt.y < rcClient.top + rcSizeBox.top)
        {
            if (pt.x < rcClient.left + rcSizeBox.left)
                return HTTOPLEFT;
            if (pt.x > rcClient.right - rcSizeBox.right)
                return HTTOPRIGHT;
            return HTTOP;
        }
        else if (pt.y > rcClient.bottom - rcSizeBox.bottom)
        {
            if (pt.x < rcClient.left + rcSizeBox.left)
                return HTBOTTOMLEFT;
            if (pt.x > rcClient.right - rcSizeBox.right)
                return HTBOTTOMRIGHT;
            return HTBOTTOM;
        }
        if (pt.x < rcClient.left + rcSizeBox.left)
            return HTLEFT;
        if (pt.x > rcClient.right - rcSizeBox.right)
            return HTRIGHT;
    }

    RECT rcCaption = m_pm.GetCaptionRect();
    if (pt.x >= rcClient.left + rcCaption.left &&
        pt.x < rcClient.right - rcCaption.right &&
        pt.y >= rcCaption.top &&
        pt.y < rcCaption.bottom)
    {
        DuiLib::CControlUI* pControl = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(pt));
        LPCTSTR className = pControl ? pControl->GetClass() : NULL;
        if (pControl &&
            _tcscmp(className, DUI_CTR_BUTTON) != 0 &&
            _tcscmp(className, DUI_CTR_OPTION) != 0 &&
            _tcscmp(className, DUI_CTR_TEXT) != 0 &&
            _tcscmp(className, L"ComboUI") != 0)	//增加对Combo控件过滤
            return HTCAPTION;
    }

    return HTCLIENT;
}

LRESULT DuiWindow::OnNCLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    *bHandled = false;
    auto& miniSize = m_pm.GetMinInfo();
    auto& maxSize = m_pm.GetMaxInfo();
    if (miniSize.cx && miniSize.cx == maxSize.cx && miniSize.cy && miniSize.cy == maxSize.cy)
    {
        *bHandled = true;
    }
    return 0;
}

LRESULT DuiWindow::OnSize(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    if (wParam == SIZE_MAXIMIZED)
    {
        if (m_restore)m_restore->SetVisible(true);
        if (m_max)m_max->SetVisible(false);
    }
    else if (wParam == SIZE_RESTORED)
    {
        if (m_restore)m_restore->SetVisible(false);
        if (m_max)m_max->SetVisible(true);
    }

    SIZE szRoundCorner = m_pm.GetRoundCorner();
    if (!::IsIconic(*this))//&& ( szRoundCorner.cx != 0 || szRoundCorner.cy != 0 )
    {
        DuiLib::CDuiRect rcWnd;
        ::GetWindowRect(m_hWnd, &rcWnd);
        rcWnd.Offset(-rcWnd.left, -rcWnd.top);
        rcWnd.right++;
        rcWnd.bottom++;

        HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom,
            szRoundCorner.cx, szRoundCorner.cy);
        ::SetWindowRgn(*this, hRgn, TRUE);
        ::DeleteObject(hRgn);
    }

    *bHandled = FALSE;
    return 0;
}

LRESULT DuiWindow::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
    bool isChildWindow = (dwStyle & WS_CHILD) || (dwStyle & WS_CHILDWINDOW);

    HWND hWnd = m_hWnd;
    //         if (isChildWindow)
    //             while (::GetParent(hWnd) != NULL) 
    //                 hWnd = ::GetParent(hWnd);

    MONITORINFO Monitor = {};
    Monitor.cbSize = sizeof(Monitor);
    ::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &Monitor);
    CDuiRect rcWork = isChildWindow ? Monitor.rcMonitor : Monitor.rcWork;
    if (Monitor.dwFlags != MONITORINFOF_PRIMARY)//主屏
    {
        ::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
    }

    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
    // 		lpMMI->ptMaxPosition.x = rcWork.left;
    // 		lpMMI->ptMaxPosition.y = rcWork.top;
    // 		lpMMI->ptMaxSize.x = rcWork.right - rcWork.left;
    // 		lpMMI->ptMaxSize.y = rcWork.bottom - rcWork.top;
    // 		lpMMI->ptMaxTrackSize.x = rcWork.right - rcWork.left;
    // 		lpMMI->ptMaxTrackSize.y = rcWork.bottom - rcWork.top;
    // 		lpMMI->ptMinTrackSize.x = m_pm.GetMinInfo().cx;
    // 		lpMMI->ptMinTrackSize.y = m_pm.GetMinInfo().cy;

    lpMMI->ptMaxPosition.x = rcWork.left;
    lpMMI->ptMaxPosition.y = rcWork.top;
    lpMMI->ptMaxSize.x = rcWork.GetWidth();
    lpMMI->ptMaxSize.y = rcWork.GetHeight();

    SIZE maxSize = m_pm.GetMaxInfo();
    lpMMI->ptMaxTrackSize.x = maxSize.cx == 0 ? rcWork.GetWidth() : maxSize.cx;
    lpMMI->ptMaxTrackSize.y = maxSize.cy == 0 ? rcWork.GetHeight() : maxSize.cy;

    SIZE minSize = m_pm.GetMinInfo();
    lpMMI->ptMinTrackSize.x = minSize.cx;
    lpMMI->ptMinTrackSize.y = minSize.cy;

    *bHandled = FALSE;
    return 0;
}

LRESULT DuiWindow::OnSysCommand(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    // 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
    if (wParam == SC_CLOSE)
    {
        OnDestroy(wParam, lParam, bHandled);
        *bHandled = TRUE;
        return 0;
    }
    return CWindowWnd::HandleMessage(WM_SYSCOMMAND, wParam, lParam);
}

LRESULT DuiWindow::OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL* bHandled)
{
    *bHandled = FALSE;
    return 0;
}

_END_MX_KIT_NAME_SPACE_

#endif
