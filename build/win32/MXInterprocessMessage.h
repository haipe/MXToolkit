#pragma once
#include "base/singleton_object.h"
#include "mxkit_library.h"
#include "IMXInterprocessMessage.h"

namespace mxkit
{

    class MXInterprocessMessage 
        : public IMXInterprocessMessage
        , public InterfaceImp<MXInterprocessMessage>
        , public SingletonObject<MXInterprocessMessage>
    {
    public:
        //初始化
        virtual mxkit::Result Initialize() override { return {true}; }

    };

}
