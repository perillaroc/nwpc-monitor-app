#include "perspective_manager_private.h"
#include "perspective_manager.h"
#include "iperspective.h"
#include "../mainwindow.h"

#include <plugin_manager/plugin_manager.h>

#include <QtDebug>

using namespace Core::PerspectiveSystem;
using namespace PluginSystem;

PerspectiveManagerPrivate::PerspectiveManagerPrivate(QPointer<Core::MainWindow> main_window, PerspectiveManager *parent) :
    QObject(parent),
    q{parent},
    main_window_{main_window}
{

}

PerspectiveManagerPrivate::~PerspectiveManagerPrivate()
{

}

QVector<IPerspective *> PerspectiveManagerPrivate::perspectiveList()
{
    return perspective_list_;
}

int PerspectiveManagerPrivate::perspectiveIndex(QString id)
{
    for(int i=0; i<perspective_list_.length(); i++)
    {
        if(perspective_list_[i]->id() == id)
        {
            return i;
        }
    }
    return -1;
}

IPerspective *PerspectiveManagerPrivate::perspective(QString id)
{
    for(int i=0; i<perspective_list_.length(); i++)
    {
        if(perspective_list_[i]->id() == id)
        {
            return perspective_list_[i];
        }
    }
    return nullptr;
}

void PerspectiveManagerPrivate::loadPerspectives()
{
    QList<IPerspective*> perspectives = PluginManager::getObjects<IPerspective>();

    foreach(IPerspective* pers, perspectives)
    {
        qDebug()<<"[PerspectiveManagerPrivate::loadPerspectives]"<<pers->displayName();
        addPerspective(pers);
    }
}

void PerspectiveManagerPrivate::addPerspective(IPerspective *persp)
{
    perspective_list_.append(persp);
}

void PerspectiveManagerPrivate::activatePerspective(const QString &id)
{
    main_window_->activatePerspective(id);
}
