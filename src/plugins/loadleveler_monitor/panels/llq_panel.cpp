#include "llq_panel.h"
#include "ui_llq_panel.h"

#include "../loadleveler_model/job_query_model.h"
#include "../loadleveler_model/job_query_item.h"

#include "../loadleveler_monitor_plugin.h"
#include "../loadleveler_client.h"

#include "../client_command_widget.h"
#include "../loadleveler_monitor_widget.h"

#include <QActionGroup>
#include <QMap>
#include <QMenu>
#include <QtDebug>

using namespace LoadLevelerMonitor;
using namespace LoadLevelerMonitor::Panels;
using namespace LoadLevelerMonitor::LoadLevelerModel;

LlqPanel::LlqPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LlqPanel),
    style_action_group_{new QActionGroup{this}},
    template_action_group_{new QActionGroup{this}}
{
    ui->setupUi(this);

    connect(ui->check_all_button, &QPushButton::clicked,
            [=](){
                changeAllItemsCheckState(Qt::Checked);
            }
    );

    connect(ui->uncheck_all_button, &QPushButton::clicked,
            [=](){
                changeAllItemsCheckState(Qt::Unchecked);
            }
    );

    connect(ui->query_button, &QPushButton::clicked,
            this, &LlqPanel::slotRequestQuery);

    connect(ui->argument_edit, &QLineEdit::editingFinished,
            this, &LlqPanel::slotRequestQuery);

    setupTemplate();
    setupStyle();
}

LlqPanel::~LlqPanel()
{
    delete ui;
    if(job_query_model_){
        job_query_model_->deleteLater();
    }
}

void LlqPanel::setMonitorWidget(LoadLevelerMonitorWidget *widget)
{
    monitor_widget_ = widget;
}

void LlqPanel::setJobQueryModel(QPointer<JobQueryModel> job_query_model)
{
    if(job_query_model_)
    {
        ui->table_view->setModel(0);
        job_query_model_->deleteLater();
    }
    job_query_model_ = job_query_model;

    ui->table_view->setModel(job_query_model_);

    ui->table_view->horizontalHeader()->setStretchLastSection(true);

    disconnect(ui->table_view, &QTableView::customContextMenuRequested, 0, 0);
    ui->table_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->table_view, &QTableView::customContextMenuRequested,
            this, &LlqPanel::slotQueryRecordContextMenuRequest);
}

void LlqPanel::slotStyleActionTriggered(QAction *action)
{
    int index = style_action_list_.indexOf(action);
    if(index != -1)
        ui->view_area_stacked_widget->setCurrentIndex(index);
    else
    {
        qWarning()<<"[LlqPanel::slotStyleActionTriggered] action don't find in action list:"<<action;
    }
}

void LlqPanel::slotTemplateActionTriggered(QAction *action)
{
    int index = template_action_list_.indexOf(action);
    if(index != -1)
    {

    }
    else
    {
        qWarning()<<"[LlqPanel::slotTemplateActionTriggered] action don't find in action list:"<<action;
    }
}

void LlqPanel::slotRequestQuery()
{
    QMap<QString, QString> args = monitor_widget_->getSessionArguments();
    args["command"] = "llq";
    QString arg_string = ui->argument_edit->text();
    if(!arg_string.isEmpty())
        args["command"] += " "+arg_string;

    LoadLevelerMonitorPlugin::client()->runLlqCommand(args);
}

void LlqPanel::slotQueryRecordContextMenuRequest(const QPoint &pos)
{
    //qDebug()<<"[LoadLevelerMonitorWidget::slotLlqQueryRecordContextMenuRequest]";
    QModelIndex index = ui->table_view->indexAt(pos);
    if (index.isValid()) {
        QMenu *context_menu = new QMenu{};
        QAction *detail_action = new QAction{tr("详情")};

        context_menu->addAction(detail_action);
        QAction *action = context_menu->exec(ui->table_view->mapToGlobal(pos));

        if(action == detail_action)
        {
            QModelIndex id_index = index.sibling(index.row(), 1);
            if(index.isValid())
            {
                QStandardItem *id_item = job_query_model_->itemFromIndex(id_index);
                QString id = id_item->text();
                qDebug()<<"[LoadLevelerMonitorWidget::slotLlqQueryRecordContextMenuRequest]"<<id;
                QMap<QString, QString> args = monitor_widget_->getSessionArguments();
                args["command"] = "llq -l "+id;

                //TODO: 创建一个弹出窗口，接收命令执行结果，将窗口传递给 client。
                ClientCommandWidget *command_widget = new ClientCommandWidget();
                command_widget->show();

                LoadLevelerMonitorPlugin::client()->runCommand(args, command_widget);
            }
        }
        delete context_menu;
        delete detail_action;
    }
}

void LlqPanel::setupTemplate()
{
    ui->default_template_button->setDefaultAction(ui->action_default_template);
    ui->llqn_template_button->setDefaultAction(ui->action_llqn_template);
    ui->more_template_button->setDefaultAction(ui->action_more_template);

    template_action_list_.clear();
    template_action_list_.append(ui->action_default_template);
    template_action_list_.append(ui->action_llqn_template);
    template_action_list_.append(ui->action_more_template);

    foreach(QAction *action, template_action_list_)
    {
        template_action_group_->addAction(action);
    }

    connect(template_action_group_, &QActionGroup::triggered, this, &LlqPanel::slotTemplateActionTriggered);

    ui->action_default_template->setChecked(Qt::Checked);
}

void LlqPanel::setupStyle()
{
    ui->table_style_button->setDefaultAction(ui->action_table_style);
    ui->chart_style_button->setDefaultAction(ui->action_chart_style);
    ui->text_style_button->setDefaultAction(ui->action_text_style);

    style_action_list_.clear();
    style_action_list_.append(ui->action_table_style);
    style_action_list_.append(ui->action_chart_style);
    style_action_list_.append(ui->action_text_style);

    foreach(QAction *action, style_action_list_)
    {
        style_action_group_->addAction(action);
    }

    connect(style_action_group_, &QActionGroup::triggered, this, &LlqPanel::slotStyleActionTriggered);

    ui->action_table_style->setChecked(Qt::Checked);
    ui->view_area_stacked_widget->setCurrentIndex(0);
}

void LlqPanel::changeAllItemsCheckState(Qt::CheckState check_state)
{
    QStandardItem * root_item = job_query_model_->invisibleRootItem();
    int row_count = root_item->rowCount();
    for(int i=0; i<row_count; i++)
    {
        root_item->child(i, 0)->setCheckState(check_state);
    }

}
