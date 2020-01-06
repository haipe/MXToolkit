#pragma once

#include "MXCommon.h"

namespace mxtoolkit
{
    template<typename T>
    class MXSingleObject
    {
    public:
        static T* GetInstance()
        {
            if (m_obj == nullptr)
                m_obj = new T;

            return m_obj;
        }

        static void DestroyInstance()
        {
            if (!m_obj)
                return;

            typedef MXSingleObject<T>* PtrType;
            auto obj = dynamic_cast<PtrType>(m_obj);
            if (!obj)
                return;

            obj->OnObjectDestroy();
            if (m_obj != nullptr)
            {
                delete m_obj;
                m_obj = nullptr;
            }
        }

    protected:
        virtual void OnObjectDestroy() {	}

        MXSingleObject()
        {
            static ReleaseUtil util;
        }

        virtual ~MXSingleObject() {	}

    private:

        struct ReleaseUtil
        {
            ~ReleaseUtil()
            {
                MXSingleObject<T>::DestroyInstance();
            }
        };
        static T*	m_obj;
    };

    template<typename T>
    T* MXSingleObject<T>::m_obj = nullptr;

}