#pragma once

#include "mxkit.h"
#include "base/string.h"
#include "base/string_utils.h"
_BEGIN_MX_KIT_NAME_SPACE_



struct Guid
{
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
};

class GuidUtils
{
public:
    template<typename Str = std::string>
    static Str NewGuidString()
    {
        GUID guid;
        CoCreateGuid(&guid);

        Str ret;
        Guid2String<Str>(guid, &ret);
        return ret;
    }


    template<typename Str = std::string>
    void Guid2String(const GUID& param, Str* out)
    {
        typedef typename Str::allocator_type::value_type CharType;
        CharType buf[64] = { 0 };
        if (std::is_same<CharType, wchar_t>::value)
        {
            swprintf(
                (CharType*)buf,
                sizeof(buf),
                L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1],
                guid.Data4[2], guid.Data4[3],
                guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);
        }
        else
        {
            snprintf(
                (CharType*)buf,
                sizeof(buf),
                "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1],
                guid.Data4[2], guid.Data4[3],
                guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);

        }

        *out = buf;
    }

    template<typename Str = std::string>
    void String2Guid(const Str& param, Guid* out)
    {
        typedef typename Str::allocator_type::value_type CharType;
        if (!param.empty())
        {
            std::vector<Str> vc;
            if (std::is_same<CharType, wchar_t>::value)
                SplitString<Str>(param, (const CharType*)L",", &vc);
            else
                SplitString<Str>(param, (const CharType*)",", &vc);

            if (vc.size() == 11)
            {
                long temp = 0;
                
                sscanf(vc[0].c_str(), "%x", &temp);
                out->Data1 = (uint32)temp;

                temp = 0;
                sscanf(vc[1].c_str(), "%x", &temp);
                out->Data2 = (uint16)temp;
                temp = 0;
                sscanf(vc[2].c_str(), "%x", &temp);
                out->Data3 = (uint16)temp;

                temp = 0;
                sscanf(vc[3].c_str(), "%x", &temp);
                out->Data4[0] = (uint8)temp;
                temp = 0;
                sscanf(vc[4].c_str(), "%x", &temp);
                out->Data4[1] = (uint8)temp;
                temp = 0;
                sscanf(vc[5].c_str(), "%x", &temp);
                out->Data4[2] = (uint8)temp;
                temp = 0;
                sscanf(vc[6].c_str(), "%x", &temp);
                out->Data4[3] = (uint8)temp;
                temp = 0;
                sscanf(vc[7].c_str(), "%x", &temp);
                out->Data4[4] = (uint8)temp;
                temp = 0;
                sscanf(vc[8].c_str(), "%x", &temp);
                out->Data4[5] = (uint8)temp;
                temp = 0;
                sscanf(vc[9].c_str(), "%x", &temp);
                out->Data4[6] = (uint8)temp;
                temp = 0;
                sscanf(vc[10].c_str(), "%x", &temp);
                out->Data4[7] = (uint8)temp;
            }
        }
    }
};


_END_MX_KIT_NAME_SPACE_