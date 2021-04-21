#pragma once

#include "mxkit.h"

#if _MX_CPP_VER_ >= 11
#define _MX_USE_STD_THREAD
#include <thread>
#else
#include <process.h>

#endif

_BEGIN_MX_KIT_NAME_SPACE_


class Thread
{

public:
    Thread():m_isStop(true)
    {
#ifndef _MX_USE_STD_THREAD
        m_thread = 0;
#endif
    }

    virtual ~Thread()
    {
        Stop();
#ifndef _MX_USE_STD_THREAD
        m_thread = 0;
#endif
    }

public:

    virtual bool Start()
    {
        if (!m_isStop)
            return true;

#ifdef _MX_USE_STD_THREAD
        if (m_thread.native_handle() && m_thread.joinable())
            return true;

        m_thread = std::thread([&]() { Process(); });
        m_isStop = false;
#else
        if (m_thread)
            return true;

        m_thread = (HANDLE)_beginthreadex(NULL, 0, Thread::StatisThreadFunction, this, 0, (unsigned int*)&m_thread);
        m_isStop = false;
#endif
        return true;
    }

    virtual void Stop(bool bWait = true)
    {
        m_isStop = true;

        if (bWait)
            Wait();
    }

    virtual void Wait()
    {
#ifdef _MX_USE_STD_THREAD
        if (m_thread.native_handle() && m_thread.joinable())
            m_thread.join();
#else
        if (m_thread)
        {
            WaitForSingleObject(m_thread, -1);
            m_thread = 0;
        }
#endif
    }

    virtual uint32	Process() { return 0; };

protected:
#ifndef _MX_USE_STD_THREAD
    static unsigned __stdcall StatisThreadFunction(void* wrapper)
    {
        Thread* p = (Thread*)wrapper;
        if (p)
            p->Process();

        _endthread();

        return 0;
    }

#endif
protected:
    bool        m_isStop;

#ifdef _MX_USE_STD_THREAD
    std::thread m_thread;
#else
    HANDLE m_thread;
#endif
};





_END_MX_KIT_NAME_SPACE_