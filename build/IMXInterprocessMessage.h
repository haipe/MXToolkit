#pragma once


#include "mxkit.h"

namespace mxkit
{
    //----------------------------------------------------------------------------
    // IMXInterprocessMessage,
    //----------------------------------------------------------------------------
    struct IMXInterprocessMessage
    {
        //初始化
        virtual mxkit::Result Initialize() = 0;

    };
}