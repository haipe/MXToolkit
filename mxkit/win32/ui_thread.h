#pragma once

#include <functional>

#include "mxkit.h"
#include "base/singleton_object.h"

_BEGIN_MX_KIT_NAME_SPACE_

class UIThreadUtils : public SingletonObject<UIThreadUtils>
{
public:
    static bool Run(std::function<void()> f)
    {
        return UIThreadUtils::Instance()->Do(true, f);
    }

    static bool Async(std::function<void()> f)
    {
        return UIThreadUtils::Instance()->Do(false, f);
    }

    UIThreadUtils();
    ~UIThreadUtils();

private:
    bool Do(bool wait, std::function<void()> f);

private:
    unsigned int m_wnd;
};

_END_MX_KIT_NAME_SPACE_