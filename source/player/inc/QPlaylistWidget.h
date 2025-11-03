//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_QPLAYLISTWIDGET_H
#define __OMEGA_PLAYER_QPLAYLISTWIDGET_H
//-------------------------------------------------------------------------------------------

#include "player/inc/QPLItemBase.h"
#include "player/inc/QPLProgress.h"

#include "track/db/inc/TrackFileDependencies.h"

#include <QScrollBar>
#include <QScrollArea>
#include <QMouseEvent>
#include <QUrl>
#include <QDrag>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class Player;

//-------------------------------------------------------------------------------------------

class PlayerController;

//-------------------------------------------------------------------------------------------

class QFixedHorizontalScrollArea : public QScrollArea
{
    public:
        Q_OBJECT

    public:
        typedef enum
        {
            e_dragUp,
            e_dragDown,
            e_dragNone
        } DragDirection;

    public:
        QFixedHorizontalScrollArea(QWidget *parent = 0);
        virtual ~QFixedHorizontalScrollArea();

    protected:

        QTimer *m_dragTimer;
        DragDirection m_direction;
        tfloat32 m_scrollRatio;

        virtual bool eventFilter(QObject *watched,QEvent *event);
        virtual void resizeEvent(QResizeEvent *e);

    protected slots:

        void onDragTimer();
};

//-------------------------------------------------------------------------------------------

class QPlaylistWidget : public QWidget
{
    public:
        Q_OBJECT

    public:

        typedef enum
        {
            e_dragInternal = 0,
            e_dragFiles,
            e_dragUnknown
        } DragType;

        friend class QPLItemBase;
        friend class QPLDragOverlay;
        friend class QPLProgress;
        friend class PlayerController;

    public:
        QPlaylistWidget(QAbstractScrollArea *scrollArea,QWidget *parent = 0);
        virtual ~QPlaylistWidget();

        virtual void addFile(const QString& name,QPLItemBase *prevItem=0);
        virtual void addFiles(const QStringList& name,QPLItemBase *prevItem=0,bool sortFlag=true);
        virtual void addDirectory(const QString& name,bool recursive,QPLItemBase *prevItem=0);
        virtual void savePlaylist(const QString& fileName,bool selectFlag);

        void setFont(const QFont& f,int size);

        virtual QPLItemBase *currentPlayItem();
        virtual void setCurrentPlayItem(QPLItemBase *item);
        virtual QPLItemBase *previousPlayItem();
        virtual QPLItemBase *previousPlayItem(QPLItemBase *cItem);
        virtual QPLItemBase *nextPlayItem();
        virtual QPLItemBase *nextPlayItem(QPLItemBase *cItem);
        virtual void setPlayItemToStart();

        virtual QPLItemBase *firstPlayItem();
        virtual QPLItemBase *lastItem();

        virtual QPLItemBase *currentQueuePlayItem();
        virtual void setCurrentQueuePlayItem(QPLItemBase *item);

        virtual QPLItemBase *nextShufflePlayItem();
        void buildShuffleList();
        void buildShuffleList(QPLItemBase *excludeItem);

        QImage *speakerImage();
        QImage *pauseImage();

        bool isPlay() const;
        bool isPaused() const;
        void setPlayer(Player *p);

        void doCut();
        void doCopy();
        void doDelete();
        void doPaste();
        void doSelectAll();

        void loadCurrentPlaylist();
        void saveCurrentPlaylist();

        int noPlayItems() const;

        QString getTrackLength(const common::TimeStamp& ts) const;

        void clone(QPlaylistWidget *cPlaylist);

    protected:

        Player *m_playerDialog;

        QList<QPLItemBase *> m_playRootList;
        QList<QPLItemBase *> m_viewList;

        QSet<QPLItemBase *> m_shuffleList;

        QAbstractScrollArea *m_scrollArea;

        // painter pens and fonts data members
        QFont *m_darkFont;
        QFont *m_lightFont;
        QFont *m_mediumFont;
        QFontMetricsF *m_darkFontMetric;
        QFontMetricsF *m_lightFontMetric;
        QFontMetricsF *m_mediumFontMetric;
        qreal m_timeColumnWidth;
        qreal m_trackColumnWidth;
        QImage *m_treeExpandImage;
        QImage *m_treeExpandActiveImage;
        QImage *m_treeCollapseImage;
        QImage *m_treeCollapseActiveImage;
        QImage *m_noTrackImage;
        QImage *m_speakerImage;
        QImage *m_pauseImage;
        QMap<QPair<tint,tint>,QPair<QImage *,QImage *> > m_noTrackImageMap;
        QPen m_blackPen;
        QPen m_grayPen;
        QPen m_blackDisablePen;
        QPen m_grayDisablePen;
        QImage *m_dropImage;

        tint m_treeNodeActiveItem;

        tint m_controlState;
        QPLItemBase *m_lastSelectItem;
        tint m_wheelStep;

        QPoint m_startDragPosition;
        tint m_currentDragPosition;

        QPLItemBase *m_currentPlayItem;
        QPair<QUrl,int> m_currentPlayNameDrag;
        QPLItemBase *m_currentPlayQueueItem;

        QPLProgress *m_progressControl;

        tint m_dirFileCurrent;
        tint m_dirFileTotal;

        bool m_addNoUpdateFlag;

        bool m_resizeEventFlag;

        // painter pens and fonts
        void initPainterPens();
        void freePainter();
        QFont& getDarkFont();
        QFont& getLightFont();
        QFont& getMediumFont();
        QFontMetricsF& getDarkFontMetric();
        const QFontMetricsF& getDarkFontMetric() const;
        QFontMetricsF& getLightFontMetric();
        const QFontMetricsF& getLightFontMetric() const;
        QFontMetricsF& getMediumFontMetric();
        const QFontMetricsF& getMediumFontMetric() const;
        qreal getTimeColumnWidth();
        qreal getTrackColumnWidth();
        QImage *treeExpandImage();
        QImage *treeExpandActiveImage();
        QImage *treeCollapseImage();
        QImage *treeCollapseActiveImage();
        QImage *noTrackImage();
        QImage *noTrackImage(tint w,tint h,bool greyFlag);
        QPen& blackPen();
        QPen& grayPen();
        QPen& blackDisablePen();
        QPen& grayDisablePen();
        tint paintWidth();

        QImage *convertImageToGreyscale(QImage *inImg);

        bool isTreeNodeActiveItem(QPLItemBase *item);

        void buildViewList();
        void buildViewListAppend(QPLItemBase *item,tint& idx);
        void updateViewList(tint pos);
        void updateViewListAppend(QPLItemBase *item,tint& idx);
        void updateViewListRemove(QPLItemBase *item,tint idx);

        virtual void paintEvent(QPaintEvent *e);
        virtual void resizeEvent(QResizeEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual void mouseDoubleClickEvent(QMouseEvent *e);
        virtual void dragEnterEvent(QDragEnterEvent *e);
        virtual void dragLeaveEvent(QDragLeaveEvent *e);
        virtual void dragMoveEvent(QDragMoveEvent *e);
        virtual void dropEvent(QDropEvent *e);
        virtual void contextMenuEvent(QContextMenuEvent *e);

        virtual void addDirectoryR(const QString& name,bool recursive,QVector<track::info::InfoSPtr>& trackList);
        virtual void countDirectoryR(const QString& name,bool recursive,int& count,track::db::TrackFileDependencies& dependency);

        void resizePlaylist();

        int viewIndexFromPosition(int yPos,bool lastFlag=true) const;

        void selectClear();
        void selectSingle(QPLItemBase *item,bool clearFlag);
        void selectShift(QPLItemBase *aItem,QPLItemBase *bItem);
        void selectShiftRecursive(QPLItemBase *item,QPLItemBase *aItem,QPLItemBase *bItem,tint& state);

        QString albumKey(const track::info::InfoSPtr item,bool cFlag = true) const;
        QString albumKey(QPLItemBase *item,bool cFlag = true) const;

        void buildTrackList(QVector<track::info::InfoSPtr>& items,QList<QPLItemBase *>& list);
        void buildTrackList(QVector<QPair<track::info::InfoSPtr,int> >& items,QList<QPLItemBase *>& list);
        void buildInfoList(QVector<QPLItemBase *>& list,QVector<QPair<track::info::InfoSPtr,int> >& infoList);

        void addTracks(QVector<track::info::InfoSPtr>& trackItems,QPLItemBase *prevItem = 0,bool sortFlag = true);
        void addTracks(QVector<QPair<track::info::InfoSPtr,int> >& trackItems,QPLItemBase *prevItem);

        void removeTracks(QVector<QPLItemBase *>& trackItems);
        void removeTrackRecursive(QPLItemBase *dItem,QSet<QPLItemBase *>& delItems);
        void cleanUpPlaylist();

        void getPlaylistItems(QPLItemBase *item,QVector<track::info::InfoSPtr>& pList,bool selectFlag);

        QPLItemBase *getSelectedItems(QVector<QPLItemBase *>& tList,bool allFlag = false);

        void doDrag(const QPoint& mPos);

        bool isSplitRequired(QPLItemBase *splitAfterItem,QList<QPLItemBase *>& insertTrackList);
        void splitAlbumAndInsert(QPLItemBase *splitAfterItem,QList<QPLItemBase *>& insertTrackList);
        void splitUpdateCurrent(QPLItemBase *item);

        void clearFonts();

        bool isDragPreItem(QPLItemBase *item) const;
        bool isDragPostItem(QPLItemBase *item) const;
        void setDragIcon(QDrag *drag,QVector<QPLItemBase *>& tList);

        void initCurrentPlay();
        void doCopyCut(bool cutFlag);

        int getURLChild(const QUrl& url,QString& fileName) const;

        bool loadPlaylistFromDB(int playlistID,bool appendFlag,QString& name);
        int savePlaylistToDB(int playlistID,const QString& name,bool selectFlag);

        int noPlayItemsR(const QPLItemBase *pItem) const;

        void activeProgress();
        void deactivateProgress();

        void urlListToArray(const QList<QUrl>& urlList,QByteArray& arr);
        void urlListFromArray(const QByteArray& arr,QList<QUrl>& urlList);

        QPLItemBase *itemFromPosition(const QPoint& p,int& firstNodeI);

        void selectionChanged();
        bool isSelected();
        bool isPaste();

        void paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage);

        QScrollBar *getVerticalScrollBar();

        void applyAndEmitUpdate();

        void addTracksShuffleList(QList<QPLItemBase *>& trackItems);
        void addTracksShuffleList(QPLItemBase *item);
        void removeTracksShuffleList(QVector<QPLItemBase *>& trackItems);
        void removeTracksShuffleList(QPLItemBase *item);

#if defined(OMEGA_MAC_STORE)
        virtual void dropEventProcessBookmarks(const QList<QUrl>& urlList);
#endif

    signals:

        void onUpdate();
        void onPlay();

    public slots:

        void onCut();
        void onCopy();
        void onDelete();
        void onPaste();
        void onSelectAll();
        void onClipboardChange();
};


//-------------------------------------------------------------------------------------------

inline void QPlaylistWidget::setPlayer(Player *p)
{
    m_playerDialog = p;
}

//-------------------------------------------------------------------------------------------

inline QFont& QPlaylistWidget::getDarkFont()
{
    return *m_darkFont;
}

//-------------------------------------------------------------------------------------------

inline QFont& QPlaylistWidget::getLightFont()
{
    return *m_lightFont;
}

//-------------------------------------------------------------------------------------------

inline QFont& QPlaylistWidget::getMediumFont()
{
    return *m_mediumFont;
}

//-------------------------------------------------------------------------------------------

inline QFontMetricsF& QPlaylistWidget::getDarkFontMetric()
{
    return *m_darkFontMetric;
}

//-------------------------------------------------------------------------------------------

inline QFontMetricsF& QPlaylistWidget::getLightFontMetric()
{
    return *m_lightFontMetric;
}

//-------------------------------------------------------------------------------------------

inline QFontMetricsF& QPlaylistWidget::getMediumFontMetric()
{
    return *m_mediumFontMetric;
}

//-------------------------------------------------------------------------------------------

inline const QFontMetricsF& QPlaylistWidget::getDarkFontMetric() const
{
    return *m_darkFontMetric;
}

//-------------------------------------------------------------------------------------------

inline const QFontMetricsF& QPlaylistWidget::getLightFontMetric() const
{
    return *m_lightFontMetric;
}

//-------------------------------------------------------------------------------------------

inline const QFontMetricsF& QPlaylistWidget::getMediumFontMetric() const
{
    return *m_mediumFontMetric;
}

//-------------------------------------------------------------------------------------------

inline qreal QPlaylistWidget::getTimeColumnWidth()
{
    return m_timeColumnWidth;
}

//-------------------------------------------------------------------------------------------

inline qreal QPlaylistWidget::getTrackColumnWidth()
{
    return m_trackColumnWidth;
}

//-------------------------------------------------------------------------------------------

inline QImage *QPlaylistWidget::treeExpandImage()
{
    return m_treeExpandImage;
}

//-------------------------------------------------------------------------------------------

inline QImage *QPlaylistWidget::treeCollapseImage()
{
    return m_treeCollapseImage;
}

//-------------------------------------------------------------------------------------------

inline QImage *QPlaylistWidget::treeExpandActiveImage()
{
    return m_treeExpandActiveImage;
}

//-------------------------------------------------------------------------------------------

inline QImage *QPlaylistWidget::treeCollapseActiveImage()
{
    return m_treeCollapseActiveImage;
}

//-------------------------------------------------------------------------------------------

inline QImage *QPlaylistWidget::noTrackImage()
{
    return m_noTrackImage;
}

//-------------------------------------------------------------------------------------------

inline QImage *QPlaylistWidget::speakerImage()
{
    return m_speakerImage;
}

//-------------------------------------------------------------------------------------------

inline QImage *QPlaylistWidget::pauseImage()
{
    return m_pauseImage;
}

//-------------------------------------------------------------------------------------------

inline QPen& QPlaylistWidget::blackPen()
{
    return m_blackPen;
}

//-------------------------------------------------------------------------------------------

inline QPen& QPlaylistWidget::grayPen()
{
    return m_grayPen;
}

//-------------------------------------------------------------------------------------------

inline QPen& QPlaylistWidget::blackDisablePen()
{
    return m_blackDisablePen;
}

//-------------------------------------------------------------------------------------------

inline QPen& QPlaylistWidget::grayDisablePen()
{
    return m_grayDisablePen;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
