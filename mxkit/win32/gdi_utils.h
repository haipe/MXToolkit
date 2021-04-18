#pragma once
#include <string>

#include "mxkit.h"
#include "base/string.h"

_BEGIN_MX_KIT_NAME_SPACE_

template<typename Str>
class GdiUtils
{
public:
    typedef typename Str::allocator_type::value_type CharType;
    static bool SaveDC(HDC hDc, int nWidth, int nHeight, const Str& filePath)
    {
        BOOL    bReturn = TRUE;
        HANDLE  hFile = NULL;       //handle of bitmap file which will be saved  
        DWORD   dwWritten = 0;      //written count  
        DWORD   dwOffsetSize = 0;
        DWORD   dwBufferSize = 0;
        WORD    wBitCount = 32;     //位图中每个像素所占字节数    
        HBITMAP hbitmapSave = NULL;
        HBITMAP hbitmapOld = NULL;
        HDC     hDcDev = NULL;
        HDC     hDcMem = NULL;
        BYTE* pBmpBuffer = NULL;
        int     iBits = 0;
        BITMAP              stSrcbmp;
        BITMAPINFO          srcdibbmap;
        BITMAPFILEHEADER    bmFileHeader; //位图文件头结构  
        SecureZeroMemory(&stSrcbmp, sizeof(BITMAP));
        SecureZeroMemory(&srcdibbmap, sizeof(BITMAPINFO));
        SecureZeroMemory(&bmFileHeader, sizeof(BITMAPFILEHEADER));
        // Fill bitmap information constructor  
        srcdibbmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        srcdibbmap.bmiHeader.biWidth = nWidth;
        srcdibbmap.bmiHeader.biHeight = nHeight;
        srcdibbmap.bmiHeader.biPlanes = 1;
        srcdibbmap.bmiHeader.biBitCount = wBitCount;
        srcdibbmap.bmiHeader.biCompression = BI_RGB;
        hDcMem = CreateCompatibleDC(NULL);
        do
        {
            if (NULL == hDcMem)
            {
                bReturn = FALSE;
                break;
            }
            hbitmapSave = CreateDIBSection(hDcMem, &srcdibbmap, DIB_RGB_COLORS, (void**)&pBmpBuffer, NULL, 0);
            if (NULL == hbitmapSave)
            {
                bReturn = FALSE;
                break;
            }
            hbitmapOld = (HBITMAP)SelectObject(hDcMem, hbitmapSave);
            if (NULL == hbitmapOld)
            {
                bReturn = FALSE;
                break;
            }
            //将传进来的DC画到定义的内存DC上去  
            if (!StretchBlt(hDcMem,
                0,
                0,
                nWidth,
                nHeight,
                hDc,
                0,
                0,
                nWidth,
                nHeight,
                SRCCOPY))
            {
                bReturn = FALSE;
                break;
            }

            if (0 == GetObject(hbitmapSave, sizeof(stSrcbmp), &stSrcbmp))
            {
                bReturn = FALSE;
                break;
            }
            dwBufferSize = stSrcbmp.bmWidth * stSrcbmp.bmHeight * wBitCount / 8;
            dwOffsetSize = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
            // Fill bitmap header constructor  
            bmFileHeader.bfType = 0x4D42;
            bmFileHeader.bfSize = dwOffsetSize + dwBufferSize;
            bmFileHeader.bfReserved1 = 0;
            bmFileHeader.bfReserved2 = 0;
            bmFileHeader.bfOffBits = dwOffsetSize;

            if (!std::is_same<CharType, char>::value)
                hFile = CreateFileA((LPCSTR)filePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            else
                hFile = CreateFileW((LPCWSTR)filePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (INVALID_HANDLE_VALUE == hFile)
            {
                bReturn = FALSE;
                break;
            }
            if (!WriteFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL))
            {
                bReturn = FALSE;
                break;
            }
            if (!WriteFile(hFile, &srcdibbmap, sizeof(BITMAPINFO), &dwWritten, NULL))
            {
                bReturn = FALSE;
                break;
            }
            if (!WriteFile(hFile, pBmpBuffer, dwBufferSize, &dwWritten, NULL))
            {
                bReturn = FALSE;
                break;
            }
            bReturn = TRUE;
        } while (0);

        if (INVALID_HANDLE_VALUE != hFile)
        {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
        }

        return bReturn;
    }

};


_END_MX_KIT_NAME_SPACE_