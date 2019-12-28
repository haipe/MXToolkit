#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__
#pragma once

//loki修改多语言资源获取实现方式
namespace DuiLib {
	// 控件文字查询接口
	class UILIB_API IQueryControlText
	{
	public:
		//virtual LPCTSTR QueryControlText( LPCTSTR lpstrId, LPCTSTR lpstrType ) = 0;
		virtual LPCTSTR QueryControlText( const CDuiString& name, int resource ) = 0;
	};

	class UILIB_API CResourceManager
	{
	private:
		CResourceManager(void);
		~CResourceManager(void);

	public:
		static CResourceManager* GetInstance()
		{
			static CResourceManager * p = new CResourceManager;
			return p;
		};	
		void Release(void) { delete this; }

	public:
		BOOL LoadResource(STRINGorID xml, LPCTSTR type = NULL);
		BOOL LoadResource(CMarkupNode Root);
		void ResetResourceMap();
		LPCTSTR GetImagePath(LPCTSTR lpstrId);
		LPCTSTR GetXmlPath(LPCTSTR lpstrId);

	public:
		//void SetLanguage(LPCTSTR pstrLanguage) { m_sLauguage = pstrLanguage; }
		//LPCTSTR GetLanguage() { return m_sLauguage; }
		//BOOL LoadLanguage(LPCTSTR pstrXml);
		
	public:
		//loki 增加多语言标识，表示是否需要更新多语言
		void SetLanguage( int language ){ m_nLanguage = language; }
		const int& GetLanguage() const { return m_nLanguage; }
		LPCTSTR GetText( const CDuiString& name, int resource );
		//------------------------end

		void SetTextQueryInterface( IQueryControlText* pInterface ) { m_pQuerypInterface = pInterface; }

		//CDuiString GetText(LPCTSTR lpstrId, LPCTSTR lpstrType = NULL);
		//void ReloadText();
		//void ResetTextMap();

        static const int& m_currentLanguage; //loki 增加多语言标识，表示是否需要更新多语言
	private:
		int  m_nLanguage;//loki 增加多语言标识，表示是否需要更新多语言
		//CStdStringPtrMap m_mTextResourceHashMap;	//当前语言map
		IQueryControlText* m_pQuerypInterface;		//自定义语言获取接口
		CStdStringPtrMap m_mImageHashMap;			//图片资源
		CStdStringPtrMap m_mXmlHashMap;				//xml资源
		CMarkup m_xml;
		//CDuiString m_sLauguage;
		//CStdStringPtrMap m_mTextHashMap;
	};

} // namespace DuiLib

//---------end
#endif // __UIRESOURCEMANAGER_H__