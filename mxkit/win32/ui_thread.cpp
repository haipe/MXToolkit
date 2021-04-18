
#include "ui_thread.h"
#include <windows.h>

_BEGIN_MX_KIT_NAME_SPACE_

static  const unsigned int MSG_MAIN_THREAD_ID = WM_USER + 1000;

namespace
{
    LRESULT CALLBACK __MainThreadWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool RegisterWindowClass()
    {
        WNDCLASS wc = { 0 };
        wc.style = 0;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hIcon = NULL;
        wc.lpfnWndProc = __MainThreadWndProc;
        wc.hInstance = 0;
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"Win32_Main_Thread_Window###";
        ATOM ret = ::RegisterClass(&wc);

        return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
    }

    HWND Create(LPVOID pThis)
    {
        if (!RegisterWindowClass())
            return NULL;

        HWND hWnd = ::CreateWindowEx(0, L"Win32_Main_Thread_Window###", L"Win32_Main_Thread_Window###", 0, 0, 0, 0, 0, NULL, NULL, NULL, pThis);
        ::ShowWindow(hWnd, SW_HIDE);
        return hWnd;
    }

    LRESULT CALLBACK __MainThreadWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (MSG_MAIN_THREAD_ID == uMsg && lParam)
        {
            bool wait = wParam != 0;
            std::function<void()>* pf = (std::function<void()>*)lParam;
            if (pf)
            {
                std::function<void()> rf = *pf;
                if (rf)
                    rf();

                if (!wait)
                    delete pf;
            }
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

}

UIThreadUtils::UIThreadUtils()
{
    m_wnd = (unsigned int)Create(this);
}

UIThreadUtils::~UIThreadUtils()
{
    if (m_wnd)
    {
        ::CloseWindow((HWND)m_wnd);
        ::DestroyWindow((HWND)m_wnd);
        m_wnd = 0;
    }
}

bool UIThreadUtils::Do(bool wait, std::function<void()> f)
{
    if (!f)
        return false;

    if (!::IsWindow((HWND)m_wnd))
        return false;

    BOOL r = FALSE;
    if (wait)
    {
        r = ::SendMessage((HWND)m_wnd, MSG_MAIN_THREAD_ID, wait, (LPARAM)&f);
    }
    else
    {
        std::function<void()>* ff = new std::function<void()>(f);
        r = ::PostMessage((HWND)m_wnd, MSG_MAIN_THREAD_ID, wait, (LPARAM)ff);
    }

    return true;
}

_END_MX_KIT_NAME_SPACE_