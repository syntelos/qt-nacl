#include <QtGui>
#include <QtDeclarative>

#include "declarativeviewer.h"

Q_IMPORT_PLUGIN(pepper)

int qt_pepper_main(int argc, char *argv[])
{
    // # disable timed waiting (crashes). This will cause
    // Qt to busy wait for events/timers at 100% CPU usage.
    extern bool qt_pepper_eventdispatcher_use_timed_wait;
    qt_pepper_eventdispatcher_use_timed_wait = false;

    QApplication app(argc, argv);

    DeclarativeViewer viewer;
    setScriptableObject(&viewer);
    viewer.show();

    return app.exec();
}

