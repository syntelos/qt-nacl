#include "declarativeviewer.h"

DeclarativeViewer::DeclarativeViewer()
{
    view = new QDeclarativeView(this);
    connect(view->engine(), SIGNAL(quit()), SLOT(loadDefaultUrl()), Qt::QueuedConnection);
    view->resize(this->size());
    view->show();
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(Qt::white));
    setPalette(pal);
}

DeclarativeViewer::~DeclarativeViewer()
{
    delete view;
}

void DeclarativeViewer::clear()
{
    delete view;
    view = 0;
}

void DeclarativeViewer::load(const QString &address)
{
    qDebug() << "DeclarativeViewer::loadd" << address;

    view->setResizeMode(QDeclarativeView:: SizeRootObjectToView);
    QUrl url(address);
    view->setSource(url);
}

void DeclarativeViewer::setDefaultUrl(const QString &url)
{
    defaultUrl = url;
    qDebug() << "DeclarativeViewer::setDefaultUrl" << defaultUrl;
}

void DeclarativeViewer::loadDefaultUrl()
{
    qDebug() << "DeclarativeViewer::loadDefaultUrl" << defaultUrl;
    if (!defaultUrl.isEmpty()) {
        load(defaultUrl);
    }
}
