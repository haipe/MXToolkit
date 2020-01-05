#pragma once

#include <atomic>

namespace mxtoolkit
{

template <unsigned int BaseID>
class MXBaseID
{
public:
    static unsigned int MakeID()
    {
        if(baseID_ < BaseID)
            return baseID_ = BaseID;

        return ++baseID_;
    }

protected:
    static std::atomic<unsigned int> baseID_;
};

template <unsigned int BaseID>
std::atomic<unsigned int> MXBaseID<BaseID>::baseID_ = {0};

}
