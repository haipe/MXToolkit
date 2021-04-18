#pragma once
#include <string>
#include <vector>
#include <functional>

#include "mxkit.h"
#include "base/string.h"


#include <Windows.h>

_BEGIN_MX_KIT_NAME_SPACE_



template<typename T,
    typename Str = std::string>
std::string ToString(const T& value)
{
    typedef typename Str::allocator_type::value_type CharType;
    if (std::is_same<CharType, wchar_t>::value && std::is_same<Str, std::string>::value)
    {
        std::wstringstream ss;
        ss << value;
        return ss.str();
    }
    else
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
}

template<typename Str = std::string>
Str&& ToString(const RECT& rc)
{
    typedef typename Str::allocator_type::value_type CharType;
    CharType cache[64] = { 0 };
    if (std::is_same<CharType, wchar_t>::value && std::is_same<Str, std::string>::value)
    {
        _snwprintf(
            cache,
            sizeof(cache) * sizeof(WCHAR),
            L"{l:%d,t:%d,r:%d,b:%d,w:%d,h:%d}",
            rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top);

    }
    else
    {
        snprintf(cache,
            sizeof(cache) * sizeof(CHAR),
            "{l:%d,t:%d,r:%d,b:%d,w:%d,h:%d}",
            rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top);
    }

    Str ret;
    ret = (CharType)cache;

    return std::move(ret);
}


template<typename Str = std::string>
Str& EraseLast(Str& in, const Str& willErase)
{
    //删除最后字符，并判断
    if (cnt == 0 || willErase.empty())
        return in;

    size_t pos = in.rfind(willErase);
    size_t len = in.length();
    size_t eraseLen = willErase.length();
    while (pos != Str::npos && pos == (len - 1))
    {
        in.resize(in.size() - eraseLen);
        pos = in.rfind(willErase);
        len = in.length();
    }

    return in;
}


template<typename T = std::string>
T& ReplaceString(T& in, const T& pattern, const T& newpat, unsigned int* cnt = nullptr)
{
    int count = 0;
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();

    for (size_t pos = in.find(pattern, 0); pos != T::npos; pos = in.find(pattern, pos + nsize))
    {
        in.replace(pos, psize, newpat);
        count++;
    }

    if (cnt)
        *cnt = count;

    return in;
}

template<typename T = std::string, typename Tout = std::vector<T>>
unsigned int SplitString(const T& in, const T& sp, Tout* out, bool removeEmpty = false)
{
    if (in.empty() || out == nullptr)
        return 0;

    out->clear();
    size_t bpos = 0;
    size_t pos = in.find(sp);

    T x = in.substr(bpos, pos);

    if (!removeEmpty || !x.empty())
        out->push_back(x);

    if (pos == T::npos)
        return (unsigned int)out->size();

    size_t sp_len = sp.length();
    size_t in_len = in.length();

    bpos = pos + sp_len;

    while (true)
    {
        pos = in.find(sp, bpos);

        x = in.substr(bpos, pos - bpos);
        if (!removeEmpty || !x.empty())
            out->push_back(x);

        if (pos == T::npos)
            break;

        bpos = pos + sp_len;
        if (bpos >= in_len)
        {
            if (!removeEmpty)
                out->push_back(T());

            break;
        }
    }

    return (unsigned int)out->size();
}

template<typename T = std::string, typename Tout = std::vector<T>>
unsigned int SplitString(const T& in, const T& sp, std::function<void(unsigned int index ,const T& item)> onOneString)
{
    if (in.empty() || !onOneString)
        return 0;

    size_t bpos = 0;
    size_t pos = in.find(sp);

    T x = in.substr(bpos, pos);

    unsigned int index = 0;
    onOneString(index++, x);

    if (pos == T::npos)
        return index;
    
    size_t sp_len = sp.length();
    size_t in_len = in.length();
    bpos = pos + sp_len;

    while (true)
    {
        pos = in.find(sp, bpos);

        x = in.substr(bpos, pos - bpos);
        onOneString(index++, x);

        if (pos == T::npos)
            break;

        bpos = pos + sp_len;
        if (bpos >= in_len)
        {
            onOneString(index++, T());
            return index;
        }
    }

    return index;
}


_END_MX_KIT_NAME_SPACE_