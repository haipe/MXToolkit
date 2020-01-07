#pragma once

#include <thread>


namespace mxtoolkit
{
    class MXThread
    {

    public:
        MXThread()
        {

        }

        virtual ~MXThread()
        {
            StopThread();
        }

    public:

        virtual BOOL StartThread()
        {
            if (m_thread.native_handle() && m_thread.joinable())
                return true;

            m_thread = std::thread([&]() { ThreadProcEx(); });
            return m_isStop = false;
        }

        virtual void StopThread()
        {
            m_isStop = true;

            if (m_thread.native_handle() && m_thread.joinable())
                m_thread.join();
        }

        virtual uint32	ThreadProcEx() { return 0; };

    protected:
        bool        m_isStop = true;
        std::thread m_thread;
    };

}
