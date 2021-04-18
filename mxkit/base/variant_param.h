#pragma once

#include "mxkit.h"
_BEGIN_MX_KIT_NAME_SPACE_

struct VariantParam
{
    virtual ~VariantParam() {}

    virtual unsigned int SizeOf() = 0;
    virtual VariantParam* DeepClone() = 0;  //克隆一个
};

template<class ...TypeArgs>
struct VariantParamImpl : public VariantParam
{
    VariantParamImpl(TypeArgs... args)
    {
        if (sizeof...(TypeArgs) > 0)
            data = std::make_tuple(args...);
    }

public:
    virtual unsigned int SizeOf() { return sizeof(VariantParamImpl<TypeArgs...>); }

    virtual VariantParam* DeepClone()
    {
        auto newData = new VariantParamImpl<TypeArgs...>();
        if (!newData)
            return false;

        newData->data = this->data;
        return newData;
    }

public:
    template<typename T, int index = 0>
    T& GetData() { return std::get<index>(data); }

private:
    VariantParamImpl() {}

    std::tuple<TypeArgs...> data;
};

template<class ...TypeArgs>
VariantParamImpl<TypeArgs...>* QueryVariantParam(VariantParam* data)
{
    return dynamic_cast<VariantParamImpl<TypeArgs...>*>(data);
}



_END_MX_KIT_NAME_SPACE_