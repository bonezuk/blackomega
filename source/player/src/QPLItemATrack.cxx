#include "player/inc/QPLItemATrack.h"
#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPLItemATrack::QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,QSharedPointer<track::info::Info>& info) : QPLItemBase(playListW,parent,info)
{
    m_height = static_cast<tint>(getMediumFontMetric().height() + 2.0);
}

//-------------------------------------------------------------------------------------------

QPLItemATrack::QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,tint childIdx,QSharedPointer<track::info::Info>& info) : QPLItemBase(playListW,parent,childIdx,info)
{
    m_height = static_cast<tint>(getMediumFontMetric().height() + 2.0);
}

//-------------------------------------------------------------------------------------------

QPLItemATrack::QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,QSharedPointer<track::info::Info>& info,tint subTIndex) : QPLItemBase(playListW,parent,info,subTIndex)
{
    m_height = static_cast<tint>(getMediumFontMetric().height() + 2.0);
}

//-------------------------------------------------------------------------------------------

QPLItemATrack::QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,tint childIdx,QSharedPointer<track::info::Info>& info,tint subTIndex) : QPLItemBase(playListW,parent,childIdx,info,subTIndex)
{
    m_height = static_cast<tint>(getMediumFontMetric().height() + 2.0);
}

//-------------------------------------------------------------------------------------------

QPLItemATrack::~QPLItemATrack()
{}

//-------------------------------------------------------------------------------------------

QPLItemBase::PlaylistItemType QPLItemATrack::type() const
{
    return e_AlbumTrack;
}

//-------------------------------------------------------------------------------------------

tint QPLItemATrack::height() const
{
    return static_cast<tint>(getMediumFontMetric().height() + 2.0);
}

//-------------------------------------------------------------------------------------------

void QPLItemATrack::paint(tint yTop,QPainter *painter)
{
    qreal wOffset,wTrack,wTitle,wTime;
    QFont& nFont = getMediumFont();
    QFontMetricsF& nFontMetric = getMediumFontMetric();
    QRectF boundingRect(0.0,static_cast<qreal>(yTop),static_cast<qreal>(width()-1),static_cast<qreal>(height()));
    QPen lineP(QColor(132,172,221));
    
    if(!m_selectFlag)
    {
        lineP = grayPen();
    }
    wTrack  = getTrackColumnWidth();
    wTime   = getTimeColumnWidth();
    wTitle  = boundingRect.width() - (wTrack + wTime);
    
    painter->fillRect(boundingRect,(m_viewPosition & 1) ? QColor(240,240,240) : QColor(255,255,255));
    paintBorder(boundingRect,painter);
    
    QPointF trackPtA(wTrack,boundingRect.top());
    QPointF trackPtB(wTrack,boundingRect.bottom());
    painter->setPen(lineP);
    painter->drawLine(trackPtA,trackPtB);
    
    QPointF timePtA(boundingRect.width()-wTime,boundingRect.top());
    QPointF timePtB(boundingRect.width()-wTime,boundingRect.bottom());
    painter->drawLine(timePtA,timePtB);
    
    tint trackNo = m_parentItem->childIndexOf(this) + 1;
    QRectF trackTRect(2.0,
                      (boundingRect.height() / 2.0) - (nFontMetric.height() / 2.0) + yTop,
                      wTrack - 4.0,
                      nFontMetric.height());
    painter->setFont(nFont);
    if(!isDisabled())
    {
        painter->setPen(blackPen());
    }
    else
    {
        painter->setPen(blackDisablePen());
    }
    painter->drawText(trackTRect,Qt::AlignRight | Qt::TextSingleLine,QString::number(trackNo),&trackTRect);                
    
    QPointF offsetPt(wTrack+3.0,(boundingRect.height() / 2.0) - (nFontMetric.height() / 2.0) + yTop + 2.0);
    wOffset = paintOffset(offsetPt,painter);

    QRectF titleTRect(wOffset + wTrack + 8.0,
                      (boundingRect.height() / 2.0) - (nFontMetric.height() / 2.0) + yTop,
                      wTitle - (wOffset + 11.0),
                      nFontMetric.height());
    QString titleStr;
    if(isSubTrack())
    {
        titleStr = displayString(m_info->child(subTrackIndex()).name(),titleTRect.width(),nFontMetric);
    }
    else
    {
        titleStr = displayString(m_info->title(),titleTRect.width(),nFontMetric);
    }
    painter->drawText(titleTRect,Qt::AlignLeft | Qt::TextSingleLine,titleStr,&titleTRect);
    
    QRectF timeTRect((boundingRect.width() - wTime) + 2.0,
                     (boundingRect.height() / 2.0) - (nFontMetric.height() / 2.0) + yTop,
                     wTime - 4.0,
                     nFontMetric.height());
    QString timeStr = getTrackLength();
    painter->drawText(timeTRect,Qt::AlignRight | Qt::TextSingleLine,timeStr,&timeTRect);
    
    if(this==m_playListWidget->currentPlayItem() && (m_playListWidget->isPlay() || m_playListWidget->isPaused()))
    {
        QImage *sImg = (m_playListWidget->isPlay()) ? m_playListWidget->speakerImage() : m_playListWidget->pauseImage();

        if(isRetina())
        {
            QPointF sP(timePtA.x() + 2.0,(boundingRect.height() / 2.0) + 1.0 + yTop - ((double)(sImg->height()) / 4.0));
#if defined(OMEGA_MACOSX)
            QPointF msP(2.0,0.0);
            sP -= msP;
#endif
            paintRetinaImage(painter,sP,sImg);
        }
        else
        {
            QPointF sP(timePtA.x() + 2.0,(boundingRect.height() / 2.0) + 1.0 + yTop - ((double)(sImg->height()) / 2.0));
#if defined(OMEGA_MACOSX)
            QPointF msP(2.0,0.0);
            sP -= msP;
#endif
            painter->drawImage(sP,*sImg);
        }
    }

    if(m_selectFlag || (this==m_playListWidget->currentPlayItem() && (m_playListWidget->isPlay() || m_playListWidget->isPaused())))
    {
        qreal yPos,alphaInc,alpha = 32.0;
        tint rC,gC,bC;

        if(!isDisabled())
        {
            if(m_selectFlag)
            {
                rC = 0;
                gC = 113;
                bC = 252;
            }
            else
            {
                rC = 0;
                gC = 255;
                bC = 119;
            }
        }
        else
        {
            rC = 160;
            gC = 160;
            bC = 160;
        }

        alphaInc = 64.0 / (boundingRect.bottom() - boundingRect.top());
        for(yPos=boundingRect.top()+1.0;yPos<boundingRect.bottom();yPos+=1.0,alpha+=alphaInc)
        {
            QPen cPen(QColor(rC,gC,bC,static_cast<int>(alpha)));
            QPointF sA(boundingRect.left(),yPos),sB(boundingRect.right(),yPos);
            painter->setPen(cPen);
            painter->drawLine(sA,sB);
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
