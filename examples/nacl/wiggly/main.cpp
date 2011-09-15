#include <QtGui>

#include "dialog.h"

#ifdef Q_OS_NACL
Q_IMPORT_PLUGIN(pepper);
int qt_pepper_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    QApplication app(argc, argv);
    bool smallScreen = QApplication::arguments().contains("-small-screen");

    Dialog dialog(0, smallScreen);

    if (!smallScreen)
        dialog.show();
    else
        dialog.showFullScreen();
    return app.exec();
}

