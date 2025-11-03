#include "playerapp/playercommon/inc/PlaybackStateController.h"
#include "playerapp/playercommon/inc/PlayListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlaybackStateController::PlaybackStateController(QObject *parent) : QObject(parent),
    m_pAudioInterface(),
    m_pModel(0),
    m_playbackTime(),
    m_currentId(0),
    m_nextIdList(),
    m_pbState(Pause)
{
    m_pModel = dynamic_cast<PlayListModel *>(parent);
    Q_ASSERT(m_pModel != 0);
}

//-------------------------------------------------------------------------------------------

PlaybackStateController::PlaybackStateController(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent) : QObject(parent),
    m_pAudioInterface(pAudioInterface),
    m_pModel(0),
    m_playbackTime(),
    m_currentId(0),
    m_nextIdList(),
    m_pbState(Pause)
{
    m_pModel = dynamic_cast<PlayListModel *>(parent);
    Q_ASSERT(m_pModel != 0);
}

//-------------------------------------------------------------------------------------------

PlaybackStateController::~PlaybackStateController()
{}

//-------------------------------------------------------------------------------------------

quint32 PlaybackStateController::getTimeInSeconds() const
{
    return m_playbackTime.secondsTotal();
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::setTime(quint64 tS)
{
    tuint32 lS;

    lS = m_playbackTime.secondsTotal();
    m_playbackTime = tS;
    if(lS != m_playbackTime.secondsTotal())
    {
        Q_EMIT onTimeInSecondsChanged();
    }
    Q_EMIT onTimeChanged();
}

//-------------------------------------------------------------------------------------------

qreal PlaybackStateController::getPlaybackTime() const
{
    tfloat64 t = static_cast<tfloat64>(m_playbackTime);
    return static_cast<qreal>(t);
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::setNextItem(tuint64 itemId)
{
    m_nextIdList.append(itemId);
}

//-------------------------------------------------------------------------------------------

qint32 PlaybackStateController::getIndex() const
{
    qint32 idx = -1;

    if(m_currentId != 0)
    {
        idx = m_pModel->indexFromId(m_currentId);
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

qint32 PlaybackStateController::getState() const
{
    return static_cast<qint32>(m_pbState);
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& PlaybackStateController::getTime() const
{
    return m_playbackTime;
}

//-------------------------------------------------------------------------------------------

tuint64 PlaybackStateController::getCurrentId() const
{
    return m_currentId;
}

//-------------------------------------------------------------------------------------------

bool PlaybackStateController::getIsPlayback() const
{
    return (m_currentId != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

QString PlaybackStateController::fileNameFromId(tuint64 id) const
{
    QString fileName;

    if(id)
    {
        track::db::DBInfoSPtr pItem = m_pModel->itemFromId(id);
        if(!pItem.isNull())
        {
            fileName = pItem->getFilename();
        }
    }
    return fileName;
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::onAudioStart(const QString& fileName)
{
    int i;
    tuint64 oldId = m_currentId;
    bool emitFlag = false;

    if(!fileName.isEmpty())
    {
        if(m_nextIdList.size() > 0 && fileName == fileNameFromId(m_nextIdList.first()))
        {
            m_currentId = m_nextIdList.takeFirst();
            emitFlag = true;
        }
        else if(m_currentId > 0 && fileName == fileNameFromId(m_currentId))
        {
            return;
        }
        else
        {
            for(i = 0; i < m_nextIdList.size(); i++)
            {
                if(fileName == fileNameFromId(m_nextIdList.at(i)))
                {
                    break;
                }
            }
            if(i < m_nextIdList.size())
            {
                m_currentId = m_nextIdList.at(i);
                emitFlag = true;
                while(i >= 0 && !m_nextIdList.isEmpty())
                {
                    m_nextIdList.removeFirst();
                    i--;
                }
            }
        }
    }

    if(emitFlag)
    {
        if(m_pbState == Pause)
        {
            m_pbState = Play;
            Q_EMIT onStateChanged();
        }
        if(oldId == 0 && m_currentId != 0)
        {
            Q_EMIT onIsPlaybackChanged();
        }
        Q_EMIT onIndexChanged();
    }
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::onAudioStop()
{
    m_currentId = 0;
    m_nextIdList.clear();
    Q_EMIT onIsPlaybackChanged();

    m_playbackTime = 0;
    Q_EMIT onTimeChanged();
    Q_EMIT onTimeInSecondsChanged();

    if(m_pbState == Play)
    {
        m_pbState = Pause;
        Q_EMIT onStateChanged();
    }
    Q_EMIT onIndexChanged();
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::resumeOrPausePlayback()
{
    if(m_pAudioInterface != 0)
    {
        if(m_pbState == Play)
        {
            m_pAudioInterface->pause();
        }
        else
        {
            m_pAudioInterface->play();
        }
    }
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::onSeek(qreal seekTime)
{
    if(m_pAudioInterface != 0)
    {
        common::TimeStamp tS = seekTime;
        m_pAudioInterface->seek(tS);
    }
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::onAudioPlay()
{
    if(m_pbState == Pause)
    {
        m_pbState = Play;
        Q_EMIT onStateChanged();
    }
}

//-------------------------------------------------------------------------------------------

void PlaybackStateController::onAudioPause()
{
    if(m_pbState == Play)
    {
        m_pbState = Pause;
        Q_EMIT onStateChanged();
    }
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
