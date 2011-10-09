#include <QtGui/QApplication>
#include "serverwidget.h"
#include "httpserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    quint16 port = 5103; // the nacl port (plugin allows executables served from this port)
    Server server(port);
    server.addRootPath("../../examples");
    server.addRootPath("../../demos");

    ServerWidget w;
    w.ui->label_portMessage->setText
            (QString("The server is running on http://localhost:%1/. Open the address in a "
                     "Google Native Client-enabled web browser.").arg(port));
    w.ui->label_portMessage->setOpenExternalLinks(true);
    w.ui->label_portMessage->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
    w.ui->lineEdit->setText(QDir::toNativeSeparators(QDir::currentPath() + "/nacldemo"));
    server.setSaveLocationLineEdit(w.ui->lineEdit);
    QObject::connect(w.ui->pushButton_save, SIGNAL(clicked()),
                     &server, SLOT(saveDemoFiles()));
    w.show();
    return a.exec();
}
