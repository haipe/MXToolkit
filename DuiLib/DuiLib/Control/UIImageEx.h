#pragma once  

//by jason 2019-04-02 图像控件
namespace DuiLib
{
	//////////////////////////////////////////////////////////////////////////  
	// CImageExUI  
	class UILIB_API CImageExUI : public DuiLib::CLabelUI
	{
		DECLARE_DUICONTROL(CImageExUI);
	public:
		CImageExUI(void);
		~CImageExUI(void);

		LPCTSTR GetClass() const;
		LPVOID  GetInterface(LPCTSTR pstrName);
		void    SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void    SetPos(RECT rc);
		void    SetInset(RECT rc);
		void	SetBitmap(Gdiplus::Bitmap* pBitmap);
		void    SetVisible(bool bVisible);
		void    SetInternVisible(bool bVisible);
		bool    DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void    DoEvent(TEventUI& event);
	private:
		RECT                m_rcInset;
		Gdiplus::Bitmap*	m_pBitmap;
	};
}