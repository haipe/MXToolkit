#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTileLayoutUI : public CContainerUI
	{
		DECLARE_DUICONTROL(CTileLayoutUI)
	public:
		CTileLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetPos(RECT rc, bool bNeedInvalidate = true);

		SIZE GetItemSize() const;
		void SetItemSize(SIZE szItem);
		int GetColumns() const;
		void SetColumns(int nCols);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		SIZE m_szItem;
		int m_nColumns;
	};

	//begin add by jason 2019-02-14
	class UILIB_API CTileElementUI : public CVerticalLayoutUI
	{
	public:
		CTileElementUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Activate();
		bool Select(bool bSelect = true, bool bCallback = true);
		bool IsSelected() {	return m_bSelected;	}
		void SetIndex(int index) { m_nIndex = index; }
		int GetIndex() const { return m_nIndex; }

		void DoEvent(TEventUI& event);

	protected:
		int m_nIndex;
		bool m_bSelected;
		UINT m_uButtonState;
	};
	//end 
}
#endif // __UITILELAYOUT_H__
