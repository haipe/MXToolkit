#include "StdAfx.h"
#include "UIImageEx.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CImageExUI)
	//////////////////////////////////////////////////////////////////////////
	// CImageExUI
	CImageExUI::CImageExUI(void)
	{
		m_pBitmap = NULL;
		memset(&m_rcInset, 0, sizeof(m_rcInset));
		SetAttribute(_T("bkcolor"), _T("#00000000"));
	}

	CImageExUI::~CImageExUI(void)
	{
		if (m_pBitmap)
		{
			delete m_pBitmap;
			m_pBitmap = NULL;
		}
	}

	LPCTSTR CImageExUI::GetClass() const
	{
		return _T("ImageEx");
	}

	LPVOID CImageExUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_IMAGEEX) == 0) return static_cast<CImageExUI*>(this);
		return __super::GetInterface(pstrName);
	}

	void CImageExUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("inset")) == 0)
		{
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetInset(rcInset);
		}
		Invalidate();
		__super::SetAttribute(pstrName, pstrValue);
	}

	void CImageExUI::SetPos(RECT rc)
	{
		CLabelUI::SetPos(rc);
	}

	void CImageExUI::SetInset(RECT rc)
	{
		m_rcInset = rc;
	}

	void CImageExUI::SetBitmap(Gdiplus::Bitmap* pBitmap)
	{
		if (m_pBitmap)
		{
			delete m_pBitmap;
			m_pBitmap = NULL;
		}
		m_pBitmap = pBitmap;
	}

	void CImageExUI::DoEvent(TEventUI& event)
	{
		CLabelUI::DoEvent(event);
	}

	void CImageExUI::SetVisible(bool bVisible)
	{
		CLabelUI::SetVisible(bVisible);
	}

	void CImageExUI::SetInternVisible(bool bVisible)
	{
		CLabelUI::SetInternVisible(bVisible);
	}

	bool CImageExUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return false;

		if (m_pBitmap && !IsRectEmpty(&m_rcItem))
		{
			CDuiRect rc = m_rcItem;
			Gdiplus::Graphics graphics(hDC);
			graphics.DrawImage(m_pBitmap, Gdiplus::Rect(rc.left, rc.top, rc.GetWidth(), rc.GetHeight()),
				0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight(), Gdiplus::UnitPixel);
		}

		return true;
	}


}