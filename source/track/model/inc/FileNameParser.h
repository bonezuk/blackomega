//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_FILENAMEPARSER_H
#define __OMEGA_TRACK_MODEL_FILENAMEPARSER_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QStringList>

#include "track/model/inc/TrackModelDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT FileNameParser
{
    public:
        FileNameParser();
        virtual ~FileNameParser();
        
        QStringList parse(const QString& str);
    private:
        QStringList processPart(const QString& str);
        QStringList parsePart(const QString& str);
        int trackNumberIfAvailable(const QString& str, QString& nPart);
};

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

