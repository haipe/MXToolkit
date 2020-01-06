#pragma once

#include "MXCommon.h"


namespace mxtoolkit
{
    struct BaseMsg
    {
        uint32  message;
        uint32  param;
        autoBit	userData;
    };

    typedef uint32(CALLBACK* MXBASECALLBACK)(uint32 msg, uint32 param, autoBit reserve, autoBit userData);

    //消息通知方式定义
    struct BaseNotify
    {
        enum Mode
        {
            MODE_NONE       = 0x0000,
            MODE_EVENT      = 0x0001,	//事件方式
            MODE_MESSAGE    = 0x0002,	//窗口消息方式
            MODE_INTERFACE  = 0x0003,	//接口指针方式
            MODE_CALLBACK   = 0x0004,	//回调方式
        };
        Mode notifyMode;
        union
        {
            struct
            {
                HANDLE  event;
            }EventMode;
            struct
            {
                HWND    msgWnd;
                uint32  notifyMsg;
            }MsgMode;
            struct
            {
                void*   callInterface;
            }InterfaceMode;
            struct
            {
                void*   callback;
                UINT    msgID;
                DWORD   userData;
            }CallbackMode;
        };
    };

}
