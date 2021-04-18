#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CLabelUI : public CControlUI
	{
		DECLARE_DUICONTROL(CLabelUI)
	public:
		CLabelUI();
		~CLabelUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
        void SetFocusFont(int iFont);
        void SetvValignCenter(bool bCenter);
		int GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);

        //jason add in CTextUI, loki move to here
        SIZE GetTextSize(LPCTSTR szText = NULL);
        CDuiRect GetTextRect(SIZE szAvailable);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);

		virtual bool GetAutoCalcWidth() const;
		virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
		virtual bool GetAutoCalcHeight() const;
		virtual void SetAutoCalcHeight(bool bAutoCalcHeight);
		virtual void SetText(LPCTSTR pstrText);
		
	protected:
		DWORD	m_dwTextColor;
		DWORD	m_dwDisabledTextColor;
		int		m_iFont;
        int		m_iFocusFont;   // 增加label获取焦点时字体“focusfont” add by jshuang 2019-8-21
		UINT	m_uTextStyle;
		RECT	m_rcTextPadding;
		bool	m_bShowHtml;
		bool	m_bAutoCalcWidth;
		bool	m_bAutoCalcHeight;
        bool    m_bTextCenter; //增加多行文字垂直居中属性“textcenter”，解决直接使用vlign设置在多行情况下不显示BUG add by Zero 2019-9-2

		SIZE    m_szAvailableLast;
		SIZE    m_cxyFixedLast;
		bool    m_bNeedEstimateSize;
	};
}

#endif // __UILABEL_H__