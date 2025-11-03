//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERCOMMON_QOMEGALISTMODEL_H
#define __OMEGA_PLAYERAPP_PLAYERCOMMON_QOMEGALISTMODEL_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QQmlEngine>
#include <QQmlContext>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT QOmegaListModel : public QAbstractListModel
{
    Q_OBJECT
    public:
        QOmegaListModel(QObject *parent = 0);
        virtual ~QOmegaListModel();

        virtual void reload();
        virtual void clear();

    protected:
        virtual void resetAndReload(bool isReload) = 0;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
