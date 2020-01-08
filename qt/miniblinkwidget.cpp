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
    if(!MiniBlinkWidget::isWkeInited())
        return;

    hook_request_base_id = (unsigned int)this;
    HWND wnd = (HWND)QWidget::winId();

    ////wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, NULL, 0, 0, 640, 480);
    wkeWebView webView = (wkeCreateWebWindow == nullptr) ?
                nullptr : wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, wnd, 0, 0, 640, 480);

    if (webView)
    {
        wkeOnDocumentReady(webView, MiniBlinkWidget::onHandleDocumentReady, this);
        wkeOnTitleChanged(webView, MiniBlinkWidget::onHandleTitleChanged, this);
        wkeOnURLChanged(webView,MiniBlinkWidget::onHandleUrlChanged,this);
        wkeOnCreateView(webView, MiniBlinkWidget::onHandleCreateView, this);
        wkeOnLoadUrlBegin(webView, MiniBlinkWidget::onHandleLoadUrlBegin, this);
        wkeOnLoadUrlEnd(webView, MiniBlinkWidget::onHandleLoadUrlEnd, this);

        web_view = webView;

        wkeSetCookieEnabled(webView,true);

        loadUrl(request_url);
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
    if(!isWkeInited())
        ::wkeInitialize();
}

bool MiniBlinkWidget::isWkeInited()
{
    return wkeIsInitialize && wkeIsInitialize();
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

    qDebug() << "MiniBlinkWidget::loadUrl :" << (void*)this;
    wkeJsBindFunction("onJsCall",MiniBlinkWidget::onHandleJsCall, this, 1);

    wkeLoadURL(web_view,request_url.toStdString().c_str());
}

unsigned int MiniBlinkWidget::addHookRequest(const QString &url)
{
    if(url.isEmpty())
        return 0;

    QMap<QString, unsigned int>::iterator it = hook_request.find(url);
    if(it != hook_request.end())
        return it.value();

    unsigned int id = ++hook_request_base_id;
    hook_request[url] = id;
    return id;
}

void MiniBlinkWidget::removeHookRequest(const QString &url)
{
    if(url.isEmpty())
        return;

    hook_request.remove(url);
}

void MiniBlinkWidget::runJavaScript(const QString &js)
{
    if(web_view)
    {
        wkeRunJS(web_view, js.toStdString().c_str());
    }
}

void MiniBlinkWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if(web_view)
    {
        wkeResize(web_view,event->size().width(),event->size().height());
    }
}

void WKE_CALL_TYPE MiniBlinkWidget::onHandleDocumentReady(wkeWebView webWindow, void* param)
{
    wkeShowWindow(webWindow, TRUE);

    //const utf8* ut = wkeGetSource(webWindow);
    //qDebug() << "wkeGetSource :" << ut;

    //std::string str(ut);
    //qDebug() << "strlen : "  << strlen(ut) << " len:" << str.length();
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    if(miniWidget)
        miniWidget->onDocumentReady();
}

void WKE_CALL_TYPE MiniBlinkWidget::onHandleTitleChanged(wkeWebView webWindow, void* param, const wkeString title)
{
    //qDebug() << "HandleTitleChanged:";
    wkeSetWindowTitle(webWindow, wkeGetString(title));

    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    if(miniWidget)
        miniWidget->onTitleChanged("");
}

void WKE_CALL_TYPE MiniBlinkWidget::onHandleUrlChanged(wkeWebView webView, void* param, const wkeString url)
{
    qDebug() << "url change:" << wkeGetString(url);

    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    if(miniWidget)
        miniWidget->onUrlChanged(wkeGetString(url));
}

wkeWebView WKE_CALL_TYPE MiniBlinkWidget::onHandleCreateView(
        wkeWebView, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features)
{
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    if(miniWidget)
        return miniWidget->onCreateView(navType,QString(wkeGetString(url)),features);
}

bool WKE_CALL_TYPE MiniBlinkWidget::onHandleLoadUrlBegin(wkeWebView, void* param, const char *url, void *job)
{
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    if(miniWidget)
        return miniWidget->onLoadUrlBegin(QString(url),job);
}

void WKE_CALL_TYPE MiniBlinkWidget::onHandleLoadUrlEnd(wkeWebView, void* param, const char *url, void *job, void* buf, int len)
{
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    if(miniWidget)
        miniWidget->onLoadUrlEnd(QString(url),job,buf,len);
}

jsValue WKE_CALL_TYPE MiniBlinkWidget::onHandleJsCall(const char *name, jsExecState es, void *param)
{
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    if(miniWidget)
        miniWidget->onJavaScritCall(QString(name),es);
    return 0;
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
    QMap<QString, unsigned int>::iterator it = hook_request.begin();
    while(it != hook_request.end())
    {
        if (url.indexOf(it.key()) != -1)
        {
            wkeNetHookRequest(job);
            hook_jobs[(unsigned int)job] = {it.value(),it.key()};
            break;
        }

        it++;
    }

    return true;
}

void MiniBlinkWidget::onLoadUrlEnd(const QString& url, void *job, void *buf, int len)
{
    HookUrlInfo& hookInfo = hook_jobs[(unsigned int)job];
    if(hookInfo.id == 0 || hookInfo.url.isEmpty())
        return;

    emit onHookRequest(hookInfo.id, hookInfo.url,url,QString::fromUtf8((const char*)buf,len));
}

void MiniBlinkWidget::onJavaScritCall(const QString &function, jsExecState es)
{
    if(function == "onJsCall")
    {
        const char *text = jsToString(es, jsArg(es, 0));
        qDebug() << "onJsCall(" << text <<")";
    }
}
}
