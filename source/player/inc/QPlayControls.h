//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_QPLAYCONTROLS_H
#define __OMEGA_PLAYER_QPLAYCONTROLS_H
//-------------------------------------------------------------------------------------------

#include "player/inc/QPlaylistWidget.h"

#include <QTimer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class QPlayControls : public QWidget
{
    public:
        Q_OBJECT

    public:
        QPlayControls(QWidget *parent = 0);
        virtual ~QPlayControls();

        void setPlaylistWidget(QPlaylistWidget *pCtrl);

        bool isPlaying() const;
        void setPlaying(bool flag);

        void setPlayback(const QString& fileName);
        void setPlayback(QPLItemBase *item);
        void setTime(const common::TimeStamp& t);

        void setVolume(tfloat32 v);

        bool timeToEndFlag() const;

        bool isShuffle() const;
        void setShuffle(bool flag);

        bool isRepeat() const;
        void setRepeat(bool flag);

    protected:

        tuint32 m_state;
        bool m_enabled;

        QImage *m_maskImageBack;
        QImage *m_maskImagePlay;
        QImage *m_maskImageForward;

        QImage **m_controlImageArray;

        QImage *m_imgBackA;
        QImage *m_imgBackB;
        QImage *m_imgBackC;

        QImage *m_imgBackDisableA;
        QImage *m_imgBackDisableB;
        QImage *m_imgBackDisableC;

        QImage **m_digitImageArray;
        QImage *m_colonImage;

        QImage *m_seekImage;
        QImage *m_volumeImage;
        QImage *m_seekPushImage;
        QImage *m_volumePushImage;
        QImage *m_seekHoverImage;
        QImage *m_volumeHoverImage;

        // The current play back time
        common::TimeStamp m_playTime;
        // The length of the current song
        common::TimeStamp m_seekMaxTime;
        // The volume level [0.0 - 1.0]
        tfloat32 m_volume;

        QPlaylistWidget *m_playListWidget;

        common::TimeStamp m_seekTime;
        tint m_seekState;
        tint m_seekXOffset;

        QImage *m_trackImage;
        QSharedPointer<track::info::Info> m_trackInfo;
        int m_trackSubIndex;

        QFont *m_darkFont;
        QFont *m_lightFont;
        QFontMetricsF *m_darkFontMetric;
        QFontMetricsF *m_lightFontMetric;
        tint m_timeDrawOffset;

        tint m_volumeState;
        tint m_volumeXOffset;
        tfloat32 m_volumeSave;

        bool m_timeToEndFlag;
        bool m_openFileFlag;

        int m_moveWindowState;
        QPoint m_moveWindowOffset;

        QVector<QImage *> m_shuffleImages;
        QVector<QImage *> m_repeatImages;

        int m_stateShuffle;
        int m_stateRepeat;

        bool m_shuffle;
        bool m_repeat;

        void loadResources();
        void freeResources();

        QImage *getControlImage(tint s,bool retinaFlag);
        void paintTimeStamp(QPainter *painter,const common::TimeStamp& t,bool retinaFlag);
        void paintSeekBar(QPainter *painter,bool retinaFlag);
        void paintVolumeBar(QPainter *painter,bool retinaFlag);
        void paintTrackInfo(QPainter *painter,bool retinaFlag);
        void paintPlaylistButtons(QPainter *painter,bool retinaFlag);

        virtual void paintEvent(QPaintEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual void leaveEvent(QEvent *e);
        virtual void mouseDoubleClickEvent(QMouseEvent *e);

        bool onMouseControl(QMouseEvent *e,bool releaseFlag);
        void onMouseLeave();

        bool mouseMoveEventSeek(QMouseEvent *e);
        bool mousePressEventSeek(QMouseEvent *e);
        bool mouseReleaseEventSeek(QMouseEvent *e);
        void mouseLeaveSeek();

        bool mouseMoveEventVolume(QMouseEvent *e);
        bool mousePressEventVolume(QMouseEvent *e);
        bool mouseReleaseEventVolume(QMouseEvent *e);
        void mouseLeaveEventVolume();

        bool onMouseMoveWindowPress(QMouseEvent *e);
        bool onMouseMoveWindowMove(QMouseEvent *e);
        bool onMouseMoveWindowRelease(QMouseEvent *e);

        void getSeekRegion(QRect& rectO,QRect& rectB);
        bool getSeekTime(const QPoint& mousePos,common::TimeStamp& seekT);
        void getVolumeRegion(QRect& rectO,QRect& rectB);
        bool getVolume(const QPoint& mousePos,tfloat32& v);

        void getTrackImage();

        QRect timeStampRect();
        void mouseDoubleClickEventTime(QMouseEvent *e);

        void paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage);

        QRect positionRepeatButton();
        QRect positionShuffleButton();

        bool loadPlaySetting(bool repeatFlag);
        void savePlaySetting(bool value,bool repeatFlag);

    protected slots:

        void doTrackUpdate();

    signals:

        void onPlay();
        void onPause();
        void onPrevious();
        void onNext();
        void onSeek(double t);
        void onVolume(float t);
        void onOpen();

        void onCanPlay(bool f);
        void onCanPrevious(bool f);
        void onCanNext(bool f);

        void onShuffle(bool on);
        void onRepeat(bool on);
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
