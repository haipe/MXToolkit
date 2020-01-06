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

        BOOL StartThread()
        {
            if (m_thread.native_handle() && m_thread.joinable())
                return true;

            m_thread = std::thread([&]() { ThreadProcEx(); });
            return m_isStop = false;
        }

        void StopThread()
        {
            if (m_thread.native_handle() && m_thread.joinable())
                m_thread.join();

            m_isStop = true;
        }

        virtual uint32	ThreadProcEx() { return 0; };

    protected:
        bool        m_isStop = true;
        std::thread m_thread;
    };

}
