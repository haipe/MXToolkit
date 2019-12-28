
#pragma once
#include "UIRichEdit.h"

#pragma once
#include <richole.h>
#include <vector>

namespace DuiLib {
	struct UILIB_API tagInsertImage
	{
		int         nInsertPos;
		CDuiString  sImageName;
		tagInsertImage()
		{
			nInsertPos    = 0;
			sImageName    = _T("");
		}
	};
	class CTxtWinHost;
	const TCHAR* const g_pConst_TransferHeader_String                = _T("/:E");
	const TCHAR* const g_pConst_TransferLastName_String              = _T(".bmp");

	class UILIB_API CRichEditUIEx :public CRichEditUI
	{
	public:
		CRichEditUIEx();
		~CRichEditUIEx();

	public:
		IRichEditOle* GetIRichEditOle();

		void GetWindowTextEx(CDuiString& sContentText);

		void ClearInsetImage();

		void ClearContect();

		bool GetTransferWord(CDuiString& sSrcString,CDuiString& sOutPutString,CDuiString& sEmotionName);

		tagInsertImage* FindImage( int nPos );

		tagInsertImage* FindImage( const CDuiString& sImageName );

		const CDuiString GetImageName( int nPos );

		int FindEmotion(CDuiString& sEmotionName);

		CDuiString GetEmotion(int nIndex);

	private:
		void InitializeEmotion();

		void GetImageNameEx(CDuiString& sImageName);

		virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const;
		virtual void OnTxNotify(DWORD iNotify, void *pv);
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

		void OnLinkMsg(void *pv);

	private:
		std::vector<tagInsertImage>  m_vecInsertImagePos;
		std::vector<CDuiString>      m_vecEmotionName;
		CRichEditUIEx *m_pMsgRich;

		// Martin By 2016.09.05
		mutable  bool m_bPopRMenu;  //弹出右键菜单标志
	};
}