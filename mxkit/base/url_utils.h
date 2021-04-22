#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "mxkit.h"
#include "base/string_utils.h"

_BEGIN_MX_KIT_NAME_SPACE_

class UrlUtils
{
public:


    template<
        typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = std::string
#endif
    >
	struct Url
	{
		Str protocol, user, password, host, port, path, params;

		inline uint32 uPort()	
		{
			return StringUtils::ToValue<uint32>(port);
		}
	};

    template<
        typename Str
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = std::string
#endif
    >
        static Url<Str> Parse(const Str& str)
    {
        Url<Str> ret;

        std::string tempPath;
        mxkit::StringUtils::Split<std::string>(str, "?", &tempPath, &ret.params);

        mxkit::StringUtils::Split<std::string>(tempPath, "://", &ret.protocol, &tempPath);

        std::string tempStr;
        mxkit::StringUtils::Split<std::string>(tempPath, "@", &tempStr, &tempPath);
        if (!tempPath.empty())
            mxkit::StringUtils::Split<std::string>(tempStr, ":", &ret.user, &ret.password);
        else
            tempPath = tempStr;

        mxkit::StringUtils::Split<std::string>(tempPath, "/", &tempStr, &ret.path, false);

        mxkit::StringUtils::Split<std::string>(tempStr, ":", &ret.host, &ret.port);

        return ret;
    }


};

/*

std::string tempURL = "http://www.google.com:80/path?大大撒大";
std::string tempURL = "http://user:psd@www.google.com:80/path?大大撒大";
mxkit::UrlUtils::Url parsed = mxkit::UrlUtils::ParseHttpUrl(tempURL);
std::cout << "1 " << parsed.protocol << "\n"
<< "2 " << parsed.user << "\n"
<< "3 " << parsed.password << "\n"
<< "4 " << parsed.host << "\n"
<< "5 " << parsed.port << "\n"
<< "6 " << parsed.path << "\n"
<< "7 " << parsed.params << std::endl;

std::string us1;
mxkit::Win32StringConvert::AnsiiToUtf8(tempURL.c_str(),us1);
std::cout << "encode ansii:" << mxkit::StringConvert::UrlEncode(tempURL) << "\n";
std::cout << "encode utf8 :" << mxkit::StringConvert::UrlEncode(us1) << "\n";

std::string as = "http://www.google.com/path?%B4%F3%B4%F3%C8%F6%B4%F3";
std::cout << "ansii str   :" << as << "\n";
std::string as2 = mxkit::StringConvert::UrlDecode(as.c_str());
std::cout << "decode ansii:" << as2 << "\n";

std::string us = "http://www.google.com/path?%E5%A4%A7%E5%A4%A7%E6%92%92%E5%A4%A7";
std::cout << "utf8 str    :" << us << "\n";
std::string us2 = mxkit::StringConvert::UrlDecode(us.c_str());
std::cout << "decode utf8 :" << us2 << "\n";
std::string as3;
mxkit::Win32StringConvert::Utf8ToAnsii(us2.c_str(),as3);
std::cout << "utf8 to ansi:" << as3 << "\n";

*/

_END_MX_KIT_NAME_SPACE_