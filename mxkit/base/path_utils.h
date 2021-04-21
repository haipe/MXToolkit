#pragma once

#include "mxkit.h"
#include "base/string.h"

_BEGIN_MX_KIT_NAME_SPACE_

class PathUtils
{
public:

	template<
		typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= std::string
#endif
	>
    static bool FileInfo(const Str& file, Str* path, Str* name, Str* ext)
    {
        typedef typename Str::allocator_type::value_type CharType;
        CharType dotChar, dirChar;
        if (std::is_same<CharType, wchar_t>::value)
        {
            dotChar = (CharType)_MX_DOT_CHAR_W;
            dirChar = (CharType)_MX_DIR_CHAR_W;
        }
        else
        {
            dotChar = (CharType)_MX_DOT_CHAR_A;
            dirChar = (CharType)_MX_DIR_CHAR_A;
        }

        size_t nPosDot = file.rfind(dotChar);
        size_t nPos = file.rfind(dirChar);

        if ((nPos == Str::npos || nPosDot == Str::npos)
            && (nPosDot <= nPos))
        {
            return false;
        }

        Str str = file.substr(0, nPos + 1);
        if (path)
            *path = str;

        str = file.substr(nPosDot + 1);
        if (ext) *ext = str;

        str = file.substr(nPos + 1, nPosDot - nPos - 1);
        if (name)*name = str;
        //strlwr(CONST_CAST(TCHAR*,str.c_str()));
        //std::transform(str.begin(), str.end(), name.begin(), ::tolower);

        return true;
    }

	template<
		typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= std::string
#endif
	>
    static bool FileInfo(const Str& file, Str* path, Str* name)
    {
        typedef typename Str::allocator_type::value_type CharType;
        CharType dirChar;
        if (std::is_same<CharType, wchar_t>::value)
            dirChar = (CharType)_MX_DIR_CHAR_W;
        else
            dirChar = (CharType)_MX_DIR_CHAR_A;

        size_t nPos = file.rfind(dirChar);
        if (nPos == Str::npos)
        {
            return false;
        }

        Str str = file.substr(0, nPos + 1);
        if (path)*path = str;

        str = file.substr(nPos + 1);
        if (name)*name = str;
        
        return true;
    }

	template<
		typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= std::string
#endif
	>
    static Str FileExt(const Str& file)
    {
        typedef Str::allocator_type::value_type CharType;
        CharType dirChar;
        if (std::is_same<CharType, wchar_t>::value)
            dirChar = (CharType)_MX_DIR_CHAR_W;
        else
            dirChar = (CharType)_MX_DIR_CHAR_A;

        Str ext_low = file;
        size_t nPos = file.rfind(dirChar);
        if (nPos != std::string::npos)
        {
            ext_low = file.substr(nPos + 1);
        }

        return ext_low;
    }


	template<
		typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= std::string
#endif
	>
    bool FolderInfo(const Str& folder, Str* path, Str* name)
    {
        typedef typename Str::allocator_type::value_type CharType;
        CharType dirChar;
        if (std::is_same<CharType, wchar_t>::value)
            dirChar = (CharType)_MX_DIR_CHAR_W;
        else
            dirChar = (CharType)_MX_DIR_CHAR_A;

        Str str = folder;
        size_t len = str.length();
        while (len)
        {
            len--;
            if (str[len] == dirChar)
            {
                str[len] = '\0';
                continue;
            }

            break;
        }

        size_t pos = str.rfind(dirChar);
        if (pos > 0)
        {
            *path = str.substr(0, pos + 1);
            *name = str.substr(pos + 1);
        }

        return !path->empty();
    }

#if 0
    //创建目录，多级创建
    static int CreateDir(const CHAR* path)
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
#if 0

    static bool GetMD5(const std::wstring& file, std::wstring* md5)
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
        BYTE* pbHash;
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
};

_END_MX_KIT_NAME_SPACE_