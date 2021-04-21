
#include "ui_thread.h"
#include <windows.h>

#include "window_utils.h"

_BEGIN_MX_KIT_NAME_SPACE_

static  const unsigned int MSG_MAIN_THREAD_ID = WM_USER + 1000;

namespace
{
    class Window : public WindowImpl
    {
    public:
        virtual int HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override
        {
            if (MSG_MAIN_THREAD_ID == message && lParam)
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

            return 0;
        }

    };

    Window* g_uithread_window = nullptr;
    HWND    g_uithread_window_hwnd = 0;
}

UIThreadUtils::UIThreadUtils()
{
    Window* w = new Window;
    if (w)
    {
        g_uithread_window_hwnd = w->Create("UIThreadUtils.Window");
        if(g_uithread_window_hwnd)
        {
            g_uithread_window = w;
            w->ShowWindow(false);
        }
    }
}

UIThreadUtils::~UIThreadUtils()
{
    if (g_uithread_window)
    {
        g_uithread_window->Close(0, true);
        ::CloseWindow(g_uithread_window_hwnd);
        ::DestroyWindow(g_uithread_window_hwnd);

        delete g_uithread_window;
        g_uithread_window = nullptr;
    }
}

bool UIThreadUtils::Run(bool wait, std::function<void()> f)
{
    if (!f)
        return false;

    if (!::IsWindow(g_uithread_window_hwnd))
        return false;

    BOOL r = FALSE;
    if (wait)
    {
        r = ::SendMessage(g_uithread_window_hwnd, MSG_MAIN_THREAD_ID, wait, (LPARAM)&f);
    }
    else
    {
        std::function<void()>* ff = new std::function<void()>(f);
        r = ::PostMessage(g_uithread_window_hwnd, MSG_MAIN_THREAD_ID, wait, (LPARAM)ff);
    }

    return true;
}

_END_MX_KIT_NAME_SPACE_