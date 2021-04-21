#pragma once
#include <string>

#include "windows.h"

#include <wtsapi32.h>
#pragma  comment (lib,"Wtsapi32.lib")

#include "mxkit.h"
#include "win32/windows_version.h"

_BEGIN_MX_KIT_NAME_SPACE_

class Win64DisableRedirect
{
public:
    Win64DisableRedirect()
    {
        if (isUse() < 0)
		{
			WinVersion::NTVersionInfo info;
            WinVersion os;
            os.GetWindowsVersion(info);
			isUse() = info.version > WinVersion::WIN_XP || os.Is64bitSystem();
        }

        if (isUse() > 0)
            Wow64EnableWow64FsRedirection(FALSE);
    }

    ~Win64DisableRedirect()
    {
        if (isUse() > 0)
            Wow64EnableWow64FsRedirection(TRUE);
    }

	int& isUse()
	{
		static int isUse = -1;
		return isUse;
	}
private:
};

class Win32System
{
public:
    template<
        typename StringType
#if _MX_DEFAULT_TEMPLATE_ARGUMENTS_
        = std::string
#endif
    >
    static StringType& GetLoginUseName(StringType& name)
    {
        typedef StringType::allocator_type::value_type CharType;
        if (name.empty())
        {
            CharType *pLogName = NULL;
            DWORD dwSize = 0;
            BOOL bOK = FALSE;
            if (_STD_NAME_SPACE_::is_same<CharType, wchar_t>::value)
            {
                bOK = WTSQuerySessionInformationW(
                    WTS_CURRENT_SERVER_HANDLE,
                    WTS_CURRENT_SESSION,
                    WTSUserName,
                    (LPWSTR*)&pLogName,
                    &dwSize);
            }
            else
            {
                bOK = WTSQuerySessionInformationA(
                    WTS_CURRENT_SERVER_HANDLE,
                    WTS_CURRENT_SESSION,
                    WTSUserName,
                    (LPSTR*)&pLogName,
                    &dwSize);
            }

            if (bOK)
            {
                name = pLogName;
                WTSFreeMemory(pLogName);
            }
        }
        else
        {
            CharType buff[MAX_PATH] = { 0 };
            DWORD len = 0;
            if (std::is_same<CharType, wchar_t>::value)
                GetUserNameW((LPWSTR)buff, &len);
            else
                GetUserNameA((LPSTR)buff, &len);

            name = buff;
        }

        return name;
    }



#if _MX_CPP_VER_ < 11
    template<>
    static std::string& GetLoginUseName(std::string& name)
    {
        typedef std::string::allocator_type::value_type CharType;
        if (name.empty())
        {
            CharType *pLogName = NULL;
            DWORD dwSize = 0;
            BOOL bOK = WTSQuerySessionInformationA(
                WTS_CURRENT_SERVER_HANDLE,
                WTS_CURRENT_SESSION,
                WTSUserName,
                (LPSTR*)&pLogName,
                &dwSize);
        
            if (bOK)
            {
                name = pLogName;
                WTSFreeMemory(pLogName);
            }
        }
        else
        {
            CharType buff[MAX_PATH] = { 0 };
            DWORD len = 0;
            GetUserNameA((LPSTR)buff, &len);

            name = buff;
        }

        return name;
    }

    template<>
    static std::wstring& GetLoginUseName(std::wstring& name)
    {
        typedef std::wstring::allocator_type::value_type CharType;
        if (name.empty())
        {
            CharType *pLogName = NULL;
            DWORD dwSize = 0;
            BOOL bOK = WTSQuerySessionInformationW(
                WTS_CURRENT_SERVER_HANDLE,
                WTS_CURRENT_SESSION,
                WTSUserName,
                (LPWSTR*)&pLogName,
                &dwSize);

            if (bOK)
            {
                name = pLogName;
                WTSFreeMemory(pLogName);
            }
        }
        else
        {
            CharType buff[MAX_PATH] = { 0 };
            DWORD len = 0;
            GetUserNameW((LPWSTR)buff, &len);

            name = buff;
        }

        return name;
    }
#endif


    






};


_END_MX_KIT_NAME_SPACE_