//-------------------------------------------------------------------------------------------
#ifndef __ORCUS_PLAYERAPP_PLAYERCOMMON_PLAYBACKSTATE_H
#define __ORCUS_PLAYERAPP_PLAYERCOMMON_PLAYBACKSTATE_H
//-------------------------------------------------------------------------------------------

#include <QtGlobal>
#include <QObject>
#include <QQmlEngine>

#include "common/inc/TimeStamp.h"
#include "track/db/inc/DBInfo.h"
#include "track/db/inc/TrackDB.h"
#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "playerapp/playercommon/inc/OmegaAudioInterface.h"

//-------------------------------------------------------------------------------------------
namespace orcus
{
//-------------------------------------------------------------------------------------------

class PlayListModel;

//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT PlaybackStateController : public QObject
{
	public:
		Q_OBJECT
		Q_PROPERTY(quint32 timeInSeconds READ getTimeInSeconds NOTIFY onTimeChanged)
		Q_PROPERTY(qint32 index READ getIndex NOTIFY onIndexChanged)
		Q_PROPERTY(qint32 state READ getState NOTIFY onStateChanged)

	public:
		enum PlayState
		{
			Pause = 0,
			Play
		};
		Q_ENUM(PlayState)

	public:
		PlaybackStateController(QObject *parent = 0);
		PlaybackStateController(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent = 0);
		virtual ~PlaybackStateController();
		
		virtual quint32 getTimeInSeconds() const;
		virtual qint32 getIndex() const;
		virtual qint32 getState() const;
		virtual tuint64 getCurrentId() const;
		virtual const common::TimeStamp& getTime() const;
		
		virtual void setTime(quint64 tS);
		virtual void setNextItem(tuint64 itemId);
		
		virtual void onAudioStart(const QString& fileName);
		virtual void onAudioPlay();
		virtual void onAudioPause();
		virtual void onAudioStop();

		virtual void resumeOrPausePlayback();

	signals:
		void onTimeChanged();
		void onIndexChanged();
		void onStateChanged();
		
	protected:
		QSharedPointer<OmegaAudioInterface> m_pAudioInterface;
		PlayListModel *m_pModel;
		common::TimeStamp m_playbackTime;
		tuint64 m_currentId;		
		QList<tuint64> m_nextIdList;
		enum PlayState m_pbState;
		
		virtual QString fileNameFromId(tuint64 id) const;
};

//-------------------------------------------------------------------------------------------
} // namespace orcus
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------