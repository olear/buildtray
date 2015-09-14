#include "buildtray.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("BuildTray");
    QApplication::setApplicationVersion("0.1");
    QApplication::setOrganizationDomain("fxarena.net");
    QApplication::setOrganizationName("FxArena");
    BuildTray w;
    w.show();

    return a.exec();
}
