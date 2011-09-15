#include <QtGui/QPlatformIntegrationPlugin>
#include <qbasicunixfontdatabase.h>
#include "qpepperintegration.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

class QPepperIntegrationPlugin : public QPlatformIntegrationPlugin
{
public:
    QStringList keys() const;
    QPlatformIntegration *create(const QString&, const QStringList&);
};

QStringList QPepperIntegrationPlugin::keys() const
{
    QStringList list;
    list << "Pepper";
    return list;
}

QPlatformIntegration* QPepperIntegrationPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);
    if (system.toLower() == "pepper")
        return new QPepperIntegration;
    return 0;
}

Q_EXPORT_PLUGIN2(pepper, QPepperIntegrationPlugin)

QT_END_NAMESPACE
