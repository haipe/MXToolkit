#pragma once

#include "mxkit.h"
//#include "MXCommon.h"


_BEGIN_MX_KIT_NAME_SPACE_

struct Message
{
    uint32  message;
    uint    param;
    uint	reserve;
};

typedef ReturnType(CALLBACK* _MX_MESSAGE_CALLBACK_)(uint32 msg, uint param, uint reserve);

//消息通知方式定义
struct Notify
{
    enum Mode
    {
        NONE_MODE = 0x0000,
        EVENT_MODE = 0x0001,	//事件方式
        MESSAGE_MODE = 0x0002,	//窗口消息方式
        INTERFACE_MODE = 0x0003,	//接口指针方式
        CALLBACK_MODE = 0x0004,	//回调方式
    };

    Mode notifyMode;
    union
    {
#ifdef _MX_WIN_
        struct
        {
            Handle  event;
        }EventMode;
        struct
        {
            Hwnd    msgWnd;
            uint32  notifyMsg;
        }MsgMode;
#endif
        struct
        {
            VoidPoint callInterface;//
        }InterfaceMode;
        struct
        {
            VoidPoint   callback;//_MX_MESSAGE_CALLBACK_
            uint32      msgID;
            uint        userData;
        }CallbackMode;
    };
};

_END_MX_KIT_NAME_SPACE_