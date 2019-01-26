#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include "MXCommon.h"
using namespace std;

namespace mxtoolkit
{
    class SCKit
    {
    public:
        static WString& Utf8ToUnicode(const CHAR* utf8,WString& unicode);
        static WString& AnsiiToUnicode(const CHAR* ansii,WString& unicode);
        static AString& UnicodeToUtf8(const WCHAR* unicode, AString& utf8);
        static AString& UnicodeToAnsi(const WCHAR* unicode, AString& ansii);
    };

    class StringConvert
    {
    public:

        static void UTF_8ToGB2312(string &pOut, char *pText, int pLen); //utf_8转为gb2312
        static void GB2312ToUTF_8(string &pOut, char *pText, int pLen); //gb2312 转utf_8
        static string UrlGB2312(char * str);                            //urlgb2312编码
        static string UrlUTF8(char * str);                              //urlutf8 编码
        static string UrlUTF8Decode(string str);                        //urlutf8解码
        static string UrlGB2312Decode(string str);                      //urlgb2312解码

        static bool AToW(wstring &pOut, const char *pText, int pLen);
        static bool WToA(string &pOut, const wchar_t *pText, int pLen);


    private:
        static void Gb2312ToUnicode(WCHAR* pOut, char *gbBuffer);
        static void UTF_8ToUnicode(WCHAR* pOut, char *pText);
        static void UnicodeToUTF_8(char* pOut, WCHAR* pText);
        static void UnicodeToGB2312(char* pOut, WCHAR uData);
        static char CharToInt(char ch);
        static char StrToBin(char *str);
    };
}