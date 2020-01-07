#include "StdAfx.h"
#include <algorithm>
#include <set>
#include "MXPath.h"
#include "MXCommon.h"
#ifdef WIN32
#include <WinCrypt.h>
#endif

namespace mxtoolkit
{

    bool Path::GetFilePathInfo(
        const std::string& file, std::string* path, std::string* name, std::string* ext)
    {
        int nPosDot = file.rfind('.');

        int nPos = file.rfind(_MX_DIR_STRING_A);

        if ((nPos == std::string::npos || nPosDot == std::string::npos)
            && (nPosDot <= nPos))
        {
            return false;
        }

        std::string str = file.substr(0, nPos + 1);
        //strlwr(CONST_CAST(TCHAR*,str.c_str()));
        if (path)	*path = str;
        //std::transform(str.begin(), str.end(), path.begin(), ::tolower);

        str = file.substr(nPosDot + 1);
        //strlwr(CONST_CAST(TCHAR*,str.c_str()));
        if (ext) *ext = str;
        //std::transform(str.begin(), str.end(), ext.begin(), ::tolower);

        str = file.substr(nPos + 1, nPosDot - nPos - 1);
        //strlwr(CONST_CAST(TCHAR*,str.c_str()));
        if (name)*name = str;
        //std::transform(str.begin(), str.end(), name.begin(), ::tolower);

        return true;
    }

    bool Path::GetFilePathInfo(
        const std::wstring& file, std::wstring* path, std::wstring* name, std::wstring* ext)
    {
        int nPosDot = file.rfind(L'.');
        int nPos = file.rfind(_MX_DIR_STRING_W);

        if ((nPos == std::wstring::npos || nPosDot == std::wstring::npos)
            && (nPosDot <= nPos))
        {
            return false;
        }

        std::wstring str = file.substr(0, nPos + 1);

        //wcslwr(CONST_CAST(TCHAR*,str.c_str()));
        if (path)*path = str;
        //std::transform(str.begin(), str.end(), path.begin(), towlower);

        str = file.substr(nPosDot + 1);
        //wcslwr(CONST_CAST(TCHAR*,str.c_str()));
        if (ext)*ext = str;
        //std::transform(str.begin(), str.end(), ext.begin(), towlower);

        str = file.substr(nPos + 1, nPosDot - nPos - 1);
        //wcslwr(CONST_CAST(TCHAR*,str.c_str()));
        if (name)*name = str;
        //std::transform(str.begin(), str.end(), name.begin(), towlower);

        return true;
    }

    bool Path::GetFilePathInfo(const std::string& file, std::string* path, std::string* name)
    {
        int nPos = file.rfind(_MX_DIR_STRING_A);
        if (nPos == std::string::npos)
        {
            return false;
        }

        std::string str = file.substr(0, nPos + 1);
        //strlwr(CONST_CAST(TCHAR*,str.c_str()));
        if (path)*path = str;
        //std::transform(str.begin(), str.end(), path.begin(), ::tolower);

        str = file.substr(nPos + 1);
        //strlwr(CONST_CAST(TCHAR*,str.c_str()));
        if (name)*name = str;
        //std::transform(str.begin(), str.end(), name.begin(), ::tolower);

        return true;
    }

    bool Path::GetFilePathInfo(const std::wstring& file, std::wstring* path, std::wstring* name)
    {
        int nPos = file.rfind(_MX_DIR_STRING_W);

        if (nPos == std::wstring::npos)
        {
            return false;
        }

        std::wstring str = file.substr(0, nPos + 1);

        //wcslwr(CONST_CAST(TCHAR*,str.c_str()));
        if (path)*path = str;
        //std::transform(str.begin(), str.end(), path.begin(), towlower);

        str = file.substr(nPos + 1);
        //wcslwr(CONST_CAST(TCHAR*,str.c_str()));
        if (name)*name = str;
        //std::transform(str.begin(), str.end(), name.begin(), towlower);

        return true;
    }

    bool Path::GetFolderPathInfo(const std::string& folder, std::string* path, std::string* name)
    {
        std::string str = folder;
        int len = str.length();
        while (len)
        {
            len--;
            if (str[len] == _MX_DIR_CHAR_A)
            {
                str[len] = '\0';
                continue;
            }

            break;
        }

        size_t pos = str.rfind(_MX_DIR_CHAR_A);
        if (pos > 0)
        {
            *path = str.substr(0, pos + 1);
            *name = str.substr(pos + 1);
        }

        return !path->empty();
    }

    bool Path::GetFolderPathInfo(const std::wstring& folder, std::wstring* path, std::wstring* name)
    {
        std::wstring str = folder;
        int len = str.length();
        while (len)
        {
            len--;
            if (str[len] == _MX_DIR_CHAR_W)
            {
                str[len] = L'\0';
                continue;
            }

            break;
        }

        size_t pos = str.rfind(_MX_DIR_CHAR_W);
        if (pos > 0)
        {
            *path = str.substr(0, pos + 1);
            *name = str.substr(pos + 1);
        }

        return !path->empty();
    }

    std::string Path::GetFileExtName(const std::string& file)
    {
        std::string ext_low = file;
        int nPos = file.rfind('.');
        if (nPos != std::string::npos)
        {
            ext_low = file.substr(nPos + 1);
        }

        //获取文档扩展名
        CHAR *szFind = (CHAR*)ext_low.c_str();
        if (szFind)_strlwr(szFind);

        if (szFind)
        {
            ext_low = szFind;
        }

        return ext_low;
    }

    std::wstring Path::GetFileExtName(const std::wstring& file)
    {
        std::wstring ext_low = file;
        int nPos = file.rfind(L'.');
        if (nPos != std::wstring::npos)
        {
            ext_low = file.substr(nPos + 1);
        }

        //获取文档扩展名
        WCHAR *szFind = (WCHAR*)ext_low.c_str();
        if (szFind)_wcslwr(szFind);

        if (szFind)
        {
            ext_low = szFind;
        }

        return ext_low;
    }

#ifndef WIN32

    //创建目录，多级创建
    int Path::CreateDir(const CHAR* sPathName)
    {
        std::string path = sPathName;
        ReplaceAll(&path, "//", "/");
        CHAR dirName[512] = { 0 };
        strcpy(dirName, path.c_str());
        int i = strlen(dirName);
        int len = i;
        if (dirName[len - 1] != _MX_DIR_CHAR_A)
            strcat(dirName, _MX_DIR_STRING_A);

        len = strlen(dirName);
        for (i = 1; i < len; i++)
        {
            if (dirName[i] == _MX_DIR_CHAR_A)
            {
                dirName[i] = 0;
                if (access(dirName, 0) != 0)
                {
                    if (mkdir(dirName, 0755) == -1)
                    {
                        return   -1;
                    }
                }
                dirName[i] = _MX_DIR_CHAR_A;
            }
        }

        return   0;
    }

#endif

#ifdef WIN32

    bool Path::GetMD5(const std::wstring& file, std::wstring* md5)
    {
        HANDLE hFile = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            CloseHandle(hFile);
            return FALSE;
        }

        HCRYPTPROV hProv = NULL;
        //获得CSP中一个密钥容器的句柄
        if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE)
        {
            return FALSE;
        }
        HCRYPTPROV hHash = NULL;
        //初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。
        //这个句柄接下来将被CryptHashData调用。
        if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
        {
            return FALSE;
        }

        DWORD dwFileSize = GetFileSize(hFile, 0);    //获取文件的大小
        if (dwFileSize == 0xFFFFFFFF)               //如果获取文件大小失败  
        {
            return FALSE;
        }
        BYTE* lpReadFileBuffer = new BYTE[dwFileSize];
        DWORD lpReadNumberOfBytes;
        if (ReadFile(hFile, lpReadFileBuffer, dwFileSize, &lpReadNumberOfBytes, NULL) == 0)        //读取文件  
        {
            return FALSE;
        }
        if (CryptHashData(hHash, lpReadFileBuffer, lpReadNumberOfBytes, 0) == FALSE)      //hash文件  
        {
            return FALSE;
        }

        delete[] lpReadFileBuffer;
        CloseHandle(hFile);          //关闭文件句柄
        BYTE *pbHash;
        DWORD dwHashLen = sizeof(DWORD);
        if (!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0))
        {
            return FALSE;
        }
        pbHash = (BYTE*)malloc(dwHashLen);
        if (CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))//获得md5值 
        {
            for (DWORD i = 0; i < dwHashLen; i++)         //输出md5值 
            {
                TCHAR str[10] = { 0 };
                swprintf(str, L"%02x\0", pbHash[i]);
                *md5 += str;
            }
        }

        md5->resize(32);
        //善后工作
        if (CryptDestroyHash(hHash) == FALSE)          //销毁hash对象  
        {
            return FALSE;
        }
        if (CryptReleaseContext(hProv, 0) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }

#endif
}