/*##############################################################################
* 文件：TouchKeyboardUtil.h
* 描述：显示触摸键盘模块
* 作者：Jason
* 时间：2019年04月24日
* 版权：Copyright(C) 2019 Fsmeeting.com. All rights reserved.
##############################################################################*/
#pragma once
#include "initguid.h"

namespace DuiLib
{
	// 4ce576fa-83dc-4F88-951c-9d0782b4e376
	DEFINE_GUID(CLSID_UIHostNoLaunch,
		0x4CE576FA, 0x83DC, 0x4f88, 0x95, 0x1C, 0x9D, 0x07, 0x82, 0xB4, 0xE3, 0x76);

	// 37c994e7_432b_4834_a2f7_dce1f13b834b
	DEFINE_GUID(IID_ITouchKeyboard,
		0x37c994e7, 0x432b, 0x4834, 0xa2, 0xf7, 0xdc, 0xe1, 0xf1, 0x3b, 0x83, 0x4b);

	struct ITouchKeyboard : IUnknown
	{
		virtual HRESULT STDMETHODCALLTYPE Toggle(HWND wnd) = 0;
	};

    class TouchKeyboardUtil
    {
	public:
        static void Release();
        static void ShowTouchKeyboard(HWND hWnd);

	private:
		~TouchKeyboardUtil();
		
    protected:
        static int                  m_tablePCState;
        static ITouchKeyboard *     m_touchKeyboard;
    };

}