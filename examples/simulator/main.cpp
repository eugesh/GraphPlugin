#include <QApplication>

#include "simulatormainwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationVersion("1.0");
    app.setApplicationName("GraphPluginExample");
    app.setOrganizationName("eugesh");
    app.setOrganizationDomain("www.github.com/eugesh");

    SimulatorMainWindow wnd(nullptr);

    wnd.show();

    return app.exec();
}
