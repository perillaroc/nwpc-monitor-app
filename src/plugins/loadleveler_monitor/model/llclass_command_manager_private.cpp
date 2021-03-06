#include "llclass_command_manager_private.h"

#include "llclass_command_manager.h"
#include "query_item.h"
#include "query_model.h"
#include "query_util.h"

#include "../chart/category_model_processor.h"
#include "../chart/processor_condition.h"
#include "../chart/model_processor.h"
#include "../chart/percent_bar_processor.h"

#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QtDebug>

#include <algorithm>
#include <vector>

using namespace LoadLevelerMonitor::Model;
using namespace LoadLevelerMonitor::Chart;

const std::vector<DefaultQueryCategoryType> kLlclassDefaultQueryCategoryList = {
    std::make_tuple(Constant::Llclass::Name,           "Name",                 "Name",                 QueryValueType::String,  "",     kQueryTableRecordParser,    QVariantList{}    ),
    std::make_tuple(Constant::Llclass::MaxJobCpu,      "Max Job Cpu",          "MaxJobCPUd+hh:mm:ss",  QueryValueType::String,  "",     kQueryTableRecordParser,    QVariantList{}    ),
    std::make_tuple(Constant::Llclass::MaxProcCpu,     "Max Proc Cpu",         "MaxProcCPUd+hh:mm:ss", QueryValueType::String,  "",     kQueryTableRecordParser,    QVariantList{}    ),
    std::make_tuple(Constant::Llclass::FreeSlots,      "Free Slots",           "FreeSlots",            QueryValueType::String,  "",     kQueryTableRecordParser,    QVariantList{}    ),
    std::make_tuple(Constant::Llclass::MaxSlots,       "Max Slots",            "MaxSlots",             QueryValueType::String,  "",     kQueryTableRecordParser,    QVariantList{}    ),
    std::make_tuple(Constant::Llclass::Description,    "Description",          "Description",          QueryValueType::String,  "",     kQueryTableRecordParser,    QVariantList{}    ),

    // additional categories not used in command argument.
    std::make_tuple(Constant::Llclass::No,             "No.",                  "No.",                  QueryValueType::Number,  "",     kQueryTableRecordParser,    QVariantList{}   )    // row number in result records
};

const std::vector<DefaultQueryCategoryType> kLlclassDetailQueryCategoryList = {
    std::make_tuple(Constant::Llclass::Name,               "Name",             "Name",                 QueryValueType::String,  "",     kDetailLabelParser,    QVariantList{"Name"}    ),
    std::make_tuple(Constant::Llclass::ExcludeUsers,       "Exclude Users",    "Exclude_Users",        QueryValueType::String,  "",     kDetailLabelParser,    QVariantList{"Exclude_Users"}    ),
    std::make_tuple(Constant::Llclass::IncludeUsers,       "Include Users",    "Include_Users",        QueryValueType::String,  "",     kDetailLabelParser,    QVariantList{"Include_Users"}    ),
    std::make_tuple(Constant::Llclass::WallColockLimit,    "Wall clock limit", "Wall_clock_limit",     QueryValueType::String,  "",     kDetailLabelParser,    QVariantList{"Wall_clock_limit"}    ),
    std::make_tuple(Constant::Llclass::FreeSlots,          "Free Slots",       "Free_slots",           QueryValueType::String,  "",     kDetailLabelParser,    QVariantList{"Free_slots"}    ),
    std::make_tuple(Constant::Llclass::MaxSlots,           "Max Slots",        "Maximum_slots",        QueryValueType::String,  "",     kDetailLabelParser,    QVariantList{"Maximum_slots"}    ),

    // additional categories not used in command argument.
  //std::make_tuple(Constant::Llclass::No,                 "No.",                  "No.",              QueryValueType::Number,  "",     kDetailLabelParser,    QVariantList{"No."}    )    // row number in result records
};

LlclassCommandManagerPrivate::LlclassCommandManagerPrivate(LlclassCommandManager *parent) :
    QObject{parent},
    p{parent}
{

}

LlclassCommandManagerPrivate::~LlclassCommandManagerPrivate()
{

}

void LlclassCommandManagerPrivate::initCategoryList()
{
    default_query_category_list_.clear();
    foreach(auto record, kLlclassDefaultQueryCategoryList)
    {
        default_query_category_list_.append(QueryCategoryFactory::createLlclassDefaultCategory(record));
    }

    detail_query_category_list_.clear();
    foreach(auto record, kLlclassDetailQueryCategoryList)
    {
        QueryCategory c = QueryCategoryFactory::createLlclassDetailCategory(record);
        detail_query_category_list_.append(c);
    }
}

QueryCategory LlclassCommandManagerPrivate::findDefaultQueryCategory(const QString &label)
{
    if(default_query_category_list_.containsLabel(label))
        return default_query_category_list_.categoryFromLabel(label);
    else
        return QueryCategoryFactory::createDefaultQueryCategory();
}

QueryCategory LlclassCommandManagerPrivate::findDetailQueryCategory(const QString &label) const
{
    if(detail_query_category_list_.containsLabel(label))
        return detail_query_category_list_.categoryFromLabel(label);
    else
        return QueryCategoryFactory::createLlclassDetailCategory();
}

QueryModel *LlclassCommandManagerPrivate::buildQueryModelFromResponse(const QString &response_str)
{
    QJsonDocument doc = QJsonDocument::fromJson(response_str.toUtf8());
    if(!doc.isObject())
    {
        qDebug()<<"[LlclassCommandManagerPrivate::buildLlclassQueryModelFromResponse] result is not a json string.";
        return nullptr;
    }

    return buildQueryModelFromResponse(doc);
}

QueryModel *LlclassCommandManagerPrivate::buildQueryModelFromResponse(const QJsonDocument &response_json_document)
{
    qDebug()<<"[LlclassCommandManagerPrivate::buildLlclassQueryModelFromResponse] start";
    QJsonObject result_object = response_json_document.object();

    if( result_object.contains("error"))
    {
        QString error_message = result_object["data"].toObject()["message"].toObject()["error_message"].toString();
        qDebug()<<"[LlclassCommandManagerPrivate::buildLlclassQueryModelFromResponse] ERROR:"<<error_message;
        return nullptr;
    }

    QString app = result_object["app"].toString();
    QString type = result_object["type"].toString();
    QJsonObject data = result_object["data"].toObject();

    // request command
    QJsonObject request = data["request"].toObject();
    QString command = request["command"].toString();
    QJsonArray arguments = request["arguments"].toArray();
    QStringList arg_list;
    foreach(QJsonValue an_argument, arguments)
    {
        arg_list.append(an_argument.toString());
    }

    // response message
    QJsonObject message = data["response"].toObject()["message"].toObject();
    QString output_message = message["output"].toString();
    Q_ASSERT(!output_message.isEmpty());
    QStringList lines = output_message.split('\n');

    // build model
    QueryModel *model = nullptr;
    if(QueryUtil::isDetailQuery(command, arg_list))
    {
        model = QueryModel::buildFromLlclassDetailQueryResponse(lines);
    }
    else
    {
        model = QueryModel::buildFromLlclassDefaultQueryResponse(lines);
    }

    qDebug()<<"[LlclassCommandManagerPrivate::buildLlclassQueryModelFromResponse] end";
    return model;
}

void LlclassCommandManagerPrivate::initModelProcessor()
{
    // condition -> processor
    QueryCategory item_category = default_query_category_list_.categoryFromId("llclass.name");
    QueryCategory total_category = default_query_category_list_.categoryFromId("llclass.max_slots");
    QueryCategory free_category = default_query_category_list_.categoryFromId("llclass.free_slots");
    Q_ASSERT(item_category.isValid());
    Q_ASSERT(total_category.isValid());
    Q_ASSERT(free_category.isValid());

    PercentBarProcessorCondition *condition = new PercentBarProcessorCondition{item_category,total_category,free_category};
    PercentBarProcessor *processor = new PercentBarProcessor{item_category,total_category,free_category};
    processor->setDisplayName("slots");
    registerProcessorMap(condition, processor);
}

void LlclassCommandManagerPrivate::registerProcessorMap(ProcessorCondition *condition, ModelProcessor *processor)
{
    processor_list_.insert(processor);
    processor_condition_list_.insert(condition);
    processor_map_.insert(condition, processor);
}
