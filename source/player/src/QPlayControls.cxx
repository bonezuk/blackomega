#include "player/inc/QPlayControls.h"
#include "track/db/inc/DBInfo.h"
#include "player/inc/Player.h"
#include "player/inc/QPlayerApplication.h"

#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPlayControls::QPlayControls(QWidget *parent) : QWidget(parent),
    m_state(0),
    m_enabled(false),
    m_maskImageBack(0),
    m_maskImagePlay(0),
    m_maskImageForward(0),
    m_controlImageArray(0),
    m_imgBackA(0),
    m_imgBackB(0),
    m_imgBackC(0),
    m_imgBackDisableA(0),
    m_imgBackDisableB(0),
    m_imgBackDisableC(0),
    m_digitImageArray(0),
    m_colonImage(0),
    m_seekImage(0),
    m_volumeImage(0),
    m_seekPushImage(0),
    m_volumePushImage(0),
    m_seekHoverImage(0),
    m_volumeHoverImage(0),
    m_playTime(0.0),
    m_seekMaxTime(60.0),
    m_volume(1.0f),
    m_playListWidget(0),
    m_seekTime(),
    m_seekState(0),
    m_seekXOffset(0),
    m_trackImage(0),
    m_trackInfo(),
    m_trackSubIndex(-1),
    m_darkFont(0),
    m_lightFont(0),
    m_darkFontMetric(0),
    m_lightFontMetric(0),
    m_timeDrawOffset(0),
    m_volumeState(0),
    m_volumeXOffset(0),
    m_volumeSave(1.0f),
    m_timeToEndFlag(false),
    m_openFileFlag(false),
    m_moveWindowState(0),
    m_moveWindowOffset(),
    m_shuffleImages(),
    m_repeatImages(),
    m_stateShuffle(0),
    m_stateRepeat(0),
    m_shuffle(false),
    m_repeat(false)
{
    loadResources();
    setMouseTracking(true);
    m_shuffle = loadPlaySetting(false);
    m_repeat = loadPlaySetting(true);
}

//-------------------------------------------------------------------------------------------

QPlayControls::~QPlayControls()
{
    freeResources();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::loadResources()
{
    tint i;
    bool retinaFlag = false;

#if QT_VERSION >= 0x050000
    qreal dpr = QPlayerApplication::playerInstance()->devicePixelRatio();
#else
    qreal dpr = 1.0;
#endif

    if(dpr >= 1.25)
    {
        fprintf(stdout,"Use Retina\n");
        retinaFlag = true;
    }
    else
    {
        fprintf(stdout,"No Retina\n");
    }

    freeResources();

    m_controlImageArray = new QImage *[128];
    for(i=0;i<128;i++)
    {
        m_controlImageArray[i] = 0;
    }

    m_digitImageArray = new QImage *[10];
    for(i=0;i<10;i++)
    {
        QString n = ":/digits/Resources/digits/no" + QString::number(i);
        if(retinaFlag)
        {
            n += "@2x";
        }
        n += ".png";
        m_digitImageArray[i] = new QImage(n);
    }
    if(retinaFlag)
    {
        m_colonImage = new QImage(":/digits/Resources/digits/colon@2x.png");
    }
    else
    {
        m_colonImage = new QImage(":/digits/Resources/digits/colon.png");
    }

    m_maskImageBack = new QImage(":/playback/Resources/playback/back_mask.png");
    m_maskImagePlay = new QImage(":/playback/Resources/playback/play_mask.png");
    m_maskImageForward = new QImage(":/playback/Resources/playback/forward_mask.png");

    if(!retinaFlag)
    {
        m_imgBackA = new QImage(":/panel/Resources/panel/backA.png");
        m_imgBackB = new QImage(":/panel/Resources/panel/backB.png");
        m_imgBackC = new QImage(":/panel/Resources/panel/backC.png");

        m_imgBackDisableA = new QImage(":/panel/Resources/panel/backA_disable.png");
        m_imgBackDisableB = new QImage(":/panel/Resources/panel/backB_disable.png");
        m_imgBackDisableC = new QImage(":/panel/Resources/panel/backC_disable.png");

        m_seekImage = new QImage(":/panel/Resources/panel/slide_button.png");
        m_volumeImage = new QImage(":/panel/Resources/panel/volume_button.png");
        m_seekPushImage = new QImage(":/panel/Resources/panel/slide_button_push.png");
        m_volumePushImage = new QImage(":/panel/Resources/panel/volume_button_push.png");
        m_seekHoverImage = new QImage(":/panel/Resources/panel/slide_button_select.png");
        m_volumeHoverImage = new QImage(":/panel/Resources/panel/volume_button_select.png");
    }
    else
    {
        m_imgBackA = new QImage(":/panel/Resources/panel/backA@2x.png");
        m_imgBackB = new QImage(":/panel/Resources/panel/backB@2x.png");
        m_imgBackC = new QImage(":/panel/Resources/panel/backC@2x.png");

        m_imgBackDisableA = new QImage(":/panel/Resources/panel/backA_disable@2x.png");
        m_imgBackDisableB = new QImage(":/panel/Resources/panel/backB_disable@2x.png");
        m_imgBackDisableC = new QImage(":/panel/Resources/panel/backC_disable@2x.png");

        m_seekImage = new QImage(":/panel/Resources/panel/slide_button@2x.png");
        m_volumeImage = new QImage(":/panel/Resources/panel/volume_button@2x.png");
        m_seekPushImage = new QImage(":/panel/Resources/panel/slide_button_push@2x.png");
        m_volumePushImage = new QImage(":/panel/Resources/panel/volume_button_push@2x.png");
        m_seekHoverImage = new QImage(":/panel/Resources/panel/slide_button_select@2x.png");
        m_volumeHoverImage = new QImage(":/panel/Resources/panel/volume_button_select@2x.png");
    }

    QStringList buttonNameList;
    buttonNameList << "disable_off";
    buttonNameList << "disable_on";
    buttonNameList << "disable_off_hover";
    buttonNameList << "disable_on_hover";
    buttonNameList << "enable_off";
    buttonNameList << "enable_on";
    buttonNameList << "enable_off_hover";
    buttonNameList << "enable_on_hover";

    for(QStringList::const_iterator ppI=buttonNameList.begin();ppI!=buttonNameList.end();ppI++)
    {
        QImage *sImg,*rImg;
        QString bName,sName,rName;

        bName = *ppI;
        if(retinaFlag)
        {
            bName += "@2x";
        }
        bName += ".png";
        sName = ":/panelbutton/Resources/panelbutton/shuffle_" + bName;
        rName = ":/panelbutton/Resources/panelbutton/repeat_" + bName;

        sImg = new QImage(sName);
        m_shuffleImages.append(sImg);
        rImg = new QImage(rName);
        m_repeatImages.append(rImg);
    }

#if defined(OMEGA_WIN32)
    m_darkFont = new QFont("Arial Unicode MS",8);
    m_lightFont = new QFont("Arial Unicode MS",6);
#elif defined(OMEGA_MACOSX)
    m_darkFont = new QFont("Arial Unicode MS",10);
    m_lightFont = new QFont("Arial Unicode MS",8);
#elif defined(OMEGA_LINUX)
    m_darkFont = new QFont("Arial Unicode MS",10);
    m_lightFont = new QFont("Arial Unicode MS",8);
#endif
    m_lightFont->setWeight(QFont::Light);
    m_darkFontMetric = new QFontMetricsF(*m_darkFont);
    m_lightFontMetric = new QFontMetricsF(*m_lightFont);

    QSettings settings;

    settings.beginGroup("playcontrols");
    if(settings.contains("timeToEndFlag"))
    {
        m_timeToEndFlag = settings.value("timeToEndFlag",QVariant(false)).toBool();
    }
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::freeResources()
{
    tint i;

    QSettings settings;

    settings.beginGroup("playcontrols");
    settings.setValue("timeToEndFlag",QVariant(m_timeToEndFlag));
    settings.endGroup();

    if(m_trackImage!=0)
    {
        delete m_trackImage;
        m_trackImage = 0;
    }

    if(m_controlImageArray!=0)
    {
        for(i=0;i<128;i++)
        {
            if(m_controlImageArray[i]!=0)
            {
                delete m_controlImageArray[i];
            }
        }
        delete m_controlImageArray;
        m_controlImageArray = 0;
    }

    if(m_digitImageArray!=0)
    {
        for(i=0;i<10;i++)
        {
            delete m_digitImageArray[i];
        }
        delete m_digitImageArray;
        m_digitImageArray = 0;
    }
    if(m_colonImage!=0)
    {
        delete m_colonImage;
        m_colonImage = 0;
    }

    if(m_maskImageBack!=0)
    {
        delete m_maskImageBack;
        m_maskImageBack = 0;
    }
    if(m_maskImagePlay!=0)
    {
        delete m_maskImagePlay;
        m_maskImagePlay = 0;
    }
    if(m_maskImageForward!=0)
    {
        delete m_maskImageForward;
        m_maskImageForward = 0;
    }

    if(m_imgBackA!=0)
    {
        delete m_imgBackA;
        m_imgBackA = 0;
    }
    if(m_imgBackB!=0)
    {
        delete m_imgBackB;
        m_imgBackB = 0;
    }
    if(m_imgBackC!=0)
    {
        delete m_imgBackC;
        m_imgBackC = 0;
    }

    if(m_imgBackDisableA!=0)
    {
        delete m_imgBackDisableA;
        m_imgBackDisableA = 0;
    }
    if(m_imgBackDisableB!=0)
    {
        delete m_imgBackDisableB;
        m_imgBackDisableB = 0;
    }
    if(m_imgBackDisableC!=0)
    {
        delete m_imgBackDisableC;
        m_imgBackDisableC = 0;
    }

    if(m_darkFontMetric!=0)
    {
        delete m_darkFontMetric;
        m_darkFontMetric = 0;
    }
    if(m_lightFontMetric!=0)
    {
        delete m_lightFontMetric;
        m_lightFontMetric = 0;
    }
    if(m_darkFont!=0)
    {
        delete m_darkFont;
        m_darkFont = 0;
    }
    if(m_lightFont!=0)
    {
        delete m_lightFont;
        m_lightFont = 0;
    }

    if(m_seekImage!=0)
    {
        delete m_seekImage;
        m_seekImage = 0;
    }
    if(m_volumeImage!=0)
    {
        delete m_volumeImage;
        m_volumeImage = 0;
    }
    if(m_seekPushImage!=0)
    {
        delete m_seekPushImage;
        m_seekPushImage = 0;
    }
    if(m_volumePushImage!=0)
    {
        delete m_volumePushImage;
        m_volumePushImage = 0;
    }
    if(m_seekHoverImage!=0)
    {
        delete m_seekHoverImage;
        m_seekHoverImage = 0;
    }
    if(m_volumeHoverImage!=0)
    {
        delete m_volumeHoverImage;
        m_volumeHoverImage = 0;
    }

    for(QVector<QImage *>::iterator ppI=m_shuffleImages.begin();ppI!=m_shuffleImages.end();ppI++)
    {
        delete *ppI;
    }
    m_shuffleImages.clear();

    for(QVector<QImage *>::iterator ppI=m_repeatImages.begin();ppI!=m_repeatImages.end();ppI++)
    {
        delete *ppI;
    }
    m_repeatImages.clear();
}

//-------------------------------------------------------------------------------------------

QImage *QPlayControls::getControlImage(tint s,bool retinaFlag)
{
    QImage *img = 0;

    if(m_controlImageArray!=0)
    {
        s &= 0x0000007f;
        if(m_controlImageArray[s]==0)
        {
            tint i,p[3];
            QString n;

            n = (s & 0x00000040) ? "B" : "A";
            p[0] = (s >> 4) & 0x00000003;
            p[1] = (s >> 2) & 0x00000003;
            p[2] = s & 0x00000003;
            for(i=0;i<3;i++)
            {
                n += QString::number(p[i]);
            }
            if(retinaFlag)
            {
                n += "@2x";
            }
            n = ":/playback/Resources/playback/" + n + ".png";

            img = new QImage(n);
            if(img->isNull())
            {
                delete img;
                img = 0;
            }
            else
            {
                m_controlImageArray[s] = img;
            }
        }
        else
        {
            img = m_controlImageArray[s];
        }
    }
    return img;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage)
{
    QPixmap pixMap = QPixmap::fromImage(*pImage);
#if QT_VERSION >= 0x050000
    pixMap.setDevicePixelRatio(2.0);
#endif
    painter->drawPixmap(pos,pixMap);
}

//-------------------------------------------------------------------------------------------

void QPlayControls::paintTimeStamp(QPainter *painter,const common::TimeStamp& t,bool retinaFlag)
{
    const tint c_yPos = 16;

    tint rOffset = width() - 5;
    tint hrA,hrB,minA,minB,secA,secB;
    common::TimeStamp tA;

    if(m_timeToEndFlag)
    {
        tA = m_seekMaxTime - t;
    }
    else
    {
        tA = t;
    }
    if(m_trackInfo.data()!=0)
    {
        if(t > m_seekMaxTime)
        {
            tA = 0.0;
        }
    }

    hrA  = tA.hour();
    minA = tA.minute();
    secA = tA.second();

    secB = secA / 10;
    secA = secA % 10;

    if(retinaFlag)
    {
        paintRetinaImage(painter,QPoint(rOffset - 14,c_yPos),m_digitImageArray[secA]);
        paintRetinaImage(painter,QPoint(rOffset - 28,c_yPos),m_digitImageArray[secB]);
        paintRetinaImage(painter,QPoint(rOffset - 33,c_yPos),m_colonImage);
    }
    else
    {
        painter->drawImage(QPoint(rOffset - 14,c_yPos),*(m_digitImageArray[secA]));
        painter->drawImage(QPoint(rOffset - 28,c_yPos),*(m_digitImageArray[secB]));
        painter->drawImage(QPoint(rOffset - 33,c_yPos),*(m_colonImage));
    }

    minB = minA / 10;
    minA = minA % 10;
    if(retinaFlag)
    {
        paintRetinaImage(painter,QPoint(rOffset - 47,c_yPos),m_digitImageArray[minA]);
    }
    else
    {
        painter->drawImage(QPoint(rOffset - 47,c_yPos),*(m_digitImageArray[minA]));
    }
    if(minB>0 || hrA>0)
    {
        if(retinaFlag)
        {
            paintRetinaImage(painter,QPoint(rOffset - 61,c_yPos),m_digitImageArray[minB]);
        }
        else
        {
            painter->drawImage(QPoint(rOffset - 61,c_yPos),*(m_digitImageArray[minB]));
        }

        if(hrA>0)
        {
            hrB = hrA / 10;
            hrA = hrA % 10;
            hrB = hrB % 10;
            if(retinaFlag)
            {
                paintRetinaImage(painter,QPoint(rOffset - 66,c_yPos),m_colonImage);
                paintRetinaImage(painter,QPoint(rOffset - 80,c_yPos),m_digitImageArray[hrA]);
            }
            else
            {
                painter->drawImage(QPoint(rOffset - 66,c_yPos),*(m_colonImage));
                painter->drawImage(QPoint(rOffset - 80,c_yPos),*(m_digitImageArray[hrA]));
            }
            if(hrB>0)
            {
                if(retinaFlag)
                {
                    paintRetinaImage(painter,QPoint(rOffset - 94,c_yPos),m_digitImageArray[hrB]);
                }
                else
                {
                    painter->drawImage(QPoint(rOffset - 94,c_yPos),*(m_digitImageArray[hrB]));
                }
                m_timeDrawOffset = rOffset - 94;
            }
            else
            {
                m_timeDrawOffset = rOffset - 80;
            }
        }
        else
        {
            m_timeDrawOffset = rOffset - 61;
        }
    }
    else
    {
        m_timeDrawOffset = rOffset - 47;
    }
}

//-------------------------------------------------------------------------------------------

void QPlayControls::paintSeekBar(QPainter *painter,bool retinaFlag)
{
    if(m_enabled)
    {
        QRect rO,rB;
        getSeekRegion(rO,rB);
        if(m_seekState==1)
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,rB.topLeft(),m_seekHoverImage);
            }
            else
            {
                painter->drawImage(rB.topLeft(),*(m_seekHoverImage));
            }
        }
        else if(m_seekState==2)
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,rB.topLeft(),m_seekPushImage);
            }
            else
            {
                painter->drawImage(rB.topLeft(),*(m_seekPushImage));
            }
        }
        else
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,rB.topLeft(),m_seekImage);
            }
            else
            {
                painter->drawImage(rB.topLeft(),*(m_seekImage));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlayControls::paintVolumeBar(QPainter *painter,bool retinaFlag)
{
    if(m_enabled)
    {
        QRect rO,rB;
        getVolumeRegion(rO,rB);
        if(m_volumeState==1)
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,rB.topLeft(),m_volumeHoverImage);
            }
            else
            {
                painter->drawImage(rB.topLeft(),*(m_volumeHoverImage));
            }
        }
        else if(m_volumeState==2)
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,rB.topLeft(),m_volumePushImage);
            }
            else
            {
                painter->drawImage(rB.topLeft(),*(m_volumePushImage));
            }
        }
        else
        {
            if(retinaFlag)
            {
                paintRetinaImage(painter,rB.topLeft(),m_volumeImage);
            }
            else
            {
                painter->drawImage(rB.topLeft(),*(m_volumeImage));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
//TODO - Adjust text offset
void QPlayControls::paintTrackInfo(QPainter *painter,bool retinaFlag)
{
    if(m_trackInfo.data()!=0)
    {
        tint xPos = 96;
        tint volXLimit = width() - (80 + (17 * 2));

        if(m_trackImage!=0)
        {
            if(retinaFlag)
            {
                QPixmap trackPix = QPixmap::fromImage(*m_trackImage);
#if QT_VERSION >= 0x050000
                trackPix.setDevicePixelRatio(2.0);
#endif
                painter->drawPixmap(QPoint(96,16),trackPix);
            }
            else
            {
                painter->drawImage(QPoint(96,16),*m_trackImage);
            }
            xPos += 38;
        }

        if(m_timeDrawOffset < volXLimit)
        {
            volXLimit = m_timeDrawOffset;
        }

        QRect titleRect(xPos,13,m_timeDrawOffset-xPos,static_cast<int>(m_darkFontMetric->height()));
        QRect albumRect(xPos,titleRect.bottom(),volXLimit-xPos,static_cast<int>(m_lightFontMetric->height()));
        QRect artistRect(xPos,albumRect.bottom(),volXLimit-xPos,static_cast<int>(m_lightFontMetric->height()));

        painter->setFont(*m_darkFont);
        painter->setPen(QColor(0,0,0));

        if(m_trackSubIndex>=0)
        {
            if(m_trackInfo->isChildren())
            {
                if(m_trackSubIndex>=m_trackInfo->noChildren())
                {
                    m_trackSubIndex = m_trackInfo->noChildren() - 1;
                }
            }
            else
            {
                m_trackSubIndex = -1;
            }
        }

        if(m_trackSubIndex>=0)
        {
            painter->drawText(titleRect,Qt::AlignLeft,m_trackInfo->child(m_trackSubIndex).name());
        }
        else
        {
            painter->drawText(titleRect,Qt::AlignLeft,m_trackInfo->title());
        }

        painter->setFont(*m_lightFont);
        painter->setPen(QColor(48,48,48));
        painter->drawText(albumRect,Qt::AlignLeft,m_trackInfo->album());
        painter->drawText(artistRect,Qt::AlignLeft,m_trackInfo->artist());
    }
}

//-------------------------------------------------------------------------------------------

void QPlayControls::paintEvent(QPaintEvent *e)
{
    tint xPos = 0,w = width();
    QPainter painter(this);
    QRect rect(0,0,width(),height());
    QImage *imgA,*imgB,*imgC;
    bool retinaFlag = false;

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        retinaFlag = true;
    }
#endif

    if(m_enabled)
    {
        imgA = m_imgBackA;
        imgB = m_imgBackB;
        imgC = m_imgBackC;
    }
    else
    {
        imgA = m_imgBackDisableA;
        imgB = m_imgBackDisableB;
        imgC = m_imgBackDisableC;
    }

    painter.fillRect(rect,palette().color(QPalette::Window));

    if(retinaFlag)
    {
        paintRetinaImage(&painter,QPoint(0,0),imgA);
        xPos += imgA->width() / 2;
        while(xPos < (w - (imgC->width() / 2)))
        {
            paintRetinaImage(&painter,QPoint(xPos,0),imgB);
            xPos += imgB->width() / 2;
        }
        paintRetinaImage(&painter,QPoint(w - (imgC->width() / 2),0),imgC);
    }
    else
    {
        painter.drawImage(QPoint(0,0),*imgA);
        xPos += imgA->width();
        while(xPos < (w - imgC->width()))
        {
            painter.drawImage(QPoint(xPos,0),*imgB);
            xPos += imgB->width();
        }
        painter.drawImage(QPoint(w - imgC->width(),0),*imgC);
    }

    QImage *ctrlImg = getControlImage(static_cast<tint>(m_state),retinaFlag);
    if(ctrlImg!=0)
    {
        if(retinaFlag)
        {
            QPixmap ctrlPix = QPixmap::fromImage(*ctrlImg);
#if QT_VERSION >= 0x050000
            ctrlPix.setDevicePixelRatio(2.0);
#endif
            painter.drawPixmap(QPointF(-1,-1),ctrlPix);
        }
        else
        {
            painter.drawImage(QPointF(-1,-1),*ctrlImg);
        }
    }
    paintTimeStamp(&painter,(m_seekState!=2) ? m_playTime : m_seekTime,retinaFlag);
    paintSeekBar(&painter,retinaFlag);
    paintVolumeBar(&painter,retinaFlag);
    paintTrackInfo(&painter,retinaFlag);
    paintPlaylistButtons(&painter,retinaFlag);
}

//-------------------------------------------------------------------------------------------

void QPlayControls::mouseMoveEvent(QMouseEvent *e)
{
    if(!onMouseMoveWindowMove(e))
    {
        onMouseControl(e,false);
        mouseMoveEventSeek(e);
        mouseMoveEventVolume(e);
    }
    QWidget::mouseMoveEvent(e);
}

//-------------------------------------------------------------------------------------------

void QPlayControls::mousePressEvent(QMouseEvent *e)
{
    if(!onMouseControl(e,false))
    {
        if(!mousePressEventSeek(e))
        {
            if(!mousePressEventVolume(e))
            {
                onMouseMoveWindowPress(e);
            }
        }
    }
    QWidget::mousePressEvent(e);
}

//-------------------------------------------------------------------------------------------

void QPlayControls::mouseReleaseEvent(QMouseEvent *e)
{
    if(!onMouseMoveWindowRelease(e))
    {
        onMouseControl(e,true);
        mouseReleaseEventSeek(e);
        mouseReleaseEventVolume(e);
    }
    QWidget::mouseReleaseEvent(e);
}

//-------------------------------------------------------------------------------------------

void QPlayControls::leaveEvent(QEvent *e)
{
    m_stateShuffle = 0;
    m_stateRepeat = 0;
    onMouseLeave();
    mouseLeaveSeek();
    mouseLeaveEventVolume();
    m_moveWindowState = 0;
    QWidget::leaveEvent(e);
}

//-------------------------------------------------------------------------------------------

void QPlayControls::mouseDoubleClickEvent(QMouseEvent *e)
{
    mouseDoubleClickEventTime(e);
    QWidget::mouseDoubleClickEvent(e);
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::onMouseControl(QMouseEvent *e,bool releaseFlag)
{
    tint xP,yP,state;
    tuint32 p[3];
    bool prevPressFlag = false;
    bool playPressFlag = false;
    bool nextPressFlag = false;
    bool pausePressFlag = false;
    bool res = false;

    qreal xPos = static_cast<qreal>(e->pos().x());
    qreal yPos = static_cast<qreal>(e->pos().y());
    qreal xRatio = static_cast<qreal>(m_maskImageBack->width()) / static_cast<qreal>(95.0);
    qreal yRatio = static_cast<qreal>(m_maskImageBack->height()) / static_cast<qreal>(52.0);
    xPos *= xRatio;
    yPos *= yRatio;
    xP = static_cast<int>(xPos);
    yP = static_cast<int>(yPos);

    if((xP>=0 && xP<m_maskImageBack->width()) && (yP>=0 && yP<m_maskImageBack->height()))
    {
        if((m_maskImagePlay->pixel(xP,yP) & 0x00ffffff) != 0x00ffffff)
        {
            state = 2;
            res = true;
        }
        else if((m_maskImageBack->pixel(xP,yP) & 0x00ffffff) != 0x00ffffff)
        {
            state = 1;
            res = true;
        }
        else if((m_maskImageForward->pixel(xP,yP) & 0x00ffffff) != 0x00ffffff)
        {
            state = 3;
            res = true;
        }
        else
        {
            state = 0;
        }
    }
    else
    {
        state = 0;
    }

    p[0] = (m_state >> 4) & 0x3;
    p[1] = (m_state >> 2) & 0x3;
    p[2] = m_state & 0x3;

    if(state==0)
    {
        if(p[0]>=2)
        {
            p[0] = 1;
        }
        if(p[1]>=2)
        {
            p[1] = 1;
        }
        if(p[2]>=2)
        {
            p[2] = 1;
        }
    }
    else
    {
        tint i,o;

        if(state==1)
        {
            if(p[0])
            {
                p[0] = (e->button()==Qt::LeftButton && !releaseFlag) ? 3 : 2;
                if(releaseFlag)
                {
                    prevPressFlag = true;
                }
            }
            o = 0;
        }
        else if(state==2)
        {
            if(p[1])
            {
                p[1] = (e->button()==Qt::LeftButton && !releaseFlag) ? 3 : 2;
                if(releaseFlag)
                {
                    if(isPlaying())
                    {
                        pausePressFlag = true;
                    }
                    else
                    {
                        playPressFlag = true;
                    }
                }
            }
            o = 1;
        }
        else
        {
            if(p[2])
            {
                p[2] = (e->button()==Qt::LeftButton && !releaseFlag) ? 3 : 2;
                if(releaseFlag)
                {
                    nextPressFlag = true;
                }
            }
            o = 2;
        }

        for(i=0;i<3;i++)
        {
            if(i!=o)
            {
                if(p[i]>=2)
                {
                    p[i] = 1;
                }
            }
        }
    }
    m_state = (m_state & 0x00000040) | ((p[0] << 4) & 0x00000030) | ((p[1] << 2) & 0x0000000C) | (p[2] & 0x00000003);

    if(prevPressFlag)
    {
        Q_EMIT onPrevious();
    }
    if(playPressFlag)
    {
        if(m_openFileFlag)
        {
            Q_EMIT onOpen();
        }
        else
        {
            Q_EMIT onPlay();
        }
    }
    if(pausePressFlag)
    {
        Q_EMIT onPause();
    }
    if(nextPressFlag)
    {
        Q_EMIT onNext();
    }

    repaint();

    return res;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::onMouseLeave()
{
    tint i;
    tuint32 p[3];

    p[0] = (m_state >> 4) & 0x3;
    p[1] = (m_state >> 2) & 0x3;
    p[2] = m_state & 0x3;
    for(i=0;i<3;i++)
    {
        if(p[i])
        {
            p[i] = 1;
        }
    }
    m_state = (m_state & 0x00000040) | ((p[0] << 4) & 0x00000030) | ((p[1] << 2) & 0x0000000C) | (p[2] & 0x00000003);
    repaint();
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::mouseMoveEventSeek(QMouseEvent *e)
{
    bool res = false;

    if(m_seekState==2)
    {
        common::TimeStamp seekT;

        if(getSeekTime(e->pos(),seekT))
        {
            m_seekTime = seekT;
            res = true;
        }
        else
        {
            m_seekTime = m_playTime;
            m_seekState = 0;
        }
        repaint();
    }
    else
    {
        QRect rectO,rectB;

        getSeekRegion(rectO,rectB);
        if(rectB.contains(e->pos()))
        {
            if(m_seekState==0)
            {
                m_seekState = 1;
                repaint();
                res = true;
            }
        }
        else
        {
            if(m_seekState==1)
            {
                m_seekState = 0;
                repaint();
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::mousePressEventSeek(QMouseEvent *e)
{
    QRect rectO,rectB;
    bool res = false;

    getSeekRegion(rectO,rectB);
    if(rectO.contains(e->pos()))
    {
        common::TimeStamp seekT;

        if(rectB.contains(e->pos()))
        {
            m_seekXOffset = e->pos().x() - rectB.left();
        }
        else
        {
            m_seekXOffset = rectB.width() / 2;
        }

        if(getSeekTime(e->pos(),seekT))
        {
            m_seekTime = seekT;
            m_seekState = 2;
            repaint();
            res = true;
        }
        else
        {
            m_seekState = 0;
        }
    }
    else
    {
        m_seekState = 0;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::mouseReleaseEventSeek(QMouseEvent *e)
{
    bool res = false;

    if(m_seekState>0)
    {
        common::TimeStamp seekT;

        if(getSeekTime(e->pos(),seekT))
        {
            m_seekTime = seekT;
            m_playTime = seekT;
            m_seekState = 1;
            Q_EMIT onSeek(static_cast<tfloat64>(m_playTime));
        }
        else
        {
            m_seekTime = m_playTime;
            m_seekState = 0;
        }
        repaint();
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::mouseLeaveSeek()
{
    if(m_seekState!=0)
    {
        m_seekTime = m_playTime;
        m_seekState = 0;
        repaint();
    }
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::mouseMoveEventVolume(QMouseEvent *e)
{
    bool res = false;

    if(m_volumeState==2)
    {
        tfloat32 v;

        if(getVolume(e->pos(),v))
        {
            m_volume = v;
        }
        else
        {
            m_volume = m_volumeSave;
            m_volumeState = 0;
        }
        Q_EMIT onVolume(m_volume);
        repaint();
        res = true;
    }
    else
    {
        QRect rectO,rectB;

        getVolumeRegion(rectO,rectB);
        if(rectB.contains(e->pos()))
        {
            if(m_volumeState==0)
            {
                m_volumeState = 1;
                repaint();
            }
            res = true;
        }
        else
        {
            if(m_volumeState==1)
            {
                m_volumeState = 0;
                repaint();
            }
        }
    }

    {
        int pState = m_stateRepeat;
        QRect rPos = positionRepeatButton();
        if(rPos.contains(e->pos()))
        {
            if(!m_stateRepeat)
            {
                m_stateRepeat = 1;
            }
        }
        else
        {
            m_stateRepeat = 0;
        }
        if(pState!=m_stateRepeat)
        {
            repaint();
        }
    }

    {
        int pState = m_stateShuffle;
        QRect rPos = positionShuffleButton();
        if(rPos.contains(e->pos()))
        {
            if(!m_stateShuffle)
            {
                m_stateShuffle = 1;
            }
        }
        else
        {
            m_stateShuffle = 0;
        }
        if(pState!=m_stateShuffle)
        {
            repaint();
        }
    }


    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::mousePressEventVolume(QMouseEvent *e)
{
    QRect rectO,rectB;
    bool res = false;

    getVolumeRegion(rectO,rectB);
    if(rectO.contains(e->pos()))
    {
        tfloat32 v;

        if(rectB.contains(e->pos()))
        {
            m_volumeXOffset = e->pos().x() - rectB.left();
        }
        else
        {
            m_volumeXOffset = rectB.width() / 2;
        }

        if(getVolume(e->pos(),v))
        {
            m_volumeSave = m_volume;
            m_volumeState = 2;
            repaint();
            res = true;
        }
        else
        {
            m_volumeState = 0;
        }
        m_stateShuffle = 0;
        m_stateRepeat = 0;
    }
    else
    {
        m_volumeState = 0;
    }

    {
        int pState = m_stateRepeat;
        QRect rPos = positionRepeatButton();
        if(rPos.contains(e->pos()))
        {
            m_stateRepeat = 2;
        }
        else
        {
            m_stateRepeat = 0;
        }
        if(pState!=m_stateRepeat)
        {
            repaint();
        }
    }

    {
        int pState = m_stateShuffle;
        QRect rPos = positionShuffleButton();
        if(rPos.contains(e->pos()))
        {
            m_stateShuffle = 2;
        }
        else
        {
            m_stateShuffle = 0;
        }
        if(pState!=m_stateShuffle)
        {
            repaint();
        }
    }

    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::mouseReleaseEventVolume(QMouseEvent *e)
{
    bool res = false;

    if(m_volumeState>0)
    {
        tfloat32 v;

        res = true;
        if(getVolume(e->pos(),v))
        {
            m_volume = v;
            m_volumeState = 1;
            Q_EMIT onVolume(m_volume);
        }
        else
        {
            m_volume = m_volumeSave;
            m_volumeState = 0;
            res = false;
        }
        repaint();
    }

    {
        int pState = m_stateRepeat;
        QRect rPos = positionRepeatButton();
        if(rPos.contains(e->pos()))
        {
            if(m_stateRepeat==2)
            {
                m_repeat = (m_repeat) ? false : true;
                savePlaySetting(m_repeat,true);
                Q_EMIT onRepeat(m_repeat);
            }
            m_stateRepeat = 1;
        }
        else
        {
            m_stateRepeat = 0;
        }
        if(pState!=m_stateRepeat)
        {
            repaint();
        }
    }

    {
        int pState = m_stateShuffle;
        QRect rPos = positionShuffleButton();
        if(rPos.contains(e->pos()))
        {
            if(m_stateShuffle==2)
            {
                m_shuffle = (m_shuffle) ? false : true;
                savePlaySetting(m_shuffle,false);
                Q_EMIT onShuffle(m_shuffle);
            }
            m_stateShuffle = 1;
        }
        else
        {
            m_stateShuffle = 0;
        }
        if(pState!=m_stateShuffle)
        {
            repaint();
        }
    }

    return res;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::mouseLeaveEventVolume()
{
    if(m_volumeState!=0)
    {
        m_volume = m_volumeSave;
        m_volumeState = 0;
        Q_EMIT onVolume(m_volume);
        repaint();
    }
}

//-------------------------------------------------------------------------------------------

void QPlayControls::getSeekRegion(QRect& rectO,QRect& rectB)
{
    tfloat64 cT,mT,xPos,xLen;

    if(m_seekState==2)
    {
        if(m_seekTime > m_seekMaxTime)
        {
            m_seekTime = m_seekMaxTime;
        }
        cT = static_cast<tfloat64>(m_seekTime);
    }
    else
    {
        if(m_playTime > m_seekMaxTime)
        {
            m_playTime = m_seekMaxTime;
        }
        cT = static_cast<tfloat64>(m_playTime);
    }
    if(cT < 0.0)
    {
        cT = 0.0;
    }
    mT = static_cast<tfloat64>(m_seekMaxTime);
    xLen = static_cast<tfloat64>(width() - (91 + 33));
    xPos = (cT * (xLen / mT)) + 91.0;

    QRect rO(91,4,width()-(91+6),10);
    rectO = rO;

    QRect rB(static_cast<int>(xPos),4,28,10);
    rectB = rB;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::getSeekTime(const QPoint& mousePos,common::TimeStamp& seekT)
{
    QRect rO,rB;

    getSeekRegion(rO,rB);
    if(rO.contains(mousePos))
    {
        tint xP;
        tfloat64 cT,mT,xPos,xLen;

        xP = mousePos.x() - m_seekXOffset;
        if(xP<rO.left())
        {
            xP = rO.left();
        }
        else if(xP > (rO.right() - rB.width()))
        {
            xP = rO.right() - rB.width();
        }

        mT = static_cast<tfloat64>(m_seekMaxTime);
        xLen = static_cast<tfloat64>(rO.width() - rB.width());
        xPos = static_cast<tfloat64>(xP);
        cT = (mT * (xPos - 91.0)) / xLen;

        seekT = cT;
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

void QPlayControls::getVolumeRegion(QRect& rectO,QRect& rectB)
{
    tfloat32 xPos = static_cast<tfloat64>(width() - 65) + (m_volume * 49.0);
    QRect rO(static_cast<tint>(xPos),36,11,10);
    QRect rB(width() - 63,36,62,12);
    rectO = rB;
    rectB = rO;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::getVolume(const QPoint& mousePos,tfloat32& v)
{
    QRect rO,rB;

    getVolumeRegion(rO,rB);
    if(rO.contains(mousePos))
    {
        tint xP;
        tfloat64 cT,xPos,xLen;

        xP = mousePos.x() - m_volumeXOffset;
        if(xP<rO.left())
        {
            xP = rO.left();
        }
        else if(xP > (rO.right() - rB.width()))
        {
            xP = rO.right() - rB.width();
        }

        xLen = static_cast<tfloat64>(rO.width() - rB.width());
        xPos = static_cast<tfloat64>(xP);
        cT = (1.0 * (xPos - rO.left())) / xLen;

        v = static_cast<tfloat32>(cT);
        if(v<0.0f)
        {
            v = 0.0f;
        }
        else if(v>1.0f)
        {
            v = 1.0f;
        }
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::isPlaying() const
{
    return (m_state & 0x00000040) ? true : false;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setPlaying(bool flag)
{
    if(flag)
    {
        m_state |= 0x00000040;
    }
    else
    {
        m_state &= 0xffffffbf;
    }
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setPlaylistWidget(QPlaylistWidget *pCtrl)
{
    if(m_playListWidget!=0)
    {
        QObject::disconnect(pCtrl,SIGNAL(onUpdate()),this,SLOT(doTrackUpdate()));
    }
    m_playListWidget = pCtrl;
    if(m_playListWidget!=0)
    {
        QObject::connect(pCtrl,SIGNAL(onUpdate()),this,SLOT(doTrackUpdate()));
    }
}

//-------------------------------------------------------------------------------------------

void QPlayControls::doTrackUpdate()
{
    if(m_playListWidget!=0)
    {
        tint i,p[3];
        QPLItemBase *item[3];

        item[0] = m_playListWidget->previousPlayItem();
        item[1] = m_playListWidget->currentPlayItem();
        item[2] = m_playListWidget->nextPlayItem();

        p[0] = (m_state >> 4) & 0x3;
        p[1] = (m_state >> 2) & 0x3;
        p[2] = m_state & 0x3;

        if(item[0]!=0 || item[1]!=0 || item[2]!=0)
        {
            m_openFileFlag = false;
        }
        else
        {
            m_openFileFlag = (m_playListWidget->noPlayItems()==0) ? true : false;
            item[1] = (QPLItemBase *)(1);
        }

        for(i=0;i<3;i++)
        {
            if(item[i]!=0)
            {
                if(!p[i])
                {
                    p[i] = 1;
                }
            }
            else
            {
                p[i] = 0;
            }
        }
        m_state = (m_state & 0x00000040) | ((p[0] << 4) & 0x00000030) | ((p[1] << 2) & 0x0000000C) | (p[2] & 0x00000003);

        Q_EMIT onCanPlay((p[1]) ? true : false);
        Q_EMIT onCanPrevious((p[0]) ? true : false);
        Q_EMIT onCanNext((p[2]) ? true : false);
    }
    else
    {
        setPlaying(false);
        m_enabled = false;
    }
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setPlayback(const QString& fileName)
{
    bool flag = true;

    if(!fileName.isEmpty())
    {
        QSharedPointer<track::info::Info> tInfo = track::db::DBInfo::readInfo(fileName);

        if(tInfo.data()!=0)
        {
            m_trackInfo = tInfo;
            getTrackImage();
            m_seekMaxTime = tInfo->length();
            setPlaying(true);
            m_enabled = true;
            flag = false;
        }
    }
    if(flag)
    {
        QSharedPointer<track::info::Info> tInfo;
        m_trackInfo = tInfo;
        m_seekMaxTime = 1.0;
        setPlaying(false);
        m_enabled = false;
    }
    m_playTime = 0.0;
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setPlayback(QPLItemBase *item)
{
    track::info::InfoSPtr tInfo = item->info();

    m_trackInfo = tInfo;
    getTrackImage();
    m_trackSubIndex = item->subTrackIndex();
    if(item->subTrackIndex()>=0)
    {
        m_seekMaxTime = tInfo->child(item->subTrackIndex()).length();
    }
    else
    {
        m_seekMaxTime = tInfo->length();
    }
    setPlaying(true);
    m_enabled = true;
    m_playTime = 0.0;
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setTime(const common::TimeStamp& t)
{
    m_playTime = t;
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::getTrackImage()
{
    tint wPic = 36;
    tint hPic = 31;

    if(m_trackImage!=0)
    {
        delete m_trackImage;
        m_trackImage = 0;
    }

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        wPic *= 2;
        hPic *= 2;
    }
#endif

    if(m_trackInfo.data()!=0 && m_trackInfo->isImage())
    {
        track::info::Info::ImageFormat iFormat;
        track::info::ImageInfoArray *pArr;

        pArr = m_trackInfo->getImageData(iFormat);
        if(pArr!=0)
        {
            QByteArray qArr(reinterpret_cast<const tchar *>(pArr->GetData()),pArr->GetSize());
            QBuffer qBuffer(&qArr);
            QString format;
            bool res;

            switch(iFormat)
            {
                case track::info::Info::e_imageJPEG:
                    format = "JPEG";
                    break;
                case track::info::Info::e_imageGIF:
                    format = "GIF";
                    break;
                case track::info::Info::e_imagePNG:
                    format = "PNG";
                    break;
                case track::info::Info::e_imageBMP:
                    format = "BMP";
                    break;
                case track::info::Info::e_imageUnknown:
                default:
                    format = "";
                    break;
            }
            m_trackImage = new QImage;
            qBuffer.open(QIODevice::ReadOnly);
            if(!format.isEmpty())
            {
                res = m_trackImage->load(&qBuffer,format.toLatin1().constData());
            }
            else
            {
                res = m_trackImage->load(&qBuffer,0);
            }
            if(res)
            {
                (*m_trackImage) = m_trackImage->scaled(QSize(wPic,hPic),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            }
            else
            {
                delete m_trackImage;
                m_trackImage = 0;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

QRect QPlayControls::timeStampRect()
{
    const tint c_yPos = 16;
    tint rOffset = width() - 5;
    QRect r(m_timeDrawOffset,c_yPos,rOffset - m_timeDrawOffset,m_colonImage->height());
    return r;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::mouseDoubleClickEventTime(QMouseEvent *e)
{
    QRect r = timeStampRect();

    if(r.contains(e->pos()))
    {
        m_timeToEndFlag = (m_timeToEndFlag) ? false : true;
        repaint();
    }
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setVolume(tfloat32 v)
{
    if(v<0.0f)
    {
        v = 0.0f;
    }
    else if(v>1.0f)
    {
        v = 1.0f;
    }
    m_volume = v;
    repaint();
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::timeToEndFlag() const
{
    return m_timeToEndFlag;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::onMouseMoveWindowPress(QMouseEvent *e)
{
    bool res = false;

    if(window()->windowFlags() & Qt::FramelessWindowHint)
    {
        if(e->button()==Qt::LeftButton)
        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QPoint gPos(e->globalPos());
#else
            QPoint gPos(static_cast<int>(e->globalPosition().x()), static_cast<int>(e->globalPosition().y()));
#endif
            m_moveWindowState = 1;
            m_moveWindowOffset = gPos - window()->pos();
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::onMouseMoveWindowMove(QMouseEvent *e)
{
    bool res = false;

    if(m_moveWindowState==1)
    {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPoint gPos(e->globalPos());
#else
        QPoint gPos(static_cast<int>(e->globalPosition().x()), static_cast<int>(e->globalPosition().y()));
#endif
        QPoint nP = gPos - m_moveWindowOffset;
        window()->move(nP);
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::onMouseMoveWindowRelease(QMouseEvent *e)
{
    bool res = false;

    if(m_moveWindowState==1)
    {
        if(e->button()==Qt::LeftButton)
        {
            onMouseMoveWindowMove(e);
            m_moveWindowState = 0;
        }
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::isShuffle() const
{
    return m_shuffle;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setShuffle(bool flag)
{
    m_shuffle = flag;
    savePlaySetting(m_shuffle,false);
    repaint();
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::isRepeat() const
{
    return m_repeat;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::setRepeat(bool flag)
{
    m_repeat = flag;
    savePlaySetting(m_repeat,true);
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlayControls::paintPlaylistButtons(QPainter *painter,bool retinaFlag)
{
    int sIndex = 0,rIndex = 0;

    if(m_enabled)
    {
        sIndex |= 4;
        rIndex |= 4;
    }
    if(m_stateShuffle > 0)
    {
        sIndex |= 2;
    }
    if(m_stateRepeat > 0)
    {
        rIndex |= 2;
    }
    if(isShuffle())
    {
        sIndex |= 1;
    }
    if(isRepeat())
    {
        rIndex |= 1;
    }

    QImage *imgRepeatButton = m_repeatImages.at(rIndex);
    QImage *imgShuffleButton = m_shuffleImages.at(sIndex);

    painter->drawImage(positionShuffleButton().topLeft(),*imgShuffleButton);
    painter->drawImage(positionRepeatButton().topLeft(),*imgRepeatButton);
}

//-------------------------------------------------------------------------------------------

QRect QPlayControls::positionRepeatButton()
{
    const tint h = 36;
    QImage *imgC = (m_enabled) ? m_imgBackC : m_imgBackDisableC;
    tint w = width();
    tint iWid = imgC->width();

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        iWid /= 2;
    }
#endif

    tint wA = w - (iWid + (14 * 1));
    return QRect(wA,h,13,12);
}

//-------------------------------------------------------------------------------------------

QRect QPlayControls::positionShuffleButton()
{
    const tint h = 36;
    QImage *imgC = (m_enabled) ? m_imgBackC : m_imgBackDisableC;
    tint w = width();
    tint iWid = imgC->width();

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        iWid /= 2;
    }
#endif
    tint wA = w - (iWid + (14 * 2));
    return QRect(wA,h,13,12);
}

//-------------------------------------------------------------------------------------------

bool QPlayControls::loadPlaySetting(bool repeatFlag)
{
    QString sName = (repeatFlag) ? "repeat" : "shuffle";
    QSettings settings;
    bool pFlag;

    settings.beginGroup("playcontrol");
    if(settings.contains(sName))
    {
        pFlag = settings.value(sName,QVariant(false)).toBool();
    }
    else
    {
        pFlag = false;
    }
    settings.endGroup();
    return pFlag;
}

//-------------------------------------------------------------------------------------------

void QPlayControls::savePlaySetting(bool value,bool repeatFlag)
{
    QString sName = (repeatFlag) ? "repeat" : "shuffle";
    QSettings settings;

    settings.beginGroup("playcontrol");
    settings.setValue(sName,QVariant(value));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
