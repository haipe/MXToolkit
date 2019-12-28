#include "stdafx.h"
#include "ResourceCacheUtil.h"
#include "Core/UIManager.h"

namespace DuiLib
{

    ResourceCacheUtil*  ResourceCacheUtil::m_zipManager = nullptr;

    ResourceCacheUtil::ResourceCacheUtil()
    {
    }


    ResourceCacheUtil::~ResourceCacheUtil()
    {
        m_xmlData.clear();
        m_imageData.clear();
    }

    ResourceCacheUtil* ResourceCacheUtil::GetInstance()
    {
        if (m_zipManager == nullptr)
            m_zipManager = new ResourceCacheUtil;

        return m_zipManager;
    }

    bool ResourceCacheUtil::Init()
    {
        return LoadAllZipResource();
    }

    void ResourceCacheUtil::Release()
    {
        if (m_zipManager)
        {
            if (!CPaintManagerUI::IsCachedResourceZip())
                CloseZip(m_zipManager->m_zip);

            delete m_zipManager;

            m_zipManager = nullptr;
        }
    }

    void ResourceCacheUtil::ResetAll()
    {
        m_xmlData.clear();
        m_imageData.clear();
    }

    void ResourceCacheUtil::ResetXml(LPCTSTR res)
    {
        CDuiString key(res);
        key.MakeLower();
        auto findIt = m_xmlData.find(key);
        if (findIt != m_xmlData.end())
            m_xmlData.erase(findIt);            
    }

    void ResourceCacheUtil::ResetImage(LPCTSTR res)
    {
        CDuiString key(res);
        key.MakeLower();
        auto findIt = m_imageData.find(key);
        if (findIt != m_imageData.end())
            m_imageData.erase(findIt);
    }

    bool ResourceCacheUtil::PreLoadingXmlResource(LPCTSTR res)
    {
        DWORD size;
        LPBYTE data = LoadResource(res, size);

        if (!data)
            return false;

        if (size == 0)
        {
            delete[] data;
            return false;
        }

        CDuiString key(res);
        key.MakeLower();
        m_xmlData.insert(std::make_pair(key, DataInfoPtr(new DataInfo(size, data))));

        return true;
    }

    bool ResourceCacheUtil::PreLoadingImageResource(LPCTSTR res)
    {
        DWORD size;
        LPBYTE data = LoadResource(res, size);

        if (!data)
            return false;

        if (size == 0)
        {
            delete[] data;
            return false;
        }

        CDuiString key(res);
        key.MakeLower();
        m_imageData.insert(std::make_pair(key, DataInfoPtr(new DataInfo(size, data))));

        return true;
    }

    bool ResourceCacheUtil::FindXml(LPCTSTR res, LPBYTE& data, DWORD& size)
    {
        if (FindXmlFromMemmry(res, data, size))
            return true;

        DWORD dwSize = 0;
        LPBYTE pData = LoadResource(res, dwSize);

        if (!pData)
            return false;

        if (dwSize == 0)
        {
            delete[] pData;
            return false;
        }

        CDuiString key(res);
        key.MakeLower();
        m_xmlData.insert(std::make_pair(key, DataInfoPtr(new DataInfo(dwSize, pData))));

        data = new BYTE[dwSize];
        if (!data)
            return false;

        size = dwSize;
        memcpy(data, pData, size);
        return true;
    }

    bool ResourceCacheUtil::FindXmlFromMemmry(LPCTSTR res, LPBYTE& data, DWORD& size)
    {
        do
        {
            CDuiString key(res);
            key.MakeLower();
            auto findIt = m_xmlData.find(key);
            if (findIt != m_xmlData.end() && findIt->second)
            {
                data = new BYTE[findIt->second->size];
                if (!data)
                    break;

                size = findIt->second->size;
                memcpy(data, findIt->second->data, size);

                return true;
            }
        } while (0);

        data = nullptr;
        size = 0;
        return false;
    }

    bool ResourceCacheUtil::FindImage(LPCTSTR res, LPBYTE& data, DWORD& size)
    {
        if (FindImageFromMemmry(res, data, size))
            return true;

        data = LoadResource(res,size);

        if (!data)
            return false;

        if (size == 0)
        {
            delete[] data;
            return false;
        }

        CDuiString key(res);
        key.MakeLower();
        m_imageData.insert(std::make_pair(key, DataInfoPtr(new DataInfo(size, data))));

        return true;
    }

    bool ResourceCacheUtil::FindImageFromInstance(HINSTANCE instance, LPCTSTR type, LPCTSTR res, LPBYTE& data, DWORD& size)
    {
        if (FindImageFromMemmry(res, data, size))
            return true;

        HINSTANCE dllinstance = NULL;
        if (instance)
            dllinstance = instance;
        else
            dllinstance = CPaintManagerUI::GetResourceDll();

        HRSRC hResource = ::FindResource(dllinstance, res, type);
        if (hResource == NULL) 
            return false;

        HGLOBAL hGlobal = ::LoadResource(dllinstance, hResource);
        if (hGlobal == NULL) 
        {
            FreeResource(hResource);
            return false;
        }

        size = ::SizeofResource(dllinstance, hResource);
        if (size == 0)
        {
            FreeResource(hResource);
            return false;
        }
        data = new BYTE[size];
        if (!data)
        {
            size = 0;
            FreeResource(hResource);
            return false;
        }

        ::CopyMemory(data, (LPBYTE)::LockResource(hGlobal), size);
        ::FreeResource(hResource);

        CDuiString key(res);
        key.MakeLower();
        m_imageData.insert(std::make_pair(key, DataInfoPtr(new DataInfo(size, data))));

        return true;
    }

    bool ResourceCacheUtil::FindImageFromLocal(LPCTSTR res, LPBYTE& data, DWORD& size)
    {
        if (FindImageFromMemmry(res, data, size))
            return true;

        //读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(res, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return false;

        size = ::GetFileSize(hFile, NULL);
        if (size == 0)
        {
            ::CloseHandle(hFile);
            return false;
        }

        DWORD dwRead = 0;
        data = new BYTE[size];
        if (!data)
        {
            size = 0;
            ::CloseHandle(hFile);
            return false;
        }

        ::ReadFile(hFile, data, size, &dwRead, NULL);
        ::CloseHandle(hFile);

        if (dwRead != size)
        {
            size = 0;
            delete[] data;
            data = NULL;
            ::CloseHandle(hFile);
            return false;
        }

        CDuiString key(res);
        key.MakeLower();
        m_imageData.insert(std::make_pair(key, DataInfoPtr(new DataInfo(size, data))));
        return true;
    }

    bool ResourceCacheUtil::FindImageFromMemmry(LPCTSTR res, LPBYTE& data, DWORD& size)
    {
        CDuiString key(res);
        key.MakeLower();
        auto findIt = m_imageData.find(key);
        if (findIt != m_imageData.end() && findIt->second)
        {
            size = findIt->second->size;
            data = findIt->second->data;

            return true;
        }

        data = nullptr;
        size = 0;
        return nullptr;
    }

    LPBYTE ResourceCacheUtil::LoadResource(LPCTSTR res, DWORD& size)
    {
        size = 0;
        LPBYTE pData = nullptr;
        CDuiString sFile = CPaintManagerUI::GetResourcePath();
        if (!CPaintManagerUI::GetResourceZip().IsEmpty() && (pData = LoadZipResource(res, size)))
        {
            return pData;
        }

        return LoadLocalResource(res, size);
    }


    LPBYTE ResourceCacheUtil::LoadLocalResource(LPCTSTR res, DWORD& size)
    {
        if (!CPaintManagerUI::GetResourceZip().IsEmpty())
            return nullptr;

        CDuiString sFile = CPaintManagerUI::GetResourcePath();

        sFile += res;
        HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return nullptr;

        DWORD dwSize = ::GetFileSize(hFile, NULL);
        if (dwSize == 0)
            return nullptr;

        DWORD dwRead = 0;

        LPBYTE data = new BYTE[dwSize];
        ::ReadFile(hFile, data, dwSize, &dwRead, NULL);
        ::CloseHandle(hFile);

        if (dwRead != dwSize)
        {
            delete[] data;
            data = NULL;
            return nullptr;
        }

        size = dwSize;
        return data;
    }

    LPBYTE ResourceCacheUtil::LoadZipResource(LPCTSTR res, DWORD& size)
    {
        CDuiString sFile = CPaintManagerUI::GetResourcePath();
        sFile += CPaintManagerUI::GetResourceZip();

        if (m_zip == nullptr)
        {
            if (CPaintManagerUI::IsCachedResourceZip())
            {
                m_zip = (HZIP)CPaintManagerUI::GetResourceZipHandle();
            }
            else
            {
                CDuiString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
#ifdef UNICODE
                char* pwd = w2a((wchar_t*)sFilePwd.GetData());
                m_zip = OpenZip(sFile.GetData(), pwd);
                if (pwd) delete[] pwd;
#else
                m_zip = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
            }
        }

        if (m_zip == NULL)
            return nullptr;

        CDuiString key = res;
        while (key.Replace(_T("\\"), _T("/")) != 0);

        ZIPENTRY ze;
        int i = 0;
        if (FindZipItem(m_zip, key, true, &i, &ze) != 0)
            return nullptr;

        DWORD dwSize = 0;
        dwSize = ze.unc_size;
        if (dwSize == 0)
            return nullptr;

        LPBYTE data = new BYTE[dwSize];
        ZRESULT zr = UnzipItem(m_zip, i, data, dwSize);
        if (zr != 0x00000000 && zr != 0x00000600)
        {
            delete[] data;
            data = NULL;
            return nullptr;
        }

        size = dwSize;
        return data;
    }

    bool ResourceCacheUtil::LoadAllZipResource()
    {
        CDuiString sFile = CPaintManagerUI::GetResourcePath();
        sFile += CPaintManagerUI::GetResourceZip();

        if (m_zip == nullptr)
        {
            if (CPaintManagerUI::IsCachedResourceZip())
            {
                m_zip = (HZIP)CPaintManagerUI::GetResourceZipHandle();
            }
            else
            {
                CDuiString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
#ifdef UNICODE
                char* pwd = w2a((wchar_t*)sFilePwd.GetData());
                m_zip = OpenZip(sFile.GetData(), pwd);
                if (pwd) delete[] pwd;
#else
                m_zip = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
            }
        }

        if (m_zip == NULL)
            return false;

        int index = 0;
        while(1)
        {
            ZIPENTRY ze;
            if (GetZipItem(m_zip, index++, &ze) != 0)
                break;

            if(ze.unc_size <= 0)
                continue;

            int len = _tcslen(ze.name);
            if (len <= 0)
                continue;

            DWORD dwSize = ze.unc_size;
            LPBYTE data = new BYTE[dwSize];
            ZRESULT zr = UnzipItem(m_zip, index-1, data, dwSize);
            if (zr != 0x00000000 && zr != 0x00000600)
            {
                delete[] data;
                data = NULL;
                continue;
            }
            
            _tcslwr(ze.name);
            TCHAR* ext = _tcsstr(ze.name, _T(".xml"));
            bool isXml = ext && (len - (ext - ze.name) == 4);

            int index = 0;
            do 
            {
                TCHAR& c = ze.name[index++];
                if (c == L'\0')
                    break;

                if (c == L'/')
                    c = L'\\';
            } while (1);


            if (isXml)
                m_xmlData.insert(std::make_pair(ze.name, DataInfoPtr(new DataInfo(dwSize, data))));
            else
                m_imageData.insert(std::make_pair(ze.name, DataInfoPtr(new DataInfo(dwSize, data))));
        }

        return true;
    }

}