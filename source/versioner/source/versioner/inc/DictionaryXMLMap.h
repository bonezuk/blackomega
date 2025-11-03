//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_VERSIONER_DICTIONARYXMLMAP_H
#define __OMEGA_VERSIONER_DICTIONARYXMLMAP_H
//-------------------------------------------------------------------------------------------

#include <QMap>
#include <QString>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace versioner
{
//-------------------------------------------------------------------------------------------

class DictionaryXMLMap
{
    public:
        static QMap<QString,QString> read(const QString& fileName);
        static bool write(const QString& fileName,const QMap<QString,QString>& dict);
};

//-------------------------------------------------------------------------------------------
} // namespace versioner
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

