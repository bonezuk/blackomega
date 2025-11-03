#include "player/inc/QPLItemAlbum.h"
#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPLItemAlbum::QPLItemAlbum(QPlaylistWidget *playListW,QSharedPointer<track::info::Info>& info) : QPLItemBase(playListW,0,info)
{
    m_height = static_cast<tint>((getDarkFontMetric().height() * 2.0) + 4.0);
    m_expandFlag = true;
}

//-------------------------------------------------------------------------------------------

QPLItemAlbum::~QPLItemAlbum()
{}

//-------------------------------------------------------------------------------------------

QPLItemBase::PlaylistItemType QPLItemAlbum::type() const
{
    return e_Album;
}

//-------------------------------------------------------------------------------------------

tint QPLItemAlbum::height() const
{
    return static_cast<tint>((getDarkFontMetric().height() * 2.0) + 4.0);
}

//-------------------------------------------------------------------------------------------

void QPLItemAlbum::paint(tint yTop,QPainter *painter)
{
    qreal wOffset,wPic,wTitle;
    QFont& nFont = getDarkFont();
    QFont& lFont = getLightFont();
    QFontMetricsF& nFontMetric = getDarkFontMetric();
    QFontMetricsF& lFontMetric = getLightFontMetric();
    QRectF boundingRect(0.0,static_cast<qreal>(yTop),static_cast<qreal>(width()-1),static_cast<qreal>(height()));
    QPen lineP(QColor(132,172,221));
    bool rFlag = isRetina();
    
    if(!m_selectFlag)
    {
        lineP = grayPen();
    }
    wPic    = getTrackColumnWidth();
    wTitle  = boundingRect.width() - wPic;
    
    painter->fillRect(boundingRect,(m_viewPosition & 1) ? QColor(240,240,240) : QColor(255,255,255));
    paintBorder(boundingRect,painter);
    
    QPointF picLinePtA(wPic,boundingRect.top());
    QPointF picLinePtB(wPic,boundingRect.bottom());
    painter->setPen(lineP);
    painter->drawLine(picLinePtA,picLinePtB);

    tint imgWidth = static_cast<tint>(wPic) - ((rFlag) ? 0 : 1);
    tint imgHeight = static_cast<tint>(boundingRect.height());

    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        imgWidth *= 2;
        imgHeight *= 2;
    }

    QImage *tImage = getImage(imgWidth,imgHeight,isDisabled());
    if(tImage!=0)
    {
        QRect imgRect(1,yTop+1,imgWidth,imgHeight);
        QPixmap tPix = QPixmap::fromImage(*tImage);
        if(rFlag)
        {
            QRect retinaRect(0,yTop,imgWidth / 2,imgHeight / 2);
            tPix.setDevicePixelRatio(2.0);
            imgRect = retinaRect;
        }
        painter->drawPixmap(imgRect,tPix);
    }
    else
    {
        wTitle += wPic;
        wPic = 0.0;
    }
    
    QPointF offsetPt(wPic+3.0,(boundingRect.height() / 2.0) - nFontMetric.height() + yTop + 4.0);
    wOffset = paintOffset(offsetPt,painter);
    
    QRectF albumTRect(wOffset + wPic + 8.0,
                     (boundingRect.height() / 2.0) - (nFontMetric.height() + 1.0) + yTop,
                     wTitle - (wOffset + 11.0),
                     nFontMetric.height());
    QString albumStr = displayString(m_info->album(),albumTRect.width(),nFontMetric);
    painter->setFont(nFont);
    if(!isDisabled())
    {
        painter->setPen(blackPen());
    }
    else
    {
        painter->setPen(blackDisablePen());
    }
    painter->drawText(albumTRect,Qt::AlignLeft | Qt::TextSingleLine,albumStr,&albumTRect);

    QString timeStr = getTime();

    QRectF timeTRect((boundingRect.width() - lFontMetric.horizontalAdvance(timeStr)) - 4.0,
                     (boundingRect.height() / 2.0) + 1.0 + yTop,
                     lFontMetric.horizontalAdvance(timeStr),
                     lFontMetric.height());
    
    painter->setFont(lFont);
    if(!isDisabled())
    {
        painter->setPen(grayPen());
    }
    else
    {
        painter->setPen(grayDisablePen());
    }
    painter->drawText(timeTRect,Qt::AlignRight | Qt::TextSingleLine,timeStr,&timeTRect);


    QRectF artistTRect(wOffset + wPic + 8.0,
                       (boundingRect.height() / 2.0) + 1.0 + yTop,
                       boundingRect.width() - (wOffset + wPic + 16.0 + timeTRect.width()),
                       lFontMetric.height());
    QString artistStr = displayString(getArtist(),artistTRect.width(),lFontMetric);
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



    if(m_selectFlag)
    {
        qreal yPos,alpha = 32.0,alphaInc;
        bool dFlag = isDisabled();

        alphaInc = 64.0 / (boundingRect.bottom() - boundingRect.top());
        for(yPos=boundingRect.top()+1.0;yPos<boundingRect.bottom();yPos+=1.0,alpha+=alphaInc)
        {
            QPen cPen;
            
            if(!dFlag)
            {
                cPen.setColor(QColor(0,113,252,static_cast<int>(alpha)));
            }
            else
            {
                cPen.setColor(QColor(160,160,160,static_cast<int>(alpha)));
            }
            QPointF sA(wPic,yPos),sB(boundingRect.right(),yPos);
            painter->setPen(cPen);
            painter->drawLine(sA,sB);
        }
    }
}

//-------------------------------------------------------------------------------------------

QString QPLItemAlbum::getArtist()
{
    int i,j;
    QMap<QString,tint> aCMap;
    QMap<QString,tint>::iterator ppI;
    QString name;

    for(i=0;i<noChildren();i++)
    {
        track::info::InfoSPtr pInfo(child(i)->info());

        for(j=0;j<3;j++)
        {
            QString a;

            switch(j)
            {
                case 0:
                    a = pInfo->artist();
                    break;
                case 1:
                    a = pInfo->originalArtist();
                    break;
                case 2:
                    a = pInfo->composer();
                    break;
            }
            if(!a.isEmpty())
            {
                ppI = aCMap.find(a);
                if(ppI!=aCMap.end())
                {
                    int& c = ppI.value();
                    c++;
                }
                else
                {
                    aCMap.insert(a,1);
                }
            }
        }
    }

    if(aCMap.size()>0)
    {
        int tCount = 0,cMax = 0;
        tfloat32 ratio;

        for(ppI=aCMap.begin();ppI!=aCMap.end();ppI++)
        {
            int cValue = ppI.value();
            if(cMax < cValue)
            {
                name = ppI.key();
                cMax = cValue;
            }
            tCount += cValue;
        }
        ratio = static_cast<tfloat32>(cMax) / static_cast<tfloat32>(tCount);
        if(ratio < 0.4f)
        {
            name = "Various Artists";
        }
    }
    else
    {
        name = info()->artist();
    }
    return name;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp QPLItemAlbum::lengthOfTrack()
{
    common::TimeStamp aTime;

    for(int i=0;i<noChildren();i++)
    {
        aTime += child(i)->lengthOfTrack();
    }
    return aTime;
}

//-------------------------------------------------------------------------------------------

QString QPLItemAlbum::getTime()
{
    common::TimeStamp aTime;
    QString tStr;

    aTime = lengthOfTrack();
    int noMins = aTime.secondsTotal() / 60;
    if((aTime.secondsTotal() % 60)>=30)
    {
        noMins++;
    }
    tStr = QString::number(noChildren()) + " Songs, " + QString::number(noMins) + " Mins.";
    return tStr;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
