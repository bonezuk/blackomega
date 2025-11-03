//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_QPLITEMBASE_H
#define __OMEGA_PLAYER_QPLITEMBASE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include <QPair>
#include <QMap>
#include <QVector>
#include <QPainter>
#include <QFont>
#include <QFontMetricsF>
#include <QImage>
#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QByteArray>
#include <QBuffer>

#include "track/info/inc/Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class QPlaylistWidget;

//-------------------------------------------------------------------------------------------

class QPLItemBase : public QObject
{
    public:
        Q_OBJECT

    public:

        typedef enum
        {
            e_Album = 0,
            e_AlbumTrack,
            e_Single
        } PlaylistItemType;

    public:
        QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,QSharedPointer<track::info::Info>& info);
        QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,tint childIdx,QSharedPointer<track::info::Info>& info);
        QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,QSharedPointer<track::info::Info>& info,tint subTIndex);
        QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,tint childIdx,QSharedPointer<track::info::Info>& info,tint subTIndex);
        virtual ~QPLItemBase();

        virtual PlaylistItemType type() const = 0;

        virtual bool isVisible() const;
        virtual bool isExpanded() const;
        void setExpanded(bool flag);

        QSharedPointer<track::info::Info>& info();

        QPLItemBase *parent();
        const QPLItemBase *parent() const;

        bool isSubTrack() const;
        tint subTrackIndex() const;

        bool isChildren() const;
        tint noChildren() const;
        QPLItemBase *child(tint idx);
        const QPLItemBase *child(tint idx) const;
        void addChild(QPLItemBase *item);
        void insertChild(int idx,QPLItemBase *item);
        void delChild(QPLItemBase *item);
        void delChildren();
        tint childIndexOf(QPLItemBase *cItem);

        QPLItemBase *prevSibling();
        QPLItemBase *nextSibling();

        const tint& viewPosition() const;
        void setViewPosition(tint pos);
        const tint& viewHeight() const;
        void setViewHeight(tint h);

        virtual tint height() const = 0;
        tint width() const;
        tint depth() const;

        virtual void paint(tint yTop,QPainter *painter) = 0;

        bool isInsideTreeNode(const QPoint& p);

        bool isSelected() const;
        virtual void setSelected(bool v);

        QImage *getImageIcon();

        virtual common::TimeStamp lengthOfTrack();

    protected:

        QPlaylistWidget *m_playListWidget;
        QPLItemBase *m_parentItem;
        QSharedPointer<track::info::Info> m_info;
        QVector<QPLItemBase *> m_children;

        bool m_expandFlag;
        tint m_viewPosition;
        tint m_viewHeight;
        tint m_height;
        bool m_selectFlag;

        QMap<QPair<tint,tint>,QPair<QImage *,QImage *> > m_imageMap;

        QRect m_treeNodeRect;

        tint m_subTrackIndex;

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
        QImage& treeExpandImage();
        QImage& treeCollapseImage();
        QImage& noTrackImage();
        QPen& blackPen();
        QPen& grayPen();
        QPen& blackDisablePen();
        QPen& grayDisablePen();

        bool isDisabled();

        virtual qreal paintOffset(const QPointF& pos,QPainter *painter);

        virtual QImage *getImage(tint w,tint h,bool greyFlag);
        virtual QString displayString(const QString& org,qreal width,QFontMetricsF& fm);
        virtual QString getTrackLength();
        virtual QString getTrackLength(const common::TimeStamp& ts);

        virtual void paintBorder(const QRectF& rect,QPainter *painter);

        bool isRetina() const;
        void paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage) const;
        void paintRetinaImage(QPainter *painter,QPointF pos,QImage *pImage) const;
};

//-------------------------------------------------------------------------------------------

inline const tint& QPLItemBase::viewPosition() const
{
    return m_viewPosition;
}

//-------------------------------------------------------------------------------------------

inline void QPLItemBase::setViewPosition(tint pos)
{
    m_viewPosition = pos;
}

//-------------------------------------------------------------------------------------------

inline const tint& QPLItemBase::viewHeight() const
{
    return m_viewHeight;
}

//-------------------------------------------------------------------------------------------

inline void QPLItemBase::setViewHeight(tint h)
{
    m_viewHeight = h;
}

//-------------------------------------------------------------------------------------------

inline QPLItemBase *QPLItemBase::parent()
{
    return m_parentItem;
}

//-------------------------------------------------------------------------------------------

inline const QPLItemBase *QPLItemBase::parent() const
{
    return m_parentItem;
}

//-------------------------------------------------------------------------------------------

inline bool QPLItemBase::isChildren() const
{
    return (m_children.size()>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

inline tint QPLItemBase::noChildren() const
{
    return m_children.size();
}

//-------------------------------------------------------------------------------------------

inline QPLItemBase *QPLItemBase::child(tint idx)
{
    return m_children[idx];
}

//-------------------------------------------------------------------------------------------

inline const QPLItemBase *QPLItemBase::child(tint idx) const
{
    return m_children.at(idx);
}

//-------------------------------------------------------------------------------------------

inline bool QPLItemBase::isSelected() const
{
    return m_selectFlag;
}

//-------------------------------------------------------------------------------------------

inline bool QPLItemBase::isSubTrack() const
{
    if(m_subTrackIndex>=0 && m_subTrackIndex<m_info->noChildren())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

inline tint QPLItemBase::subTrackIndex() const
{
    return m_subTrackIndex;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
