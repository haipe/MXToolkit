#pragma once

#include <string>
#include<windows.h>
#include<Commdlg.h>
#include<Shlobj.h>

#pragma comment(lib,"Shell32.lib")

#include "mxkit.h"

#include "base/string_utils.h"
#include "base/string_convert.h"


_BEGIN_MX_KIT_NAME_SPACE_

template<
typename StrType = std::wstring,
typename CharType = StrType::allocator_type::value_type
>
class FileSelector
{
public:
    static StrType file(const wchar_t* filter, HWND parent, bool save = false)
    {
        StrType ret;
        wchar_t szBuffer[MAX_PATH] = { 0 };//用于接收文件名

        OPENFILENAMEW ofn = { 0 };
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.hwndOwner = parent;                                                         //拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
        ofn.lpstrFilter = filter;                                                       // L"所有文件\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0";          //设置过滤
        ofn.nFilterIndex = 1;                                                           //过滤器索引
        ofn.lpstrFile = szBuffer;                                                       //接收返回的文件名，注意第一个字符需要为NULL
        ofn.nMaxFile = sizeof(szBuffer);                                                //缓冲区长度
        ofn.lpstrInitialDir = L"c:\\";                                                  //初始目录为默认

        BOOL openRet = 0;
        if (save)
        {
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;     //目录必须存在，覆盖文件前发出警告
            ofn.lpstrTitle = TEXT("File Save");                                         //使用系统默认标题留空即可
            //ofn.lpstrDefExt = TEXT("cpp");                                            //默认追加的扩
            openRet = GetSaveFileName(&ofn);
        }
        else
        {
            ofn.lpstrTitle = TEXT("File Selector");                                     //使用系统默认标题留空即可
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;       //文件、目录必须存在，隐藏只读选项
            openRet = GetOpenFileName(&ofn);
        }

        std::cout << "openRet :" << openRet << std::endl;
        if (openRet)
        {
            if (std::is_same<CharType, wchar_t>::value && std::is_same<StrType, std::wstring>::value)
                ret = StrType((CharType*)szBuffer);
            else
                Win32StringConvert::UnicodeToAnsii((const WCHAR*)szBuffer, (AString&)ret);
        }

        return ret;
    }

    static StrType folder(HWND parent)
    {
        StrType ret;

        wchar_t szBuffer[MAX_PATH] = { 0 };
        BROWSEINFOW bi = { 0 };
        bi.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
        bi.pszDisplayName = szBuffer;//接收文件夹的缓冲区
        bi.lpszTitle = L"Folder Selector";//标题
        bi.ulFlags = BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST idl = SHBrowseForFolderW(&bi);
        if (SHGetPathFromIDListW(idl, szBuffer))
        {
            if (std::is_same<CharType, wchar_t>::value && std::is_same<StrType, std::wstring>::value)
                ret = StrType((CharType*)szBuffer);
            else
                Win32StringConvert::UnicodeToAnsii((const WCHAR*)szBuffer, (AString&)ret);
        }

        return ret;
    }
};

_END_MX_KIT_NAME_SPACE_