#pragma once
#include <Windows.h>
#include <fstream>
#include "MXStringKit.h"
#include "MXAnonymousPip.h"

namespace mxtoolkit
{
    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    bool FileExist(const StrType* path, unsigned int* fileSize = nullptr)
    {
        if (!path)
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

            FindClose(hFind);
        }
        return bValue;
    }

    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    bool FileExist2(const T& path, unsigned int* fileSize = nullptr)
    {
        if (path.empty())
            return false;

        //if (std::is_same<StrType, wchar_t>::value)
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

    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    BOOL FolderExist(const StrType* path)
    {
        if (!path)
            return false;

        bool bValue = false;
        if (std::is_same<StrType, wchar_t>::value)
        {
            std::wstring strPath((const wchar_t*)path);
            EraseLastString<std::wstring>(strPath, _MX_DIR_STRING_W);

            WIN32_FIND_DATAW  findFileData;
            HANDLE hFind = FindFirstFileW((LPCWSTR)strPath.c_str(), &findFileData);
            if ((hFind != INVALID_HANDLE_VALUE) && (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                bValue = true;
            }

            FindClose(hFind);
        }
        else
        {
            std::string strPath((const char*)path);
            EraseLastString<std::string>(strPath, _MX_DIR_STRING_A);

            WIN32_FIND_DATAA  findFileData;
            HANDLE hFind = FindFirstFileA((LPCSTR)strPath.c_str(), &findFileData);
            if ((hFind != INVALID_HANDLE_VALUE) && (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                bValue = true;
            }

            FindClose(hFind);
        }
        return bValue;
    }

    template<typename T = std::string, typename StrType = T::allocator_type::value_type>
    T GetMD5(const std::string& file)
    {
    doAgain:
        std::string cmd = "certutil -hashfile ";
        cmd += file;
        cmd += " MD5";

        std::string res;
        MXAnonymousPip::CreateCmd<std::string>(cmd, [&res](const std::string & result) {
            res += result;
        });

        std::vector<std::string> resSplit;
        mxtoolkit::SplitString<std::string>(res, "\r\n", &resSplit);

        if (resSplit.size() >= 2 && resSplit[1].length() == 32)
        {
            printf("file MD5:%s.\n", resSplit[1].c_str());
            if (std::is_same<StrType, wchar_t>::value)
            {
                T r;
                mxtoolkit::WAConvert<std::string, std::wstring>(resSplit[1].c_str(), (std::wstring*)&r);
                return r;
            }

            return resSplit[1];
        }

        printf("Get MD5 Again.\n");
        goto doAgain;
        return "";
    }
}