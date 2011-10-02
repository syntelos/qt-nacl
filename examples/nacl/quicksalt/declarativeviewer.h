#ifndef DECLARATIVE_VIEVER
#define DECLARATIVE_VIEVER

#include <QtGui>
#include <QtDeclarative>

class DeclarativeViewer : public QWidget
{
Q_OBJECT
public:
    DeclarativeViewer();
    ~DeclarativeViewer();
public Q_SLOTS:
    void clear();
    void load(const QString &url);
    void setDefaultUrl(const QString &url);
    void loadDefaultUrl();
private:
    QDeclarativeView *view;
    QString defaultUrl;
};

#endif
