DEFINES += LOADLEVELER_MONITOR_LIBRARY

include(../../../nwpc-monitor-app.pri)

QT       += core gui widgets charts webenginewidgets webchannel

TARGET = loadleveler_monitor
TEMPLATE = lib
CONFIG += plugin c++11

INCLUDEPATH += ../../libs
INCLUDEPATH += ../../plugins

LIBS += -L$$build_lib_dir -lplugin_system -lprogress_util -lutil\
        -L$$build_plugins_dir -lcore_plugin \
        -L$$build_plugins_dir/python_env -lpython_env

DESTDIR = $$build_plugins_dir/loadleveler_monitor

SOURCES += loadleveler_monitor_plugin.cpp \
    loadleveler_monitor_perspective.cpp \
    loadleveler_monitor_widget.cpp \
    loadleveler_client.cpp \
    client_command_widget.cpp \
    panels/llq_panel.cpp \
    model/llq_command_manager.cpp \
    model/llq_command_manager_private.cpp \
    model/query_category.cpp \
    model/query_item.cpp \
    model/query_model.cpp \
    model/query_category_list.cpp \
    chart/processor_condition.cpp \
    chart/model_processor.cpp \
    chart/category_model_processor.cpp \
    panels/style_page.cpp \
    panels/text_style_page.cpp \
    panels/chart_style_page.cpp \
    panels/table_style_page.cpp \
    panels/llclass_panel.cpp \
    panels/query_panel.cpp \
    model/llclass_command_manager.cpp \
    model/llclass_command_manager_private.cpp \
    chart/percent_bar_processor.cpp \
    model/query_util.cpp \
    model/query_item_value_saver.cpp \
    model/query_record_parser.cpp \
    model/special_value_saver.cpp \
    model/special_record_parser.cpp \
    organize_system/filter_value_checker.cpp \
    organize_system/filter_value_extractor.cpp \
    organize_system/filter_condition.cpp \
    organize_system/filter.cpp \
    organize_system/filter_query_model.cpp \
    widgets/job_detail_widget.cpp \
    widgets/file_viewer_widget.cpp

HEADERS += loadleveler_monitor_plugin.h \
    loadleveler_monitor_global.h \
    loadleveler_monitor_perspective.h \
    loadleveler_monitor_widget.h \
    loadleveler_client.h \
    client_command_widget.h \
    panels/llq_panel.h \
    model/llq_command_manager.h \
    model/llq_command_manager_private.h \
    model/query_category.h \
    model/query_item.h \
    model/query_model.h \
    model/query_category_list.h \
    chart/processor_condition.h \
    chart/model_processor.h \
    chart/category_model_processor.h \
    panels/style_page.h \
    panels/text_style_page.h \
    panels/chart_style_page.h \
    panels/table_style_page.h \
    panels/llclass_panel.h \
    panels/query_panel.h \
    model/llclass_command_manager.h \
    model/llclass_command_manager_private.h \
    chart/percent_bar_processor.h \
    model/model_constants.h \
    model/query_util.h \
    model/query_item_value_saver.h \
    model/query_record_parser.h \
    model/special_value_saver.h \
    model/special_record_parser.h \
    organize_system/filter_value_checker.h \
    organize_system/filter_value_extractor.h \
    organize_system/filter_condition.h \
    organize_system/filter.h \
    organize_system/filter_query_model.h \
    widgets/job_detail_widget.h \
    widgets/file_viewer_widget.h

DISTFILES += loadleveler_monitor.json \
    nwpc_loadleveler/loadleveler.py \
    doc/llq.introduction.md

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    loadleveler_monitor_widget.ui \
    client_command_widget.ui \
    panels/text_style_page.ui \
    panels/chart_style_page.ui \
    panels/table_style_page.ui \
    panels/query_panel.ui \
    widgets/job_detail_widget.ui \
    widgets/file_viewer_widget.ui

RESOURCES += \
    loadleveler_monitor.qrc


nwpc_loadleveler_target_dir = $${DESTDIR}/nwpc_loadleveler
nwpc_loadleveler_target_dir~=s,/,\\,g
nwpc_loadleveler_dir = $$PWD/nwpc_loadleveler
nwpc_loadleveler_dir~=s,/,\\,g

QMAKE_POST_LINK += $$quote(IF NOT EXIST $${nwpc_loadleveler_target_dir} (MKDIR $${nwpc_loadleveler_target_dir})$$escape_expand(\n\t))
QMAKE_POST_LINK += $$quote(XCOPY $${nwpc_loadleveler_dir} $${nwpc_loadleveler_target_dir} /E /Y$$escape_expand(\n\t))
