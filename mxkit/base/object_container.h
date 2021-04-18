#pragma once

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_

template<typename ObjType>
class ObjectContainer
{
public:
    typedef ObjType* ObjectPoint;
    ObjectContainer(ObjectPoint obj = nullptr) :obj_ptr(obj) {}

    void SetObject(ObjectPoint obj)
    {
        obj_ptr = obj;
    }

    inline ObjectPoint Object()
    {
        return obj_ptr;
    }

protected:
    ObjectPoint	obj_ptr;
};

_END_MX_KIT_NAME_SPACE_