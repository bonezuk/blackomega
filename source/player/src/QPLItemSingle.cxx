#include "player/inc/QPLItemSingle.h"
#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPLItemSingle::QPLItemSingle(QPlaylistWidget *playListW,QSharedPointer<track::info::Info>& info) : QPLItemBase(playListW,0,info)
{
    m_height = static_cast<tint>((getDarkFontMetric().height() * 2.0) + 4.0);
}

//-------------------------------------------------------------------------------------------

QPLItemSingle::QPLItemSingle(QPlaylistWidget *playListW,QSharedPointer<track::info::Info>& info,tint subTIndex) : QPLItemBase(playListW,0,info,subTIndex)
{
    m_height = static_cast<tint>((getDarkFontMetric().height() * 2.0) + 4.0);
}

//-------------------------------------------------------------------------------------------

QPLItemSingle::~QPLItemSingle()
{}

//-------------------------------------------------------------------------------------------

QPLItemBase::PlaylistItemType QPLItemSingle::type() const
{
    return e_Single;
}

//-------------------------------------------------------------------------------------------

tint QPLItemSingle::height() const
{
    return static_cast<tint>((getDarkFontMetric().height() * 2.0) + 4.0);
}

//-------------------------------------------------------------------------------------------

void QPLItemSingle::paint(tint yTop,QPainter *painter)
{
    qreal wOffset,wPic,wTitle,wTime;
    QFont& nFont = getMediumFont();
    QFont& lFont = getLightFont();
    QFontMetricsF& nFontMetric = getMediumFontMetric();
    QFontMetricsF& lFontMetric = getLightFontMetric();
    QRectF boundingRect(0.0,static_cast<qreal>(yTop),static_cast<qreal>(width()-1),static_cast<qreal>(height()));
    QPen lineP(QColor(132,172,221));
    bool rFlag = isRetina();
    
    if(!m_selectFlag)
    {
        lineP = grayPen();
    }
    
    wPic    = getTrackColumnWidth();
    wTime   = getTimeColumnWidth();
    wTitle  = boundingRect.width() - (wPic + wTime);
    
    painter->fillRect(boundingRect,(m_viewPosition & 1) ? QColor(240,240,240) : QColor(255,255,255));
    paintBorder(boundingRect,painter);
    
    QPointF picLinePtA(wPic,boundingRect.top());
    QPointF picLinePtB(wPic,boundingRect.bottom());
    painter->setPen(lineP);
    painter->drawLine(picLinePtA,picLinePtB);
    
    QPointF timePtA(boundingRect.width()-wTime,boundingRect.top());
    QPointF timePtB(boundingRect.width()-wTime,boundingRect.bottom());
    painter->drawLine(timePtA,timePtB);
    
    tint imgWidth = static_cast<tint>(wPic) - ((rFlag) ? 0 : 1);
    tint imgHeight = static_cast<tint>(boundingRect.height());

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        imgWidth *= 2;
        imgHeight *= 2;
    }
#endif

    QImage *tImage = getImage(imgWidth,imgHeight,isDisabled());
    if(tImage!=0)
    {
        QRect imgRect(1,yTop+1,imgWidth,imgHeight);
        QPixmap tPix = QPixmap::fromImage(*tImage);
        if(rFlag)
        {
            QRect retinaRect(0,yTop,imgWidth / 2,imgHeight / 2);
#if QT_VERSION >= 0x050000
            tPix.setDevicePixelRatio(2.0);
#endif
            imgRect = retinaRect;
        }
        painter->drawPixmap(imgRect,tPix);
    }
    else
    {
        wTitle += wPic;
        wPic = 0.0;
    }
    
    QPointF offsetPt(wPic+3.0,(boundingRect.height() / 2.0) - nFontMetric.height() + yTop + 2.0);
    wOffset = paintOffset(offsetPt,painter);
    
    QRectF trackTRect(wOffset + wPic + 8.0,
                      (boundingRect.height() / 2.0) - (nFontMetric.height() + 1.0) + yTop,
                      wTitle - (wOffset + 11.0),
                      nFontMetric.height());
    QString trackStr;
    if(isSubTrack())
    {
        trackStr = displayString(m_info->child(subTrackIndex()).name(),trackTRect.width(),nFontMetric);
    }
    else
    {
        trackStr = displayString(m_info->title(),trackTRect.width(),nFontMetric);
    }
    painter->setFont(nFont);
    if(!isDisabled())
    {
        painter->setPen(blackPen());
    }
    else
    {
        painter->setPen(blackDisablePen());
    }
    painter->drawText(trackTRect,Qt::AlignLeft | Qt::TextSingleLine,trackStr,&trackTRect);
    
    QRectF artistTRect(wOffset + wPic + 8.0,
                       (boundingRect.height() / 2.0) + 1.0 + yTop,
                       wTitle - (wOffset + 11.0),
                       lFontMetric.height());
    QString artistStr = displayString(m_info->artist(),artistTRect.width(),lFontMetric);
    painter->setFont(lFont);
    if(!isDisabled())
    {
        painter->setPen(grayPen());
    }
    else
    {
        painter->setPen(grayDisablePen());
    }
    painter->drawText(artistTRect,Qt::AlignLeft | Qt::TextSingleLine,artistStr,&artistTRect);
    
    QRectF timeTRect((boundingRect.width() - wTime) + 2.0,
                     (boundingRect.height() / 2.0) - (nFontMetric.height() / 2.0) + yTop,
                     wTime - 4.0,
                     nFontMetric.height());
    QString timeStr = getTrackLength();
    painter->setFont(nFont);
    if(!isDisabled())
    {
        painter->setPen(blackPen());
    }
    else
    {
        painter->setPen(blackDisablePen());
    }
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
            QPointF sA(wPic,yPos),sB(boundingRect.right(),yPos);
            painter->setPen(cPen);
            painter->drawLine(sA,sB);
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
