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
    bool getAppCompleted = false;
    DuiLib::CLabelUI* msgLabel_ = nullptr;

};

