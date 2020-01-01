#pragma once

#include "MXDuiWnd.h"

class MainWindow 
    : public mxtoolkit::MXDuiWnd
    , public DuiLib::INotifyUI
{
public:
    MainWindow();
    ~MainWindow();

protected:
    virtual HRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) override;

    virtual void Notify(DuiLib::TNotifyUI& msg) override;

protected:
    bool getAppCompleted_ = false;
    unsigned int getProcess_ = 0;

    DuiLib::CVerticalLayoutUI* msgArea_ = nullptr;
    DuiLib::CLabelUI* msgLabel_ = nullptr;
    DuiLib::CLabelUI* processLabel_ = nullptr;

};

