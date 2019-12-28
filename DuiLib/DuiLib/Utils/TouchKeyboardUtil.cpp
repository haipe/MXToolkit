#include "stdafx.h"
#include "TouchKeyboardUtil.h"

namespace DuiLib
{
    int TouchKeyboardUtil::m_tablePCState = -1024;//未初始化
    ITouchKeyboard * TouchKeyboardUtil::m_touchKeyboard = nullptr;

    void TouchKeyboardUtil::Release()
    {
        if (m_touchKeyboard)
            m_touchKeyboard->Release();
    }
	
	void TouchKeyboardUtil::ShowTouchKeyboard(HWND hWnd)
	{
        if (m_tablePCState == -1024)
        {
            //是否是平板电脑类型
            m_tablePCState = GetSystemMetrics(SM_TABLETPC);  //SM_TABLETPC
        }
        if (m_tablePCState <= 0)
            return;

		if (!m_touchKeyboard)
			CoCreateInstance(CLSID_UIHostNoLaunch, 0, CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER,
				IID_ITouchKeyboard, (void**)&m_touchKeyboard);

		if (m_touchKeyboard)
		{
			m_touchKeyboard->Toggle(hWnd);
		}
	}
}