#pragma once

#include "MXCommon.h"

namespace mxtoolkit
{
    struct MXParam
    {
        virtual ~MXParam() {}

        virtual unsigned int SizeOf() = 0;
        virtual MXParam* DeepClone() = 0;  //克隆一个
    };

    template<class ...TypeArgs>
    struct MXVarParam : public MXParam
    {
        MXVarParam(TypeArgs... args)
        {
            if (sizeof...(TypeArgs) > 0)
                data = std::make_tuple(args...);
        }

    public:
        virtual unsigned int SizeOf() { return sizeof(MXVarParam<TypeArgs...>); }

        virtual MXParam* DeepClone()
        {
            auto newData = new MXVarParam<TypeArgs...>();
            if (!newData)
                return false;

            newData->data = this->data;
            return newData;
        }

    public:
        template<typename T, int index = 0>
        T& GetData() { return std::get<index>(data); }

    private:
        MXVarParam() {}

        std::tuple<TypeArgs...> data;
    };    
    
    template<class ...TypeArgs>
    MXVarParam<TypeArgs...>* MakeMXVarParamFrom(MXParam* data)
    {
        return dynamic_cast<MXVarParam<TypeArgs...>*>(data);
    }

	};

}