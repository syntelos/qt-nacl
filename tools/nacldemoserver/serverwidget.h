#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QtGui/QWidget>
#include "ui_serverwidget.h"

namespace Ui
{
    class ServerWidget;
}

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    ServerWidget(QWidget *parent = 0);
    ~ServerWidget();

    Ui::ServerWidget *ui;
};

#endif // SERVERWIDGET_H
