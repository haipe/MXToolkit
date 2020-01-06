#include "stdafx.h"
#include "AsynRequestManager.h"

namespace mxwebrequest
{

    AsynRequestManager::AsynRequestManager(void)
    {
        m_notifyIn.notifyMode = mxtoolkit::BaseNotify::MODE_CALLBACK;
        m_notifyIn.CallbackMode.msgID = _URL_TASK_TO_MANAGER_MSG_;
        m_notifyIn.CallbackMode.userData = (mxtoolkit::autoBit)this;
        m_notifyIn.CallbackMode.callback = (void*)TaskCallBack;
    }

    AsynRequestManager::~AsynRequestManager(void)
    {
    }

    void AsynRequestManager::SetNotify(const mxtoolkit::BaseNotify &notify)
    {
        m_notifyDefaultOut = notify;
    }

    uint32 AsynRequestManager::AddWebRequestTask(const Request &requestParam)
    {
        return AddWebRequestTask(requestParam, m_notifyDefaultOut);
    }

    uint32 AsynRequestManager::AddWebRequestTask(const Request &requestParam, const mxtoolkit::BaseNotify &notify)
    {
        if (m_isStop)
            return 0;

        RequestTask *pTask = RequestTask::CreateTask(requestParam);

        if (pTask == nullptr)
            return 0;

        AddTaskToContainer(pTask->m_nTaskID, std::make_pair(pTask, notify));

        pTask->SetNotify(m_notifyIn);
        pTask->SetMsg(&m_msgAllocator, &m_msgQueue);

        pTask->StartThread();

        return pTask->m_nTaskID;
    }

    uint32 AsynRequestManager::CallBackHandle(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2)
    {
        if (msg == _URL_TASK_TO_MANAGER_MSG_)
        {
            uint32 notifyID = LOWORD(param);
            uint32 nTaskID = HIWORD(param);

            _Value_Type taskInfo;
            if (!GetTaskInfo(nTaskID, &taskInfo) || taskInfo.first == nullptr)
                return -1;

            switch (notifyID)
            {
            case REQUEST_HEADER_RESPOND_NOTIFY:
            case REQUEST_DATA_RESPOND_NOTIFY:
            {
                if (taskInfo.second.notifyMode == mxtoolkit::BaseNotify::MODE_CALLBACK && taskInfo.second.CallbackMode.callback)
                {
                    ((WEBREQUSET_CALLBACK)taskInfo.second.CallbackMode.callback)(
                        taskInfo.second.CallbackMode.msgID,
                        MAKEWPARAM(notifyID, 0),
                        reserve1,
                        reserve2,
                        taskInfo.second.CallbackMode.userData);
                }
            }
            break;
            case REQUEST_COMPLETE_NOTIFY:
            {
                //请求结束
                if (taskInfo.second.notifyMode == mxtoolkit::BaseNotify::MODE_CALLBACK && taskInfo.second.CallbackMode.callback)
                {
                    char *pData = nullptr;
                    uint32 nSize = 0;
                    uint32 retCode = 0;
                    retCode = taskInfo.first->GetRespondData(&pData, &nSize);
                    if (retCode != 0)
                    {
                        if ((pData = (char *)taskInfo.first->GetRespondErrorString()))
                            nSize = strlen(pData);
                        else
                            nSize = 0;
                    }

                    ((WEBREQUSET_CALLBACK)taskInfo.second.CallbackMode.callback)(
                        taskInfo.second.CallbackMode.msgID,
                        MAKEWPARAM(notifyID, retCode),
                        (LPARAM)pData,
                        nSize,
                        taskInfo.second.CallbackMode.userData);
                }
            }
            break;
            default:break;
            }
        }

        return 0;
    }


    void CALLBACK AsynRequestManager::TaskCallBack(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2, mxtoolkit::autoBit userData)
    {
        if (userData == 0)
            return;

        AsynRequestManager *pManager = static_cast<AsynRequestManager *>((void *)userData);

        if (pManager)
        {
            pManager->CallBackHandle(msg, param, reserve1, reserve2);
        }
    }

}