#pragma once

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_

template<typename T>
class SingletonObject
{
public:
    static T* Instance()
    {
        if (m_obj == nullptr)
            m_obj = new T;

        return m_obj;
    }

    static void DestroyInstance()
    {
        if (!m_obj)
            return;

        typedef SingletonObject<T>* PtrType;
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

    SingletonObject()
    {
        static ReleaseUtil util;
    }

    virtual ~SingletonObject() {	}

private:

    struct ReleaseUtil
    {
        ~ReleaseUtil()
        {
            SingletonObject<T>::DestroyInstance();
        }
    };
    static T* m_obj;
};

template<typename T>
T* SingletonObject<T>::m_obj = nullptr;

_END_MX_KIT_NAME_SPACE_