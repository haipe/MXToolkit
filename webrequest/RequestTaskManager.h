#pragma once

#include <map>

#include "MXBaseNotify.h"
#include "MXThread.h"
#include "MXLock.h"

#include "RequestTask.h"


namespace mxwebrequest
{

    template<class _TaskKey, class _TaskInfo>
    class RequestTaskManager : public mxtoolkit::MXThread
    {
    public:

        typedef typename std::map<_TaskKey, _TaskInfo> _TaskMap;

        typedef typename std::map<_TaskKey, _TaskInfo>::key_type     _Key_Type;
        typedef typename std::map<_TaskKey, _TaskInfo>::mapped_type  _Value_Type;

        RequestTaskManager(void)
        {
            StartThread();
        };

        virtual ~RequestTaskManager(void)
        {
            m_msgQueue.PushMsg(nullptr);

            StopThread();

            if (!m_tasks.empty())
            {
                typename _TaskMap::iterator it = m_tasks.begin();

                while (it != m_tasks.end())
                {
                    OnReleaseTask(it->second);

                    it++;
                }

                m_tasks.clear();
            }
        };
        
    public:
        virtual void StopThread()
        {
            m_msgQueue.PushMsg(nullptr);

            mxtoolkit::MXThread::StopThread();
        }
    protected:

        bool AddTaskToContainer(const _TaskKey &key, const _Value_Type &value)
        {
            mxtoolkit::MXAutoLock autoLock(m_mutex);

            auto ret = m_tasks.insert(make_pair(key, value));

            return ret.second;
        };

        void RemoveTaskFromContainer(const _TaskKey &key)
        {
            mxtoolkit::MXAutoLock autoLock(m_mutex);

            typename _TaskMap::iterator it = m_tasks.find(key);

            if (it != m_tasks.end())
                m_tasks.erase(it);
        };

        bool GetTaskInfo(const _TaskKey &key, _Value_Type *taskInfo)
        {
            mxtoolkit::MXAutoLock autoLock(m_mutex);

            typename _TaskMap::iterator it = m_tasks.find(key);

            if (it != m_tasks.end())
            {
                *taskInfo = it->second;

                return true;
            }
            return false;
        };

        virtual void OnReleaseTask(const _Value_Type &taskInfo)
        {
            RequestTask *pTask = taskInfo.first;

            if (pTask)
            {
                pTask->StopThread();

                RequestTask::ReleaseTask(pTask);
            }
        };
        virtual void OnTaskComplete(const _Value_Type &taskInfo)
        {
            RequestTask *pTask = taskInfo.first;

            if (pTask)
            {
                pTask->StopThread();

                RequestTask::ReleaseTask(pTask);
            }
        };

    protected:
        uint32 ThreadProcEx()
        {
            mxtoolkit::uint64 cnt = 0;
            while (!m_isStop)
            {
                cnt++;
                mxtoolkit::BaseMsg *pMsg = m_msgQueue.PopMsg(-1);
                if (pMsg)
                {
                    if (pMsg->message == REQUEST_COMPLETE_NOTIFY)
                    {
                        _TaskKey taskKey = pMsg->param;

                        _Value_Type info;
                        if (GetTaskInfo(taskKey, &info))
                        {
                            OnTaskComplete(info);

                            RemoveTaskFromContainer(taskKey);
                        }
                    }

                    m_msgAllocator.Free(pMsg);
                }
            }

            return 0;
        };

    protected:
        mxtoolkit::MXElementAllocator<mxtoolkit::BaseMsg> m_msgAllocator;
        mxtoolkit::MXMsgQueue<mxtoolkit::BaseMsg>         m_msgQueue;

    private:
        std::recursive_mutex m_mutex;
        _TaskMap m_tasks;
    };

}