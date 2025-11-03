//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERCOMMON_OMEGAAUDIOINTERFACE_H
#define __OMEGA_PLAYERAPP_PLAYERCOMMON_OMEGAAUDIOINTERFACE_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"

#include <QString>
#include <QObject>

#include "common/inc/CommonTypes.h"
#include "common/inc/TimeStamp.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT OmegaAudioInterface : public QObject
{
    Q_OBJECT
    public:
        OmegaAudioInterface(QObject *parent = 0);

        virtual void playFile(const QString& fileName, bool isNext) = 0;
        virtual void playFileWithTime(const QString& fileName, const common::TimeStamp& start,const common::TimeStamp& length, bool isNext) = 0;
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;
        virtual void seek(const common::TimeStamp& t) = 0;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
