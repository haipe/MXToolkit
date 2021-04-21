#pragma once

#include <functional>

#include "mxkit.h"
#include "base/singleton_object.h"

_BEGIN_MX_KIT_NAME_SPACE_


class UIThreadUtils : public SingletonObject<UIThreadUtils>
{
public:
    static bool Sync(std::function<void()> f)
    {
        return UIThreadUtils::Instance()->Run(true, f);
    }

    static bool Async(std::function<void()> f)
    {
        return UIThreadUtils::Instance()->Run(false, f);
    }

    UIThreadUtils();
    ~UIThreadUtils();

private:
    bool Run(bool wait, std::function<void()> f);

};

_END_MX_KIT_NAME_SPACE_