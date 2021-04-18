#pragma once

#include <iostream>
#include <string>
#include <windows.h>

#include "mxkit.h"
#include "base/string.h"

_BEGIN_MX_KIT_NAME_SPACE_

template<
    typename ConvertFrom,
    typename ConvertTo,
    typename BaseFromType = ConvertFrom::allocator_type::value_type,
    typename BaseToType = ConvertTo::allocator_type::value_type>
    ConvertTo& WAConvert(const typename BaseFromType* from, typename ConvertTo* to)
{
    to->clear();
    if (from == nullptr)
        return *to;

    if (std::is_same<BaseFromType, wchar_t>::value && std::is_same<ConvertTo, std::string>::value)
    {
        //W to A
        // 预算-缓冲区中多字节的长度      
        int ansiiLen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)from, -1, nullptr, 0, nullptr, nullptr);
        // 给指向缓冲区的指针变量分配内存      
        char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
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
        wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
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
    to->clear();
    if (from == nullptr)
        return *to;

    if (std::is_same<BaseFromType, wchar_t>::value && std::is_same<ConvertTo, std::string>::value)
    {
        //W to utf8
        // 预算-缓冲区中多字节的长度      
        int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)from, -1, nullptr, 0, nullptr, nullptr);
        // 给指向缓冲区的指针变量分配内存      
        char* pUtf8 = (char*)malloc(sizeof(char) * ansiiLen);
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
        wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
        // 开始向缓冲区转换字节      
        MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)from, -1, pUnicode, unicodeLen);

        to->clear();
        to->append((const BaseToType*)pUnicode);
        free(pUnicode);
    }

    return *to;
}

class SCKit
{
public:
    static WString& Utf8ToUnicode(const CHAR* utf8, WString& unicode);
    static WString& AnsiiToUnicode(const CHAR* ansii, WString& unicode);
    static AString& UnicodeToUtf8(const WCHAR* unicode, AString& utf8);
    static AString& UnicodeToAnsi(const WCHAR* unicode, AString& ansii);
};

class StringConvert
{
public:

    static void UTF_8ToGB2312(std::string& pOut, char* pText, int pLen); //utf_8转为gb2312
    static void GB2312ToUTF_8(std::string& pOut, char* pText, int pLen); //gb2312 转utf_8
    static std::string UrlGB2312(char* str);                            //urlgb2312编码
    static std::string UrlUTF8(char* str);                              //urlutf8 编码
    static std::string UrlUTF8Decode(std::string str);                        //urlutf8解码
    static std::string UrlGB2312Decode(std::string str);                      //urlgb2312解码

    static bool AToW(std::wstring& pOut, const char* pText, int pLen);
    static bool WToA(std::string& pOut, const wchar_t* pText, int pLen);


private:
    static void Gb2312ToUnicode(WCHAR* pOut, char* gbBuffer);
    static void UTF_8ToUnicode(WCHAR* pOut, char* pText);
    static void UnicodeToUTF_8(char* pOut, WCHAR* pText);
    static void UnicodeToGB2312(char* pOut, WCHAR uData);
    static char CharToInt(char ch);
    static char StrToBin(char* str);
};


_END_MX_KIT_NAME_SPACE_