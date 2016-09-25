#include <QApplication>
#include <QPointer>

#include <core_plugin/session_system/session_widget.h>
#include <core_plugin/session_system/session_manager.h>
#include <core_plugin/session_system/session.h>


using namespace Core::SessionSystem;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPointer<SessionManager> session_manager = new SessionManager;
    Session wangdp{"wangdp", "uranus.hpc.nmic.cn", "22", "wangdp", "perilla"};
    session_manager->addSession(wangdp);

    SessionWidget session_widget;
    session_widget.setSessionManager(session_manager);
    session_widget.show();


    return a.exec();
}
