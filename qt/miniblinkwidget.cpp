#include "miniblinkwidget.h"
#include <QMetaMethod>


void MessageBoxA(HWND,const char* msg, const char* func, int)
{
    qDebug() << "[msg] " << msg << " [ " << func << " ]." ;
}

namespace mxtoolkit
{


MiniBlinkWidget::MiniBlinkWidget(QWidget *parent, const QString& url)
    : QWidget(parent)
    , request_url(url)
{
    HWND wnd = (HWND)QWidget::winId();

    ////wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, NULL, 0, 0, 640, 480);
    wkeWebView webView = wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, wnd, 0, 0, 640, 480);
    if (webView)
    {
        wkeOnDocumentReady(webView, MiniBlinkWidget::MiniBlinkWidget::HandleDocumentReady, this);
        wkeOnTitleChanged(webView, MiniBlinkWidget::HandleTitleChanged, this);
        wkeOnURLChanged(webView,MiniBlinkWidget::HandleUrlChanged,this);
        wkeOnCreateView(webView, MiniBlinkWidget::HandleCreateView, this);
        wkeOnLoadUrlBegin(webView, MiniBlinkWidget::HandleLoadUrlBegin, this);
        wkeOnLoadUrlEnd(webView, MiniBlinkWidget::HandleLoadUrlEnd, this);

        web_view = webView;

        wkeSetCookieEnabled(webView,true);

        if(!request_url.isEmpty())
            wkeLoadURL(webView,request_url.toStdString().c_str());
    }
}

MiniBlinkWidget::~MiniBlinkWidget()
{
    if(web_view)
    {
        wkeDestroyWebWindow(web_view);
    }
}

void MiniBlinkWidget::wkeInit()
{
    if(!wkeIsInitialize || !wkeIsInitialize())
        ::wkeInitialize();
}

void MiniBlinkWidget::wkeFinal()
{
    if(wkeIsInitialize && wkeIsInitialize())
        wkeFinalize();
}

void MiniBlinkWidget::loadUrl(const QString &url)
{

    if(!wkeIsInitialize || !wkeIsInitialize())
        return;

    if(url.isEmpty())
        return;

    request_url = url;
    wkeLoadURL(web_view,request_url.toStdString().c_str());
}

void MiniBlinkWidget::addHookRequest(const QString &url)
{
    hook_request.insert(url);
}

void MiniBlinkWidget::removeHookRequest(const QString &url)
{
    hook_request.remove(url);
}

void MiniBlinkWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if(web_view)
    {
        wkeResize(web_view,event->size().width(),event->size().height());
    }
}

// 回调：文档加载成功
void MiniBlinkWidget::HandleDocumentReady(wkeWebView webWindow, void* param)
{
    //qDebug() << "HandleDocumentReady:";
    wkeShowWindow(webWindow, TRUE);

    //const utf8* ut = wkeGetSource(webWindow);
    //qDebug() << "wkeGetSource :" << ut;

    //std::string str(ut);
    //qDebug() << "strlen : "  << strlen(ut) << " len:" << str.length();
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    miniWidget->onDocumentReady();
}

// 回调：页面标题改变
void MiniBlinkWidget::HandleTitleChanged(wkeWebView webWindow, void* param, const wkeString title)
{
    //qDebug() << "HandleTitleChanged:";
    wkeSetWindowTitle(webWindow, wkeGetString(title));

    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    miniWidget->onTitleChanged("");
}

void MiniBlinkWidget::HandleUrlChanged(wkeWebView webView, void* param, const wkeString url)
{
    qDebug() << "url change:" << wkeGetString(url);

    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    miniWidget->onUrlChanged(wkeGetString(url));
}
// 回调：创建新的页面，比如说调用了 window.open 或者点击了 <a target="_blank" .../>
wkeWebView MiniBlinkWidget::HandleCreateView(
        wkeWebView, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features)
{
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    return miniWidget->onCreateView(navType,QString(wkeGetString(url)),features);
}

bool MiniBlinkWidget::HandleLoadUrlBegin(wkeWebView, void* param, const char *url, void *job)
{
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    return miniWidget->onLoadUrlBegin(QString(url),job);
}

void MiniBlinkWidget::HandleLoadUrlEnd(wkeWebView, void* param, const char *url, void *job, void* buf, int len)
{
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    miniWidget->onLoadUrlEnd(QString(url),job,buf,len);
}

void MiniBlinkWidget::onDocumentReady()
{
    emit onLoadUrlCompleted();
}

void MiniBlinkWidget::onTitleChanged(const QString &title)
{
    setWindowTitle(title);
}

void MiniBlinkWidget::onUrlChanged(const QString &url)
{
    request_url = url;
}

wkeWebView MiniBlinkWidget::onCreateView(wkeNavigationType navType, const QString& url, const wkeWindowFeatures *features)
{
    QMetaMethod met = QMetaMethod::fromSignal(&MiniBlinkWidget::onCreateWebView);
    MiniBlinkWidget* widget = nullptr;
    if(met.isValid())
    {
        met.invoke( this,
                    Q_RETURN_ARG(MiniBlinkWidget*, widget),
                    Q_ARG(wkeNavigationType,navType),
                    Q_ARG(const QString&,url),
                    Q_ARG(const wkeWindowFeatures*, features));
    }

    //loadUrl(url);
    return widget ? widget->web_view : this->web_view;
}

bool MiniBlinkWidget::onLoadUrlBegin(const QString& url, void *job)
{
    for(auto hookUrl : hook_request)
    {
        if (url.indexOf(hookUrl) != -1)
        {
            wkeNetHookRequest(job);
            hook_jobs[(unsigned int)job] = hookUrl;
            break;
        }
    }

    return true;
}

void MiniBlinkWidget::onLoadUrlEnd(const QString& url, void *job, void *buf, int len)
{
    QString hookUrl = hook_jobs[(unsigned int)job];
    if(hookUrl.isEmpty())
        return;

    emit onHookRequest(hookUrl,url,QString((const char*)buf));
}
}
