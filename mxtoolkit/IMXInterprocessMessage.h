#pragma once


#include "MXCommon.h"

namespace mxtoolkit
{
    //----------------------------------------------------------------------------
    // IMXInterprocessMessage,
    //----------------------------------------------------------------------------
    struct IMXInterprocessMessage
    {
        //初始化
        virtual mxtoolkit::Result Initialize() = 0;

    };
}