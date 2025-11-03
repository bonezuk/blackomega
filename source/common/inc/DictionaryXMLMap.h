//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_DICTIONARYXMLMAP_H
#define __OMEGA_COMMON_DICTIONARYXMLMAP_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

#include <QMap>
#include <QString>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT DictionaryXMLMap
{
    public:
        static QMap<QString,QString> read(const QString& fileName);
        static bool write(const QString& fileName,const QMap<QString,QString>& dict);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

