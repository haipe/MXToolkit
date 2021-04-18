// DualDisplayCtl.cpp: implementation of the DualDisplayCtl class.
//
//////////////////////////////////////////////////////////////////////

#include "display_utils.h"
#include <tchar.h>

#include "base/string.h"

#define COMPILE_MULTIMON_STUBS

_BEGIN_MX_KIT_NAME_SPACE_

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DisplayInfo::AddMonitorInfo(const MONITORINFO& info)
{
    if (info.dwFlags == DDENUM_ATTACHEDSECONDARYDEVICES && m_vecDisplayInfo.size())
    {
        m_vecDisplayInfo.insert(m_vecDisplayInfo.begin(), info);
    }
    else
    {
        m_vecDisplayInfo.push_back(info);
    }
}

BOOL DisplayInfo::GetDualDisplayRect(size_t nIndex, MONITORINFO* info)
{
    if (nIndex < 0 || nIndex >= m_vecDisplayInfo.size())
        return FALSE;
    *info = m_vecDisplayInfo[nIndex];

    return TRUE;
}

BOOL DisplayInfo::GetDualDisplayRect(const POINT& pt, MONITORINFO* info)
{
    for (size_t i = 0; i < m_vecDisplayInfo.size(); i++)
    {
        RECT rcWork = m_vecDisplayInfo[i].rcWork;
        if (::PtInRect(&rcWork, pt))
        {
            *info = m_vecDisplayInfo[i];
            return TRUE;
        }
    }

    return FALSE;
}

void DisplayInfo::ModifyDisplayInfo(size_t nIndex, const MONITORINFO& info)
{
    if (nIndex < 0 || nIndex >= m_vecDisplayInfo.size())
        return;

    m_vecDisplayInfo[nIndex] = info;
}

void DisplayInfo::Clear()
{
    m_vecDisplayInfo.clear();
}

std::vector<POINT> DisplayUtils::m_vecDisplaySrcPos;
DisplayDevList DisplayUtils::m_lsDisplayDev;
DWORD DisplayUtils::m_dwRealDevCount = 0;
DisplayInfo DisplayUtils::m_DisplayInfo;
int  DisplayUtils::m_nOldScreenCount = 0;	//NOLINT(legal/name)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DisplayUtils::DisplayUtils()
{
}

DisplayUtils::~DisplayUtils()
{

}

BOOL WINAPI DDEnumCallbackEx(
    GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm)
{
    UNREFERENCED_PARAMETER(lpDriverDescription);

    /*DDRAW_MATCH* pDDMatch = ( DDRAW_MATCH* ) lpContext;
    if( pDDMatch->hMonitor == hm )
    {
    pDDMatch->bFound = true;
    strcpy_s( pDDMatch->strDriverName, 512, lpDriverName );
    memcpy( &pDDMatch->guid, lpGUID, sizeof( GUID ) );
    }*/
    MONITORINFO info;
    info.cbSize = sizeof(info);
    if (GetMonitorInfo(hm, &info))
    {
        DisplayInfo* pDisplayInfo = (DisplayInfo*)lpContext;
        if (pDisplayInfo)
        {
            pDisplayInfo->AddMonitorInfo(info);
        }
    }
    return TRUE;
}

BOOL DisplayUtils::Init()
{
    EnumDisplayDevice();

    EnumDisplay(&m_DisplayInfo);

    m_nOldScreenCount = m_DisplayInfo.GetDisplayCount();

    return TRUE;
}

void DisplayUtils::EnumDisplayDevice()
{
    m_lsDisplayDev.clear();
    DWORD iDevNum = 0;
    DISPLAY_DEVICE device;
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);

    while (EnumDisplayDevices(NULL, iDevNum, &device, 0)) {

        ++iDevNum;
        if (device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
            continue;

        DEVMODE devMode;
        memset(&devMode, 0, sizeof(devMode));
        devMode.dmSize = sizeof(devMode);
        if (EnumDisplaySettings(device.DeviceName, ENUM_CURRENT_SETTINGS, &devMode))
        {

            ++m_dwRealDevCount;
            m_lsDisplayDev.push_back(device);
        }

    }
}

void DisplayUtils::EnumDisplay(DisplayInfo* info)
{
#ifdef _WIN32_WCE
    HRESULT hr = DirectDrawEnumerateEx(DDEnumCallbackExW, this,
        DDENUM_ATTACHEDSECONDARYDEVICES |
        DDENUM_DETACHEDSECONDARYDEVICES );
#else
    LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx;
    HINSTANCE               hDDrawDLL;

    // Do a GetModuleHandle and GetProcAddress in order to get the
    // DirectDrawEnumerateEx function
    hDDrawDLL = LoadLibrary(_T("ddraw.dll"));
    if (!hDDrawDLL)
    {
        return;
    }
    HRESULT hr = E_NOTIMPL;
    pDirectDrawEnumerateEx = (LPDIRECTDRAWENUMERATEEXA)GetProcAddress(hDDrawDLL, "DirectDrawEnumerateExA");
    if (pDirectDrawEnumerateEx)
    {
        hr = pDirectDrawEnumerateEx(DDEnumCallbackEx, info,
            DDENUM_ATTACHEDSECONDARYDEVICES |
            DDENUM_DETACHEDSECONDARYDEVICES |
            DDENUM_NONDISPLAYDEVICES);
    }
#endif
    }

void DisplayUtils::Release()
{
    m_dwRealDevCount = 0;
    m_lsDisplayDev.clear();
}

BOOL DisplayUtils::IsSupportDualMode()
{
    return m_dwRealDevCount >= 2;
}

BOOL DisplayUtils::SetDualModeEx(BOOL bDualMode)
{
    if (!IsSupportDualMode())
        return FALSE;

    while (m_vecDisplaySrcPos.size())
    {
        m_vecDisplaySrcPos.erase(m_vecDisplaySrcPos.begin());
    }
    INT nScreenX = GetSystemMetrics(SM_CXSCREEN);
    INT nScreenY = GetSystemMetrics(SM_CYSCREEN);

    int   nIndex = 0;
    DWORD dwRealCount = 0;
    POINT ptDisplay;
    RECT  rcDislay;
    rcDislay.left = rcDislay.top = rcDislay.right = rcDislay.bottom = 0;
    rcDislay.right = rcDislay.left + nScreenX;
    rcDislay.bottom = rcDislay.top + nScreenY;
    ptDisplay.x = ptDisplay.y = 0;
    m_vecDisplaySrcPos.push_back(ptDisplay);

    ptDisplay.x += nScreenX;
    DisplayDevList::iterator i;
    for (i = m_lsDisplayDev.begin(); i != m_lsDisplayDev.end(); i++, nIndex++)
    {
        if ((*i).StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
            continue;
        ++dwRealCount;

        if (nIndex >= 1)
        {
            if (SetDualDisplay(bDualMode, nIndex, ptDisplay))
            {
                m_vecDisplaySrcPos.push_back(ptDisplay);

                POINT ptTemp = ptDisplay;
                ptTemp.x += 1;
                ptTemp.y += 1;
                rcDislay.left = rcDislay.top = rcDislay.right = rcDislay.bottom = 0;
                GetDualDisplayRect(ptTemp, &rcDislay);
                ptDisplay.x += (rcDislay.right - rcDislay.left);
            }
        }
    }

    return TRUE;
}

BOOL DisplayUtils::SetDualMode(BOOL bDualMode)
{
    if (!IsSupportDualMode())
        return FALSE;

    DISPLAY_DEVICE device;
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);

    POINT ptDisplay;
    int   nIndex = 0;
    DWORD dwRealCount = 0;
    DisplayDevList::iterator i;
    ptDisplay.x = ptDisplay.y = 0;
    for (i = m_lsDisplayDev.begin(); i != m_lsDisplayDev.end(); i++, nIndex++) {

        if ((*i).StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
            continue;
        ++dwRealCount;

        if (dwRealCount >= 2)
            break;

        m_vecDisplaySrcPos.push_back(ptDisplay);
    }

    //
    //获取当前设置信息
    //
    if (!EnumDisplayDevices(NULL, nIndex, &device, 0))
        return FALSE;
    if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        return FALSE;

    //
    //如果没有改变参数,则返回
    //
    if (bDualMode == (device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
        return TRUE;

    DEVMODE  mode;
    ZeroMemory(&mode, sizeof(mode));
    mode.dmSize = sizeof(mode);
    mode.dmFields = DM_POSITION;
    if (bDualMode)
    {
        mode.dmPosition.x = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    }
    else
    {
        mode.dmPosition.x = 0;
    }

    ptDisplay.x = mode.dmPosition.x;
    ptDisplay.y = 0;
    m_vecDisplaySrcPos.push_back(ptDisplay);

    LONG lResult =
        ChangeDisplaySettingsEx((LPCTSTR)device.DeviceName, &mode, 0, CDS_NORESET | CDS_UPDATEREGISTRY, NULL);
    if (lResult == DISP_CHANGE_SUCCESSFUL)
    {
        lResult = ChangeDisplaySettings(NULL, CDS_RESET);
    }

    if (lResult != DISP_CHANGE_SUCCESSFUL)
    {
        return FALSE;
    }

    //
    //获取改变后的值
    //
    if (!EnumDisplayDevices(NULL, nIndex, &device, 0))
        return FALSE;

    BOOL bExtend = device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;

    return bExtend == bDualMode;
}


BOOL DisplayUtils::SetDualDisplay(BOOL bDualMode, size_t nIndex, POINT ptDisplay)
{
    if (nIndex < 1 || nIndex >= m_lsDisplayDev.size())
        return FALSE;

    DISPLAY_DEVICE device;
    ZeroMemory(&device, sizeof(device));
    device.cb = sizeof(device);

    //
    //获取当前设置信息
    //
    if (!EnumDisplayDevices(NULL, nIndex, &device, 0))
        return FALSE;
    if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        return FALSE;

    //
    //如果没有改变参数,则返回
    //
    if (bDualMode == (device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
        return TRUE;

    DEVMODE  mode;
    ZeroMemory(&mode, sizeof(mode));
    mode.dmSize = sizeof(mode);
    mode.dmFields = DM_POSITION;

    mode.dmPosition.x = ptDisplay.x;
    mode.dmPosition.y = ptDisplay.y;

    /*if( bDualMode ){
        mode.dmPosition.x = ptDisplay.x;
        }else{
        mode.dmPosition.x = 0;
        }*/

    LONG lResult =
        ChangeDisplaySettingsEx((LPCTSTR)device.DeviceName, &mode, 0, CDS_NORESET | CDS_UPDATEREGISTRY, NULL);
    if (lResult == DISP_CHANGE_SUCCESSFUL)
    {
        lResult = ChangeDisplaySettings(NULL, CDS_RESET);
    }

    if (lResult != DISP_CHANGE_SUCCESSFUL)
    {
        return FALSE;
    }

    //
    //获取改变后的值
    //
    if (!EnumDisplayDevices(NULL, nIndex, &device, 0))
        return FALSE;

    BOOL bExtend = device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;

    return bExtend == bDualMode;
}

BOOL DisplayUtils::GetDualDisplayRect(LPRECT lpRect)
{
    if (NULL == lpRect)
        return FALSE;

    POINT pt;
    INT nScreenX = GetSystemMetrics(SM_CXSCREEN);
    INT nScreenY = GetSystemMetrics(SM_CYSCREEN);
    pt.x = nScreenX + 1;
    pt.y = 1;

    HMONITOR hMotitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
    if (NULL == hMotitor)
        return FALSE;

    MONITORINFO info;
    info.cbSize = sizeof(info);
    if (!GetMonitorInfo(hMotitor, &info))
        return FALSE;

    *lpRect = info.rcMonitor;
    return TRUE;
}

BOOL DisplayUtils::GetDualDisplayRect(POINT ptPos, LPRECT lpRect)
{
    if (NULL == lpRect)
        return FALSE;

    HMONITOR hMotitor = MonitorFromPoint(ptPos, MONITOR_DEFAULTTONULL);
    if (NULL == hMotitor)
    {
        //LOG_DESKTOP(_T("MonitorFromPoint error"));
        return FALSE;
    }
    MONITORINFO info;
    info.cbSize = sizeof(info);
    if (!GetMonitorInfo(hMotitor, &info))
    {
        return FALSE;
    }

    *lpRect = info.rcMonitor;

    return TRUE;
}

BOOL DisplayUtils::GetDualDisplayRect(size_t nIndex, LPRECT lpRect)
{
    if (nIndex <= 0 || nIndex < m_vecDisplaySrcPos.size())
    {
        POINT ptDisplay = m_vecDisplaySrcPos[nIndex];
        ptDisplay.x += 1;
        ptDisplay.y += 1;

        return GetDualDisplayRect(ptDisplay, lpRect);
    }

    return FALSE;
}

BOOL DisplayUtils::GetDualDisplayRect(HWND hWnd, LPRECT lpRect)
{
    if (NULL == lpRect)
        return FALSE;

    HMONITOR hMotitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
    if (NULL == hMotitor)
        return FALSE;
    MONITORINFO info;
    info.cbSize = sizeof(info);
    if (!GetMonitorInfo(hMotitor, &info))
        return FALSE;

    *lpRect = info.rcMonitor;
    return TRUE;
}

void DisplayUtils::OnDeviceChange()
{
    DisplayInfo DisplayInfo;

    EnumDisplayDevice();

    EnumDisplay(&DisplayInfo);

    MONITORINFO info;
    DisplayInfo.GetDualDisplayRect(0, &info);
    DisplayUtils::ModifyDisplayInfo(0, info);

    m_nOldScreenCount = DisplayInfo.GetDisplayCount();
}

void DisplayUtils::ModifyDisplayInfo(int nIndex, const MONITORINFO& info)
{
    m_DisplayInfo.ModifyDisplayInfo(nIndex, info);
}

void DisplayUtils::EmptyDisplayInfo(int nIndex)
{
    MONITORINFO info;
    m_DisplayInfo.GetDualDisplayRect(nIndex, &info);

    SetRect(&info.rcWork, 0, 0, 0, 0);
    m_DisplayInfo.ModifyDisplayInfo(nIndex, info);
}

BOOL DisplayUtils::IsPrimaryMonitor(HMONITOR hMonitor)
{
    if (hMonitor == NULL)
        return FALSE;
    MONITORINFO info;
    m_DisplayInfo.GetDualDisplayRect(0, &info);

    if (MonitorFromRect(&info.rcWork, MONITOR_DEFAULTTONULL) == hMonitor)
        return TRUE;

    return FALSE;
}

BOOL DisplayUtils::IsRepeatMonitor(HMONITOR hMonitor, int nIndex)
{
    int nCount = m_DisplayInfo.GetDisplayCount();
    if (nIndex >= nCount || nIndex <= 0)
        return FALSE;

    if (hMonitor == NULL)
        return FALSE;

    for (int i = 0; i < nIndex; ++i)
    {
        MONITORINFO info;
        m_DisplayInfo.GetDualDisplayRect(i, &info);

        if (::IsRectEmpty(&info.rcWork))
            continue;

        if (MonitorFromRect(&info.rcWork, MONITOR_DEFAULTTONULL) == hMonitor)
            return TRUE;
    }
    return FALSE;
}

bool DisplayUtils::GetPrimaryMonitorID(std::string* sMonitorID)
{
#ifdef UNICODE
    return false;
#else
    int nSize = m_lsDisplayDev.size();

    for (int i = 0; i < nSize; ++i)
    {
        if (m_lsDisplayDev[i].StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        {
            *sMonitorID = m_lsDisplayDev[i].DeviceKey;
            return true;
        }
    }

    return true;
#endif
}

bool DisplayUtils::GetPrimaryMonitorID(std::wstring* sMonitorID)
{
#ifdef UNICODE
    int nSize = m_lsDisplayDev.size();

    for (int i = 0; i < nSize; ++i)
    {
        if (m_lsDisplayDev[i].StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        {
            *sMonitorID = m_lsDisplayDev[i].DeviceKey;
            return true;
        }
    }

    return true;
#else
    return false;
#endif
}

int DisplayUtils::GetOldScreenCount()
{
    return m_nOldScreenCount;
}

bool DisplayUtils::IsPrimaryMonitorChanged()
{
    TString sOldPrimaryID = _T(""), sNewPrimaryID = _T("");
    DisplayUtils::GetPrimaryMonitorID(&sOldPrimaryID);
    DisplayUtils::EnumDisplayDevice();
    DisplayUtils::GetPrimaryMonitorID(&sNewPrimaryID);

    bool bChanged = _tcsicmp(sNewPrimaryID.c_str(), sOldPrimaryID.c_str()) != 0;

    if (bChanged)
    {
        m_DisplayInfo.Clear();

        EnumDisplay(&m_DisplayInfo);
    }

    return bChanged;
}

_END_MX_KIT_NAME_SPACE_