#pragma once
#include <memory>
#include <map>

namespace DuiLib
{

    class ResourceCacheUtil
    {
    public:
        ResourceCacheUtil();
        ~ResourceCacheUtil();

        static ResourceCacheUtil*  GetInstance();

    public:
        bool                Init();
        void                Release();

        void                ResetAll();
        void                ResetXml(LPCTSTR res);
        void                ResetImage(LPCTSTR res);

    public:
        bool                PreLoadingXmlResource(LPCTSTR res);
        bool                PreLoadingImageResource(LPCTSTR res);

    public:
        bool                FindXml(LPCTSTR res, LPBYTE& data, DWORD& size);
        bool                FindXmlFromMemmry(LPCTSTR res, LPBYTE& data, DWORD& size);

        bool                FindImage(LPCTSTR res, LPBYTE& data, DWORD& size);
        bool                FindImageFromInstance(HINSTANCE instance, LPCTSTR type, LPCTSTR res, LPBYTE& data, DWORD& size);
        bool                FindImageFromLocal(LPCTSTR res, LPBYTE& data, DWORD& size);
        bool                FindImageFromMemmry(LPCTSTR res, LPBYTE& data, DWORD& size);

    public:
        LPBYTE              LoadResource(LPCTSTR res, DWORD& size);

        LPBYTE              LoadLocalResource(LPCTSTR res, DWORD& size);
        LPBYTE              LoadZipResource(LPCTSTR res, DWORD& size);

    protected:
        bool                LoadAllZipResource();

    protected:
        HZIP                            m_zip = NULL;

        struct DataInfo
        {
            DWORD   size = 0;
            LPBYTE  data = nullptr;

            DataInfo() {}
            DataInfo(DWORD s, LPBYTE d)
                : size(s)
                , data(d) {}

            ~DataInfo() { if (data)delete[] data; data = nullptr; size = 0; }
        };

        typedef std::shared_ptr<DataInfo>   DataInfoPtr;

        std::map<CDuiString, DataInfoPtr>   m_xmlData;
        std::map<CDuiString, DataInfoPtr>   m_imageData;

    protected:
        static ResourceCacheUtil*  m_zipManager;
    };

}