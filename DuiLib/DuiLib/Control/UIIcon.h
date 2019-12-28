#pragma once  

namespace DuiLib
{
	//////////////////////////////////////////////////////////////////////////  
	// CIconUI  
	class UILIB_API CIconUI : public DuiLib::CControlUI
	{
		DECLARE_DUICONTROL(CIconUI);
	public:
		CIconUI(void);
		~CIconUI(void);

		LPCTSTR GetClass() const;
		LPVOID  GetInterface(LPCTSTR pstrName);
		void    SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void    SetPos(RECT rc);
		void    SetInset(RECT rc);
		void	SetIcon(HICON hIcon);
		void    SetVisible(bool bVisible);
		void    SetInternVisible(bool bVisible);
		bool    DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void    DoEvent(TEventUI& event);
	private:
		DuiLib::CDuiString  m_sIcoImage;
		RECT                m_rcInset;
		HICON	m_hIcon;
	};
}