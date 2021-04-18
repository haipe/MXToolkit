#pragma once

#include <mutex>

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_


class AutoLocker
{
public:
    AutoLocker(std::recursive_mutex& mutex)
        : m_auto(mutex) {}

    std::lock_guard<std::recursive_mutex>& lock() { return m_auto; }

private:
    std::lock_guard<std::recursive_mutex> m_auto;
};

_END_MX_KIT_NAME_SPACE_