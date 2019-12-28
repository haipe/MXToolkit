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
    virtual void Notify(DuiLib::TNotifyUI& msg) override;


};

