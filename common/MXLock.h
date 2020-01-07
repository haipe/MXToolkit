#pragma once

#include <mutex>
#include "MXCommon.h"


namespace mxtoolkit
{

    class MXAutoLock
    {
    public:
        MXAutoLock(std::recursive_mutex& mutex)
            : m_auto(mutex) {}

        std::lock_guard<std::recursive_mutex>& lock() { return m_auto; }

    private:
        std::lock_guard<std::recursive_mutex> m_auto;
    };

}