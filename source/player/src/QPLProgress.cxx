#include "player/inc/QPLProgress.h"
#include "player/inc/QPlaylistWidget.h"
#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPLProgress::QPLProgress(QPlaylistWidget *parent) : common::AbstractProgressInterface(parent),
    m_parentWidget(parent),
    m_leftImage(0),
    m_rightImage(0),
    m_rightMaskImage(0),
    m_rightHoverImage(0),
    m_rightClickImage(0),
    m_waitImageList(),
    m_progressValue(0.0f),
    m_waitImageIndex(0),
    m_waitTimer(0),
    m_progressState(0),
    m_progressReferenceCount(0),
    m_cancelFlag(false)
{
    int i;
    bool retinaFlag = isRetina();

    if(!retinaFlag)
    {
        m_leftImage = new QImage(":/progress/Resources/progress/progressLeft.png");
        m_rightImage = new QImage(":/progress/Resources/progress/progressRight.png");
        m_rightHoverImage = new QImage(":/progress/Resources/progress/progressRightHover.png");
        m_rightClickImage = new QImage(":/progress/Resources/progress/progressRightClick.png");
    }
    else
    {
        m_leftImage = new QImage(":/progress/Resources/progress/progressLeft@2x.png");
        m_rightImage = new QImage(":/progress/Resources/progress/progressRight@2x.png");
        m_rightHoverImage = new QImage(":/progress/Resources/progress/progressRightHover@2x.png");
        m_rightClickImage = new QImage(":/progress/Resources/progress/progressRightClick@2x.png");
    }
    for(i=0;i<12;i++)
    {
        QString rIName = ":/progress/Resources/progress/waitCursor";
        rIName += common::BString::Int(i+1,2,true);
        rIName += (retinaFlag) ? "@2x.png" : ".png";
        QImage *wImage = new QImage(rIName);
        m_waitImageList.append(wImage);
    }

    m_rightMaskImage = new QImage(":/progress/Resources/progress/progressRightMask.png");
    m_waitTimer = new QTimer(this);
    QObject::connect(m_waitTimer,SIGNAL(timeout()),this,SLOT(onWaitTimer()));
}

//-------------------------------------------------------------------------------------------

QPLProgress::~QPLProgress()
{
    delete m_leftImage;
    delete m_rightImage;
    delete m_rightMaskImage;
    delete m_rightHoverImage;
    delete m_rightClickImage;
    while(m_waitImageList.size()>0)
    {
        QImage *wImage = m_waitImageList.takeFirst();
        delete wImage;
    }
}

//-------------------------------------------------------------------------------------------

bool QPLProgress::isActive()
{
    return (m_progressState>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool QPLProgress::isCancelled()
{
    return m_cancelFlag;
}

//-------------------------------------------------------------------------------------------

void QPLProgress::activate(bool useReference)
{
    if(!useReference || m_progressReferenceCount==0)
    {
        m_progressState = 1;
        m_progressValue = 0.0f;
        m_cancelFlag = false;
        m_waitTimer->start(180);
        m_parentWidget->update();
    }
    if(useReference)
    {
        m_progressReferenceCount++;
    }
}

//-------------------------------------------------------------------------------------------

void QPLProgress::deactivate(bool useReference)
{
    if(useReference)
    {
        m_progressReferenceCount--;
        if(m_progressReferenceCount<0)
        {
            m_progressReferenceCount = 0;
        }
    }
    if(!useReference || m_progressReferenceCount==0)
    {
        m_progressState = 0;
        m_waitTimer->stop();
        m_parentWidget->update();
    }
}

//-------------------------------------------------------------------------------------------

void QPLProgress::paint(QPainter *painter)
{
    static const int c_pEndCoords[15][2] = {
        {34,4}, {32,8}, {31,10}, {30,12}, {29,14},
        {29,14}, {28,16}, {28,16}, {28,16}, {29,14},
        {29,14}, {30,12}, {31,10}, {32,8}, {34,4}
    };

    static const int c_pLineColor[23] = {
        200, 195, 190,  82,  89,
         96, 103, 110, 117, 124,
        131, 138, 145, 152, 159,
        166, 173, 180, 187, 194,
         97,  91,  86};

    int i;
    int wL,wR,wP;
    int hA,hB,hP;
    QPlaylistWidget *plWidget = dynamic_cast<QPlaylistWidget *>(m_parentWidget);
    int yPos = plWidget->getVerticalScrollBar()->value();
    int viewHeight = plWidget->m_scrollArea->viewport()->size().height();
    QSize pSize = m_parentWidget->size();
    QColor bkColor(0,0,0,16);
    bool retinaFlag = isRetina();

    if(!m_progressState)
    {
        return;
    }

    hP = 72;

    hA = ((viewHeight - hP) / 2) + yPos;
    hB = (viewHeight - (hA + hP)) + yPos;

    wL = static_cast<int>(m_parentWidget->getTrackColumnWidth());
    wR = static_cast<int>(m_parentWidget->getTimeColumnWidth());
    wP = pSize.width() - (wL + wR);

    if(hA>=0 && hB>=0 && wP>=88)
    {
        int pBWidth = 8 + 7;
        QRect tRect(0,0,pSize.width(),hA);
        QRect bRect(0,hA+hP,pSize.width(),hB);
        QRect lRect(0,hA,wL,hP);
        QRect rRect(wL+wP,hA,wR,hP);

        painter->fillRect(tRect,bkColor);
        painter->fillRect(bRect,bkColor);
        painter->fillRect(lRect,bkColor);
        painter->fillRect(rRect,bkColor);

        QColor baseColor(0,0,0,65);

        if(retinaFlag)
        {
            paintRetinaImage(painter,QPoint(wL,hA),m_leftImage);
        }
        else
        {
            painter->drawImage(QRect(wL,hA,49,hP),*m_leftImage);
        }

        if(m_progressState==1)
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,QPoint(pSize.width() - (wR+39),hA),m_rightImage);
            }
            else
            {
                painter->drawImage(QRect(pSize.width() - (wR+39),hA,39,hP),*m_rightImage);
            }
        }
        else if(m_progressState==2)
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,QPoint(pSize.width() - (wR+39),hA),m_rightHoverImage);
            }
            else
            {
                painter->drawImage(QRect(pSize.width() - (wR+39),hA,39,hP),*m_rightHoverImage);
            }
        }
        else
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,QPoint(pSize.width() - (wR+39),hA),m_rightClickImage);
            }
            else
            {
                painter->drawImage(QRect(pSize.width() - (wR+39),hA,39,hP),*m_rightClickImage);
            }
        }

        if(wP>88)
        {
            int i;

            painter->fillRect(QRect(wL+49,hA,wP-88,25),baseColor);
            painter->fillRect(QRect(wL+49,hA+48,wP-88,24),baseColor);

            for(i=0;i<23;i++)
            {
                int cB = c_pLineColor[i];
                QColor cBaseC(cB,cB,cB,195);
                QPen cBaseP(cBaseC);
                painter->setPen(cBaseP);

                if(retinaFlag)
                {
                    QPointF pA((qreal)(wL+49) + 0.5,(qreal)(hA+i+25) + 0.5);
                    QPointF pB((qreal)(wL+48+(wP-88)) + 0.5,(qreal)(hA+i+25) + 0.5);
                    painter->drawLine(pA,pB);
                }
                else
                {
                    painter->drawLine(wL+49,hA+i+25,wL+48+(wP-88),hA+i+25);
                }
            }

            pBWidth += wP - 88;
        }

        QColor pBarColor(0,0,0,80);
        QPen pBarPen(pBarColor);
        pBarPen.setWidth(1);
        painter->setPen(pBarColor);

        int pCPos = static_cast<int>((m_progressValue * static_cast<tfloat32>(pBWidth)) / 100.0f);

        for(i=0;i<pCPos && i<8;i++)
        {
            if(retinaFlag)
            {
                QPointF pA((qreal)(wL+i+41) + 0.5,(qreal)(hA + c_pEndCoords[i][0]) + 0.5);
                QPointF pB((qreal)(wL+i+41) + 0.5,(qreal)(hA + c_pEndCoords[i][0] + c_pEndCoords[i][1]) + 0.5);
                painter->drawLine(pA,pB);
            }
            else
            {
                painter->drawLine(wL+i+41,hA + c_pEndCoords[i][0],wL+i+41,hA + c_pEndCoords[i][0] + c_pEndCoords[i][1]);
            }
        }
        if(i<pCPos)
        {
            pCPos -= 8;
            if(pCPos<(wP - 88))
            {
                if(pCPos>0)
                {
                    painter->fillRect(QRect(wL+49,hA+28,pCPos,17),pBarColor);
                }
            }
            else
            {
                painter->fillRect(QRect(wL+49,hA+28,wP-88,17),pBarColor);
                pCPos -= wP - 88;
                for(i=0;i<pCPos && i<7;i++)
                {
                    if(retinaFlag)
                    {
                        QPointF pA((qreal)(wL+49+(wP-88)+i) + 0.5,(qreal)(hA + c_pEndCoords[i+8][0]) + 0.5);
                        QPointF pB((qreal)(wL+49+(wP-88)+i) + 0.5,(qreal)(hA + c_pEndCoords[i+8][0] + c_pEndCoords[i+8][1]) + 0.5);
                        painter->drawLine(pA,pB);
                    }
                    else
                    {
                        painter->drawLine(wL+49+(wP-88)+i,hA + c_pEndCoords[i+8][0],wL+49+(wP-88)+i,hA + c_pEndCoords[i+8][0] + c_pEndCoords[i+8][1]);
                    }
                }
            }
        }

        if(retinaFlag)
        {
            paintRetinaImage(painter,QPoint(wL+4,hA+20),m_waitImageList[m_waitImageIndex]);
        }
        else
        {
            painter->drawImage(QRect(wL+4,hA+20,32,32),*(m_waitImageList[m_waitImageIndex]));
        }
    }
    else
    {
        QRect cRect(0,0,pSize.width(),pSize.height());
        painter->fillRect(cRect,bkColor);
    }
}

//-------------------------------------------------------------------------------------------

void QPLProgress::onWaitTimer()
{
    m_waitImageIndex++;
    if(m_waitImageIndex>=m_waitImageList.size())
    {
        m_waitImageIndex = 0;
    }
    m_parentWidget->repaint();
}

//-------------------------------------------------------------------------------------------

void QPLProgress::mouseMoveEvent(QMouseEvent *e)
{
    if(m_progressState>0)
    {
        bool rPaintF = false;

        if(isOnCancelButton(e))
        {
            if(m_progressState==1 && !(e->buttons() & Qt::LeftButton))
            {
                m_progressState = 2;
                rPaintF = true;
            }
        }
        else
        {
            if(m_progressState>=2)
            {
                m_progressState = 1;
                rPaintF = true;
            }
        }
        if(rPaintF)
        {
            m_parentWidget->repaint();
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPLProgress::mousePressEvent(QMouseEvent *e)
{
    if(m_progressState>0)
    {
        if(isOnCancelButton(e) && e->button()==Qt::LeftButton)
        {
            m_progressState = 3;
            m_parentWidget->repaint();
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPLProgress::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_progressState==3 && e->button()==Qt::LeftButton)
    {
        if(isOnCancelButton(e))
        {
            m_progressState = 2;
            m_cancelFlag = true;
        }
        else
        {
            m_progressState = 1;
        }
        m_parentWidget->repaint();
    }
}

//-------------------------------------------------------------------------------------------

bool QPLProgress::isOnCancelButton(QMouseEvent *e)
{
    int wR,hA,hP;
    QSize pSize = m_parentWidget->size();

    hP = 72;
    hA = (pSize.height() - hP) / 2;

    wR = static_cast<int>(m_parentWidget->getTimeColumnWidth());

    QRect mRect(pSize.width() - (wR+39),hA,39,hP);
    if(mRect.contains(e->pos()))
    {
        int xPos = e->pos().x() - mRect.left();
        int yPos = e->pos().y() - mRect.top();
        QRgb mP = m_rightMaskImage->pixel(xPos,yPos);
        if(qRed(mP)<100)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

void QPLProgress::setProgress(tfloat32 v)
{
    if(v<0.0f)
    {
        v = 0.0f;
    }
    else if(v>100.0f)
    {
        v = 100.0f;
    }
    if(static_cast<int>(v)!=static_cast<int>(m_progressValue))
    {
        m_progressValue = v;
        m_parentWidget->repaint();
    }
    else
    {
        m_progressValue = v;
    }
}

//-------------------------------------------------------------------------------------------

tfloat32 QPLProgress::getProgress()
{
    if(m_progressState>0)
    {
        m_parentWidget->repaint();
    }
    return m_progressValue;
}

//-------------------------------------------------------------------------------------------

bool QPLProgress::isRetina() const
{
    bool retinaFlag = false;

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        retinaFlag = true;
    }
#endif
    return retinaFlag;
}

//-------------------------------------------------------------------------------------------

void QPLProgress::paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage) const
{
    QPixmap pixMap = QPixmap::fromImage(*pImage);
#if QT_VERSION >= 0x050000
    pixMap.setDevicePixelRatio(2.0);
#endif
    painter->drawPixmap(pos,pixMap);
}

//-------------------------------------------------------------------------------------------

void QPLProgress::paintRetinaImage(QPainter *painter,QPointF pos,QImage *pImage) const
{
    QPixmap pixMap = QPixmap::fromImage(*pImage);
#if QT_VERSION >= 0x050000
    pixMap.setDevicePixelRatio(2.0);
#endif
    painter->drawPixmap(pos,pixMap);
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
