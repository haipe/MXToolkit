#pragma  once

#include <string>
#include <windows.h>

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_

class WindowUtil
{
public:
    static HWND Parent(HWND hWnd, bool owner = false);

    static DWORD Style(HWND hWnd, bool exStyle = false);
    static DWORD SetStyle(HWND hWnd, DWORD add, DWORD remve, bool exStyle = false);

    static void TopMost(HWND hWnd);
};

class WindowImpl
{
public:
    WindowImpl(HINSTANCE hInst = 0);
    ~WindowImpl();

public:
    bool RegWindow(const std::string& cls = "");
    bool RegWindow(const std::wstring& cls);
    // 创建窗口
    HWND Create(const std::string& title = "", HWND parent = 0, int left = 0, int top = 0, unsigned int w = 64, unsigned int h = 64);
    HWND Create(const std::wstring& title = L"", HWND parent = 0, int left = 0, int top = 0, unsigned int w = 64, unsigned int h = 64);

    void CenterWindow();
    void ShowWindow(bool bShow = true, bool bTakeFocus = false);
    int ShowModal();

    void Close(int nRet = 0, bool post = false);

    // 返回窗口对象句柄
    HWND HWnd(){ return m_hWnd; }

    // 消息处理。需要后续默认处理则需要返回0；停止该消息后续处理，则返回 1
    virtual int HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
       
public:
    static void MessageLoop();
    static void BreakMessageLoop();

private:
    // 原始窗口过程
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE       m_hInstance;
    HWND            m_hWnd;
    std::wstring    m_windowClass;
};

_END_MX_KIT_NAME_SPACE_