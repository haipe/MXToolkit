#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace mxtoolkit
{


    template<typename T = std::string, typename Tout = std::vector<T>>
    unsigned int SplitString(const T& in, const T& sp, Tout* out)
    {
        if (in.empty() || out == nullptr)return 0;

        out->clear();
        size_t bpos = 0;
        size_t pos = in.find(sp);

        T x = in.substr(bpos, pos);
        out->push_back(x);
        if (pos == T::npos)return out->size();

        unsigned int sp_len = sp.length();
        unsigned int in_len = in.length();

        while (true)
        {
            bpos = pos + sp_len;
            if (bpos >= in_len)
                break;

            pos = in.find(sp, bpos);

            x = in.substr(bpos, pos - bpos);
            out->push_back(x);

            if (pos == T::npos)
                break;
        }

        return out->size();
    }

    template<
        typename ConvertFrom,
        typename ConvertTo,
        typename BaseFromType = ConvertFrom::allocator_type::value_type,
        typename BaseToType = ConvertTo::allocator_type::value_type>
    ConvertTo& WAConvert(const typename BaseFromType* from, typename ConvertTo* to)
    {
        if (std::is_same<BaseFromType, wchar_t>::value && std::is_same<ConvertTo, std::string>::value)
        {
            //W to A
            // 预算-缓冲区中多字节的长度      
            int ansiiLen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)from, -1, nullptr, 0, nullptr, nullptr);
            // 给指向缓冲区的指针变量分配内存      
            char *pAssii = (char*)malloc(sizeof(char)*ansiiLen);
            // 开始向缓冲区转换字节      
            WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)from, -1, pAssii, ansiiLen, nullptr, nullptr);

            to->clear();
            to->append((const BaseToType*)pAssii);
            free(pAssii);
        }
        else if (std::is_same<BaseFromType, char>::value && std::is_same<ConvertTo, std::wstring>::value)
        {
            //A to W
            // 预算-缓冲区中宽字节的长度      
            int unicodeLen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)from, -1, nullptr, 0);
            // 给指向缓冲区的指针变量分配内存      
            wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen);
            // 开始向缓冲区转换字节      
            MultiByteToWideChar(CP_ACP, 0, (LPCSTR)from, -1, pUnicode, unicodeLen);

            to->clear();
            to->append((const BaseToType*)pUnicode);
            free(pUnicode);
        }

        return *to;
    }


    template<
        typename ConvertFrom, 
        typename ConvertTo, 
        typename BaseFromType = ConvertFrom::allocator_type::value_type,
        typename BaseToType = ConvertTo::allocator_type::value_type>
    ConvertTo& WUtf8Convert(const typename BaseFromType* from, typename ConvertTo* to)
    {
        if (std::is_same<BaseFromType, wchar_t>::value && std::is_same<ConvertTo, std::string>::value)
        {
            //W to utf8
            // 预算-缓冲区中多字节的长度      
            int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)from, -1, nullptr, 0, nullptr, nullptr);
            // 给指向缓冲区的指针变量分配内存      
            char *pUtf8 = (char*)malloc(sizeof(char)*ansiiLen);
            // 开始向缓冲区转换字节      
            WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)from, -1, pUtf8, ansiiLen, nullptr, nullptr);

            to->append((const BaseToType*)pUtf8);
            free(pUtf8);
        }
        else if (std::is_same<BaseFromType, char>::value && std::is_same<ConvertTo, std::wstring>::value)
        {
            //utf8 to W
            // 预算-缓冲区中宽字节的长度      
            int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)from, -1, nullptr, 0);
            // 给指向缓冲区的指针变量分配内存      
            wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen);
            // 开始向缓冲区转换字节      
            MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)from, -1, pUnicode, unicodeLen);
            
            to->clear();
            to->append((const BaseToType*)pUnicode);
            free(pUnicode);
        }

        return *to;
    }
}



