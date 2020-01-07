#pragma once

#include <condition_variable>
#include <mutex>
#include "MXCommon.h"

namespace mxtoolkit
{
    class MXSemaphore
    {
    public:
        MXSemaphore(uint32 count = 0) : m_count(count) {}

        MXSemaphore(const MXSemaphore&) = delete;

        MXSemaphore& operator=(const MXSemaphore&) = delete;

        void Signal()
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                ++m_count;
            }
            m_cv_uptr.notify_one();
        }

        bool Wait(uint32 waitMsc)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            bool bTimeout = false;
            if (m_count == 0) 
            {
                // we may have spurious wakeup!
                //m_cv_uptr.wait(lock);
                bTimeout = m_cv_uptr.wait_for(lock, std::chrono::milliseconds(waitMsc)) == std::cv_status::timeout;
            }
            --m_count;

            return bTimeout;
        }

    private:
        std::mutex m_mutex;
        std::condition_variable m_cv_uptr;
        int32 m_count;
    };

}
