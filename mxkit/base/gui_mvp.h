#pragma once

#include "mxkit.h"
#include "base/object_ontainer.h"

#define MVPInterface  struct

#ifdef _MX_WIN_

#define SAFE_CLOSEWND(wnd)                  \
if ((wnd))                                  \
{                                           \
    ::SendMessage((wnd), WM_CLOSE, 0, 0);   \
    (wnd) = 0;                              \
}    
#else
#define SAFE_CLOSEWND(wnd)     

#endif
_BEGIN_MX_KIT_NAME_SPACE_

template<typename Obj = void>
MVPInterface MvpModel : public mxkit::ObjectContainer<Obj>
{
    typedef Obj* ObjPtr;
    typedef Obj ModelObjectContainerType;
    typedef ModelObjectContainerType* ModelObjectContainerTypePoint;

    virtual ~MvpModel() {};

    virtual bool InitModel(ModelObjectContainerTypePoint obj) = 0;
    virtual void DestoryModel() = 0;
};

MVPInterface MvpView
{
    virtual ~MvpView() {};

    virtual Hwnd InitView(Hwnd parent) = 0;

    virtual void DestoryView() = 0;
};

template<typename Model, typename View>
MVPInterface MvpPresenter
{
public:
    virtual ~MvpPresenter() {};

    typedef Model* ModelPtr;
    typedef View* ViewPtr;

    virtual bool	InitModel(typename Model::ModelObjContainerTypePtr model) = 0;
    virtual ViewPtr	InitView(Hwnd parent) = 0;

    virtual Hwnd	GetViewWnd() { return m_viewWnd; }

    virtual void	Destroy()
    {
        auto view = dynamic_cast<MvpView*>(m_view);
        if (view)view->DestoryView();

        Hwnd wnd = GetViewWnd();

        SAFE_CLOSEWND(wnd);

        SetViewWnd(nullptr);

        if (m_model)
            m_model->Release();
    }

protected:
    virtual Hwnd    SetViewWnd(Hwnd wnd) { return m_viewWnd = wnd; }

protected:
    ModelPtr	m_model = nullptr;

    ViewPtr		m_view = nullptr;
    Hwnd        m_viewWnd = nullptr;
};


_END_MX_KIT_NAME_SPACE_