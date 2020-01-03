#ifndef MINIBLINKWIDGET_H
#define MINIBLINKWIDGET_H

#include <QWidget>
#include "_mx_qt_include.h"
#include "wke.h"

namespace mxtoolkit
{

class MiniBlinkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MiniBlinkWidget(QWidget *parent = nullptr, const QString& url = "");

    virtual ~MiniBlinkWidget() override;

    static void wkeInit();
    static bool isWkeInited();
    static void wkeFinal();

    virtual void loadUrl(const QString& url);

    virtual unsigned int addHookRequest(const QString& url);
    virtual void removeHookRequest(const QString& url);

signals:
    //自定义信号
    void onLoadUrlCompleted();

    mxtoolkit::MiniBlinkWidget* onCreateWebView(
            wkeNavigationType navType, const QString& url, const wkeWindowFeatures* features);

    void onHookRequest(unsigned int, const QString& url, const QString& request, const QString& respons);

public slots:

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

protected:
    static void HandleDocumentReady(wkeWebView webWindow, void* param);
    static void HandleTitleChanged(wkeWebView webWindow, void* param, const wkeString title);
    static void HandleUrlChanged(wkeWebView webView, void *param, const wkeString url);
    static wkeWebView HandleCreateView(wkeWebView webWindow, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features);
    static bool HandleLoadUrlBegin(wkeWebView webView, void* param, const char *url, void *job);
    static void HandleLoadUrlEnd(wkeWebView webView, void* param, const char *url, void *job, void* buf, int len);

protected:
    // 回调：文档加载成功
    virtual void onDocumentReady();
    // 回调：页面标题改变
    virtual void onTitleChanged(const QString& title);
    virtual void onUrlChanged(const QString& url);
    virtual wkeWebView onCreateView(wkeNavigationType navType, const QString& url, const wkeWindowFeatures* features);
    virtual bool onLoadUrlBegin(const QString& url, void *job);
    virtual void onLoadUrlEnd(const QString& url, void *job, void* buf, int len);

protected:
    QString request_url;

    unsigned int hook_request_base_id;

    QMap<QString,unsigned int> hook_request;
    struct HookUrlInfo
    {
        unsigned int id = 0;
        QString url;
    };

    QMap<unsigned int,HookUrlInfo> hook_jobs;

    wkeWebView web_view;
};

}
#endif // MINIBLINKWIDGET_H
