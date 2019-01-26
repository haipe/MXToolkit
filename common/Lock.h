#pragma once

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace mxtoolkit
{

#ifdef WIN32
#define OriginalMutex CRITICAL_SECTION
#else
    typedef pthread_mutex_t OriginalMutex;//互斥锁
#endif

//自解锁封装，推荐使用自解锁进行锁操作，避免异常，忘记解锁等不完全的锁操作发生
    class Mutex;
    class AutoLock
    {
    public:
        AutoLock(Mutex *pLock);
        ~AutoLock();
        void Unlock();

    private:
        Mutex * m_pMutex;
        bool m_bLocked;
    };

    //锁基本操作封装
    class Mutex
    {
    private:
        //互斥锁系统对象
        OriginalMutex m_mutex;
    public:
        Mutex();
        ~Mutex();
        //加锁
        void Lock();
        //解锁
        void Unlock();
    };

}
