#pragma once

#include <memory>
#include "mxkit.h"


_BEGIN_MX_KIT_NAME_SPACE_

class WMIUtils
{
public:
    WMIUtils();
    ~WMIUtils();

    Result Run(const char* wql);

    struct IniComtUtils
    {
        IniComtUtils();
        ~IniComtUtils();

        HRESULT hres;
    };

private:
    std::shared_ptr<IniComtUtils> m_initUtils;
};


_END_MX_KIT_NAME_SPACE_