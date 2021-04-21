#pragma once

#include "base/string.h"

#if _MX_VS_VER_ >= 2010


#include <string>
#include <vector>
#include <functional>

#include <windows.h>

#include "base/string_convert.h"

_BEGIN_MX_KIT_NAME_SPACE_

namespace RegKey
{
    enum Name
    {
        CLASSES_ROOT,
        CURRENT_USER,
        LOCAL_MACHINE,
        USERS,
        CURRENT_CONFIG
    };


}


struct RegKeyUtils
{
    RegKeyUtils(){ key = 0; }
    ~RegKeyUtils() { if (key) ::RegCloseKey(key); }

    operator PHKEY(){ return &key; }
    operator HKEY(){ return key; }

    HKEY key;
};

template<class T_CHAR>
class Regeditor
{
public:
    typedef T_CHAR*         TCHAR_PTR;
    typedef const T_CHAR*   CONST_TCHAR_PTR;
    typedef std::basic_string<T_CHAR, std::char_traits<T_CHAR>, std::allocator<T_CHAR>> StdTString;

    Regeditor(RegKey::Name k);

    ~Regeditor(){}

    //选择根键
    void Select(RegKey::Name nKey)
    {
        switch (nKey)
        {
        case RegKey::CLASSES_ROOT:
            m_rootKey.key = HKEY_CLASSES_ROOT;
            break;
        case RegKey::LOCAL_MACHINE:
            m_rootKey.key = HKEY_LOCAL_MACHINE;
            break;
        case RegKey::USERS:
            m_rootKey.key = HKEY_USERS;
            break;
        case RegKey::CURRENT_CONFIG:
            m_rootKey.key = HKEY_CURRENT_CONFIG;
            break;
        case RegKey::CURRENT_USER:
        default:
            m_rootKey.key = HKEY_CURRENT_USER;
            break;
        }
    }

    bool Exist(CONST_TCHAR_PTR keyPath)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            return false;

        return true;
    }

    bool CreateKey(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS == reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
        {
            RegKeyUtils hNextKey;
            return (ERROR_SUCCESS == reg_create_key(hKey, keyName, hNextKey));
        }

        StdTString kp(keyPath);
        kp += StdTString(1, '\\');
        kp += keyName;
        return CreateKey(kp.c_str());
    }

    bool CreateKey(CONST_TCHAR_PTR keyPath)
    {
        /*
        std::vector<StdTString> pathVec;
        if (UTILS_NAME_SPACE_::SplitString<StdTString, std::vector<StdTString>>(StdTString(keyPath), StdTString(1, '\\'), &pathVec, true) > 0)
        {
        int i = 0;
        StdTString parent;
        StdTString kp;
        do
        {
        kp += pathVec[i++];

        KeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, kp.c_str(), 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
        {
        if (ERROR_SUCCESS == reg_open_key(m_rootKey, parent.c_str(), 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
        {
        KeyUtils hNextKey;
        StdTString key = pathVec[i - 1];
        reg_create_key(hKey, key.c_str(), hNextKey);
        }
        }

        kp += StdTString(1, '\\');

        if (!parent.empty())
        parent += StdTString(1, '\\');
        parent += pathVec[i - 1];

        } while (i != pathVec.size());

        return true;
        }
        */

        //找到第一个打不开的，一次创建
        StdTString kp(keyPath);
        StdTString sp(1, '\\');

        StdTString parent;
        size_t pos = 0;
        do
        {
            size_t np = kp.find_first_of(sp.c_str(), pos);
            StdTString key = kp.substr(0, np);

            RegKeyUtils hKey;
            if (ERROR_SUCCESS != reg_open_key(m_rootKey, key.c_str(), 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            {
                if (ERROR_SUCCESS == reg_open_key(m_rootKey, parent.c_str(), 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
                {
                    RegKeyUtils hNextKey;
                    key = kp.substr(pos);
                    LSTATUS ls = RegCreateKeyA(hKey, key.c_str(), hNextKey);
                    //reg_create_key(hKey, key.c_str(), hNextKey);

                    DWORD er = GetLastError();
                    er = er;
                    return true;
                }
            }

            parent = key;
            pos = np + 1;

        } while (true);

        return false;
    }

    bool CreateString(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName, CONST_TCHAR_PTR keyValue, DWORD len)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            return false;

        return (ERROR_SUCCESS == reg_set_value(hKey, keyName, 0, REG_SZ, (const BYTE*)keyValue, len));
    }

    bool CreateStringEx(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName, const std::basic_string<T_CHAR, std::char_traits<T_CHAR>, std::allocator<T_CHAR> >& value)
    {
        return CreateString(keyPath, keyName, value.c_str(), value.length());
    }

    bool CreateValue(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName, DWORD value)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            return false;

        return (ERROR_SUCCESS == reg_set_value(hKey, keyName, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD)));
    }

    bool DeleteValue(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            return false;

        return (ERROR_SUCCESS == reg_delete_key(hKey, keyName));
    }

    bool GetString(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName, T_CHAR* value, DWORD& len)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_READ | KEY_WOW64_64KEY, hKey))
            return false;

        DWORD dwType = REG_SZ;
        return (ERROR_SUCCESS == reg_query_value(hKey, keyName, 0, &dwType, (LPBYTE)&value, &len));
    }

    bool GetStringEx(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName, std::basic_string<T_CHAR, std::char_traits<T_CHAR>, std::allocator<T_CHAR> >& value)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_READ | KEY_WOW64_64KEY, hKey))
            return false;

        DWORD dwKeyValueSize = 1024;
        T_CHAR buffer[1024] = { 0 };
        DWORD dwType = REG_SZ;
        if (ERROR_SUCCESS == reg_query_value(hKey, keyName, 0, &dwType, (LPBYTE)buffer, &dwKeyValueSize))
        {
            value = buffer;
            return true;
        }

        return false;
    }

    bool GetValue(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName, DWORD& value)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_READ | KEY_WOW64_64KEY, hKey))
            return false;

        DWORD dwGetValue = 0;
        DWORD dwSize = sizeof(DWORD);
        DWORD dwType = REG_DWORD;
        if (ERROR_SUCCESS == reg_query_value(hKey, keyName, 0, &dwType, (LPBYTE)&dwGetValue, &dwSize))
        {
            value = dwGetValue;
            return true;
        }

        return false;
    }

    bool GetData(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName, BYTE* value, DWORD& len)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_READ | KEY_WOW64_64KEY, hKey))
            return false;

        DWORD dwType = REG_BINARY;
        return if (ERROR_SUCCESS == reg_query_value(hKey, keyName, 0, &dwType, (LPBYTE), , &len));
    }

    bool BackupKey(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR fileName)
    {
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            return false;
        }

        LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        int b = GetLastError();

        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            return false;

        return (ERROR_SUCCESS != reg_save_to_file(hKey, fileName, NULL));
    }

    bool RestoreKey(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR fileName, bool isSure)
    {
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            return false;
        }

        LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            return false;

        DWORD dwFlag;
        if (isSure)
        {
            dwFlag = REG_FORCE_RESTORE;
        }
        else
        {
            dwFlag = REG_WHOLE_HIVE_VOLATILE;
        }

        return (ERROR_SUCCESS != reg_restore_from_file(hKey, fileName, dwFlag));
    }

    bool DeleteKey(CONST_TCHAR_PTR keyPath, CONST_TCHAR_PTR keyName)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hKey))
            return false;

        return (ERROR_SUCCESS == reg_delete_key(hKey, keyName));
    }

    bool ForEachKey(CONST_TCHAR_PTR keyPath, _STD_FUNCTION_<bool(const RegKeyUtils& key, CONST_TCHAR_PTR path, CONST_TCHAR_PTR name)> f)
    {
        RegKeyUtils hKey;
        if (ERROR_SUCCESS != reg_open_key(m_rootKey, keyPath, 0, KEY_SET_VALUE | KEY_WOW64_64KEY | KEY_ENUMERATE_SUB_KEYS, hKey))
            return false;

        for (unsigned int i = 0;;i++)
        {
            T_CHAR buffer[MAX_PATH] = { 0 };
            DWORD nLen = MAX_PATH;
            LSTATUS ls = reg_enum_key(hKey, i, buffer, nLen);
            DWORD er = GetLastError();

            if (ls != ERROR_SUCCESS)
                break;

            RegKeyUtils hSubKey;
            StdTString s = keyPath;
            s.append(1, '\\');
            s += buffer;
            if (ERROR_SUCCESS == reg_open_key(m_rootKey, s.c_str(), 0, KEY_SET_VALUE | KEY_WOW64_64KEY, hSubKey))
            {
                if (!f(hSubKey, s.c_str(), buffer))
                    break;
            }
        }
        return true;
    }

protected:
    RegKeyUtils m_rootKey;

    _STD_FUNCTION_<LSTATUS(HKEY, CONST_TCHAR_PTR, DWORD, REGSAM, PHKEY)>              reg_open_key;
    _STD_FUNCTION_<LSTATUS(HKEY, CONST_TCHAR_PTR, PHKEY)>                             reg_create_key;
    _STD_FUNCTION_<LSTATUS(HKEY, CONST_TCHAR_PTR, DWORD, DWORD, const BYTE*, DWORD)>  reg_set_value;
    _STD_FUNCTION_<LSTATUS(HKEY, CONST_TCHAR_PTR)>                                    reg_delete_key;
    _STD_FUNCTION_<LSTATUS(HKEY, CONST_TCHAR_PTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD)> reg_query_value;

    _STD_FUNCTION_<LSTATUS(HKEY, CONST_TCHAR_PTR, LPSECURITY_ATTRIBUTES)>             reg_save_to_file;
    _STD_FUNCTION_<LSTATUS(HKEY, CONST_TCHAR_PTR, DWORD)>                             reg_restore_from_file;

    _STD_FUNCTION_<LSTATUS(HKEY, DWORD, TCHAR_PTR, DWORD)>                      reg_enum_key;

    _STD_FUNCTION_<LSTATUS(HKEY, DWORD, TCHAR_PTR, LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD)> reg_enum_value;
};

template<>
Regeditor<char>::Regeditor(RegKey::Name k)
{
    reg_open_key = std::bind(RegOpenKeyExA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5);
    reg_create_key = std::bind(RegCreateKeyA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3);
    reg_set_value = std::bind(RegSetValueExA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5, _STD_PLACE_HOLDERS_::_6);
    reg_delete_key = std::bind(RegDeleteValueA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2);
    reg_query_value = std::bind(RegQueryValueExA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5, _STD_PLACE_HOLDERS_::_6);

    reg_save_to_file = std::bind(RegSaveKeyA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3);
    reg_restore_from_file = std::bind(RegRestoreKeyA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3);

    reg_enum_key = std::bind(RegEnumKeyA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4);
    reg_enum_value = std::bind(RegEnumValueA, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5, _STD_PLACE_HOLDERS_::_6, _STD_PLACE_HOLDERS_::_7, _STD_PLACE_HOLDERS_::_8);

    Select(k);
}


template<>
Regeditor<wchar_t>::Regeditor(RegKey::Name k)
{
    reg_open_key = std::bind(RegOpenKeyExW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5);
    reg_create_key = std::bind(RegCreateKeyW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3);
    reg_set_value = std::bind(RegSetValueExW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5, _STD_PLACE_HOLDERS_::_6);
    reg_delete_key = std::bind(RegDeleteValueW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2);
    reg_query_value = std::bind(RegQueryValueExW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5, _STD_PLACE_HOLDERS_::_6);

    reg_save_to_file = std::bind(RegSaveKeyW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3);
    reg_restore_from_file = std::bind(RegRestoreKeyW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3);

    reg_enum_key = std::bind(RegEnumKeyW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4);
    reg_enum_value = std::bind(RegEnumValueW, _STD_PLACE_HOLDERS_::_1, _STD_PLACE_HOLDERS_::_2, _STD_PLACE_HOLDERS_::_3, _STD_PLACE_HOLDERS_::_4, _STD_PLACE_HOLDERS_::_5, _STD_PLACE_HOLDERS_::_6, _STD_PLACE_HOLDERS_::_7, _STD_PLACE_HOLDERS_::_8);

    Select(k);
}


#endif

_END_MX_KIT_NAME_SPACE_