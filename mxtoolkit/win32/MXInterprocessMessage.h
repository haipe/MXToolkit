#pragma once
#include "MXSingleObject.h"
#include "MXDllExportDefine.h"
#include "IMXInterprocessMessage.h"

namespace mxtoolkit
{

    class MXInterprocessMessage 
        : public IMXInterprocessMessage
        , public MXInterfaceImp<MXInterprocessMessage>
        , public MXSingleObject<MXInterprocessMessage>
    {
    public:
        //初始化
        virtual mxtoolkit::Result Initialize() override { return {true}; }

    };

}
