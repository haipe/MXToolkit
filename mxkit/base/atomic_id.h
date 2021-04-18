#pragma once

#include <atomic>
#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_

template <uint T_BaseID>
class AtomicID
{
public:
    static unsigned int MakeID()
    {
        if(s_base_id < T_BaseID)
            return s_base_id = T_BaseID;

        return ++s_base_id;
    }

protected:
    static std::atomic<uint> s_base_id;
};

template <unsigned int T_BaseID>
std::atomic<unsigned int> AtomicID<T_BaseID>::s_base_id = {0};



_END_MX_KIT_NAME_SPACE_