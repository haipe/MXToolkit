#include "miniblinkwidget.h"

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
        wkeOnCreateView(webView, MiniBlinkWidget::HandleCreateView, this);
        wkeOnLoadUrlBegin(webView, MiniBlinkWidget::HandleLoadUrlBegin, this);
        wkeOnLoadUrlEnd(webView, MiniBlinkWidget::HandleLoadUrlEnd, this);

        web_view = webView;

        //wkeMoveToCenter(webView);

        //QString path = "file:///" + QCoreApplication::applicationDirPath() + "/docs/index.html";
        //wkeLoadURL(webView, path.toLocal8Bit());
        //wkeLoadURL(webView, "https://danjuanapp.com/djapi/plan/CSI666/trade_history?size=20&page=1");
        //wkeLoadURL(webView, "https://danjuanapp.com/strategy/CSI666?channel=1100106186&source=rqzh");

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

// 回调：创建新的页面，比如说调用了 window.open 或者点击了 <a target="_blank" .../>
wkeWebView MiniBlinkWidget::HandleCreateView(wkeWebView, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features)
{
    //qDebug() << "HandleCreateView:" << wkeGetString(url);
    //wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, features->x, features->y, features->width, features->height);
    //wkeShowWindow(newWindow, true);
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    return miniWidget->onCreateView(navType,QString(wkeGetString(url)),features);
}

bool MiniBlinkWidget::HandleLoadUrlBegin(wkeWebView, void* param, const char *url, void *job)
{
    //qDebug() << "HandleLoadUrlBegin:" << url;
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    return miniWidget->onLoadUrlBegin(QString(url),job);
}

void MiniBlinkWidget::HandleLoadUrlEnd(wkeWebView, void* param, const char *url, void *job, void* buf, int len)
{
    //qDebug() << "HandleLoadUrlEnd:" << url << " buff :"<< (const char*)buf;
    MiniBlinkWidget* miniWidget = static_cast<MiniBlinkWidget*>(param);
    miniWidget->onLoadUrlEnd(QString(url),job,buf,len);
}

void MiniBlinkWidget::onDocumentReady()
{
    emit onLoadUrlCompleted();
}

void MiniBlinkWidget::onTitleChanged(const QString &title)
{

}

wkeWebView MiniBlinkWidget::onCreateView(wkeNavigationType navType, const QString& url, const wkeWindowFeatures *features)
{
    loadUrl(url);
    return web_view;
    //return nullptr;
}

bool MiniBlinkWidget::onLoadUrlBegin(const QString& url, void *job)
{
//    if (url.indexOf("https://kyfw.12306.cn/otn/leftTicket/queryZ?") != -1)
//    {
//        wkeNetHookRequest(job);
//    }

    return true;
}

void MiniBlinkWidget::onLoadUrlEnd(const QString& url, void *job, void *buf, int len)
{

}
}
