#include "stdafx.h"
#include "SynRequestManager.h"
#include "AsynRequestManager.h"

namespace mxwebrequest
{

    SynRequestManager::SynRequestManager(void)
    {
    }

    SynRequestManager::~SynRequestManager(void)
    {
    }

    uint32 SynRequestManager::AddWebRequestTask(const Request &requestParam, Respond *pRespond)
    {
        RequestTask *pTask = RequestTask::CreateTask(requestParam);

        mxtoolkit::BaseNotify taskNotify;
        taskNotify.notifyMode = mxtoolkit::BaseNotify::MODE_CALLBACK;
        taskNotify.CallbackMode.msgID = _URL_TASK_TO_MANAGER_MSG_;
        taskNotify.CallbackMode.userData = (mxtoolkit::autoBit)this;
        taskNotify.CallbackMode.callback = (void*)TaskCallBack;

        pTask->SetNotify(taskNotify);
        pTask->SetMsg(&m_msgAllocator, &m_msgQueue);

        AddTaskToContainer(pTask->m_nTaskID, std::make_pair(pTask, pRespond));

        pTask->StartThread();
        pTask->StopThread();

        return pRespond->code;
    }

    uint32 SynRequestManager::CallBackHandle(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2)
    {
        if (msg == _URL_TASK_TO_MANAGER_MSG_)
        {
            uint32 notifyID = LOWORD(param);
            uint32 nTaskID = HIWORD(param);

            switch (notifyID)
            {
            case REQUEST_HEADER_RESPOND_NOTIFY:
                break;
            case REQUEST_DATA_RESPOND_NOTIFY:
                break;
            case REQUEST_COMPLETE_NOTIFY:
            {
                //请求结束
                _Value_Type taskInfo;
                if (!GetTaskInfo(nTaskID, &taskInfo))
                    return -1;

                RequestTask *pTask = taskInfo.first;
                Respond *pRespond = taskInfo.second;

                if (pTask && pRespond)
                {
                    uint32 usize = 0;
                    pRespond->code = pTask->GetRespondData(&pRespond->buffer, &usize);
                    if (pRespond->code != 0)
                    {
                        pRespond->buffer_size = usize;
                        pRespond->buffer = (char *)pTask->GetRespondErrorString();
                        pRespond->buffer_size = strlen(pRespond->buffer) + 1;
                    }
                }
            }
            break;
            default:break;
            }
        }

        return 0;
    }

    void CALLBACK SynRequestManager::TaskCallBack(uint32 msg, uint32 param, mxtoolkit::autoBit reserve1, mxtoolkit::autoBit reserve2, mxtoolkit::autoBit userData)
    {
        if (userData == 0)
            return;

        SynRequestManager *pManager = static_cast<SynRequestManager *>((void *)userData);

        if (pManager)
        {
            pManager->CallBackHandle(msg, param, reserve1, reserve2);
        }
    }

}