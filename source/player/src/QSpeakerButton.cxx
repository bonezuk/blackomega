#include "player/inc/QSpeakerButton.h"
#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QSpeakerButton::QSpeakerButton(audioio::ChannelType t,QWidget *parent) : QWidget(parent),
    m_type(t),
    m_speakerEnabledImage(0),
    m_speakerDisabledImage(0),
    m_playNormalImage(0),
    m_playHoverImage(0),
    m_playClickImage(0),
    m_playMaskImage(0),
    m_state(0)
{
    setup();
    setMouseTracking(true);
}

//-------------------------------------------------------------------------------------------

QSpeakerButton::~QSpeakerButton()
{
    if(m_speakerEnabledImage!=0)
    {
        delete m_speakerEnabledImage;
        m_speakerEnabledImage = 0;
    }
    if(m_speakerDisabledImage!=0)
    {
        delete m_speakerDisabledImage;
        m_speakerDisabledImage = 0;
    }
    if(m_playNormalImage!=0)
    {
        delete m_playNormalImage;
        m_playNormalImage = 0;
    }
    if(m_playHoverImage!=0)
    {
        delete m_playHoverImage;
        m_playHoverImage = 0;
    }
    if(m_playClickImage!=0)
    {
        delete m_playClickImage;
        m_playClickImage = 0;
    }
    if(m_playMaskImage!=0)
    {
        delete m_playMaskImage;
        m_playMaskImage = 0;
    }
}

//-------------------------------------------------------------------------------------------

void QSpeakerButton::setup()
{
    static int coords[9][4] = {
        { 50, 130, 100, 140}, {490, 130, 100, 140},
        {270, 130, 100,  85}, {275, 407,  90,  84},
        { 20, 346, 100, 100}, {520, 346, 100, 100},
        {150, 393, 100, 100}, {390, 393, 100, 100},
        {272, 260,  96,  96}
    };
    
    int cIndex = static_cast<int>(m_type);
    QString eName,dName;
    bool retinaFlag = isRetina();
    
    switch(m_type)
    {
        case audioio::e_FrontLeft:
            if(retinaFlag)
            {
                eName = ":/speakers/Resources/speakers/frontLeft@2x.png";
                dName = ":/speakers/Resources/speakers/frontLeftDisabled@2x.png";
            }
            else
            {
                eName = ":/speakers/Resources/speakers/frontLeft.png";
                dName = ":/speakers/Resources/speakers/frontLeftDisabled.png";
            }
            break;
        case audioio::e_FrontRight:
            if(retinaFlag)
            {
                eName = ":/speakers/Resources/speakers/frontRight@2x.png";
                dName = ":/speakers/Resources/speakers/frontRightDisabled@2x.png";
            }
            else
            {
                eName = ":/speakers/Resources/speakers/frontRight.png";
                dName = ":/speakers/Resources/speakers/frontRightDisabled.png";
            }
            break;
        case audioio::e_Center:
            if(retinaFlag)
            {
                eName = ":/speakers/Resources/speakers/centre@2x.png";
                dName = ":/speakers/Resources/speakers/centreDisabled@2x.png";
            }
            else
            {
                eName = ":/speakers/Resources/speakers/centre.png";
                dName = ":/speakers/Resources/speakers/centreDisabled.png";
            }
            break;
        case audioio::e_LFE:
            if(retinaFlag)
            {
                eName = ":/speakers/Resources/speakers/subwoofer@2x.png";
                dName = ":/speakers/Resources/speakers/subwooferDisabled@2x.png";
            }
            else
            {
                eName = ":/speakers/Resources/speakers/subwoofer.png";
                dName = ":/speakers/Resources/speakers/subwooferDisabled.png";
            }
            break;
        case audioio::e_SurroundLeft:
        case audioio::e_RearLeft:
            if(retinaFlag)
            {
                eName = ":/speakers/Resources/speakers/surroundLeft@2x.png";
                dName = ":/speakers/Resources/speakers/surroundLeftDisabled@2x.png";
            }
            else
            {
                eName = ":/speakers/Resources/speakers/surroundLeft.png";
                dName = ":/speakers/Resources/speakers/surroundLeftDisabled.png";
            }
            break;
        case audioio::e_SurroundRight:
        case audioio::e_RearRight:
            if(retinaFlag)
            {
                eName = ":/speakers/Resources/speakers/surroundRight@2x.png";
                dName = ":/speakers/Resources/speakers/surroundRightDisabled@2x.png";
            }
            else
            {
                eName = ":/speakers/Resources/speakers/surroundRight.png";
                dName = ":/speakers/Resources/speakers/surroundRightDisabled.png";
            }
            break;
        case audioio::e_UnknownChannel:
        default:
            if(retinaFlag)
            {
                eName = ":/speakers/Resources/speakers/human@2x.png";
                dName = ":/speakers/Resources/speakers/human@2x.png";            
            }
            else
            {
                eName = ":/speakers/Resources/speakers/human.png";
                dName = ":/speakers/Resources/speakers/human.png";
            }
            break;
    }
    if(!eName.isEmpty() && !dName.isEmpty())
    {
        m_speakerEnabledImage = new QImage(eName);
        m_speakerDisabledImage = new QImage(dName);
    }
    
    if(retinaFlag)
    {
        m_playNormalImage = loadPlayButton(":/speakers/Resources/speakers/playNormal60x60@2x.png");
        m_playHoverImage = loadPlayButton(":/speakers/Resources/speakers/playHover60x60@2x.png");
        m_playClickImage = loadPlayButton(":/speakers/Resources/speakers/playClick60x60@2x.png");
    }
    else
    {
        m_playNormalImage = loadPlayButton(":/speakers/Resources/speakers/playNormal60x60.png");
        m_playHoverImage = loadPlayButton(":/speakers/Resources/speakers/playHover60x60.png");
        m_playClickImage = loadPlayButton(":/speakers/Resources/speakers/playClick60x60.png");
    }
    m_playMaskImage = new QImage(":/speakers/Resources/speakers/playMask60x60.png");
    
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setGeometry(coords[cIndex][0] - 10,coords[cIndex][1] - 8 + 27 + 60,coords[cIndex][2],coords[cIndex][3]);
}

//-------------------------------------------------------------------------------------------

QImage *QSpeakerButton::loadPlayButton(const QString& imgName)
{
    QImage pButton(imgName);
    QImage pBFormat;
    
    if(pButton.format()!=QImage::Format_ARGB32)
    {
        pBFormat = pButton.convertToFormat(QImage::Format_ARGB32);
    }
    else
    {
        pBFormat = pButton;
    }
    return new QImage(pBFormat);
}

//-------------------------------------------------------------------------------------------

void QSpeakerButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    bool retinaFlag = isRetina();
    
    if(m_speakerEnabledImage!=0)
    {
        if(isEnabled())
        {
            if(retinaFlag)
            {
                paintRetinaImage(&painter,QPoint(0,0),m_speakerEnabledImage);
            }
            else
            {
                painter.drawImage(QPoint(0,0),*m_speakerEnabledImage);
            }
            
            if(m_state>0)
            {
                QRect wRect(0,0,width(),height());
                QPoint mousePos = mapFromGlobal(QCursor::pos());
                
                if(wRect.contains(mousePos))
                {
                    float dis,rLen,amount;
                    QPoint c = wRect.center();
                    
                    dis  = static_cast<float>((mousePos.x() - c.x()) * (mousePos.x() - c.x()));
                    dis += static_cast<float>((mousePos.y() - c.y()) * (mousePos.y() - c.y()));
                    dis  = sqrt(dis);
                    rLen = static_cast<float>((c.x() * c.x()) + (c.y() * c.y()));
                    rLen = sqrt(rLen);
                    amount = 1.0f - (dis / rLen);
                
                    QImage *pButton = transPlayButton(amount);
                    if(retinaFlag)
                    {
                        paintRetinaImage(&painter,QPoint(c.x() - (pButton->width()/4),c.y() - (pButton->height()/4)),pButton);
                    }
                    else
                    {
                        painter.drawImage(QPoint(c.x() - (pButton->width()/2),c.y() - (pButton->height()/2)),*pButton);
                    }
                }
            }
        }
        else
        {
            if(retinaFlag)
            {
                paintRetinaImage(&painter,QPoint(0,0),m_speakerDisabledImage);
            }
            else
            {
                painter.drawImage(QPoint(0,0),*m_speakerDisabledImage);
            }
        }
    }
    else
    {
        if(retinaFlag)
        {
            paintRetinaImage(&painter,QPoint(0,0),currentPlayImage());
        }
        else
        {
            painter.drawImage(QPoint(0,0),*(currentPlayImage()));
        }
    }
}

//-------------------------------------------------------------------------------------------

QImage *QSpeakerButton::transPlayButton(float amount)
{
    int xPos,yPos;
    QImage *nImg = new QImage(currentPlayImage()->copy());
    
    if(amount<0.0f)
    {
        amount = 0.0f;
    }
    else if(amount>1.0f)
    {
        amount = 1.0f;
    }
    
    for(yPos=0;yPos<nImg->height();yPos++)
    {
        for(xPos=0;xPos<nImg->width();xPos++)
        {
            QRgb p = nImg->pixel(xPos,yPos);
            int aP = qAlpha(p);
            aP = static_cast<int>(static_cast<float>(aP) * amount);
            p = qRgba(qRed(p),qGreen(p),qBlue(p),aP);
            nImg->setPixel(xPos,yPos,p);
        }
    }
    return nImg;
}

//-------------------------------------------------------------------------------------------

QImage *QSpeakerButton::currentPlayImage()
{
    QRect wRect(0,0,width(),height());
    QPoint mousePos = mapFromGlobal(QCursor::pos());
    QPoint c = wRect.center();
    QRect pRect(c.x() - (m_playMaskImage->width()/2),c.y() - (m_playMaskImage->height()/2),m_playMaskImage->width(),m_playMaskImage->height());
    QImage *pImage;
    
    if(m_state==2)
    {
        pImage = m_playClickImage;
    }
    else if(m_state==1)
    {
        if(pRect.contains(mousePos))
        {
            QRgb pC;
            
            mousePos -= pRect.topLeft();
            pC = m_playMaskImage->pixel(mousePos);
            if(qRed(pC) > 127)
            {
                pImage = m_playNormalImage;
            }
            else
            {
                pImage = m_playHoverImage;
            }
        }
        else
        {
            pImage = m_playNormalImage;
        }
    }
    else
    {
        pImage = m_playNormalImage;
    }
    return pImage;
}

//-------------------------------------------------------------------------------------------

void QSpeakerButton::mouseMoveEvent(QMouseEvent *e)
{
    int pState = m_state;
    
    if(rect().contains(e->pos()))
    {
        if(!pState)
        {
            m_state = 1;
        }
        repaint();
    }
    else
    {
        m_state = 0;
        if(pState!=m_state)
        {
            repaint();
        }
    }
}

//-------------------------------------------------------------------------------------------

void QSpeakerButton::mousePressEvent(QMouseEvent *e)
{
    if(m_state==1 && m_playHoverImage==currentPlayImage() && e->button()==Qt::LeftButton)
    {
        m_state = 2;
        repaint();
    }
}

//-------------------------------------------------------------------------------------------

void QSpeakerButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_state==2 && e->button()==Qt::LeftButton)
    {
        m_state = 1;
        mouseMoveEvent(e);
        emit clicked();
    }
}

//-------------------------------------------------------------------------------------------

void QSpeakerButton::leaveEvent(QEvent *e)
{
    m_state = 0;
    repaint();
}

//-------------------------------------------------------------------------------------------

bool QSpeakerButton::isRetina() const
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

void QSpeakerButton::paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage) const
{
    QPixmap pixMap = QPixmap::fromImage(*pImage);
#if QT_VERSION >= 0x050000
    pixMap.setDevicePixelRatio(2.0);
#endif
    painter->drawPixmap(pos,pixMap);
}

//-------------------------------------------------------------------------------------------

void QSpeakerButton::paintRetinaImage(QPainter *painter,QPointF pos,QImage *pImage) const
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
