#include "stdafx.h"
#include "AsynRequestExManager.h"
#include "MXCommon.h"

namespace mxwebrequest
{

    AsynRequestExManager::AsynRequestExManager(void)
    {
        m_notifyDefaultOut = nullptr;

        m_notifyIn.notifyMode = mxtoolkit::BaseNotify::MODE_CALLBACK;
        m_notifyIn.CallbackMode.msgID = _URL_TASK_TO_MANAGER_MSG_;
        m_notifyIn.CallbackMode.userData = (mxtoolkit::autoBit)this;
        m_notifyIn.CallbackMode.callback = (void*)TaskCallBack;
    }

    AsynRequestExManager::~AsynRequestExManager(void)
    {
    }

    void AsynRequestExManager::SetNotify(IRespondNotify *pNotify)
    {
        m_notifyDefaultOut = pNotify;
    }

    uint32 AsynRequestExManager::AddWebRequestTask(const Request &requestParam)
    {
        return AddWebRequestTask(requestParam, m_notifyDefaultOut);
    }

    uint32 AsynRequestExManager::AddWebRequestTask(const Request &requestParam, IRespondNotify *pNotify)
    {
        if (m_isStop)
            return 0;

        if (pNotify == nullptr)
            return 0;

        RequestTask *pTask = RequestTask::CreateTask(requestParam);

        if (pTask == nullptr)
            return 0;

        AddTaskToContainer(pTask->m_nTaskID, std::make_pair(pTask, pNotify));

        pTask->SetNotify(m_notifyIn);
        pTask->SetMsg(&m_msgAllocator, &m_msgQueue);

        pTask->StartThread();

        return pTask->m_nTaskID;
    }

    uint32 AsynRequestExManager::CallBackHandle(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2)
    {
        if (msg == _URL_TASK_TO_MANAGER_MSG_)
        {
            uint32 notifyID = LOWORD(param);
            uint32 nTaskID = HIWORD(param);

            _Value_Type taskInfo;
            if (!GetTaskInfo(nTaskID, &taskInfo) || taskInfo.first == nullptr || taskInfo.second == nullptr)
                return -1;

            switch (notifyID)
            {
            case REQUEST_HEADER_RESPOND_NOTIFY:
            {
                taskInfo.second->OnHeaderRespond(nTaskID, (char *)reserve1, reserve2);
            }
            break;
            case REQUEST_DATA_RESPOND_NOTIFY:
            {
                taskInfo.second->OnDataRespond(nTaskID, (char *)reserve1, reserve2);
            }
            break;
            case REQUEST_COMPLETE_NOTIFY:
            {
                //请求结束
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

                taskInfo.second->OnCompleteRespond(nTaskID, retCode, pData, nSize);
            }
            break;
            default:break;
            }
        }

        return 0;
    }

    void CALLBACK AsynRequestExManager::TaskCallBack(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2, mxtoolkit::autoBit userData)
    {
        if (userData == 0)
            return;

        AsynRequestExManager *pManager = static_cast<AsynRequestExManager *>((void *)userData);

        if (pManager)
        {
            pManager->CallBackHandle(msg, param, reserve1, reserve2);
        }
    }

}