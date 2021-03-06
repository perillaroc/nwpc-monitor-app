#include "session_dialog.h"
#include "ui_session_dialog.h"
#include "session_manager.h"
#include "session_setting_dialog.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QtDebug>

using namespace Core::SessionSystem;

SessionDialog::SessionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionDialog),
    session_model_{new QStandardItemModel{this}}
{
    ui->setupUi(this);

    connect(ui->create_button, &QPushButton::clicked, this, &SessionDialog::createSession);
    connect(ui->edit_button, &QPushButton::clicked, this, &SessionDialog::editSession);
    connect(ui->clone_button, &QPushButton::clicked, this, &SessionDialog::cloneSession);
    connect(ui->remove_button, &QPushButton::clicked, this, &SessionDialog::removeSession);

    connect(ui->connect_button, &QPushButton::clicked, this, &SessionDialog::accept);
    connect(ui->cancel_button, &QPushButton::clicked, this, &QDialog::reject);

    connect(ui->session_table_view, &QTableView::doubleClicked, [=](const QModelIndex &index){
         accept();
    });

    ui->session_table_view->setModel(session_model_);
}

SessionDialog::SessionDialog(SessionManager *manager, QWidget *parent):
    SessionDialog(parent)
{
    setSessionManager(manager);
}

SessionDialog::~SessionDialog()
{
    delete ui;
}

void SessionDialog::setSessionManager(SessionManager *manager)
{
    session_manager_ = manager;
    updateSessionList();
}

Session SessionDialog::getSelectedSession() const
{
    return selected_session_;
}

void SessionDialog::createSession()
{
    SessionSettingDialog setting_dialog{this};
    if(setting_dialog.exec())
    {
        Session *session = setting_dialog.getSession();
        session_manager_->addSession(*session);
    }
    updateSessionList();
}

void SessionDialog::editSession()
{
    if(!checkSelectedSession())
        return;
    QString id = selected_session_.name_;

    SessionSettingDialog setting_dialog{this};
    setting_dialog.setSession(&selected_session_);
    if(setting_dialog.exec())
    {
        Session *session = setting_dialog.getSession();
        // session name can be edited. so delete old session first.
        session_manager_->removeSession(id);
        // NOTE: update session using addSession
        session_manager_->addSession(*session);
    }
    updateSessionList();
}

void SessionDialog::removeSession()
{
    if(!checkSelectedSession())
        return;
    session_manager_->removeSession(selected_session_.name_);
    updateSessionList();
}

void SessionDialog::cloneSession()
{
    if(!checkSelectedSession())
        return;
    SessionSettingDialog setting_dialog{this};
    setting_dialog.setSession(&selected_session_);
    if(setting_dialog.exec())
    {
        Session *session = setting_dialog.getSession();
        session_manager_->addSession(*session);
    }
    updateSessionList();
}

void SessionDialog::accept()
{
    if(checkSelectedSession())
        QDialog::accept();
    else
        return;
}

void SessionDialog::updateSessionList()
{
    session_model_->clear();
    auto session_map = session_manager_->sessionMap();
    QMapIterator<QString, Session> i(session_map);
    while (i.hasNext()) {
        i.next();
        QString id = i.key();
        Session session = i.value();
        session_model_->invisibleRootItem()->appendRow(
            QList<QStandardItem*>()
                    << new QStandardItem{session.name_}
                    << new QStandardItem{session.host_ + ":" + session.port_}
                    << new QStandardItem{session.user_}
        );
    }
    session_model_->setHorizontalHeaderLabels(QStringList()<<"Name"<<"Address"<<"User");
}

bool SessionDialog::checkSelectedSession()
{
    auto index_list = ui->session_table_view->selectionModel()->selectedRows();
    if(index_list.isEmpty())
    {
        qDebug()<<"[SessionDialog::checkSelectedSession] no row is selected selection";
        return false;
    }
    QModelIndex index = index_list.first();
    QString session_name = session_model_->itemFromIndex(index)->text();
    selected_session_ = session_manager_->getSession(session_name);
    return true;
}
