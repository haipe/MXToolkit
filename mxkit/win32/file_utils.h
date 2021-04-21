#pragma once

#include <vector>
#include <fstream>
#include <functional>
#include <windows.h>

#include "mxkit.h"
#include "base/string.h"

_BEGIN_MX_KIT_NAME_SPACE_


class FileReader
{
public:
    static bool ReadUnicode(const char* file, WString& buffer)
    {
        if (!file)
            return false;

        HANDLE hFile = CreateFileA(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return false;

        DWORD dwSize;
        DWORD dwFileSize = GetFileSize(hFile, NULL);
        int len = dwFileSize / 2 + 1;
        buffer.resize(len);
        wchar_t* pContent = (wchar_t*)buffer.c_str();

        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        WORD wdHead;
        ReadFile(hFile, &wdHead, 2, &dwSize, NULL);
        if (wdHead != 0xfeff)
        {
            SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        }

        ReadFile(hFile, pContent, dwFileSize, &dwSize, NULL);
        CloseHandle(hFile);
        return true;
    }

    static bool ReadUtf8(const char* file, AString& buffer)
    {
        if (!file)
            return false;

        std::ifstream sourceFile(file);
        bool b = sourceFile.is_open();
        if (b)
        {
            while (sourceFile.eof() == 0)
            {
                std::string line;
                std::getline(sourceFile, line);
                buffer += line;
            }
        }

        sourceFile.close();

        return !buffer.empty();
    }
};

class FileSystem
{
public:
    template<typename StringType>
    static void CreateFolder(const StringType& folderPath)
    {
        typedef StringType::allocator_type::value_type CharType;
        
        StringType file_name;
        StringType file_part_path = folderPath;
        StringType file_path;

        int symbol_index = 0;
        while (symbol_index != StringType::npos)
        {
            symbol_index = file_part_path.find('\\');

            file_name = file_part_path.substr(0, symbol_index + 1);
            file_part_path = file_part_path.substr(symbol_index + 1);

            file_name = symbol_index == -1 ? file_part_path : file_name;

            if (!file_name.empty())
            {
                file_path += file_name;

                if (_STD_NAME_SPACE_::is_same<CharType, wchar_t>::value)
                    CreateDirectoryW((LPCWSTR)file_path.c_str(), NULL);
                else
                    CreateDirectoryA((LPCSTR)file_path.c_str(), NULL);
            }
        }
    }

    template<typename StringType>
    static HANDLE FindFirstFile(const StringType& p, void* findData)
    {
        typedef StringType::allocator_type::value_type CharType;

        if (_STD_NAME_SPACE_::is_same<CharType, char>::value)
            return ::FindFirstFileA((LPCSTR)p.c_str(), (LPWIN32_FIND_DATAA)findData);

        return ::FindFirstFileW((LPCWSTR)p.c_str(), (LPWIN32_FIND_DATAW)findData);
    }

    template<typename StringType>
    static bool IsValidPath(void* findData, bool& isFolder, StringType& name)
    {
        typedef StringType::allocator_type::value_type CharType;

        bool isValid = true;
        if (_STD_NAME_SPACE_::is_same<CharType, char>::value)
        {
            LPWIN32_FIND_DATAA data = (LPWIN32_FIND_DATAA)findData;
            isFolder = (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;            
            name = (const CharType*)data->cFileName;
            if (isFolder)
                isValid = (strcmp(data->cFileName, ".") != 0 && strcmp(data->cFileName, "..") != 0);
        }
        else
        {
            LPWIN32_FIND_DATAW data = (LPWIN32_FIND_DATAW)findData;
            isFolder = (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            name = (const CharType*)data->cFileName;
            if (isFolder)
                isValid = (wcscmp(data->cFileName, L".") != 0 && wcscmp(data->cFileName, L"..") != 0);
        }

        return isValid;
    }

    template<typename StringType>
    static bool IsLastFile(HANDLE hFind, void* findData)
    {
        typedef StringType::allocator_type::value_type CharType;

        if (_STD_NAME_SPACE_::is_same<CharType, char>::value)
        {
            LPWIN32_FIND_DATAA data = (LPWIN32_FIND_DATAA)findData;
            return (!FindNextFileA(hFind, data));
        }
        
        LPWIN32_FIND_DATAW data = (LPWIN32_FIND_DATAW)findData;
        return (!FindNextFileW(hFind, data));
    }

    template<typename StringType>
    static void FoeachFolder(const StringType& folderPath, bool foreachChild, std::function<void(const StringType& /*file path*/, const StringType& /*file name*/, bool /*is folder*/)> f)
    {
        typedef StringType::allocator_type::value_type CharType;

        DWORD dwAttr = 0;
        if (_STD_NAME_SPACE_::is_same<CharType, char>::value)
            dwAttr = ::GetFileAttributesA((LPCSTR)folderPath.c_str());
        else
            dwAttr = ::GetFileAttributesW((LPCWSTR)folderPath.c_str());

        if (dwAttr == -1)
            return;

        if (!(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
        {
            StringType path = folderPath;
            path = path.substr(path.rfind('\\') + 1);
            //是文件
            if (f)
                f(folderPath, path, false);

            return;
        }

        StringType findStr = folderPath;        
        findStr.append(1, '\\');
        findStr.append(1, '*');
        findStr.append(1, '.');
        findStr.append(1, '*');

        std::vector<char> findFileDataBuffer(_STD_NAME_SPACE_::is_same<CharType, char>::value ? sizeof(WIN32_FIND_DATAA) : sizeof(WIN32_FIND_DATAW));
        //typedef _STD_NAME_SPACE_::function<LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData> dd;

        HANDLE hFind = FindFirstFile<StringType>(findStr.c_str(), (void*)&findFileDataBuffer[0]);
        if (INVALID_HANDLE_VALUE == hFind)
            return;

        while (TRUE)
        {
            bool isFolder = false;
            StringType pathName;
            if (IsValidPath<StringType>((void*)&findFileDataBuffer[0], isFolder, pathName))
            {
                StringType folder = folderPath;
                folder.append(1, '\\');
                folder += pathName;

                if (foreachChild)
                    FoeachFolder(folder, foreachChild, f);

                if (f)
                    f(folder, pathName, isFolder);
            }

            if (IsLastFile<StringType>(hFind, (void*)&findFileDataBuffer[0]))
                break;
        }

        FindClose(hFind);
    }

    template<typename StringType>
    static void FindFile(const StringType& lpPath, bool foreachChild, const StringType& filter, std::function<void(const std::string&, const std::string&, bool)> f)
    {
        FoeachFolder<StringType>(lpPath, foreachChild, [&](const std::string& path, const std::string& name, bool isFolder)
        {
            if (isFolder)
                return;

            if (name.find(filter) == std::string::npos)
                return;
            
            f(path, name, false);
        });
    }

    template<typename T>
    static bool FileExist(const T* filePath) { return false; }

    template<>
    static bool FileExist(const char* filePath)
    {
        if (!filePath)
            return false;
        return GetFileAttributesA(filePath) != -1;
    }

    template<>
    static bool FileExist(const wchar_t* filePath)
    {
        if (!filePath)
            return false;
        return GetFileAttributesW(filePath) != -1;
    }

    enum FILE_TYPE
    {
        FILE = 1,
        FOLDER = 2,
    };

    template<typename T>
    static int FileType(const T* filePath)
    {
        //0 不存在 ，1 文件，-1 目录
        return 0; 
    }

    template<>
    static int FileType(const char* filePath)
    {
        if (!FileExist<char>(filePath))
            return 0;
        return (GetFileAttributesA(filePath) & FILE_ATTRIBUTE_DIRECTORY) == 0 ? FILE : FOLDER;
    }

    template<>
    static int FileType(const wchar_t* filePath)
    {
        if (!FileExist<wchar_t>(filePath))
            return 0;
        return (GetFileAttributesW(filePath) & FILE_ATTRIBUTE_DIRECTORY) == 0 ? FILE : FOLDER;
	}


	template<
		typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= std::string
#endif
		,typename CharType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= typename Str::allocator_type::value_type
#endif
	>
    bool FileSize(const CharType* path, unsigned int* fileSize = nullptr)
    {
        if (!path || !FileExist<CharType>(path))
            return false;

        bool bValue = false;
        if (std::is_same<StrType, wchar_t>::value)
        {
            WIN32_FIND_DATAW  findFileData;
            HANDLE hFind = FindFirstFileW((LPCWSTR)path, &findFileData);
            if ((hFind != INVALID_HANDLE_VALUE))
            {
                bValue = true;
            }

            *fileSize = findFileData.nFileSizeLow;
            FindClose(hFind);
        }
        else
        {
            WIN32_FIND_DATAA  findFileData;
            HANDLE hFind = FindFirstFileA((LPCSTR)path, &findFileData);
            if ((hFind != INVALID_HANDLE_VALUE))
            {
                bValue = true;
            }

            *fileSize = findFileData.nFileSizeLow;
            FindClose(hFind);
        }

        return bValue;
    }

	template<
		typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= std::string
#endif
	>
    bool FileSize2(const Str& path, unsigned int* fileSize = nullptr)
    {
        typedef typename Str::allocator_type::value_type CharType;
        if (!path || !FileExist<CharType>(path))
            return false;

        if (std::is_same<StrType, wchar_t>::value)
        {
            std::wfstream f(path, std::ios::in);
            if (f.is_open())
            {
                if (fileSize)
                {
                    f.seekp(0, std::ios::end);
                    unsigned long long fileSz = f.tellg();
                    *fileSize = fileSz;
                }

                f.close();
                return true;
            }
        }

        return false;
    }


	template<
		typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
		= std::string
#endif
	>
    Str FileMd5(const Str& file)
    {
        typedef typename Str::allocator_type::value_type CharType;
    doAgain:
        std::string cmd = "certutil -hashfile ";
        cmd += file;
        cmd += " MD5";

        std::string res;
        AnonymousPip::CreateCmd<std::string>(cmd, [&res](const std::string& result) {
            res += result;
            });

        std::vector<std::string> resSplit;
        mxkit::SplitString<std::string>(res, "\r\n", &resSplit);

        if (resSplit.size() >= 2 && resSplit[1].length() == 32)
        {
            //printf("file MD5:%s.\n", resSplit[1].c_str());
            if (std::is_same<CharType, wchar_t>::value)
            {
                T r;
                mxkit::WAConvert<std::string, std::wstring>(resSplit[1].c_str(), (std::wstring*)&r);
                return r;
            }

            return resSplit[1];
        }

        //printf("Get MD5 Again.\n");
        goto doAgain;
        return "";
    }
};



_END_MX_KIT_NAME_SPACE_