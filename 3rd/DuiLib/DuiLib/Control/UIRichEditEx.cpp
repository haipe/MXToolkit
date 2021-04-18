#include "stdafx.h"
#include "UIRichEditEx.h"

namespace DuiLib
{
	CRichEditUIEx::CRichEditUIEx()
	{
		InitializeEmotion();
		m_bPopRMenu = false;
	}

	CRichEditUIEx::~CRichEditUIEx()
	{

	}

	IRichEditOle* CRichEditUIEx::GetIRichEditOle()
	{
		LRESULT lResult;
		IRichEditOle* pRichEditOle = NULL;
		TxSendMessage(EM_GETOLEINTERFACE, 0, reinterpret_cast<LPARAM>(&pRichEditOle), &lResult);
		return pRichEditOle;
	}

	void CRichEditUIEx::GetWindowTextEx(CDuiString& sContentText)
	{
		sContentText = GetText();	
		IRichEditOle* pRichEditOle = GetIRichEditOle();
		if(pRichEditOle == NULL)
			return;

		int  nEmotionLen = 0;
		int  nEmotionCount = 0;
		CDuiString  sTempContentText = sContentText;
		int nPrePos = 0;
		for(int i=0; i<pRichEditOle->GetObjectCount(); i++)
		{
			REOBJECT reobject;
			ZeroMemory(&reobject, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);
			DWORD dwFlag = REO_GETOBJ_ALL_INTERFACES;
			pRichEditOle->GetObject( i, &reobject, dwFlag);
			CDuiString sEmotionName = GetEmotion( reobject.dwUser );
			GetImageNameEx( sEmotionName );
			if ( sEmotionName.IsEmpty() || reobject.dwUser < 0
				|| reobject.dwUser >= m_vecEmotionName.size() )
				continue;

			int  nContentLen = sContentText.GetLength();
			if(nContentLen <= reobject.cp)
				continue;

			sTempContentText   = sTempContentText.Left(reobject.cp + nPrePos);
			sTempContentText  += sEmotionName;
			sTempContentText  += sContentText.Right(nContentLen-reobject.cp-1);

			nPrePos += sEmotionName.GetLength() - 1;
		}
		sContentText = sTempContentText;
	}

	void CRichEditUIEx::ClearInsetImage()
	{
		m_vecInsertImagePos.clear();
	}

	void CRichEditUIEx::ClearContect()
	{
		SetSel(0, -1);
		ReplaceSel(_T(""), FALSE);
		ClearInsetImage();
	}

	bool CRichEditUIEx::GetTransferWord(CDuiString& sSrcString,CDuiString& sOutPutString,CDuiString& sEmotionName)
	{
		if(sSrcString.IsEmpty())
			return false;

		int nLength = sSrcString.GetLength(); 
		int nHeaderLen = _tcslen(g_pConst_TransferHeader_String) + 1;
		int nPos = sSrcString.Find(g_pConst_TransferHeader_String);
		if(nPos == -1 || nPos >= sSrcString.GetLength())
			return false;

		if(sSrcString.GetAt(nPos + nHeaderLen) != '(')
		{
			sOutPutString = sSrcString.Left(nPos + nHeaderLen);
			sSrcString = sSrcString.Right(nLength - nPos - nHeaderLen);
			sEmotionName.Empty();
			return true;
		}

		int nEnd = sSrcString.Find(_T(")"), nPos);
		if( nEnd == -1)
			return false;

		sEmotionName = sSrcString.Mid(nPos + nHeaderLen,nEnd - nPos - nHeaderLen + 1);
		sEmotionName.MakeLower();

		sEmotionName += g_pConst_TransferLastName_String;

		//找不到图片，就使用源字符串替代输出
		if (FindEmotion(sEmotionName) == -1)
		{
			sOutPutString = sSrcString.Mid(nPos,nEnd - nPos + 1);
		}	
		else
		{
			sOutPutString = sSrcString.Left(nPos);
		}
		
		sSrcString = sSrcString.Right(nLength - nEnd - 1);

		return true;
	}

	tagInsertImage* CRichEditUIEx::FindImage(int nPos)
	{
		for (int i=0; i<m_vecInsertImagePos.size(); i++)
		{
			if (m_vecInsertImagePos[i].nInsertPos == nPos)
			{
				return &m_vecInsertImagePos[i];
			}
		}
		return NULL;
	}

	tagInsertImage* CRichEditUIEx::FindImage( const CDuiString& sImageName )
	{
		for (int i=0; i<m_vecInsertImagePos.size(); i++)
		{
			if (m_vecInsertImagePos[i].sImageName == sImageName)
			{
				return &m_vecInsertImagePos[i];
			}
		}
		return NULL;
	}

	const CDuiString CRichEditUIEx::GetImageName( int nPos )
	{
		tagInsertImage* pInfo = FindImage( nPos );
		if (pInfo)
		{
			return pInfo->sImageName;
		}

		return _T("");
	}

	int CRichEditUIEx::FindEmotion(CDuiString& sEmotionName)
	{
		int nInex = -1;
		for (int i=0; i<m_vecEmotionName.size(); i++)
		{
			if(_tcsicmp(m_vecEmotionName[i], sEmotionName) == 0)
				return i;
		}
		return nInex;
	}

	DuiLib::CDuiString CRichEditUIEx::GetEmotion(int nIndex)
	{
		if( nIndex < 0 || nIndex >= m_vecEmotionName.size())
			return _T("");

		return m_vecEmotionName[nIndex];
	}

	void CRichEditUIEx::InitializeEmotion()
	{
		m_vecEmotionName.push_back(_T("(no).bmp"));
		m_vecEmotionName.push_back(_T("(ok).bmp"));
		m_vecEmotionName.push_back(_T("(jus).bmp"));
		m_vecEmotionName.push_back(_T("(toux).bmp"));
		m_vecEmotionName.push_back(_T("(zaij).bmp"));
		m_vecEmotionName.push_back(_T("(fad).bmp"));
		m_vecEmotionName.push_back(_T("(kaf).bmp"));
		m_vecEmotionName.push_back(_T("(ku).bmp"));
		m_vecEmotionName.push_back(_T("(jio).bmp"));
		m_vecEmotionName.push_back(_T("(huaix).bmp"));
		m_vecEmotionName.push_back(_T("(dax).bmp"));
		m_vecEmotionName.push_back(_T("(qiang).bmp"));
		m_vecEmotionName.push_back(_T("(dey).bmp"));
		m_vecEmotionName.push_back(_T("(weix).bmp"));
		m_vecEmotionName.push_back(_T("(xind).bmp"));
		m_vecEmotionName.push_back(_T("(xins).bmp"));
		m_vecEmotionName.push_back(_T("(wosh).bmp"));
		m_vecEmotionName.push_back(_T("(wuyu).bmp"));
		m_vecEmotionName.push_back(_T("(yun).bmp"));
		m_vecEmotionName.push_back(_T("(han).bmp"));
		m_vecEmotionName.push_back(_T("(dand).bmp"));
		m_vecEmotionName.push_back(_T("(zhut).bmp"));
		m_vecEmotionName.push_back(_T("(shqi).bmp"));
		m_vecEmotionName.push_back(_T("(yiwen).bmp"));
		m_vecEmotionName.push_back(_T("(se).bmp"));
		m_vecEmotionName.push_back(_T("(hua).bmp"));
		m_vecEmotionName.push_back(_T("(kum).bmp"));
		m_vecEmotionName.push_back(_T("(shuai).bmp"));
		m_vecEmotionName.push_back(_T("(tiaop).bmp"));
		m_vecEmotionName.push_back(_T("(jiub).bmp"));
		m_vecEmotionName.push_back(_T("(biz).bmp"));
		m_vecEmotionName.push_back(_T("(nang).bmp"));
		m_vecEmotionName.push_back(_T("(fan).bmp"));
		m_vecEmotionName.push_back(_T("(kul).bmp"));
		m_vecEmotionName.push_back(_T("(guz).bmp"));
	}

	void CRichEditUIEx::GetImageNameEx(CDuiString& sImageName)
	{
		if(sImageName.IsEmpty())
			return;

		int nPos = sImageName.Find(_T('.'), 0);
		if (nPos == -1)
			return ;

		sImageName.SetAt( nPos, '\0' );
		sImageName = _tcsupr((WCHAR*)sImageName.GetData());
		int nTransLen = sImageName.GetLength();
		TCHAR tNameBuf[255] = {0};
		_stprintf( tNameBuf, _T("%s%d%s"), g_pConst_TransferHeader_String, nPos, sImageName.GetData());
		sImageName = tNameBuf;
	}


	HRESULT CRichEditUIEx::TxSendMessage( UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult ) const
	{	
		if (msg == WM_CONTEXTMENU)
		{
			if (m_bPopRMenu)
			{
				m_bPopRMenu = false;
			}
			else
			{
				m_pManager->SendNotify( const_cast<CRichEditUIEx*>( this ), DUI_MSGTYPE_RICH_RMENU, wparam, lparam );
			}
			return S_OK;
		}
		return __super::TxSendMessage(msg,wparam,lparam,plresult);
	}

	void CRichEditUIEx::OnTxNotify( DWORD iNotify, void *pv )
	{
		if (iNotify == EN_LINK)
		{
			OnLinkMsg(pv);
		}
		else
		{
			__super::OnTxNotify(iNotify, pv);
		}
	}

	void CRichEditUIEx::OnLinkMsg( void *pv )
	{
		REQRESIZE *preqsz = (REQRESIZE *)pv;
		ENLINK* l_pEnLink = NULL;
		CDuiString strMsgType;

		do 
		{
			if (preqsz == NULL)
				break;

			 l_pEnLink = ( ENLINK* )&preqsz->nmhdr ;
			 if (l_pEnLink == NULL)
				 break;

			 if (l_pEnLink->msg == WM_LBUTTONDOWN)
			 {
				   strMsgType = DUI_MSGTYPE_RICH_LLINK;
			 }
			 else if (l_pEnLink->msg == WM_RBUTTONUP)
			 {
				   strMsgType =DUI_MSGTYPE_RICH_RLINK ;
				   m_bPopRMenu = true;
			 } 
			 else
			 {
				 break;
			 }			 

			 m_pManager->SendNotify(static_cast<CControlUI*>(this),strMsgType,(WPARAM)& l_pEnLink->chrg,l_pEnLink->lParam);
		} while (0);
	}

	LRESULT CRichEditUIEx::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
	{

		TCHAR* tchar = NULL;
		BOOL bIsOk = FALSE;

		do 
		{
			if(uMsg != WM_KEYDOWN)
				break;

			if(!(GetKeyState(VK_CONTROL) & 0x8000))
				break;

			if (wParam != 86)
				break;

			if(!IsFocused())
				break;

			if(IsReadOnly())
				break;

			if(!OpenClipboard(NULL))
				break;

			HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
			tchar = (TCHAR*)GlobalLock(hClipboardData);
	
			CDuiString CopyText(tchar);

			GlobalUnlock(hClipboardData);
			CloseClipboard();
			
		    ReplaceSel(CopyText, true);
			bIsOk = TRUE;
		} while (false);

		if (bIsOk)
			return 0;

		return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
	}
}