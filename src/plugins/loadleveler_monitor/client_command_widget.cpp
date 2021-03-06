#include "client_command_widget.h"
#include "ui_client_command_widget.h"

#include <python_env_plugin/python_command.h>
#include "loadleveler_monitor_plugin.h"
#include "loadleveler_client.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QtDebug>

using namespace LoadLevelerMonitor;
using namespace PythonEnv;

ClientCommandWidget::ClientCommandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientCommandWidget),
    python_command_{nullptr}
{
    ui->setupUi(this);
    ui->message_tab_widget->setCurrentIndex(0);

    //setAttribute(Qt::WA_DeleteOnClose, true);
}

ClientCommandWidget::~ClientCommandWidget()
{
    qDebug()<<"[ClientCommandWidget::~ClientCommandWidget] delete";
    delete ui;
}

void ClientCommandWidget::setCommandText(const QString &command)
{
    ui->command_edit->setText(command);
}

void ClientCommandWidget::receiveResponse(const QString &response)
{
    setResponseText(response);

    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
    if(!doc.isObject())
    {
        qDebug()<<"[ClientCommandWidget::receiveResponse] result is not a json string.";
        return;
    }

    QJsonObject result_object = doc.object();

    QString error_message = result_object["data"].toObject()["response"].toObject()["message"].toObject()["error_output"].toString();
    setErrorOutputText(error_message);

    QString output_message = result_object["data"].toObject()["response"].toObject()["message"].toObject()["output"].toString();
    setOutputText(output_message);
}

void ClientCommandWidget::setOutputText(const QString &output)
{
    ui->output_edit->setText(output);
}

void ClientCommandWidget::setErrorOutputText(const QString &error_output)
{
    ui->error_output_edit->setText(error_output);
}

void ClientCommandWidget::setResponseText(const QString &output)
{
    ui->response_edit->setText(output);
}

void ClientCommandWidget::closeEvent(QCloseEvent *event)
{
    qDebug()<<"[ClientCommandWidget::closeEvent] closeEvent";
    this->deleteLater();
}
