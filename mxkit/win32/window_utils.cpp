
#include "window_utils.h"
#include <set>
#include <sstream>

#include "base/string_utils.h"
#include "base/string_convert.h"

_BEGIN_MX_KIT_NAME_SPACE_

namespace
{
    static bool s_breakMsgLoop = false;
    static std::set<std::wstring> s_registeredClassMap;
}

HWND WindowUtil::Parent(HWND hWnd, bool owner /*= false*/)
{
    if (owner)
        return ::GetWindow(hWnd, GW_OWNER);
    else
        return ::GetParent(hWnd);
}

DWORD WindowUtil::Style(HWND hWnd, bool ex)
{
    return GetWindowLong(hWnd, ex ? GWL_EXSTYLE : GWL_STYLE);
}

DWORD WindowUtil::SetStyle(HWND hWnd, DWORD add, DWORD rm, bool exStyle)
{
    DWORD dwStyle = Style(hWnd, exStyle);
    DWORD dwNewStyle = (dwStyle & ~rm) | add;
    if (dwStyle != dwNewStyle)
        ::SetWindowLong(hWnd, exStyle ? GWL_EXSTYLE : GWL_STYLE, dwNewStyle);

    return dwNewStyle;
}

void WindowUtil::TopMost(HWND hWnd)
{
    ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

WindowImpl::WindowImpl(HINSTANCE hInst)
{
    m_hWnd = NULL;
    m_hInstance = hInst;

    RegWindow(L"");
}

WindowImpl::~WindowImpl()
{
    if (m_hWnd != NULL && ::IsWindow(m_hWnd)) // C++对象被销毁之前，销毁窗口对象
    {
        ::DestroyWindow(m_hWnd);  // Tell system to destroy hWnd and Send WM_DESTROY to wndproc
    }
}

bool WindowImpl::RegWindow(const std::string& cls /*= ""*/)
{
    std::wstring ws;
    SCKit::AnsiiToUnicode(cls.c_str(), ws);
    return RegWindow(ws);
}

bool WindowImpl::RegWindow(const std::wstring& cls /*= ""*/)
{
    std::wstring ws;
    const wchar_t* clsName = cls.c_str();
    // 如果没注册过，则进行注册
    if (cls.empty())
    {
        std::wstringstream wss;
        wss << this;
        ws = wss.str();
        clsName = ws.c_str();
    }

    auto findIt = s_registeredClassMap.find(clsName);
    if (findIt != s_registeredClassMap.end())
    {
        m_windowClass = *findIt;
        return true;
    }

    //注册窗口类
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowImpl::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = clsName;
    wcex.hIconSm = NULL;

    if (0 != ::RegisterClassExW(&wcex))
    {
        m_windowClass = clsName;
        s_registeredClassMap.insert(clsName);
        return true;
    }

    return false;
}

HWND WindowImpl::Create(const std::string& title, HWND parent, int left, int top, unsigned int w, unsigned int h)
{
    if (title.empty())
    {
        return Create(L"", parent, left, top, w, h);
    }

    std::wstring wt;
    SCKit::AnsiiToUnicode(title.c_str(), wt);
    return Create(wt, parent, left, top, w, h);
}

HWND WindowImpl::Create(const std::wstring& title, HWND parent, int left, int top, unsigned int w, unsigned int h)
{
    // 创建窗口
    if (m_hWnd != NULL)
        return 0;

    if (m_windowClass.empty() && !RegWindow())
        return 0;

    HWND hwnd = ::CreateWindowW(
        m_windowClass.c_str(),
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        left,
        top,
        w,
        h,
        NULL,
        NULL,
        m_hInstance,
        (LPVOID)this
        );

    if (hwnd == NULL)
    {
        m_hWnd = NULL;
        wchar_t msg[100];
        ::wsprintf(msg, L"CreateWindow() fail:%ld", ::GetLastError());
        ::MessageBox(NULL, msg, L"Error", MB_OK);

        return 0;
    }

    return hwnd;
}

void WindowImpl::CenterWindow()
{
    if (!::IsWindow(m_hWnd) || WindowUtil::Style(m_hWnd) & WS_CHILD)
        return;

    RECT rcDlg = { 0 };
    ::GetWindowRect(m_hWnd, &rcDlg);
    RECT rcArea = { 0 };
    RECT rcCenter = { 0 };
    HWND hWnd = m_hWnd;
    HWND hWndParent = WindowUtil::Parent(m_hWnd);
    HWND hWndCenter = WindowUtil::Parent(m_hWnd, true);
    if (hWndCenter != NULL)
        hWnd = hWndCenter;

    // 处理多显示器模式下屏幕居中
    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
    rcArea = oMonitor.rcWork;

    if (hWndCenter == NULL || ::IsIconic(hWndCenter) || !IsWindowVisible(hWndCenter))
        rcCenter = rcArea;
    else
        ::GetWindowRect(hWndCenter, &rcCenter);

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // Find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // The dialog is outside the screen, move it inside
    if (xLeft < rcArea.left) xLeft = rcArea.left;
    else if (xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
    if (yTop < rcArea.top) yTop = rcArea.top;
    else if (yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;

    ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void WindowImpl::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
    if (!::IsWindow(m_hWnd))
        return;

    ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}

int WindowImpl::ShowModal()
{
    UINT nRet = 0;
    HWND hWndParent = WindowUtil::Parent(m_hWnd, true);
    ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
    ::EnableWindow(hWndParent, FALSE);
    MSG msg = { 0 };
    while (::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd)
        {
            nRet = msg.wParam;
            ::EnableWindow(hWndParent, TRUE);
            ::SetFocus(hWndParent);
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        if (msg.message == WM_QUIT)
            break;
    }

    ::EnableWindow(hWndParent, TRUE);
    ::SetFocus(hWndParent);

    return nRet;
}

void WindowImpl::Close(int nRet /*= 0*/)
{
    if (m_hWnd)
    {
        ::PostMessage(m_hWnd, WM_CLOSE, nRet, 0);
    }
}

int WindowImpl::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void WindowImpl::MessageLoop()
{
    s_breakMsgLoop = false;

    MSG msg = { 0 };
    while (!s_breakMsgLoop && ::GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

void WindowImpl::BreakMessageLoop()
{
    s_breakMsgLoop = true;
    ::PostQuitMessage(0);
}

LRESULT CALLBACK WindowImpl::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowImpl* pObj = NULL;
    if (message == WM_CREATE)
    {
        pObj = (WindowImpl*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
        pObj->m_hWnd = hWnd;
        ::SetWindowLong(hWnd, GWL_USERDATA, (LONG)pObj);
    }

    pObj = (WindowImpl*)::GetWindowLong(hWnd, GWL_USERDATA);

    switch (message)
    {
    case WM_CREATE:
        pObj->HandleMessage(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
        if (pObj != NULL)
        {
            pObj->m_hWnd = NULL;
        }
        break;
    default:
        pObj = (WindowImpl*)::GetWindowLong(hWnd, GWL_USERDATA);
        // 调用子类的消息处理虚函数
        if (!pObj || pObj->HandleMessage(hWnd, message, wParam, lParam) == 0)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;
    }

    return 0;
}

















_END_MX_KIT_NAME_SPACE_