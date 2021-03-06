#pragma once

#include "../core_plugin_global.h"

#include <QObject>
#include <QFuture>

namespace Core {

class CorePlugin;

namespace ProgressSystem{

class ProgressView;
class ProgressItemWidget;
class ProgressStatusBarWidget;

class CORE_PLUGIN_EXPORT ProgressManager : public QObject
{
    Q_OBJECT
public:
    static ProgressManager *instance();
    static void init();

    static ProgressView *progressView();
    static void setProgressView(ProgressView *view);
    static ProgressStatusBarWidget *statusBarWidget();

    static ProgressItemWidget *addTask(const QFuture<void> &future, const QString &title);
signals:

public slots:

private:
    explicit ProgressManager(QObject *parent = 0);
    ~ProgressManager();

    friend class Core::CorePlugin;
};

}
}
