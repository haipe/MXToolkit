#pragma once

#include <iostream>
#include <string>
#include <windows.h>

#include "mxkit.h"
#include "base/string.h"
#include "base/string_utils.h"

_BEGIN_MX_KIT_NAME_SPACE_


#ifdef _MX_WIN_

template<
    typename ConvertFrom
    ,typename ConvertTo
    ,typename BaseFromType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
    = typename ConvertFrom::allocator_type::value_type
#endif
    ,typename BaseToType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
    = typename ConvertTo::allocator_type::value_type
#endif
>
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
    typename ConvertFrom
    , typename ConvertTo
    , typename BaseFromType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
    = typename ConvertFrom::allocator_type::value_type
#endif
    , typename BaseToType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
    = typename ConvertTo::allocator_type::value_type
#endif
>
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

class Win32StringConvert
{
public:

	static WString& Utf8ToUnicode(const CHAR* utf8, WString& unicode)
	{
		// 预算-缓冲区中宽字节的长度      
		int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
		// 给指向缓冲区的指针变量分配内存      
		wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
		// 开始向缓冲区转换字节      
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, pUnicode, unicodeLen);

		unicode = pUnicode;
		free(pUnicode);

		return unicode;
	}

	static AString& Utf8ToAnsii(const CHAR* utf8, AString& ansii)
	{
		ansii.clear();
		std::wstring unicode;
		if (Utf8ToUnicode(utf8, unicode).empty())
			return ansii;

		UnicodeToAnsii(unicode.c_str(), ansii);
		return ansii;
	}

	static WString& AnsiiToUnicode(const CHAR* ansii, WString& unicode)
	{
		// 预算-缓冲区中宽字节的长度      
		int unicodeLen = MultiByteToWideChar(CP_ACP, 0, ansii, -1, nullptr, 0);
		// 给指向缓冲区的指针变量分配内存      
		wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
		// 开始向缓冲区转换字节      
		MultiByteToWideChar(CP_ACP, 0, ansii, -1, pUnicode, unicodeLen);

		unicode = pUnicode;
		free(pUnicode);

		return unicode;
	}

	static AString& AnsiiToUtf8(const CHAR* ansii, AString& utf8)
	{
		utf8.clear();
		std::wstring unicode;
		if (AnsiiToUnicode(ansii, unicode).empty())
			return utf8;

		UnicodeToUtf8(unicode.c_str(), utf8);
		return utf8;
	}

	static AString& UnicodeToUtf8(const WCHAR* unicode, AString& utf8)
	{
		// 预算-缓冲区中多字节的长度      
		int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, nullptr, 0, nullptr, nullptr);
		// 给指向缓冲区的指针变量分配内存      
		char* pUtf8 = (char*)malloc(sizeof(char) * ansiiLen);
		// 开始向缓冲区转换字节      
		WideCharToMultiByte(CP_UTF8, 0, unicode, -1, pUtf8, ansiiLen, nullptr, nullptr);

		utf8 = pUtf8;
		free(pUtf8);

		return utf8;
	}

	static AString& UnicodeToAnsii(const WCHAR* unicode, AString& ansii)
	{
		// 预算-缓冲区中多字节的长度      
		int ansiiLen = WideCharToMultiByte(CP_ACP, 0, unicode, -1, nullptr, 0, nullptr, nullptr);
		// 给指向缓冲区的指针变量分配内存      
		char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
		// 开始向缓冲区转换字节      
		WideCharToMultiByte(CP_ACP, 0, unicode, -1, pAssii, ansiiLen, nullptr, nullptr);

		ansii = pAssii;
		free(pAssii);

		return ansii;

	}	
};

#endif

class StringConvert
{
public:

	
	/*
	URL Encode & Decode
	*/
	static AString UrlEncode(const AString &str_source)
	{
		char const *in_str = str_source.c_str();
		size_t in_str_len = strlen(in_str);
		std::string out_str;
#if __cplusplus > 199711L
		unsigned char c;
#else
		register unsigned char c;
#endif
		unsigned char *to, *start;
		unsigned char const *from, *end;
		unsigned char hexchars[] = "0123456789ABCDEF";

		from = (unsigned char *)in_str;
		end = (unsigned char *)in_str + in_str_len;
		start = to = (unsigned char *)malloc(3 * in_str_len + 1);

		while (from < end) {
			c = *from++;

			if (c == ' ') {
				*to++ = '+';
			}
			else if ((c < '0' && c != '-' && c != '.') ||
				(c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') ||
				(c > 'z')) {
					to[0] = '%';
					to[1] = hexchars[c >> 4]; // equal hexchars[c / 16];
					to[2] = hexchars[c & 15]; // equal hexchars[c % 16];
					to += 3;
			}
			else {
				*to++ = c;
			}
		}
		*to = 0;

		out_str = (char *)start;
		free(start);
		return out_str;
	}

	static inline int _htoi(char *s)
	{
		int value;
		int c;

		c = ((unsigned char *)s)[0];
		if (isupper(c))
			c = tolower(c);
		value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

		c = ((unsigned char *)s)[1];
		if (isupper(c))
			c = tolower(c);
		value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

		return (value);
	}

	static AString UrlDecode(const AString &str_source)
	{
		char const *in_str = str_source.c_str();
		size_t in_str_len = strlen(in_str);
		std::string out_str;
		char *str;

#ifdef WIN32
		str = _strdup(in_str);
#else
		str = strdup(in_str);
#endif

		char *dest = str;
		char *data = str;

		while (in_str_len--) {
			if (*data == '+') {
				*dest = ' ';
			}
			else if (*data == '%' && in_str_len >= 2 && isxdigit((int) *(data + 1))
				&& isxdigit((int) *(data + 2))) {
					*dest = (char)_htoi(data + 1);
					data += 2;
					in_str_len -= 2;
			}
			else {
				*dest = *data;
			}
			data++;
			dest++;
		}
		*dest = '\0';
		out_str = str;
		free(str);
		return out_str;
	}
};


_END_MX_KIT_NAME_SPACE_