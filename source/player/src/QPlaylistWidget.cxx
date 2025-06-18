#include "player/inc/QPlaylistWidget.h"
#include "player/inc/QPLItemAlbum.h"
#include "player/inc/QPLItemATrack.h"
#include "player/inc/QPLItemSingle.h"
#include "track/db/inc/DBInfo.h"
#include "common/inc/DiskOps.h"
#include "player/inc/Player.h"
#include "track/db/inc/PlaylistAbstractIO.h"
#include "player/inc/PlayerController.h"
#include "player/inc/QPlayerApplication.h"
#include "common/inc/Random.h"

#if defined(OMEGA_MAC_STORE)
#include "common/inc/CommonDirectoriesForFiles.h"
#include "widget/inc/ImportPlaylistDialog.h"
#endif

#include <QSet>
#include <QApplication>
#include <QUrl>
#include <QPixmap>
#include <QClipboard>
#include <QMimeData>

#if defined(OMEGA_POSIX)
#include <dirent.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPlaylistWidget::QPlaylistWidget(QAbstractScrollArea *scrollArea,QWidget *parent) : QWidget(parent),
    m_playRootList(),
    m_viewList(),
    m_scrollArea(scrollArea),
    m_darkFont(0),
    m_lightFont(0),
    m_mediumFont(0),
    m_darkFontMetric(0),
    m_lightFontMetric(0),
    m_mediumFontMetric(0),
    m_timeColumnWidth(5.0),
    m_trackColumnWidth(5.0),
    m_treeExpandImage(0),
    m_treeExpandActiveImage(0),
    m_treeCollapseImage(0),
    m_treeCollapseActiveImage(0),
    m_noTrackImage(0),
    m_speakerImage(0),
    m_pauseImage(0),
    m_noTrackImageMap(),
    m_blackPen(),
    m_grayPen(),
    m_blackDisablePen(),
    m_grayDisablePen(),
    m_dropImage(0),
    m_treeNodeActiveItem(-1),
    m_controlState(0),
    m_wheelStep(0),
    m_startDragPosition(-1,-1),
    m_currentDragPosition(-1),
    m_currentPlayItem(0),
    m_currentPlayNameDrag(),
    m_currentPlayQueueItem(0),
    m_progressControl(0),
    m_dirFileCurrent(0),
    m_dirFileTotal(0),
    m_addNoUpdateFlag(false),
    m_resizeEventFlag(false)
{
    initPainterPens();
    setMouseTracking(true);
    setAcceptDrops(true);
    
    setMinimumSize(300,10);

    m_progressControl = new QPLProgress(this);

    connect(QApplication::clipboard(),SIGNAL(changed(QClipboard::Mode)),this,SLOT(onClipboardChange()));
}

//-------------------------------------------------------------------------------------------

QPlaylistWidget::~QPlaylistWidget()
{
    QList<QPLItemBase *>::iterator ppI;
    
    freePainter();
    
    for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
    {
        QPLItemBase *item = *ppI;
        delete item;
    }
    m_playRootList.clear();
    m_viewList.clear();
    
    if(m_progressControl!=0)
    {
        delete m_progressControl;
        m_progressControl = 0;
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::initPainterPens()
{
    bool retinaFlag = false;

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        retinaFlag = true;
    }
#endif

    freePainter();
    
#if defined(OMEGA_WIN32)
    m_darkFont = new QFont("Arial Unicode MS",12);
    m_mediumFont = new QFont("Arial Unicode MS",10);
    m_lightFont = new QFont("Arial Unicode MS",8);
#elif defined(OMEGA_MACOSX)
    m_darkFont = new QFont("Arial Unicode MS",14);
    m_mediumFont = new QFont("Arial Unicode MS",12);
    m_lightFont = new QFont("Arial Unicode MS",10);
#elif defined(OMEGA_LINUX)
    m_darkFont = new QFont("Arial Unicode MS",14);
    m_mediumFont = new QFont("Arial Unicode MS",12);
    m_lightFont = new QFont("Arial Unicode MS",10);
#endif

    m_darkFontMetric = new QFontMetricsF(*m_darkFont);
    m_lightFont->setWeight(QFont::Light);
    m_lightFontMetric = new QFontMetricsF(*m_lightFont);
    m_mediumFontMetric = new QFontMetricsF(*m_mediumFont);

    m_treeExpandImage = new QImage(":/player/Resources/tree_expand.png");
    m_treeExpandActiveImage = new QImage(":/player/Resources/tree_expand_active.png");
    m_treeCollapseImage = new QImage(":/player/Resources/tree_collapse.png");
    m_treeCollapseActiveImage = new QImage(":/player/Resources/tree_collapse_active.png");
    m_noTrackImage = new QImage(":/player/Resources/note.png");
#if defined(OMEGA_MACOSX)
    if(retinaFlag)
    {
        m_speakerImage = new QImage(":/player/Resources/speaker_small_20x20@2x.png");
    }
    else
    {
        m_speakerImage = new QImage(":/player/Resources/speaker_small_20x20.png");
    }
#else
    if(retinaFlag)
    {
        m_speakerImage = new QImage(":/player/Resources/speaker_small@2x.png");
    }
    else
    {
        m_speakerImage = new QImage(":/player/Resources/speaker_small.png");
    }
#endif
    if(retinaFlag)
    {
        m_pauseImage = new QImage(":/player/Resources/pause_small@2x.png");
    }
    else
    {
        m_pauseImage = new QImage(":/player/Resources/pause_small.png");
    }
    
    if(retinaFlag)
    {
        m_dropImage = new QImage(":/player/Resources/droptarget@2x.png");
    }
    else
    {
        m_dropImage = new QImage(":/player/Resources/droptarget.png");
    }
    
    m_blackPen.setColor(QColor(0,0,0));
    m_grayPen.setColor(QColor(128,128,128));
    m_blackDisablePen.setColor(QColor(128,128,128));
    m_grayDisablePen.setColor(QColor(192,192,192));

    m_timeColumnWidth = m_darkFontMetric->horizontalAdvance("0:00:00") + 5.0;
    m_trackColumnWidth = (((m_darkFontMetric->height() * 2.0) + 4.0) * 1.112);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::freePainter()
{
    QMap<QPair<tint,tint>,QPair<QImage *,QImage *> >::iterator ppI;
    
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
    if(m_mediumFont!=0)
    {
        delete m_mediumFont;
        m_mediumFont = 0;
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
    if(m_mediumFontMetric!=0)
    {
        delete m_mediumFontMetric;
        m_mediumFontMetric = 0;
    }
    if(m_treeExpandImage!=0)
    {
        delete m_treeExpandImage;
        m_treeExpandImage = 0;
    }
    if(m_treeExpandActiveImage!=0)
    {
        delete m_treeExpandActiveImage;
        m_treeExpandActiveImage = 0;
    }
    if(m_treeCollapseImage!=0)
    {
        delete m_treeCollapseImage;
        m_treeCollapseImage = 0;
    }
    if(m_treeCollapseActiveImage!=0)
    {
        delete m_treeCollapseActiveImage;
        m_treeCollapseActiveImage = 0;
    }
    if(m_noTrackImage!=0)
    {
        delete m_noTrackImage;
        m_noTrackImage = 0;
    }
    for(ppI=m_noTrackImageMap.begin();ppI!=m_noTrackImageMap.end();++ppI)
    {
        QImage *img = ppI.value().first;
        delete img;
        img = ppI.value().second;
        if(img!=0)
        {
            delete img;
        }
    }
    m_noTrackImageMap.clear();

    if(m_speakerImage!=0)
    {
        delete m_speakerImage;
        m_speakerImage = 0;
    }
    if(m_pauseImage!=0)
    {
        delete m_pauseImage;
        m_pauseImage = 0;
    }
    if(m_dropImage!=0)
    {
        delete m_dropImage;
        m_dropImage = 0;
    }
}

//-------------------------------------------------------------------------------------------

QImage *QPlaylistWidget::convertImageToGreyscale(QImage *inImg)
{
    int i;
    QVector<QRgb> cTable(256);
    QImage *gImg,greyImage;

    for(i=0;i<256;i++)
    {
        cTable[i] = qRgb(i,i,i);
    }
    greyImage = inImg->convertToFormat(QImage::Format_Indexed8,cTable,Qt::ThresholdDither);
    gImg = new QImage(greyImage);
    return gImg;
}


//-------------------------------------------------------------------------------------------

tint QPlaylistWidget::paintWidth()
{
    return width();
}

//-------------------------------------------------------------------------------------------

QImage *QPlaylistWidget::noTrackImage(tint w,tint h,bool greyFlag)
{
    QPair<tint,tint> idx(w,h);
    QMap<QPair<tint,tint>,QPair<QImage *,QImage *> >::iterator ppI;
    QImage *img;
    
    ppI = m_noTrackImageMap.find(idx);
    if(ppI==m_noTrackImageMap.end())
    {
        img = new QImage(noTrackImage()->scaled(QSize(w,h),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        if(greyFlag)
        {
            QImage *gImg = convertImageToGreyscale(img);
            m_noTrackImageMap.insert(idx,QPair<QImage *,QImage *>(img,gImg));
            img = gImg;
        }
        else
        {
            m_noTrackImageMap.insert(idx,QPair<QImage *,QImage *>(img,0));
        }
    }
    else
    {
        img = ppI.value().first;
        if(greyFlag)
        {
            if(ppI.value().second==0)
            {
                img = convertImageToGreyscale(img);
                ppI.value().second = img;
            }
            else
            {
                img = ppI.value().second;
            }
        }
    }
    return img;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::buildViewList()
{
    int idx;
    QList<QPLItemBase *>::iterator ppI;
        
    m_viewList.clear();
    for(idx=0,ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
    {
        buildViewListAppend(*ppI,idx);
    }
    resizePlaylist();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::buildViewListAppend(QPLItemBase *item,tint& idx)
{
    if(item->isVisible())
    {
        m_viewList.append(item);
        item->setViewPosition(idx++);
    }
    else
    {
        item->setViewPosition(-1);
    }
    
    if(item->isChildren())
    {
        tint i;
        
        for(i=0;i<item->noChildren();i++)
        {
            buildViewListAppend(item->child(i),idx);
        }    
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::updateViewList(tint pos)
{
    if(pos>=0 && pos<m_viewList.size())
    {
        tint i;
        QPLItemBase *item = m_viewList.at(pos);
        
        if(item->isChildren())
        {
            pos++;
            if(item->isExpanded())
            {
                for(i=0;i<item->noChildren();i++)
                {
                    updateViewListAppend(item->child(i),pos);
                }
            }
            else
            {
                for(i=0;i<item->noChildren();i++)
                {
                    updateViewListRemove(item->child(i),pos);
                }
            }
            while(pos<m_viewList.size())
            {
                m_viewList[pos]->setViewPosition(pos);
                pos++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::updateViewListAppend(QPLItemBase *item,tint& idx)
{
    m_viewList.insert(idx,item);
    item->setViewPosition(idx);
    idx++;
    
    if(item->isChildren() && item->isExpanded())
    {
        tint i;
        
        for(i=0;i<item->noChildren();i++)
        {
            updateViewListAppend(item->child(i),idx);
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::updateViewListRemove(QPLItemBase *item,tint idx)
{
    m_viewList.removeAt(idx);
    item->setViewPosition(-1);
    
    if(item->isChildren() && item->isExpanded())
    {
        tint i;
        
        for(i=0;i<item->noChildren();i++)
        {
            updateViewListRemove(item->child(i),idx);
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QRect rect(e->rect());
    bool retinaFlag = false;
    
#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        retinaFlag = true;
    }
#endif

    painter.fillRect(rect,QColor(255,255,255));

    if(!m_viewList.empty())
    {
        tint yPos,rPos;
        QPLItemBase *item;
    
        rPos = viewIndexFromPosition(rect.y());
        item = m_viewList[rPos];
        yPos = item->viewHeight();
        
        while(yPos<rect.bottom() && rPos<m_viewList.size())
        {
            QPLItemBase *item = m_viewList[rPos++];
            item->paint(yPos,&painter);
            yPos += item->height();            
        }
        if(rPos>=m_viewList.size() && yPos<rect.bottom())
        {
            painter.setPen(m_grayPen);
            painter.drawLine(rect.left(),yPos,rect.right(),yPos);
        }
    }
    else
    {
        QString dropText("Drag music files or folders here.");
        tint dPosX,tPosY;
        
        if(retinaFlag)
        {
            dPosX = (rect.width() / 2) - (m_dropImage->width() / 4);
            tPosY = (m_dropImage->height() / 2) + 15;
        }
        else
        {
            dPosX = (rect.width() / 2) - (m_dropImage->width() / 2);
            tPosY = m_dropImage->height() + 15;
        }
        if(dPosX<0)
        {
            dPosX = 0;
        }
        if(retinaFlag)
        {
            paintRetinaImage(&painter,QPoint(dPosX,10),m_dropImage);
        }
        else
        {
            painter.drawImage(QPoint(dPosX,10),*m_dropImage);
        }
        painter.setFont(*m_darkFont);
        painter.setPen(m_blackPen);
        painter.drawText(5,tPosY,rect.width()-10,m_darkFontMetric->height(),Qt::AlignCenter | Qt::TextWordWrap,dropText);
    }

    if(m_progressControl->isActive())
    {
        m_progressControl->paint(&painter);
    }
    if(e!=0)
    {
        QWidget::paintEvent(e);
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage)
{
    QPixmap pixMap = QPixmap::fromImage(*pImage);
    pixMap.setDevicePixelRatio(2.0);
    painter->drawPixmap(pos,pixMap);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::resizePlaylist()
{
    tint h,yTotal = 0,viewHeight;
    QList<QPLItemBase *>::iterator ppI;
    
    viewHeight = m_scrollArea->viewport()->size().height();

    if(!m_viewList.isEmpty())
    {
        for(ppI=m_viewList.begin();ppI!=m_viewList.end();++ppI)
        {
            QPLItemBase *item = *ppI;
        
            h = item->height();
            item->setViewHeight(yTotal);
            yTotal += h;
        }
        yTotal++;
    }

    if(viewHeight > yTotal)
    {
        yTotal = viewHeight;
    }
    
    QSize newSize(m_scrollArea->viewport()->size().width(),yTotal);
    if(!(newSize == size()))
    {
        resize(newSize);
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::resizeEvent(QResizeEvent *e)
{
    if(!m_resizeEventFlag)
    {
        m_resizeEventFlag = true;
        resizePlaylist();
        m_resizeEventFlag = false;
    }
    QWidget::resizeEvent(e);
}

//-------------------------------------------------------------------------------------------

int QPlaylistWidget::viewIndexFromPosition(int yPos,bool lastFlag) const
{
    int positionIndex = 0;

    if(!m_viewList.empty())
    {
        if(yPos>=0)
        {
            const QPLItemBase *item = m_viewList.at(m_viewList.size() - 1);
            
            if(yPos<(item->viewHeight() + item->height()))
            {
                tint min,mid,max;
    
                min = 0;
                max = m_viewList.size() - 1;
                mid = 0;

                while(min<=max)
                {
                    mid = (max + min) / 2;
                    item = m_viewList.at(mid);
                    
                    if(yPos<item->viewHeight())
                    {
                        max = mid - 1;
                    }
                    else if(yPos>=(item->viewHeight() + item->height()))
                    {
                        min = mid + 1;
                    }
                    else
                    {
                        break;
                    }
                }
                positionIndex = mid;
            }
            else
            {
                if(lastFlag)
                {
                    positionIndex = m_viewList.size() - 1;
                }
                else
                {
                    positionIndex = -1;
                }
            }
        }
    }
    return positionIndex;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addTracks(QVector<track::info::InfoSPtr>& tracks,QPLItemBase *prevItem,bool sortFlag)
{
    tint i,j;
    QString name;
    QMap<QString,QMap<QString,tint> > albumMap;
    QMultiMap<QString,tint> nonAlbumMap;
    QMap<QString,QMap<QString,tint> >::iterator ppI;
    QMap<QString,tint>::iterator ppJ;
    QVector<QPair<track::info::InfoSPtr,int> > trackItems;
    
    if(tracks.size()>0)
    {    
        if(sortFlag)
        {
            for(i=0;i<tracks.size();++i)
            {
                bool contentFlag = false;
                track::info::InfoSPtr track(tracks[i]);
                
                if(track.data()!=0)
                {
                    if(!track->album().isEmpty() && !track->title().isEmpty())
                    {
                        QString kName = albumKey(track);
                        
                        ppI = albumMap.find(kName);
                        if(ppI==albumMap.end())
                        {
                            QMap<QString,tint> aMap;
                            albumMap.insert(kName,aMap);
                            ppI = albumMap.find(kName);
                        }
                        {
                            QMap<QString,tint>& aMap = ppI.value();
                            
                            name = track->getName("%Z%N%T",contentFlag,true);
                            if(contentFlag)
                            {
                                aMap.insert(name,i);
                            }
                        }
                    }
                    
                    if(!contentFlag)
                    {
                        if(!track->title().isEmpty())
                        {
                            name = track->getName("%T",contentFlag,true);
                        }
                        else
                        {
                            name = track->getName("%O",contentFlag,true);
                            track->title() = name;
                        }
                        nonAlbumMap.insert(name,i);
                    }
                }
            }
            
            ppI = albumMap.begin();
            while(ppI!=albumMap.end())
            {
                QMap<QString,tint>& aMap = ppI.value();
                
                if(aMap.size()<2)
                {
                    ppJ = aMap.begin();
                    if(ppJ!=aMap.end())
                    {
                        track::info::InfoSPtr track(tracks[ppJ.value()]);
                        if(!track->isChildren())
                        {
                            name = tracks[ppJ.value()]->getName("%L:%T");
                            nonAlbumMap.insert(name,ppJ.value());
                            ppI = albumMap.erase(ppI);
                        }
                        else
                        {
                            ++ppI;
                        }
                    }
                    else
                    {
                        ++ppI;
                    }
                }
                else
                {
                    ++ppI;
                }
            }

            for(ppI=albumMap.begin();ppI!=albumMap.end();++ppI)
            {
                QMap<QString,tint>& trackMap = ppI.value();

                for(ppJ=trackMap.begin();ppJ!=trackMap.end();++ppJ)
                {
                    tint tIdx = ppJ.value();
                    track::info::InfoSPtr tInfo = tracks[tIdx];
                    if(tInfo->isChildren())
                    {
                        for(i=0;i<tInfo->noChildren();i++)
                        {
                            trackItems.append(QPair<track::info::InfoSPtr,int>(tInfo,i));
                        }
                    }
                    else
                    {
                        trackItems.append(QPair<track::info::InfoSPtr,int>(tInfo,-1));
                    }
                }
            }
            
            for(QMultiMap<QString,tint>::iterator ppK=nonAlbumMap.begin();ppK!=nonAlbumMap.end();++ppK)
            {
                tint tIdx = ppK.value();
                track::info::InfoSPtr tInfo = tracks[tIdx];
                if(tInfo->isChildren())
                {
                    for(i=0;i<tInfo->noChildren();i++)
                    {
                        trackItems.append(QPair<track::info::InfoSPtr,int>(tInfo,i));
                    }
                }
                else
                {
                    trackItems.append(QPair<track::info::InfoSPtr,int>(tInfo,-1));
                }
            }
        }
        else
        {
            for(j=0;j<tracks.size();j++)
            {
                track::info::InfoSPtr tInfo = tracks[j];
                if(tInfo->isChildren())
                {
                    for(i=0;i<tInfo->noChildren();i++)
                    {
                        trackItems.append(QPair<track::info::InfoSPtr,int>(tInfo,i));
                    }
                }
                else
                {
                    trackItems.append(QPair<track::info::InfoSPtr,int>(tInfo,-1));
                }
            }
        }
        
        addTracks(trackItems,prevItem);
    }
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::firstPlayItem()
{
    QPLItemBase *item;
    
    if(!m_playRootList.isEmpty())
    {
        item = m_playRootList.first();
        if(item->type()==QPLItemBase::e_Album)
        {
            item = nextPlayItem(item);
        }
    }
    else
    {
        item = 0;
    }
    return item    ;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::initCurrentPlay()
{
    if(!m_playRootList.isEmpty())
    {
        if(m_currentPlayItem==0)
        {
            m_currentPlayItem = m_playRootList.first();
            if(m_currentPlayItem->type()==QPLItemBase::e_Album)
            {
                m_currentPlayItem = nextPlayItem();
            }
        }
    }
    else
    {
        m_currentPlayItem = 0;
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addFile(const QString& name,QPLItemBase *prevItem)
{
    QVector<track::info::InfoSPtr> tList;

    activeProgress();
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    if(track::info::Info::isSupported(name))
    {
#if defined(OMEGA_MAC_STORE)
        common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
        track::db::TrackFileDependencies dependency;
        QStringList accessFileList;
    
        dependency.add(name);

        QSet<QString> allDep = dependency.allDependencies();
        for(QSet<QString>::iterator ppI=allDep.begin();ppI!=allDep.end();++ppI)
        {
            const QString& lPath = *ppI;
            if(!sbBookmark->has(lPath,true))
            {
                accessFileList << lPath;
            }
        }
        
        if(accessFileList.size() > 0)
        {
            QFileInfo fInfo(name);
            QSet<QString> pathSet = common::CommonDirectoriesForFiles::find(accessFileList);
            QStringList pathList(pathSet.begin(), pathSet.end());
            
            widget::ImportPlaylistDialog importDialog(this);
            importDialog.setDirectories(pathList);
            importDialog.setFileDependAccessOn();
            importDialog.setModal(true);
            importDialog.exec();
        }
#endif
    
        track::info::InfoSPtr info = track::db::DBInfo::readInfo(name);
        if(info.data()!=0)
        {
            m_progressControl->setProgress(50.0f);
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            tList.append(info);
            addTracks(tList,prevItem,true);
        }
    }
    else if(track::db::PlaylistAbstractIO::isSupported(name))
    {
        QSharedPointer<track::db::PlaylistAbstractIO> pLoader = track::db::PlaylistIOFactory::createShared(track::db::PlaylistAbstractIO::factoryName(name));
        if(pLoader.data()!=0)
        {
            pLoader->setParent(parentWidget());
            if(pLoader->load(name,tList,m_progressControl))
            {
                addTracks(tList,prevItem,false);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addFiles(const QStringList& name,QPLItemBase *prevItem,bool sortFlag)
{
    tint i;
    QVector<track::info::InfoSPtr> tList;
    tfloat32 cP=0.0f,cPInc = static_cast<tfloat32>(90.0f / name.size());
    
    activeProgress();
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    
#if defined(OMEGA_MAC_STORE)
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    track::db::TrackFileDependencies dependency;
    QStringList accessFileList;
    
    for(i=0;i<name.size();i++)
    {
        dependency.add(name.at(i));
    }
    
    QSet<QString> allDep = dependency.allDependencies();
    for(QSet<QString>::iterator ppI=allDep.begin();ppI!=allDep.end();++ppI)
    {
        const QString& lPath = *ppI;
        if(!sbBookmark->has(lPath,true))
        {
            accessFileList << lPath;
        }
    }
        
    if(accessFileList.size() > 0)
    {
        QFileInfo fInfo(name.at(0));
        QSet<QString> pathSet = common::CommonDirectoriesForFiles::find(accessFileList);
        QStringList pathList(pathSet.begin(), pathSet.end());
            
        widget::ImportPlaylistDialog importDialog(this);
        importDialog.setDirectories(pathList);
        importDialog.setFileDependAccessOn();
        importDialog.setModal(true);
        importDialog.exec();
    }
#endif
    
    for(i=0;i<name.size() && !m_progressControl->isCancelled();i++)
    {
        cP += cPInc;
    
        if(track::info::Info::isSupported(name.at(i)))
        {
            track::info::InfoSPtr info = track::db::DBInfo::readInfo(name.at(i));
            if(info.data()!=0)
            {
                tList.append(info);
            }
            m_progressControl->setProgress(cP);
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
        else if(track::db::PlaylistAbstractIO::isSupported(name.at(i)))
        {
            QSharedPointer<track::db::PlaylistAbstractIO> pLoader = track::db::PlaylistIOFactory::createShared(track::db::PlaylistAbstractIO::factoryName(name.at(i)));
            if(pLoader.data()!=0)
            {
                QVector<track::info::InfoSPtr> pList;
                if(pLoader->load(name.at(i),pList,m_progressControl))
                {
                    addTracks(pList,prevItem,false);
                }
            }
        }
    }
    if(tList.size()>0 && !m_progressControl->isCancelled())
    {
        m_progressControl->setProgress(95.0f);
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        addTracks(tList,prevItem,sortFlag);
    }
    deactivateProgress();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addDirectory(const QString& name,bool recursive,QPLItemBase *prevItem)
{
    track::db::TrackFileDependencies dependency;
    QVector<track::info::InfoSPtr> trackList;
    
    activeProgress();

    m_dirFileCurrent = 0;
    m_dirFileTotal = 0;
    countDirectoryR(name,recursive,m_dirFileTotal,dependency);
    if(!m_progressControl->isCancelled() && m_dirFileTotal>0)
    {
    
#if defined(OMEGA_MAC_STORE)
        QStringList accessFileList;
        common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
        QSet<QString> allDep = dependency.allDependencies();
        for(QSet<QString>::iterator ppI=allDep.begin();ppI!=allDep.end();++ppI)
        {
            const QString& lPath = *ppI;
            if(!sbBookmark->has(lPath,true))
            {
                accessFileList << lPath;
            }
        }
        
        if(accessFileList.size() > 0)
        {
            QFileInfo fInfo(name);
            QSet<QString> pathSet = common::CommonDirectoriesForFiles::find(accessFileList);
            QStringList pathList(pathSet.begin(), pathSet.end());
                
            widget::ImportPlaylistDialog importDialog(this);
            importDialog.setDirectories(pathList);
            importDialog.setFileDependAccessOn();
            importDialog.setModal(true);
            importDialog.exec();
        }
#endif

        m_progressControl->setProgress(0.0f);
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        
        addDirectoryR(name,recursive,trackList);
        
        if(trackList.size()>0 && !m_progressControl->isCancelled())
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            addTracks(trackList,prevItem);
        }
    }
    deactivateProgress();
    initCurrentPlay();
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addDirectoryR(const QString& name,bool recursive,QVector<track::info::InfoSPtr>& trackList)
{
    QString tmp,fullPath(name);
    QSet<QString> dirList,fileList;
    QSet<QString>::iterator ppI;
    LPCWSTR wStr;
    HANDLE h;
    struct _stat fileStat;
    WIN32_FIND_DATAW fData;
    
    common::DiskOps::formatDirectoryPath(fullPath);
    wStr = reinterpret_cast<LPCWSTR>(fullPath.utf16());
    if(::_wstat(wStr,&fileStat)!=0)
    {
        return;
    }
    if(!(S_IFDIR & fileStat.st_mode))
    {
        return;
    }
    
    memset(&fData,0,sizeof(WIN32_FIND_DATAW));
    tmp = fullPath + "\\*";
    
    wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
    h = FindFirstFileW(wStr,&fData);
    if(h!=INVALID_HANDLE_VALUE)
    {
        do
        {
            QString cName(QString::fromUtf16(reinterpret_cast<const char16_t *>(fData.cFileName)));
            
            if(common::DiskOps::dotCheck(cName))
            {
                tmp = fullPath + "\\" + cName;
                wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
                if(_wstat(wStr,&fileStat)==0)
                {
                    if(S_IFDIR & fileStat.st_mode)
                    {
                        if(cName!=".AppleDouble")
                        {
                            dirList.insert(tmp);
                        }
                    }
                    else if(S_IFREG & fileStat.st_mode)
                    {
                        if(track::info::Info::isSupported(tmp))
                        {
                            fileList.insert(tmp);
                        }
                    }
                }
            }
        } while(FindNextFileW(h,&fData) && !m_progressControl->isCancelled());
        FindClose(h);
    }
    
    if(recursive)
    {
        for(ppI=dirList.begin();ppI!=dirList.end() && !m_progressControl->isCancelled();++ppI)
        {
            addDirectoryR(*ppI,recursive,trackList);
        }
    }
    for(ppI=fileList.begin();ppI!=fileList.end() && !m_progressControl->isCancelled();++ppI)
    {
        track::info::InfoSPtr info = track::db::DBInfo::readInfo(*ppI);
        if(info.data()!=0)
        {
            m_dirFileCurrent++;
            tfloat32 cProgress = static_cast<tfloat32>(m_dirFileCurrent * 100.0f) / static_cast<tfloat32>(m_dirFileTotal);
            m_progressControl->setProgress(cProgress);
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            
            trackList.append(info);
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::countDirectoryR(const QString& name,bool recursive,int& count,track::db::TrackFileDependencies& dependency)
{
    QString tmp,fullPath(name);
    QSet<QString> dirList;
    QSet<QString>::iterator ppI;
    LPCWSTR wStr;
    HANDLE h;
    struct _stat fileStat;
    WIN32_FIND_DATAW fData;
    
    common::DiskOps::formatDirectoryPath(fullPath);
    wStr = reinterpret_cast<LPCWSTR>(fullPath.utf16());
    if(::_wstat(wStr,&fileStat)!=0)
    {
        return;
    }
    if(!(S_IFDIR & fileStat.st_mode))
    {
        return;
    }
    
    memset(&fData,0,sizeof(WIN32_FIND_DATAW));
    tmp = fullPath + "\\*";
    
    wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
    h = FindFirstFileW(wStr,&fData);
    if(h!=INVALID_HANDLE_VALUE)
    {
        do
        {
            QString cName(QString::fromUtf16(reinterpret_cast<const char16_t *>(fData.cFileName)));
            
            if(common::DiskOps::dotCheck(cName))
            {
                tmp = fullPath + "\\" + cName;
                wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
                if(_wstat(wStr,&fileStat)==0)
                {
                    if(S_IFDIR & fileStat.st_mode)
                    {
                        if(cName!=".AppleDouble")
                        {
                            dirList.insert(tmp);
                        }
                    }
                    else if(S_IFREG & fileStat.st_mode)
                    {
                        if(track::info::Info::isSupported(tmp))
                        {
                            count++;
                        }
                    }
                }
            }
        } while(FindNextFileW(h,&fData) && !m_progressControl->isCancelled());
        FindClose(h);
    }
    
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    
    if(recursive)
    {
        for(ppI=dirList.begin();ppI!=dirList.end() && !m_progressControl->isCancelled();++ppI)
        {
            countDirectoryR(*ppI,recursive,count,dependency);
        }
    }
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addDirectoryR(const QString& name,bool recursive,QVector<track::info::InfoSPtr>& trackList)
{
    QString tmp,fullPath(name);
    QSet<QString> dirList,fileList;
    QSet<QString>::iterator ppI;
    DIR *h;
    struct dirent *entry;
    struct stat fileStat;
    
    common::DiskOps::formatDirectoryPath(fullPath);
    if(::stat(fullPath.toUtf8().constData(),&fileStat)!=0)
    {
        return;
    }
    if(!(S_IFDIR & fileStat.st_mode))
    {
        return;
    }
    
    h = ::opendir(fullPath.toUtf8().constData());
    if(h!=0)
    {
        while(entry=::readdir(h),entry!=0 && !m_progressControl->isCancelled())
        {
            if(common::DiskOps::dotCheck(entry->d_name))
            {
                QString fName = QString::fromUtf8(entry->d_name);
                tmp = fullPath + "/" + fName;
                if(::stat(tmp.toUtf8().constData(),&fileStat)==0)
                {
                    if(S_IFDIR & fileStat.st_mode)
                    {
                        if(fName!=".AppleDouble")
                        {
                            dirList.insert(tmp);
                        }
                    }
                    else if(S_IFREG & fileStat.st_mode)
                    {
                        if(track::info::Info::isSupported(tmp))
                        {
                            fileList.insert(tmp);
                        }
                    }
                }
            }
        }
        ::closedir(h);
    }
    
    if(recursive)
    {
        for(ppI=dirList.begin();ppI!=dirList.end() && !m_progressControl->isCancelled();++ppI)
        {
            addDirectoryR(*ppI,recursive,trackList);
        }
    }
    for(ppI=fileList.begin();ppI!=fileList.end() && !m_progressControl->isCancelled();++ppI)
    {
#if defined(OMEGA_MAC_STORE)
        common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
        const QString& fName = *ppI;
        if(!sbBookmark->has(fName,common::SBBookmark::isReadOnly(fName)))
        {
            sbBookmark->add(fName,common::SBBookmark::isReadOnly(fName));
        }
#endif

        track::info::InfoSPtr info = track::db::DBInfo::readInfo(*ppI);
        if(info.data()!=0)
        {
            m_dirFileCurrent++;
            tfloat32 cProgress = static_cast<tfloat32>(m_dirFileCurrent * 100.0f) / static_cast<tfloat32>(m_dirFileTotal);
            m_progressControl->setProgress(cProgress);
            QCoreApplication::processEvents(QEventLoop::AllEvents);

            trackList.append(info);
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::countDirectoryR(const QString& name,bool recursive,int& count,track::db::TrackFileDependencies& dependency)
{
    QString tmp,fullPath(name);
    QSet<QString> dirList;
    QSet<QString>::iterator ppI;
    DIR *h;
    struct dirent *entry;
    struct stat fileStat;
    
    common::DiskOps::formatDirectoryPath(fullPath);
    if(::stat(fullPath.toUtf8().constData(),&fileStat)!=0)
    {
        return;
    }
    if(!(S_IFDIR & fileStat.st_mode))
    {
        return;
    }
    
    h = ::opendir(fullPath.toUtf8().constData());
    if(h!=0)
    {
        while(entry=::readdir(h),entry!=0 && !m_progressControl->isCancelled())
        {
            if(common::DiskOps::dotCheck(entry->d_name))
            {
                QString fName = QString::fromUtf8(entry->d_name);
                tmp = fullPath + "/" + fName;
                if(::stat(tmp.toUtf8().constData(),&fileStat)==0)
                {
                    if(S_IFDIR & fileStat.st_mode)
                    {
                        if(fName!=".AppleDouble")
                        {
                            dirList.insert(tmp);
                        }
                    }
                    else if(S_IFREG & fileStat.st_mode)
                    {
                        if(track::info::Info::isSupported(tmp))
                        {
                            dependency.add(tmp);
                            count++;
                        }
                    }
                }
            }
        }
        ::closedir(h);
    }
    
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    
    if(recursive)
    {
        for(ppI=dirList.begin();ppI!=dirList.end() && !m_progressControl->isCancelled();++ppI)
        {
            countDirectoryR(*ppI,recursive,count,dependency);
        }
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void QPlaylistWidget::savePlaylist(const QString& fileName,bool selectFlag)
{
    if(track::db::PlaylistAbstractIO::isSupported(fileName))
    {
        try
        {
            QSharedPointer<track::db::PlaylistAbstractIO> pSaver = track::db::PlaylistIOFactory::createShared(track::db::PlaylistAbstractIO::factoryName(fileName));
            if(pSaver.data()!=0)
            {
                QVector<track::info::InfoSPtr> pList;
            
                getPlaylistItems(0,pList,selectFlag);
                if(!pList.isEmpty())
                {
                    pSaver->save(fileName,pList,m_progressControl);
                }
            }
        }
        catch(...) {}
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::getPlaylistItems(QPLItemBase *item,QVector<track::info::InfoSPtr>& pList,bool selectFlag)
{
    if(item==0)
    {
        QList<QPLItemBase *>::iterator ppI;
        
        for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
        {
            getPlaylistItems(*ppI,pList,selectFlag);
        }
    }
    else
    {
        if(item->type()==QPLItemBase::e_AlbumTrack || item->type()==QPLItemBase::e_Single)
        {
            track::info::InfoSPtr pInfo(item->info());

            if(selectFlag)
            {
                if(item->isSelected())
                {
                    pList.append(pInfo);
                }
            }
            else
            {
                pList.append(pInfo);
            }
        }
        if(item->isChildren())
        {
            for(int i=0;i<item->noChildren();i++)
            {
                getPlaylistItems(item->child(i),pList,selectFlag);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isTreeNodeActiveItem(QPLItemBase *item)
{
    bool res;
    tint vIdx = item->viewPosition();
    
    if(vIdx>=0)
    {
        res = (m_treeNodeActiveItem==vIdx) ? true : false;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(!m_progressControl->isActive())
    {
        if(!m_viewList.empty())
        {
            if(e->buttons().testFlag(Qt::LeftButton)!=0 && m_startDragPosition.x()>=0 && m_startDragPosition.y()>=0 && (e->pos() - m_startDragPosition).manhattanLength()>=QApplication::startDragDistance())
            {
                doDrag(e->pos());
            }
        }
    }
    else
    {
        m_progressControl->mouseMoveEvent(e);
    }
    QWidget::mouseMoveEvent(e);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::mousePressEvent(QMouseEvent *e)
{
    m_controlState = 0;
    m_startDragPosition = e->pos();
    
    if(m_progressControl->isActive())
    {
        m_progressControl->mousePressEvent(e);
        QWidget::mousePressEvent(e);
        return;
    }
    
    if(!m_viewList.empty())
    {
        if(e->button()==Qt::LeftButton)
        {
            tint yPos,treeNodeI;
            QPLItemBase *item;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            yPos = e->pos().y();
#else
            yPos = e->position().y();
#endif
            
            treeNodeI = viewIndexFromPosition(yPos,false);
            if(treeNodeI>=0)
            {
                item = m_viewList[treeNodeI];
                if(!item->isSelected())
                {
                    if(e->modifiers().testFlag(Qt::ControlModifier))
                    {
                        selectSingle(item,false);
                    }
                    else if(e->modifiers().testFlag(Qt::ShiftModifier))
                    {
                        selectShift(item,m_lastSelectItem);
                        m_lastSelectItem = item;
                    }
                    else
                    {
                        if(!item->isSelected())
                        {
                            selectSingle(item,true);
                        }
                    }
                }
                else
                {
                    if(e->modifiers().testFlag(Qt::ShiftModifier))
                    {
                        selectShift(item,m_lastSelectItem);
                        m_lastSelectItem = item;
                    }
                    m_controlState = 1;
                }
            }
        }
        repaint();
    }
    QWidget::mousePressEvent(e);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_startDragPosition = QPoint(-1,-1);
    
    if(m_progressControl->isActive())
    {
        m_progressControl->mouseReleaseEvent(e);
        QWidget::mouseReleaseEvent(e);
        return;
    }
    
    if(!m_viewList.empty() && e->button()==Qt::LeftButton)
    {
        tint yPos,treeNodeI;
        QPLItemBase *item;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        yPos = e->pos().y();
#else
        yPos = e->position().y();
#endif
        
        treeNodeI = viewIndexFromPosition(yPos,false);
        if(treeNodeI>=0)
        {
            item = m_viewList[treeNodeI];
            if(item->isSelected())
            {
                if(e->modifiers().testFlag(Qt::ControlModifier))
                {
                    if(m_controlState==1)
                    {
                        item->setSelected(false);
                    }
                }
                else if(e->modifiers().testFlag(Qt::ShiftModifier))
                {
                    // Nothing
                }
                else
                {
                    if(item->isSelected())
                    {
                        selectSingle(item,true);
                    }                
                }
            }
        }
        else
        {
            selectClear();
        }
        repaint();
        selectionChanged();
    }
    QWidget::mouseReleaseEvent(e);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(m_progressControl->isActive())
    {
        return;
    }
    
    if(!m_viewList.empty())
    {
        if(e->button()==Qt::LeftButton)
        {            
            tint yPos,treeNodeI;
            QPLItemBase *item;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            yPos = e->pos().y();
#else
            yPos = e->position().y();
#endif

            treeNodeI = viewIndexFromPosition(yPos);
            item = m_viewList[treeNodeI];
            if(item!=0)
            {
                if(item->isSelected())
                {
                    item->setSelected(false);
                }
                setCurrentPlayItem(item);
                setCurrentQueuePlayItem(0);
                selectionChanged();
                emit onPlay();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::selectClear()
{
    QList<QPLItemBase *>::iterator ppI;
    
    for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
    {
        QPLItemBase *item = *ppI;
        item->setSelected(false);
    }
    m_lastSelectItem = 0;
    selectionChanged();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::selectSingle(QPLItemBase *item,bool clearFlag)
{
    if(clearFlag)
    {
        selectClear();
    }
    if(item!=0)
    {
        item->setSelected(true);
        m_lastSelectItem = item;
        selectionChanged();
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::selectShift(QPLItemBase *aItem,QPLItemBase *bItem)
{
    if(aItem!=0 && bItem!=0)
    {
        tint state = 0;
        QList<QPLItemBase *>::iterator ppI;
        
        for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
        {
            QPLItemBase *item = *ppI;
            selectShiftRecursive(item,aItem,bItem,state);
        }
        selectionChanged();
    }
    m_lastSelectItem =aItem;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::selectShiftRecursive(QPLItemBase *item,QPLItemBase *aItem,QPLItemBase *bItem,tint& state)
{
    QPLItemBase *sItem = 0;

    if((item==aItem || item==bItem) && !state)
    {
        sItem = item;
        state = 1;
    }
    item->setSelected((state) ? true : false);
    if(item->isChildren())
    {
        for(tint i=0;i<item->noChildren();i++)
        {
            selectShiftRecursive(item->child(i),aItem,bItem,state);
        }
    }    
    if((item==aItem || item==bItem) && ((sItem!=aItem && sItem!=bItem) || aItem==bItem) && state)
    {
        state = 0;
    }
}

//-------------------------------------------------------------------------------------------

QString QPlaylistWidget::albumKey(const track::info::InfoSPtr item,bool cFlag) const
{
    QString name;
    
    if(item.data()!=0)
    {
        if(item->isChildren() && cFlag)
        {
            bool contentFlag = true;
            name = item->album() + ":" + item->year() + ":" + item->directoryGroup() + ":" + item->getName("%Z%N%T",contentFlag,true);
        }
        else
        {
            name = item->album() + ":" + item->year() + ":" + item->directoryGroup();
        }
    }
    return name;
}

//-------------------------------------------------------------------------------------------

QString QPlaylistWidget::albumKey(QPLItemBase *item,bool cFlag) const
{
    return albumKey(item->info(),cFlag);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::buildTrackList(QVector<track::info::InfoSPtr>& items,QList<QPLItemBase *>& list)
{
    int i;
    QVector<track::info::InfoSPtr>::iterator ppI;
    QList<QPair<QString,QVector<track::info::InfoSPtr> > > albumList;
    QList<QPair<QString,QVector<track::info::InfoSPtr> > >::iterator ppJ;
    
    for(ppI=items.begin();ppI!=items.end();ppI++)
    {
        QString key;
        track::info::InfoSPtr info = *ppI;
        
        key = albumKey(info);
        if(!key.isEmpty())
        {
            if(albumList.size()>0)
            {
                QPair<QString,QVector<track::info::InfoSPtr> >& lEntry = albumList.last();
                
                if(lEntry.first!=key)
                {
                    QVector<track::info::InfoSPtr> tList;
                    tList.append(info);
                    albumList.append(QPair<QString,QVector<track::info::InfoSPtr> >(key,tList));                    
                }
                else
                {
                    lEntry.second.append(info);
                }
            }
            else
            {
                QVector<track::info::InfoSPtr> tList;
                tList.append(info);
                albumList.append(QPair<QString,QVector<track::info::InfoSPtr> >(key,tList));
            }
        }
    }
    
    for(ppJ=albumList.begin();ppJ!=albumList.end();++ppJ)
    {
        QPair<QString,QVector<track::info::InfoSPtr> >& lEntry = *ppJ;
        QVector<track::info::InfoSPtr>& tList = lEntry.second;
        
        if(tList.size()>1)
        {
            QPLItemAlbum *aItem = new QPLItemAlbum(this,tList.first());
            
            for(ppI=tList.begin();ppI!=tList.end();ppI++)
            {
                track::info::InfoSPtr info = *ppI;
                if(info->isChildren())
                {
                    for(i=0;i<info->noChildren();i++)
                    {
                        new QPLItemATrack(this,aItem,info,i);
                    }
                }
                else
                {
                    new QPLItemATrack(this,aItem,info);
                }
            }
            list.append(aItem);
        }
        else
        {
            track::info::InfoSPtr& sTrack = tList.first();
            if(sTrack->isChildren())
            {
                QPLItemAlbum *aItem = new QPLItemAlbum(this,sTrack);
                for(i=0;i<sTrack->noChildren();i++)
                {
                    new QPLItemATrack(this,aItem,sTrack,i);
                }
                list.append(aItem);
            }
            else
            {
                QPLItemSingle *sItem = new QPLItemSingle(this,sTrack);
                list.append(sItem);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::buildTrackList(QVector<QPair<track::info::InfoSPtr,int> >& items,QList<QPLItemBase *>& list)
{
    QVector<QPair<track::info::InfoSPtr,int> >::iterator ppI;
    QList<QPair<QString,QVector<QPair<track::info::InfoSPtr,int> > > > albumList;
    QList<QPair<QString,QVector<QPair<track::info::InfoSPtr,int> > > >::iterator ppJ;
    
    for(ppI=items.begin();ppI!=items.end();ppI++)
    {
        QString key;
        track::info::InfoSPtr info = (*ppI).first;
        int cIdx = (*ppI).second;
        
        key = albumKey(info,false);
        if(!key.isEmpty())
        {
            if(albumList.size()>0)
            {
                QPair<QString,QVector<QPair<track::info::InfoSPtr,int> > >& lEntry = albumList.last();
                
                if(lEntry.first!=key)
                {
                    QVector<QPair<track::info::InfoSPtr,int> > tList;
                    tList.append(QPair<track::info::InfoSPtr,int>(info,cIdx));
                    albumList.append(QPair<QString,QVector<QPair<track::info::InfoSPtr,int> > >(key,tList));                    
                }
                else
                {
                    lEntry.second.append(QPair<track::info::InfoSPtr,int>(info,cIdx));
                }
            }
            else
            {
                QVector<QPair<track::info::InfoSPtr,int> > tList;
                tList.append(QPair<track::info::InfoSPtr,int>(info,cIdx));
                albumList.append(QPair<QString,QVector<QPair<track::info::InfoSPtr,int> > >(key,tList));
            }
        }
    }
    
    for(ppJ=albumList.begin();ppJ!=albumList.end();++ppJ)
    {
        QPair<QString,QVector<QPair<track::info::InfoSPtr,int> > >& lEntry = *ppJ;
        QVector<QPair<track::info::InfoSPtr,int> >& tList = lEntry.second;
        
        if(tList.size()>1)
        {
            QPLItemAlbum *aItem = new QPLItemAlbum(this,tList.first().first);
            
            for(ppI=tList.begin();ppI!=tList.end();ppI++)
            {
                int cIdx = (*ppI).second;
                track::info::InfoSPtr info = (*ppI).first;
                new QPLItemATrack(this,aItem,info,cIdx);
            }
            list.append(aItem);
        }
        else
        {
            int cIdx = tList.first().second;
            track::info::InfoSPtr& sTrack = tList.first().first;
            QPLItemSingle *sItem = new QPLItemSingle(this,sTrack,cIdx);
            list.append(sItem);
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addTracks(QVector<QPair<track::info::InfoSPtr,int> >& trackItems,QPLItemBase *prevItem)
{
    tint i,pIndex,aIndex;
    QPLItemBase *oItem;
    QList<QPLItemBase *> trackList;
    QList<QPLItemBase *>::iterator ppI;
    
    buildTrackList(trackItems,trackList);
    
    addTracksShuffleList(trackList);
        
    if(!isSplitRequired(prevItem,trackList))
    {
        oItem = prevItem;
        if(oItem!=0)
        {
            while(oItem->parent()!=0)
            {
                oItem = oItem->parent();
            }
            pIndex = m_playRootList.indexOf(oItem);
            if(pIndex<0)
            {
                prevItem = 0;
                pIndex = 0;
            }
            if(prevItem->type()==QPLItemBase::e_Album)
            {
                aIndex = 0;
            }
            else
            {
                if(prevItem->parent()!=0)
                {
                    aIndex = prevItem->parent()->childIndexOf(prevItem) + 1;
                }
                else
                {
                    pIndex = aIndex = m_playRootList.indexOf(prevItem);
                }
            }
        }
        else
        {    
            pIndex = -1;
            aIndex = 0;
        }

        for(ppI=trackList.begin();ppI!=trackList.end();++ppI)
        {
            QPLItemBase *item = *ppI;
        
            if(oItem!=0 && oItem->type()==QPLItemBase::e_Album)
            {
                if(albumKey(oItem)==albumKey(item))
                {
                    if(oItem->isExpanded())
                    {
                        if(item->type()==QPLItemBase::e_Album)
                        {
                            for(i=0;i<item->noChildren();i++,aIndex++)
                            {
                                oItem->insertChild(aIndex,item->child(i));
                                splitUpdateCurrent(item->child(i));
                            }
                            item->delChildren();
                            delete item;
                        }
                        else if(item->type()==QPLItemBase::e_AlbumTrack)
                        {
                            oItem->insertChild(aIndex,item);
                            splitUpdateCurrent(item);
                            aIndex++;
                        }
                        else if(item->type()==QPLItemBase::e_Single)
                        {
                            QPLItemATrack *tItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(oItem),aIndex,item->info(),item->subTrackIndex());
                            splitUpdateCurrent(tItem);
                            aIndex++;
                            delete item;
                        }
                    }
                    else
                    {
                        if(item->type()==QPLItemBase::e_Album)
                        {
                            for(i=0;i<item->noChildren();i++)
                            {
                                QPLItemBase *aCItem = item->child(i);
                                oItem->addChild(aCItem);
                                splitUpdateCurrent(aCItem);
                            }
                            item->delChildren();
                            delete item;
                        }
                        else if(item->type()==QPLItemBase::e_AlbumTrack)
                        {
                            oItem->addChild(item);
                            splitUpdateCurrent(item);
                        }    
                        else if(item->type()==QPLItemBase::e_Single)
                        {
                            QPLItemATrack *tItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(oItem),item->info(),item->subTrackIndex());
                            splitUpdateCurrent(tItem);
                            delete item;
                        }
                    }
                }
                else
                {
                    m_playRootList.insert(pIndex+1,item);
                    splitUpdateCurrent(item);
                    oItem = item;
                    pIndex++;
                }
            }
            else
            {
                m_playRootList.insert(pIndex+1,item);
                splitUpdateCurrent(item);
                oItem = item;
                pIndex++;
            }
        }
    }
    else
    {
        splitAlbumAndInsert(prevItem,trackList);
    }
    if(!m_addNoUpdateFlag)
    {
        cleanUpPlaylist();
        buildViewList();
        resizePlaylist();
        update();
        initCurrentPlay();
        applyAndEmitUpdate();
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::buildInfoList(QVector<QPLItemBase *>& list,QVector<QPair<track::info::InfoSPtr,int> >& infoList)
{
    QVector<QPLItemBase *>::iterator ppI;
    
    for(ppI=list.begin();ppI!=list.end();ppI++)
    {
        QPLItemBase *item = *ppI;
        
        if(item->type()==QPLItemBase::e_AlbumTrack || item->type()==QPLItemBase::e_Single)
        {
            infoList.append(QPair<track::info::InfoSPtr,int>(item->info(),item->subTrackIndex()));
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::removeTracks(QVector<QPLItemBase *>& trackItems)
{
    tint i;
    QSet<QPLItemBase *> delItems,rootItems,childItems;
    QVector<QPLItemBase *>::iterator ppI;
    QSet<QPLItemBase *>::iterator ppJ;
    QList<QPLItemBase *>::iterator ppK;

    removeTracksShuffleList(trackItems);

    for(ppI=trackItems.begin();ppI!=trackItems.end();ppI++)
    {
        QPLItemBase *rItem = *ppI;

        if(rItem->type()==QPLItemBase::e_AlbumTrack)
        {
            childItems.insert(rItem);
        }
    }

    for(ppI=trackItems.begin();ppI!=trackItems.end();)
    {
        QPLItemBase *rItem = *ppI;

        if(rItem->type()==QPLItemBase::e_Album)
        {
            for(i=0;i<rItem->noChildren();i++)
            {
                ppJ = childItems.find(rItem->child(i));
                if(ppJ==childItems.end())
                {
                    break;
                }
            }
            if(i>=rItem->noChildren())
            {
                ppI++;
            }
            else
            {
                ppI = trackItems.erase(ppI);
            }
        }
        else
        {
            ppI++;
        }
    }

    for(ppI=trackItems.begin();ppI!=trackItems.end();ppI++)
    {
        QPLItemBase *rItem = *ppI;
        
        ppJ = delItems.find(rItem);
        if(ppJ==delItems.end())
        {
            if(rItem->parent()==0)
            {
                rootItems.insert(rItem);
            }
            removeTrackRecursive(rItem,delItems);
        }
    }

    for(ppK=m_playRootList.begin();ppK!=m_playRootList.end();)
    {
        QPLItemBase *rItem = *ppK;

        ppJ = rootItems.find(rItem);
        if(ppJ!=rootItems.end())
        {
            ppK = m_playRootList.erase(ppK);
        }
        else
        {
            ++ppK;
        }
    }

    cleanUpPlaylist();
    buildViewList();
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::removeTrackRecursive(QPLItemBase *dItem,QSet<QPLItemBase *>& delItems)
{
    QSet<QPLItemBase *>::iterator ppI;
    
    ppI=delItems.find(dItem);
    if(ppI==delItems.end())
    {
        while(dItem->isChildren())
        {
            removeTrackRecursive(dItem->child(0),delItems);
        }
        delItems.insert(dItem);
        delete dItem;
    }
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::getSelectedItems(QVector<QPLItemBase *>& tList,bool allFlag)
{
    bool incFlag = false;
    QPLItemBase *cItem,*nPItem = m_currentPlayItem;
    
    if(m_playRootList.size()>0)
    {
        cItem = m_playRootList[0];
    }
    else
    {
        cItem = 0;
    }
    
    while(cItem!=0)
    {
        if(incFlag)
        {
            nPItem = cItem;
            if(!cItem->isSelected())
            {
                incFlag = false;
            }
        }
        else if(nPItem==cItem && cItem->isSelected())
        {
            incFlag = true;
        }
    
        if(cItem->isSelected() || allFlag)
        {
            tList.append(cItem);
        }
                
        if(cItem->isChildren())
        {
            cItem = cItem->child(0);
        }
        else
        {
            bool loop = true;
            
            while(loop && cItem!=0)
            {
                QPLItemBase *sItem = cItem->nextSibling();
                
                if(sItem!=0)
                {
                    cItem = sItem;
                    loop = false;
                }
                else
                {
                    if(cItem->parent()!=0)
                    {
                        cItem = cItem->parent();
                    }
                    else
                    {
                        cItem = 0;
                    }
                }
            }
        }
    }
    
    if(incFlag)
    {
        nPItem = 0;
    }
    return nPItem;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::doDrag(const QPoint& mPos)
{
    QVector<QPLItemBase *> tList;
    QVector<QPLItemBase *>::iterator ppJ;
    QVector<QPair<track::info::InfoSPtr,int> > infoList;
    QVector<QPair<track::info::InfoSPtr,int> >::iterator ppI;
    QPLItemBase *cItem;
    
    getSelectedItems(tList);

    m_currentPlayNameDrag = QPair<QUrl,int>(QUrl(),-1);
    for(ppJ=tList.begin();ppJ!=tList.end();ppJ++)
    {
        cItem = *ppJ;
        if(cItem==m_currentPlayItem)
        {
            m_currentPlayNameDrag = QPair<QUrl,int>(QUrl::fromLocalFile(cItem->info()->getFilename()),cItem->subTrackIndex());
            m_currentPlayItem = 0;
            break;
        }
    }
    
    QList<QUrl> urlData;
    buildInfoList(tList,infoList);
    for(ppI=infoList.begin();ppI!=infoList.end();ppI++)
    {
        QUrl u;
        QString fName;
        track::info::InfoSPtr info = (*ppI).first;
        if((*ppI).second>=0)
        {
            fName = info->getFilename() + "$$" + QString::number((*ppI).second);
        }
        else
        {
            fName = info->getFilename();
        }
        fName = QString::fromUtf8(QUrl::toPercentEncoding(fName));
        u = QUrl::fromLocalFile(fName);
        urlData.append(u);
    }
    
    if(!urlData.isEmpty())
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        QByteArray mArray;
        
        urlListToArray(urlData,mArray);
        mimeData->setData("application/blackomega-urllist",mArray);
        drag->setMimeData(mimeData);
        setDragIcon(drag,tList);
        
        Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
    
        m_currentDragPosition = -1;
        if(dropAction==Qt::MoveAction)
        {
            removeTracks(tList);
        }

        if(m_currentPlayItem==0 && m_playRootList.size()>0)
        {
            m_currentPlayItem = m_playRootList.first();
            if(m_currentPlayItem->type()==QPLItemBase::e_Album)
            {
                m_currentPlayItem = nextPlayItem();
            }
        }

        cleanUpPlaylist();
        buildViewList();
        resizePlaylist();
        update();
        initCurrentPlay();
        applyAndEmitUpdate();
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if(!m_progressControl->isActive())
    {
        if(e->mimeData()->urls().size()>0 || e->mimeData()->hasFormat("application/blackomega-urllist"))
        {
            e->acceptProposedAction();
            e->accept();
        }
        else
        {
            e->ignore();
        }
    }
    else
    {
        e->ignore();
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::dragMoveEvent(QDragMoveEvent *e)
{
    if(!m_progressControl->isActive())
    {
        if(e->mimeData()->urls().size()>0 || e->mimeData()->hasFormat("application/blackomega-urllist"))
        {
            tint yPos,treeNodeI;
            QPLItemBase *item;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            yPos = e->pos().y();
#else
            yPos = e->position().y();
#endif

            treeNodeI = viewIndexFromPosition(yPos);
            if(treeNodeI>=0 && treeNodeI<m_viewList.size())
            {
                tint sPos;
                
                item = m_viewList[treeNodeI];
                if(yPos < (item->viewHeight() + (item->height() / 2)))
                {
                    sPos = treeNodeI;
                }
                else
                {
                    sPos = treeNodeI + 1;
                }
                if(m_currentDragPosition!=sPos)
                {
                    m_currentDragPosition = sPos;
                    repaint();
                }
            }
            e->accept();
        }
        else
        {
            e->ignore();
        }
    }
    else
    {
        e->ignore();
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

void QPlaylistWidget::dropEventProcessBookmarks(const QList<QUrl>& urlList)
{
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    track::db::TrackFileDependencies dependency;
    QStringList accessFileList;
    
    for(QList<QUrl>::const_iterator ppI=urlList.begin();ppI!=urlList.end();++ppI)
    {
        QString fName;
        const QUrl& url = *ppI;
        
        fName = url.toLocalFile();
        if(!fName.isEmpty())
        {
            dependency.add(fName);
            if(!sbBookmark->has(fName,common::SBBookmark::isReadOnly(fName)))
            {
                sbBookmark->add(fName,common::SBBookmark::isReadOnly(fName));
            }
        }
    }

    QSet<QString> allDep = dependency.allDependencies();
    for(QSet<QString>::iterator ppI=allDep.begin();ppI!=allDep.end();++ppI)
    {
        const QString& lPath = *ppI;
        if(!sbBookmark->has(lPath,true))
        {
            accessFileList << lPath;
        }
    }
        
    if(accessFileList.size() > 0)
    {
        QSet<QString> pathSet = common::CommonDirectoriesForFiles::find(accessFileList);
        QStringList pathList(pathSet.begin(), pathSet.end());
            
        widget::ImportPlaylistDialog importDialog(this);
        importDialog.setDirectories(pathList);
        importDialog.setFileDependAccessOn();
        importDialog.setModal(true);
        importDialog.exec();
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void QPlaylistWidget::dropEvent(QDropEvent *e)
{
    int firstNodeI;
    QList<QUrl> urlList;
    QList<QUrl>::iterator ppI;
    const QMimeData *mimeData = e->mimeData();
    QPLItemBase *pItem;

    m_currentDragPosition = -1;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPoint dropPosition(static_cast<int>(e->pos().x()), static_cast<int>(e->pos().y()));
#else
    QPoint dropPosition(static_cast<int>(e->position().x()), static_cast<int>(e->position().y()));
#endif
    pItem = itemFromPosition(dropPosition,firstNodeI);

    if(mimeData->hasFormat("application/blackomega-urllist"))
    {
        QByteArray mArray;
        QVector<QPair<track::info::InfoSPtr,int> > infoList;
        
        mArray = mimeData->data("application/blackomega-urllist");
        urlListFromArray(mArray,urlList);
        
        for(ppI=urlList.begin();ppI!=urlList.end();++ppI)
        {
            int cIdx;
            QString fName;
            
            cIdx = getURLChild(*ppI,fName);
            if(!fName.isEmpty() && common::DiskOps::exist(fName))
            {
                track::info::InfoSPtr fInfo = track::db::DBInfo::readInfo(fName);
                
                if(fInfo.data()!=0)
                {
                    infoList.append(QPair<track::info::InfoSPtr,int>(fInfo,cIdx));
                }
            }
        }
        
        if(infoList.size()>0)
        {
            m_addNoUpdateFlag = true;
            addTracks(infoList,pItem);
            m_addNoUpdateFlag = false;
        }
    }
    else
    {
        int dCount;
        QVector<track::info::InfoSPtr> infoList;
        track::db::TrackFileDependencies dependency;
    
        urlList = mimeData->urls();
        
        activeProgress();
        m_dirFileTotal = 0;

#if defined(OMEGA_MAC_STORE)        
        dropEventProcessBookmarks(urlList);
#endif

        for(ppI=urlList.begin();ppI!=urlList.end();++ppI)
        {
            QString fName;
            const QUrl& url = *ppI;
            
            fName = url.toLocalFile();
            if(!fName.isEmpty())
            {
                switch(common::DiskOps::fileType(fName))
                {
                    case 1:
                        if(track::info::Info::isSupported(fName) || track::db::PlaylistAbstractIO::isSupported(fName))
                        {
                            m_dirFileTotal++;
                        }
                        break;
                        
                    case 2:
                        dCount = 0;
                        countDirectoryR(fName,true,dCount,dependency);
                        m_dirFileTotal += dCount;
                        break;
                        
                    default:
                        break;
                }
            }
        }
        
        m_dirFileCurrent = 0;
        if(m_dirFileTotal>0)
        {
            for(ppI=urlList.begin();ppI!=urlList.end() && !m_progressControl->isCancelled();++ppI)
            {
                QString fName;
                const QUrl& url = *ppI;
                
                fName = url.toLocalFile();
                if(!fName.isEmpty())
                {
                    switch(common::DiskOps::fileType(fName))
                    {
                        case 1:
                            if(track::info::Info::isSupported(fName))
                            {
                                track::info::InfoSPtr fInfo = track::db::DBInfo::readInfo(fName);
                                m_dirFileCurrent++;
                                tfloat32 cProgress = static_cast<tfloat32>(m_dirFileCurrent * 100.0f) / static_cast<tfloat32>(m_dirFileTotal);
                                m_progressControl->setProgress(cProgress);
                                QCoreApplication::processEvents(QEventLoop::AllEvents);
                                if(fInfo.data()!=0)
                                {
                                    infoList.append(fInfo);
                                }
                            }
                            else if(track::db::PlaylistAbstractIO::isSupported(fName))
                            {
                                QSharedPointer<track::db::PlaylistAbstractIO> pLoader = track::db::PlaylistIOFactory::createShared(track::db::PlaylistAbstractIO::factoryName(fName));
                                if(pLoader.data()!=0)
                                {
                                    QVector<track::info::InfoSPtr> pList;
                                    if(pLoader->load(fName,pList,m_progressControl))
                                    {
                                        addTracks(pList,pItem,false);
                                    }
                                }
                                m_dirFileCurrent++;
                                tfloat32 cProgress = static_cast<tfloat32>(m_dirFileCurrent * 100.0f) / static_cast<tfloat32>(m_dirFileTotal);
                                m_progressControl->setProgress(cProgress);
                            }
                            break;
                        
                        case 2:
                            addDirectoryR(fName,true,infoList);
                            break;
                            
                        default:
                            break;
                    }
                }
            }
        }
        deactivateProgress();
        
        if(infoList.size()>0 && !m_progressControl->isCancelled())
        {
            addTracks(infoList,pItem);
        }

        if(firstNodeI>=0 && firstNodeI<m_viewList.size())
        {
            QPLItemBase *vItem = m_viewList[firstNodeI];
            getVerticalScrollBar()->setValue(vItem->viewHeight());
        }
    }
    e->acceptProposedAction();
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::itemFromPosition(const QPoint& p,int& firstNodeI)
{
    tint yPos,treeNodeI;
    QPLItemBase *pItem;
    
    yPos = getVerticalScrollBar()->value();
    firstNodeI = viewIndexFromPosition(yPos);
    
    yPos = p.y();
    treeNodeI = viewIndexFromPosition(yPos);
    if(treeNodeI>=0 && treeNodeI<m_viewList.size())
    {
        pItem  = m_viewList.at(treeNodeI);
        if(yPos < (pItem->viewHeight() + (pItem->height() / 2)))
        {
            if(treeNodeI>0)
            {
                pItem = m_viewList.at(treeNodeI - 1);
            }
            else
            {
                pItem = 0;
            }
        }
    }
    else
    {
        pItem = 0;
    }
    return pItem;
}

//-------------------------------------------------------------------------------------------

int QPlaylistWidget::getURLChild(const QUrl& url,QString& fileName) const
{
    int i,idx = -1;

    fileName = url.toLocalFile();
    fileName = QUrl::fromPercentEncoding(fileName.toUtf8());
    if(!fileName.isEmpty())
    {
        int state = 0,sLen = fileName.length() - 1;

        for(i=fileName.length()-1;i>=0 && state>=0 && state<2;i--)
        {
            switch(state)
            {
                case 0:
                    if(!(fileName.at(i)>='0' && fileName.at(i)<='9'))
                    {
                        sLen = i + 1;
                        state = (fileName.at(i)=='$') ? 1 : -1;
                    }
                    break;

                case 1:
                    state = (fileName.at(i)=='$') ? 2 : -1;
                    break;
            }
        }
        if(state==2)
        {
            idx = fileName.mid(sLen).toInt();
            if(idx>=0)
            {
                fileName = fileName.mid(0,sLen-2);
            }
            else
            {
                idx = -1;
            }
        }
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isSplitRequired(QPLItemBase *splitAfterItem,QList<QPLItemBase *>& insertTrackList)
{
    bool res;

    if(splitAfterItem==0)
    {
        res = false;
    }
    else if(splitAfterItem->type()==QPLItemBase::e_Album || splitAfterItem->type()==QPLItemBase::e_AlbumTrack)
    {
        QList<QPLItemBase *>::iterator ppI;
        QString sKey = albumKey(splitAfterItem);
        bool flag = false;

        for(ppI=insertTrackList.begin();ppI!=insertTrackList.end() && !flag;++ppI)
        {
            QPLItemBase *cItem = *ppI;

            if(sKey!=albumKey(cItem))
            {
                flag = true;
            }
        }
        res = flag;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::splitUpdateCurrent(QPLItemBase *item)
{
    if(item->type()==QPLItemBase::e_AlbumTrack || item->type()==QPLItemBase::e_Single)
    {
        if(!m_currentPlayNameDrag.first.isEmpty() && QUrl::fromLocalFile(item->info()->getFilename())==m_currentPlayNameDrag.first && item->subTrackIndex()==m_currentPlayNameDrag.second)
        {
            m_currentPlayItem = item;
        }
    }
    else if(item->isChildren())
    {
        int i;

        for(i=0;i<item->noChildren();i++)
        {
            splitUpdateCurrent(item->child(i));
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::splitAlbumAndInsert(QPLItemBase *splitAfterItem,QList<QPLItemBase *>& insertTrackList)
{
    tint i,j,idx;
    QList<QPLItemBase *>::iterator ppI;
    bool appendFlag = true;
    
    if(splitAfterItem->type()==QPLItemBase::e_Album)
    {
        QPLItemBase *pItem = splitAfterItem->parent();
        
        if(pItem!=0)
        {
            idx = pItem->childIndexOf(splitAfterItem) + 1;
            if(idx>=0)
            {
                for(ppI=insertTrackList.begin();ppI!=insertTrackList.end();++ppI)
                {
                    QPLItemBase *cItem = *ppI;
                    
                    if(appendFlag)
                    {
                        if(albumKey(splitAfterItem)==albumKey(cItem))
                        {
                            if(cItem->type()==QPLItemBase::e_Single)
                            {
                                QPLItemATrack *aTItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(splitAfterItem),cItem->info(),cItem->subTrackIndex());
                                splitUpdateCurrent(aTItem);
                                delete cItem;
                            }
                            else if(cItem->type()==QPLItemBase::e_AlbumTrack)
                            {
                                splitAfterItem->addChild(cItem);
                                splitUpdateCurrent(cItem);
                            }
                        }
                        else
                        {
                            pItem->insertChild(idx,cItem);
                            splitUpdateCurrent(cItem);
                            appendFlag = false;
                            idx++;
                        }
                    }
                    else
                    {
                        pItem->insertChild(idx,cItem);
                        splitUpdateCurrent(cItem);
                        idx++;
                    }
                }
            }
        }
        else
        {
            idx = m_playRootList.indexOf(splitAfterItem);
            if(idx>=0)
            {
                for(i=insertTrackList.size()-1;i>=0;i--)
                {
                    QPLItemBase *bItem = m_playRootList[idx];
                    QPLItemBase *cItem = insertTrackList[i];
                    
                    if(cItem->type()==QPLItemBase::e_Album)
                    {
                        if(bItem->type()==QPLItemBase::e_Album && albumKey(bItem)==albumKey(cItem))
                        {
                            for(j=cItem->noChildren()-1;j>=0;j--)
                            {
                                QPLItemATrack *aTItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(bItem),0,cItem->child(j)->info(),cItem->child(j)->subTrackIndex());
                                splitUpdateCurrent(aTItem);
                            }
                            delete cItem;
                        }
                        else
                        {
                            m_playRootList.insert(idx,cItem);
                        }
                    }
                    else if(cItem->type()==QPLItemBase::e_AlbumTrack)
                    {
                        if(bItem->type()==QPLItemBase::e_Album && albumKey(bItem)==albumKey(cItem))
                        {
                            new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(bItem),0,cItem->info(),cItem->subTrackIndex());
                        }
                        else
                        {
                            QPLItemSingle *sItem = new QPLItemSingle(this,cItem->info(),cItem->subTrackIndex());
                            m_playRootList.insert(idx,sItem);
                        }
                        delete cItem;
                    }
                    else if(cItem->type()==QPLItemBase::e_Single)
                    {
                        if(bItem->type()==QPLItemBase::e_Album && albumKey(bItem)==albumKey(cItem))
                        {
                            new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(bItem),0,cItem->info(),cItem->subTrackIndex());
                            delete cItem;
                        }
                        else
                        {
                            m_playRootList.insert(idx,cItem);
                        }
                    }
                }
            }
        }
    }
    else if(splitAfterItem->type()==QPLItemBase::e_AlbumTrack)
    {
        QPLItemBase *aItem = splitAfterItem->parent();
        
        if(aItem!=0 && aItem->type()==QPLItemBase::e_Album)
        {
            if(aItem->parent()!=0)
            {
                QPLItemBase *cItem = splitAfterItem->nextSibling();
                QPLItemBase *pItem = aItem->parent();
                
                idx = aItem->parent()->childIndexOf(splitAfterItem);
                if(idx>=0)
                {
                    if(cItem!=0)
                    {
                        tint oIdx,i=0;
                        QPLItemAlbum *nItem = new QPLItemAlbum(this,aItem->info());
                        
                        while(cItem!=0)
                        {
                            QPLItemBase *mItem = cItem;
                            cItem = mItem->nextSibling();
                            aItem->delChild(mItem);
                            nItem->insertChild(i++,mItem);
                            splitUpdateCurrent(mItem);
                        }
                        
                        idx++;
                        oIdx = idx;
                        for(ppI=insertTrackList.begin();ppI!=insertTrackList.end();++ppI)
                        {
                            QPLItemBase *cItem = *ppI;
                            
                            if(appendFlag)
                            {
                                if(albumKey(splitAfterItem)==albumKey(cItem))
                                {
                                    if(cItem->type()==QPLItemBase::e_Single)
                                    {
                                        QPLItemATrack *aTItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(aItem),cItem->info(),cItem->subTrackIndex());
                                        splitUpdateCurrent(aTItem);
                                        delete cItem;
                                    }
                                    else if(cItem->type()==QPLItemBase::e_AlbumTrack)
                                    {
                                        aItem->addChild(cItem);
                                        splitUpdateCurrent(cItem);
                                    }
                                    else if(cItem->type()==QPLItemBase::e_Album)
                                    {
                                        for(tint j=0;j<cItem->noChildren();j++)
                                        {
                                            QPLItemBase *sCItem = cItem->child(j);
                                            splitAfterItem->addChild(sCItem);
                                            splitUpdateCurrent(sCItem);
                                        }
                                    }
                                }
                                else
                                {
                                    pItem->insertChild(idx,cItem);
                                    splitUpdateCurrent(cItem);
                                    appendFlag = false;
                                    idx++;
                                }
                            }
                            else
                            {
                                pItem->insertChild(idx,cItem);
                                splitUpdateCurrent(cItem);
                                idx++;
                            }
                        }
                        if(idx>oIdx && nItem->isChildren())
                        {
                            pItem->insertChild(idx,nItem);
                            splitUpdateCurrent(nItem);
                        }
                        else
                        {
                            while(nItem->noChildren()>0)
                            {
                                cItem = nItem->child(0);
                                nItem->delChild(cItem);
                                aItem->addChild(cItem);
                                splitUpdateCurrent(cItem);
                            }
                            delete nItem;
                        }
                    }
                    else
                    {
                        splitAlbumAndInsert(aItem,insertTrackList);
                    }
                }
            }
            else
            {
                QPLItemBase *cItem = splitAfterItem->nextSibling();
                
                idx = m_playRootList.indexOf(aItem);
                if(idx>=0)
                {
                    if(cItem!=0)
                    {
                        tint oIdx,i=0;
                        QPLItemAlbum *nItem = new QPLItemAlbum(this,aItem->info());
                        
                        while(cItem!=0)
                        {
                            QPLItemBase *mItem = cItem;
                            cItem = mItem->nextSibling();
                            aItem->delChild(mItem);
                            nItem->insertChild(i++,mItem);
                            splitUpdateCurrent(mItem);
                        }
                        
                        idx++;
                        oIdx = idx;
                        for(ppI=insertTrackList.begin();ppI!=insertTrackList.end();++ppI)
                        {
                            QPLItemBase *iItem = *ppI;
                            
                            if(appendFlag)
                            {
                                if(albumKey(splitAfterItem)==albumKey(iItem))
                                {
                                    if(iItem->type()==QPLItemBase::e_Single)
                                    {
                                        QPLItemATrack *aTItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(aItem),iItem->info(),iItem->subTrackIndex());
                                        splitUpdateCurrent(aTItem);
                                        delete iItem;
                                    }
                                    else if(iItem->type()==QPLItemBase::e_AlbumTrack)
                                    {
                                        aItem->addChild(iItem);
                                        splitUpdateCurrent(iItem);
                                    }
                                }
                                else
                                {
                                    m_playRootList.insert(idx,iItem);
                                    splitUpdateCurrent(iItem);
                                    appendFlag = false;
                                    idx++;
                                }
                            }
                            else
                            {
                                m_playRootList.insert(idx,iItem);
                                splitUpdateCurrent(iItem);
                                idx++;
                            }
                        }
                        if(idx>oIdx && nItem->isChildren())
                        {
                            m_playRootList.insert(idx,nItem);
                            splitUpdateCurrent(nItem);
                        }
                        else
                        {
                            while(nItem->noChildren()>0)
                            {
                                cItem = nItem->child(0);
                                nItem->delChild(cItem);
                                aItem->addChild(cItem);
                                splitUpdateCurrent(cItem);
                            }
                            delete nItem;
                        }
                    }
                    else
                    {
                        idx = m_playRootList.indexOf(aItem);
                        for(i=0;i<insertTrackList.size();i++)
                        {
                            QPLItemBase *eItem = insertTrackList[i];
                            
                            if(aItem->type()==QPLItemBase::e_Album && albumKey(aItem)==albumKey(eItem))
                            {
                                if(eItem->type()==QPLItemBase::e_Album)
                                {
                                    for(j=0;j<eItem->noChildren();j++)
                                    {
                                        QPLItemATrack *aTItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(aItem),aItem->noChildren(),eItem->child(j)->info(),eItem->child(j)->subTrackIndex());
                                        splitUpdateCurrent(aTItem);
                                    }
                                    delete eItem;
                                }
                                else
                                {
                                    QPLItemATrack *aTItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(aItem),aItem->noChildren(),eItem->info(),eItem->subTrackIndex());
                                    splitUpdateCurrent(aTItem);
                                    delete eItem;
                                }
                            }
                            else
                            {
                                idx++;
                                m_playRootList.insert(idx,eItem);
                                splitUpdateCurrent(eItem);
                            }
                            aItem = m_playRootList[idx];
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::cleanUpPlaylist()
{
    tint i;
    QPLItemBase *aItem = 0,*bItem;
    QList<QPLItemBase *>::iterator ppI;

    for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();)
    {
        bItem = *ppI;
        if(bItem->type()==QPLItemBase::e_Album && !bItem->isChildren())
        {
            ppI = m_playRootList.erase(ppI);
            delete bItem;
        }
        else
        {
            ++ppI;
        }
    }
    
    for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();)
    {
        bItem = *ppI;
        if(aItem!=0)
        {
            if(albumKey(aItem)==albumKey(bItem))
            {
                if(aItem->type()==QPLItemBase::e_Album)
                {
                    if(bItem->type()==QPLItemBase::e_Album)
                    {
                        while(bItem->isChildren())
                        {
                            QPLItemBase *cItem = bItem->child(0);
                            bItem->delChild(cItem);
                            aItem->addChild(cItem);
                        }
                    }
                    else
                    {
                        QPLItemATrack *tItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(aItem),bItem->info(),bItem->subTrackIndex());
                        if(bItem==m_currentPlayItem)
                        {
                            m_currentPlayItem = tItem;
                        }
                    }
                    ppI = m_playRootList.erase(ppI);
                    delete bItem;
                }
                else if(aItem->type()==QPLItemBase::e_Single)
                {
                    if(bItem->type()==QPLItemBase::e_Album)
                    {
                        QPLItemATrack *tItem = new QPLItemATrack(this,reinterpret_cast<QPLItemAlbum *>(bItem),0,aItem->info(),aItem->subTrackIndex());
                        --ppI;
                        ppI = m_playRootList.erase(ppI);
                        ++ppI;
                        if(aItem==m_currentPlayItem)
                        {
                            m_currentPlayItem = tItem;
                        }
                        delete aItem;
                        aItem = bItem;
                    }
                    else
                    {
                        QPLItemAlbum *nItem = new QPLItemAlbum(this,aItem->info());
                        QPLItemATrack *cAItem = new QPLItemATrack(this,nItem,aItem->info(),aItem->subTrackIndex());
                        QPLItemATrack *cBItem = new QPLItemATrack(this,nItem,bItem->info(),aItem->subTrackIndex());
                        --ppI;
                        ppI = m_playRootList.erase(ppI);
                        ppI = m_playRootList.erase(ppI);
                        ppI = m_playRootList.insert(ppI,nItem);
                        ++ppI;
                        if(aItem==m_currentPlayItem)
                        {
                            m_currentPlayItem = cAItem;
                        }
                        if(bItem==m_currentPlayItem)
                        {
                            m_currentPlayItem = cBItem;
                        }
                        delete aItem;
                        delete bItem;
                        aItem = nItem;
                    }
                }
            }
            else
            {
                aItem = bItem;
                ++ppI;
            }
        }
        else
        {
            aItem = bItem;
            ++ppI;
        }
    }

    for(i=0;i<m_playRootList.size();i++)
    {
        aItem = m_playRootList[i];
        if(aItem->type()==QPLItemBase::e_Album && aItem->noChildren()<2)
        {
            QPLItemSingle *sItem = new QPLItemSingle(this,aItem->child(0)->info(),aItem->child(0)->subTrackIndex());
            m_playRootList.replace(i,sItem);
            if(aItem->child(0)==m_currentPlayItem)
            {
                m_currentPlayItem = sItem;
            }
            delete aItem;
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::setFont(const QFont& f,tint size)
{
    clearFonts();
    
    m_darkFont = new QFont(f.family(),size);
    m_mediumFont = new QFont(f.family(),size - 2);
    m_lightFont = new QFont(f.family(),size - 4);
    m_darkFontMetric = new QFontMetricsF(*m_darkFont);
    m_lightFont->setWeight(QFont::Light);
    m_lightFontMetric = new QFontMetricsF(*m_lightFont);
    m_mediumFontMetric = new QFontMetricsF(*m_mediumFont);
    m_timeColumnWidth = m_darkFontMetric->horizontalAdvance("0:00:00") + 5.0;
    m_trackColumnWidth = (((m_darkFontMetric->height() * 2.0) + 4.0) * 1.112);
    buildViewList();
    
    repaint();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::clearFonts()
{
    if(m_lightFont!=0)
    {
        delete m_lightFont;
        m_lightFont = 0;
    }
    if(m_lightFontMetric!=0)
    {
        delete m_lightFontMetric;
        m_lightFontMetric = 0;
    }

    if(m_mediumFont!=0)
    {
        delete m_mediumFont;
        m_mediumFont = 0;
    }
    if(m_mediumFontMetric!=0)
    {
        delete m_mediumFontMetric;
        m_mediumFontMetric = 0;
    }

    if(m_darkFont!=0)
    {
        delete m_darkFont;
        m_darkFont = 0;
    }
    if(m_darkFontMetric!=0)
    {
        delete m_darkFontMetric;
        m_darkFontMetric = 0;
    }    
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isDragPreItem(QPLItemBase *item) const
{
    bool res;

    if(m_currentDragPosition>0 && m_currentDragPosition==(item->viewPosition() + 1))
    {
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isDragPostItem(QPLItemBase *item) const
{
    bool res;

    if(m_currentDragPosition>=0 && m_currentDragPosition==item->viewPosition())
    {
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::setDragIcon(QDrag *drag,QVector<QPLItemBase *>& tList)
{
    tint wPic=0,hPic=0,pCount = 0;
    QSet<QString> albumSet;
    QVector<QPLItemBase *>::iterator ppI;
    QPLItemBase *item;
    
    for(ppI=tList.begin();ppI!=tList.end() && pCount<4;ppI++)
    {
        QString aKey;
        tint w,h;
        
        item = *ppI;
        aKey = albumKey(item);
        if(albumSet.find(aKey)==albumSet.end())
        {
            albumSet.insert(aKey);
            pCount++;
        }

        while(item->parent()!=0)
        {
            item = item->parent();
        }
        w = static_cast<tint>(floor(static_cast<qreal>(item->height()) * 1.112));
        h = item->height() - 1;
        if(wPic<w)
        {
            wPic = w;
        }
        if(hPic<h)
        {
            hPic = h;
        }
    }
    
    QImage dImage(wPic + ((pCount-1) * 4),hPic + ((pCount-1) * 4),QImage::Format_ARGB32);
    QPainter painter(&dImage);
    painter.fillRect(0,0,wPic+16,hPic+16,QColor(255,255,255,0));
    painter.setOpacity(0.75);

    pCount = 0;
    albumSet.clear();
    
    for(ppI=tList.begin();ppI!=tList.end() && pCount<4;ppI++)
    {
        QString aKey;
        
        item = *ppI;
        aKey = albumKey(item);
        if(albumSet.find(aKey)==albumSet.end())
        {
            QImage *img = item->getImageIcon();
            if(img!=0)
            {
                painter.drawImage(pCount<<2,pCount<<2,*img);
                albumSet.insert(aKey);
                pCount++;
            }
        }
    }
    drag->setPixmap(QPixmap::fromImage(dImage));
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::currentPlayItem()
{
    return m_currentPlayItem;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::setCurrentPlayItem(QPLItemBase *item)
{
    if(item->type()==QPLItemBase::e_Album && item->isChildren())
    {
        item = item->child(0);
    }
    m_currentPlayItem = item;
    if(m_playerDialog!=0)
    {
        if(m_playerDialog->isShuffle())
        {
            removeTracksShuffleList(item);
        }
    }
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::currentQueuePlayItem()
{
    return m_currentPlayQueueItem;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::setCurrentQueuePlayItem(QPLItemBase *item)
{
    m_currentPlayQueueItem = item;
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::previousPlayItem()
{
    QPLItemBase *cItem = currentPlayItem();
    return previousPlayItem(cItem);
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::previousPlayItem(QPLItemBase *cItem)
{
    QPLItemBase *sItem;
    
    while(cItem!=0)
    {
        if(cItem->type()==QPLItemBase::e_Album)
        {
            cItem = cItem->prevSibling();
            
            while(cItem!=0 && cItem->isChildren() && cItem->type()==QPLItemBase::e_Album)
            {
                cItem = cItem->child(cItem->noChildren() - 1);
            }
        }
        else
        {
            sItem = cItem->prevSibling();

            if(sItem!=0)
            {
                cItem = sItem;

                if(cItem->type()==QPLItemBase::e_Album)
                {
                    while(cItem!=0 && cItem->isChildren() && cItem->type()==QPLItemBase::e_Album)
                    {
                        cItem = cItem->child(cItem->noChildren() - 1);
                    }                    
                }
            }
            else
            {
                cItem = cItem->parent();
            }
        }
        
        if(cItem!=0 && (cItem->type()==QPLItemBase::e_AlbumTrack || cItem->type()==QPLItemBase::e_Single))
        {
            break;
        }
    }
    return cItem;
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::nextPlayItem()
{
    QPLItemBase *cItem = currentPlayItem();
    return nextPlayItem(cItem);
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::nextPlayItem(QPLItemBase *cItem)
{
    while(cItem!=0)
    {
        if(cItem->type()==QPLItemBase::e_Album && cItem->isChildren())
        {
            cItem = cItem->child(0);
        }
        else
        {
            bool loop = true;
            
            while(loop && cItem!=0)
            {
                QPLItemBase *sItem = cItem->nextSibling();
                
                if(sItem!=0)
                {
                    cItem = sItem;
                    loop = false;
                }
                else
                {
                    if(cItem->parent()!=0)
                    {
                        cItem = cItem->parent();
                    }
                    else
                    {
                        cItem = 0;
                    }
                }
            }
        }
        
        if(cItem!=0 && (cItem->type()==QPLItemBase::e_AlbumTrack || cItem->type()==QPLItemBase::e_Single))
        {
            break;
        }
    }
    return cItem;
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isPlay() const
{
    bool res;

    if(m_playerDialog!=0)
    {
        res =(m_playerDialog->state()==Player::e_Play) ? true : false;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isPaused() const
{
    bool res;

    if(m_playerDialog!=0)
    {
        res = (m_playerDialog->state()==Player::e_Pause) ? true : false;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::doCopyCut(bool cutFlag)
{
    QVector<QPLItemBase *> tList;
    QList<QUrl> urlData;
    QVector<QPair<track::info::InfoSPtr,int> > infoList;
    QVector<QPair<track::info::InfoSPtr,int> >::iterator ppI;
    QPLItemBase *nPItem;
    
    nPItem = getSelectedItems(tList);
    buildInfoList(tList,infoList);
    for(ppI=infoList.begin();ppI!=infoList.end();ppI++)
    {
        QUrl u;
        QString fName;
        track::info::InfoSPtr info = (*ppI).first;
        if((*ppI).second>=0)
        {
            fName = info->getFilename() + "$$" + QString::number((*ppI).second);
        }
        else
        {
            fName = info->getFilename();
        }
        fName = QString::fromUtf8(QUrl::toPercentEncoding(fName));
        u = QUrl::fromLocalFile(fName);
        urlData.append(u);
    }
    
    if(urlData.size()>0)
    {
        QMimeData *mimeData = new QMimeData;
        QByteArray mArray;
        urlListToArray(urlData,mArray);
        mimeData->setData("application/blackomega-urllist",mArray);
        
        if(cutFlag)
        {
            if(nPItem!=m_currentPlayItem)
            {
                m_currentPlayItem = nPItem;
                emit onPlay();
            }
            removeTracks(tList);
        }
        
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimeData);
    }
    selectionChanged();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::doCut()
{
    doCopyCut(true);
    initCurrentPlay();
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::doCopy()
{
    doCopyCut(false);
    initCurrentPlay();
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::doDelete()
{
    QVector<QPLItemBase *> tList;
    QPLItemBase *nPItem;
    
    nPItem = getSelectedItems(tList);
    if(nPItem!=m_currentPlayItem)
    {
        m_currentPlayItem = nPItem;
        emit onPlay();
    }
    removeTracks(tList);
    initCurrentPlay();
    selectionChanged();
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::doPaste()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData!=0)
    {
        int i,j;
        QPLItemBase *pItem = 0;
        QList<QUrl> urlList;
        QList<QUrl>::const_iterator ppI;

        if(m_playRootList.size()>0)
        {
            for(i=m_playRootList.size()-1;i>=0 && pItem==0;i--)
            {
                QPLItemBase *hItem = m_playRootList[i];
                if(hItem->isChildren())
                {
                    for(j=hItem->noChildren()-1;j>=0 && pItem==0;j--)
                    {
                        QPLItemBase *cItem = hItem->child(j);
                        if(cItem->isSelected())
                        {
                            pItem = cItem;
                        }
                    }
                }
                if(pItem==0 && hItem->isSelected())
                {
                    pItem = hItem;
                }
            }
            if(pItem==0)
            {
                pItem = m_playRootList[m_playRootList.size() - 1];
                if(pItem->isChildren())
                {
                    pItem = pItem->child(pItem->noChildren() - 1);
                }
            }
        }
        
        if(mimeData->hasFormat("application/blackomega-urllist"))
        {
            QByteArray mArray = mimeData->data("application/blackomega-urllist");
            QVector<QPair<track::info::InfoSPtr,int> > infoList;
            
            urlListFromArray(mArray,urlList);
            
            for(ppI=urlList.begin();ppI!=urlList.end();++ppI)
            {
                int cIdx;
                QString fName;
                
                cIdx = getURLChild(*ppI,fName);
                if(!fName.isEmpty() && common::DiskOps::exist(fName))
                {
                    track::info::InfoSPtr fInfo = track::db::DBInfo::readInfo(fName);
                    if(fInfo.data()!=0)
                    {
                        infoList.append(QPair<track::info::InfoSPtr,int>(fInfo,cIdx));
                    }
                }
            }            

            if(infoList.size()>0)
            {
                addTracks(infoList,pItem);
            }
        }
#if !defined(OMEGA_MAC_STORE)
        else
        {
            int dCount;
            QVector<track::info::InfoSPtr> infoList;
            track::db::TrackFileDependencies dependency;
            
            urlList = mimeData->urls();
            
            activeProgress();
            m_dirFileTotal = 0;
            for(ppI=urlList.begin();ppI!=urlList.end();++ppI)
            {
                QString fName;
                const QUrl& url = *ppI;
                
                fName = url.toLocalFile();
                if(!fName.isEmpty())
                {
                    switch(common::DiskOps::fileType(fName))
                    {
                        case 1:
                            if(track::info::Info::isSupported(fName) || track::db::PlaylistAbstractIO::isSupported(fName))
                            {
                                m_dirFileTotal++;
                            }
                            break;
                        
                        case 2:
                            dCount = 0;
                            countDirectoryR(fName,true,dCount,dependency);
                            m_dirFileTotal += dCount;
                            break;
                        
                        default:
                            break;
                    }
                }
            }
        
            m_dirFileCurrent = 0;
            if(m_dirFileTotal>0)
            {
                for(ppI=urlList.begin();ppI!=urlList.end() && !m_progressControl->isCancelled();++ppI)
                {
                    QString fName;
                    const QUrl& url = *ppI;
                    
                    fName = url.toLocalFile();
                    if(!fName.isEmpty())
                    {
                        switch(common::DiskOps::fileType(fName))
                        {
                            case 1:
                                if(track::info::Info::isSupported(fName))
                                {
                                    track::info::InfoSPtr fInfo = track::db::DBInfo::readInfo(fName);
                                    m_dirFileCurrent++;
                                    tfloat32 cProgress = static_cast<tfloat32>(m_dirFileCurrent * 100.0f) / static_cast<tfloat32>(m_dirFileTotal);
                                    m_progressControl->setProgress(cProgress);
                                    QCoreApplication::processEvents(QEventLoop::AllEvents);
                                    if(fInfo.data()!=0)
                                    {
                                        infoList.append(fInfo);
                                    }
                                }
                                else if(track::db::PlaylistAbstractIO::isSupported(fName))
                                {
                                    QSharedPointer<track::db::PlaylistAbstractIO> pLoader = track::db::PlaylistIOFactory::createShared(track::db::PlaylistAbstractIO::factoryName(fName));
                                    if(pLoader.data()!=0)
                                    {
                                        QVector<track::info::InfoSPtr> pList;
                                        if(pLoader->load(fName,pList,m_progressControl))
                                        {
                                            addTracks(pList,pItem,false);
                                        }
                                    }
                                    m_dirFileCurrent++;
                                    tfloat32 cProgress = static_cast<tfloat32>(m_dirFileCurrent * 100.0f) / static_cast<tfloat32>(m_dirFileTotal);
                                    m_progressControl->setProgress(cProgress);
                                }
                                break;
                            
                            case 2:
                                addDirectoryR(fName,true,infoList);
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
            }
            deactivateProgress();
            
            if(infoList.size()>0 && !m_progressControl->isCancelled())
            {
                addTracks(infoList,pItem);
            }
        }
#endif
    }
    initCurrentPlay();
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::onCut()
{
    doCut();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::onCopy()
{
    doCopy();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::onDelete()
{
    doDelete();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::onPaste()
{
    doPaste();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::contextMenuEvent(QContextMenuEvent *e)
{
    if(!m_progressControl->isActive())
    {
        QMenu menu(this);
        PlayerController::instance()->createContextMenu(menu,isPaste());
        menu.exec(e->globalPos());
    }
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::loadPlaylistFromDB(int playlistID,bool appendFlag,QString& name)
{
    QPLItemBase *pItem = 0;
    track::db::TrackDB *db = track::db::TrackDB::instance();
    bool res = false;
    
    if(db!=0)
    {
        QVector<QPair<track::info::InfoSPtr,int> > trackList;
        
        name = db->playlist(playlistID);
        if(db->loadPlaylist(playlistID, trackList))
        {
            if(!appendFlag)
            {
                QList<QPLItemBase *>::iterator ppJ;
                for(ppJ=m_playRootList.begin();ppJ!=m_playRootList.end();++ppJ)
                {
                    QPLItemBase *item = *ppJ;
                    delete item;
                }
                m_playRootList.clear();
                m_viewList.clear();
                pItem = 0;
            }
            else
            {
                if(m_playRootList.size()>0)
                {
                    pItem = m_playRootList[m_playRootList.size()-1];
                    if(pItem->isChildren())
                    {
                        pItem = pItem->child(pItem->noChildren()-1);
                    }
                }
                else
                {
                    pItem = 0;
                }
            }
            
            addTracks(trackList,pItem);
            initCurrentPlay();
            applyAndEmitUpdate();
            
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int QPlaylistWidget::savePlaylistToDB(int playlistID,const QString& name,bool selectFlag)
{
    track::db::TrackDB *db = track::db::TrackDB::instance();
    QVector<QPair<track::info::InfoSPtr,int> > trackList;
    QPLItemBase *cItem;
    int resID = -1;
    
    if(m_playRootList.size()>0)
    {
        cItem = m_playRootList[0];
    }
    else
    {
        cItem = 0;
    }
            
    while(cItem!=0)
    {
        if(!(selectFlag && !cItem->isSelected()))
        {
            if(cItem->type()!=QPLItemBase::e_Album)
            {
                trackList.append(QPair<track::info::InfoSPtr,int>(cItem->info(), cItem->subTrackIndex()));
            }
        }
        if(cItem->isChildren())
        {
            cItem = cItem->child(0);
        }
        else
        {
            bool loop = true;
            
            while(loop && cItem!=0)
            {
                QPLItemBase *sItem = cItem->nextSibling();
                
                if(sItem!=0)
                {
                    cItem = sItem;
                    loop = false;
                }
                else
                {
                    if(cItem->parent()!=0)
                    {
                        cItem = cItem->parent();
                    }
                    else
                    {
                        cItem = 0;
                    }
                }
            }
        }
    }
    return db->replacePlaylist(playlistID, name, trackList);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::loadCurrentPlaylist()
{
    QString name;
    loadPlaylistFromDB(0,false,name);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::saveCurrentPlaylist()
{
    savePlaylistToDB(0,"default",false);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::doSelectAll()
{
    onSelectAll();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::onSelectAll()
{
    QList<QPLItemBase *>::iterator ppI;

    for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
    {
        QPLItemBase *item = *ppI;
        item->setSelected(true);
        m_lastSelectItem = 0;
    }
    repaint();
    selectionChanged();
}

//-------------------------------------------------------------------------------------------

int QPlaylistWidget::noPlayItems() const
{
    int count = 0;
    QList<QPLItemBase *>::const_iterator ppI;

    for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
    {
        count += noPlayItemsR(*ppI);
    }
    return count;
}

//-------------------------------------------------------------------------------------------

int QPlaylistWidget::noPlayItemsR(const QPLItemBase *pItem) const
{
    int count = 0;

    if(pItem!=0)
    {
        if(pItem->type()==QPLItemBase::e_AlbumTrack || pItem->type()==QPLItemBase::e_Single)
        {
            count++;
        }
        if(pItem->isChildren())
        {
            int i;

            for(i=0;i<pItem->noChildren();i++)
            {
                count += noPlayItemsR(pItem->child(i));
            }
        }
    }
    return count;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::activeProgress()
{
    if(!m_progressControl->isActive())
    {
        PlayerController::instance()->progressStart();
        m_progressControl->activate();
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::deactivateProgress()
{
    if(m_progressControl->isActive())
    {
        selectionChanged();
        PlayerController::instance()->progressEnd(isPaste());
        m_progressControl->deactivate();
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::urlListToArray(const QList<QUrl>& urlList,QByteArray& arr)
{
    QString xStr;
    QStringList x;
    QList<QUrl>::const_iterator ppI;
    
    x << "<urllist>";
    for(ppI=urlList.begin();ppI!=urlList.end();++ppI)
    {
        x << "<url><![CDATA[";
        x << (*ppI).toString();
        x << "]]></url>";
    }
    x << "</urllist>";
    xStr = x.join("");
    arr = xStr.toUtf8();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::urlListFromArray(const QByteArray& arr,QList<QUrl>& urlList)
{
    QString xStr = QString::fromUtf8(arr.constData(),arr.size());
    
    urlList.clear();
    if(!xStr.isEmpty())
    {
        xmlDocPtr doc;
        QByteArray iMem;
        
        iMem = xStr.toUtf8();
        doc = xmlParseMemory(iMem.constData(),iMem.length());
        if(doc!=0)
        {
            xmlNode *rNode = xmlDocGetRootElement(doc);
            if(rNode!=0 && rNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(rNode->name)).toLower()=="urllist")
            {
                xmlNode *cNode = rNode->children;
                while(cNode!=0)
                {
                    if(cNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(cNode->name)).toLower()=="url")
                    {
                        xmlNode *tNode = cNode->children;
                        
                        if(tNode!=0 && (tNode->type==XML_TEXT_NODE || tNode->type==XML_CDATA_SECTION_NODE))
                        {
                            QString pName = QString::fromUtf8(reinterpret_cast<const tchar *>(tNode->content));
                            QUrl pUrl(pName);
                            urlList.append(pName);
                        }
                    }
                    cNode = cNode->next;
                }
            }
            xmlFreeDoc(doc);
        }
    }
}

//-------------------------------------------------------------------------------------------

QString QPlaylistWidget::getTrackLength(const common::TimeStamp& ts) const
{
    common::BString t;
    tuint32 s = ts.secondsTotal();
    
    if((s/3600)>0)
    {
        t  = common::BString::Int(s / 3600) + ":";
        s %= 3600;
        t += common::BString::Int(s / 60,2) + ":";
    }
    else
    {
        t = common::BString::Int(s /60) + ":";
    }
    t += common::BString::Int(s % 60,2);
    return QString::fromLatin1(static_cast<const tchar *>(t));
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::setPlayItemToStart()
{
    m_currentPlayItem = 0;
    initCurrentPlay();
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::lastItem()
{
    QPLItemBase *item = 0;

    if(m_playRootList.size()>0)
    {
        item = m_playRootList[m_playRootList.size()-1];
        if(item->isChildren())
        {
            item = item->child(item->noChildren()-1);
        }
    }
    return item;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::selectionChanged()
{
    bool flag = isSelected();
    PlayerController::instance()->clipboardActive(flag);
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isSelected()
{
    int i,j;

    for(i=0;i<m_playRootList.size();i++)
    {
        QPLItemBase *hItem = m_playRootList[i];
        if(hItem->isSelected())
        {
            return true;
        }
        if(hItem->isChildren())
        {
            for(j=0;j<hItem->noChildren();j++)
            {
                if(hItem->child(j)->isSelected())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

bool QPlaylistWidget::isPaste()
{
    bool res = false;
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData!=0)
    {
        if(mimeData->hasFormat("application/blackomega-urllist"))
        {
            res = true;
        }
#if !defined(OMEGA_MAC_STORE)        
        else if(mimeData->urls().size()>0)
        {
            res = true;
        }
#endif
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::onClipboardChange()
{
    PlayerController::instance()->pasteActive(isPaste());
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::clone(QPlaylistWidget *cPlaylist)
{
    int i;
    QList<QPLItemBase *>::iterator ppI;
    QPLItemBase *playItem = 0;
    
    for(ppI=m_playRootList.begin();ppI!=m_playRootList.end();++ppI)
    {
        QPLItemBase *item = *ppI;
        delete item;
    }
    m_playRootList.clear();
    m_viewList.clear();
    
    for(ppI=cPlaylist->m_playRootList.begin();ppI!=cPlaylist->m_playRootList.end();++ppI)
    {
        QPLItemBase *cPItem = *ppI;
        
        switch(cPItem->type())
        {
            case QPLItemBase::e_Album:
                {
                    QPLItemAlbum *aItem = new QPLItemAlbum(this,cPItem->info());
                    
                    aItem->setSelected(cPItem->isSelected());
                    for(i=0;i<cPItem->noChildren();i++)
                    {
                        QPLItemBase *cCItem = cPItem->child(i);
                        QPLItemATrack *cItem;
                        
                        if(cCItem->isSubTrack())
                        {
                            cItem = new QPLItemATrack(this,aItem,cCItem->info(),cCItem->subTrackIndex());
                        }
                        else
                        {
                            cItem = new QPLItemATrack(this,aItem,cCItem->info());
                        }
                        if(cCItem==cPlaylist->currentPlayItem())
                        {
                            playItem = cItem;
                        }
                        cItem->setSelected(cCItem->isSelected());
                    }
                    m_playRootList.append(aItem);
                }
                break;
                
            case QPLItemBase::e_Single:
                {
                    QPLItemSingle *sItem;
                    if(cPItem->isSubTrack())
                    {
                        sItem = new QPLItemSingle(this,cPItem->info(),cPItem->subTrackIndex());
                    }
                    else
                    {
                        sItem = new QPLItemSingle(this,cPItem->info());
                    }
                    if(cPItem==cPlaylist->currentPlayItem())
                    {
                        playItem = sItem;
                    }
                    sItem->setSelected(cPItem->isSelected());
                    m_playRootList.append(sItem);
                }
                break;
                
            default:
                break;
        }
    }
        
    buildViewList();
    resizePlaylist();
    update();
    
    if(playItem!=0)
    {
        setCurrentPlayItem(playItem);
    }
    else
    {
        initCurrentPlay();
    }
    applyAndEmitUpdate();
}

//-------------------------------------------------------------------------------------------

QScrollBar *QPlaylistWidget::getVerticalScrollBar()
{
    return m_scrollArea->verticalScrollBar();
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::applyAndEmitUpdate()
{
    int h = m_scrollArea->viewport()->size().height();
    int vH = h;
    if(!m_viewList.isEmpty())
    {
        int yTotal = m_viewList.last()->viewHeight() + m_viewList.last()->height();
        if(yTotal > h)
        {
            h = yTotal;
        }
    }
    resize(width(),h);
    getVerticalScrollBar()->setRange(0,h - vH);
    emit onUpdate();
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPlaylistWidget::nextShufflePlayItem()
{
    QPLItemBase *item = 0;
    
    if(!m_shuffleList.isEmpty())
    {
        int i,idx;
        tfloat64 r;
        QSet<QPLItemBase *>::iterator ppI;

        r = common::Random::instance()->randomReal2();
        idx = static_cast<int>(floor(r * static_cast<tfloat64>(m_shuffleList.size())));
        if(idx < 0)
        {
            idx = 0;
        }
        else if(idx >= m_shuffleList.size())
        {
            idx = m_shuffleList.size() - 1;
        }
        
        for(i=0,ppI=m_shuffleList.begin();ppI!=m_shuffleList.end() && item==0;++ppI,i++)
        {
            if(i==idx)
            {
                item = *ppI;
            }
        }
        
        if(item==0)
        {
            item = *(m_shuffleList.begin());
        }
    }
    return item;
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::buildShuffleList()
{
    m_shuffleList.clear();
    addTracksShuffleList(m_playRootList);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::buildShuffleList(QPLItemBase *excludeItem)
{
    QSet<QPLItemBase *>::iterator ppI;

    buildShuffleList();
    removeTracksShuffleList(excludeItem);
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addTracksShuffleList(QList<QPLItemBase *>& trackItems)
{
    for(QList<QPLItemBase *>::iterator ppI=trackItems.begin();ppI!=trackItems.end();++ppI)
    {
        addTracksShuffleList(*ppI);
    }    
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::addTracksShuffleList(QPLItemBase *item)
{
    if(item->type()==QPLItemBase::e_AlbumTrack || item->type()==QPLItemBase::e_Single)
    {
        m_shuffleList.insert(item);
    }
    if(item->isChildren())
    {
        for(int i=0;i<item->noChildren();i++)
        {
            addTracksShuffleList(item->child(i));
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::removeTracksShuffleList(QVector<QPLItemBase *>& trackItems)
{
    for(QVector<QPLItemBase *>::iterator ppI=trackItems.begin();ppI!=trackItems.end();ppI++)
    {
        removeTracksShuffleList(*ppI);
    }    
}

//-------------------------------------------------------------------------------------------

void QPlaylistWidget::removeTracksShuffleList(QPLItemBase *item)
{
    QSet<QPLItemBase *>::iterator ppK;
    
    ppK = m_shuffleList.find(item);
    if(ppK!=m_shuffleList.end())
    {
        m_shuffleList.erase(ppK);
    }
    
    if(item->isChildren())
    {
        for(int i=0;i<item->noChildren();i++)
        {
            removeTracksShuffleList(item->child(i));
        }
    }    
}

//-------------------------------------------------------------------------------------------
// QFixedHorizontalScrollArea
//-------------------------------------------------------------------------------------------

QFixedHorizontalScrollArea::QFixedHorizontalScrollArea(QWidget *parent) : QScrollArea(parent),
    m_direction(e_dragNone),
    m_scrollRatio(0.0f)
{
    QPlayerApplication::instance()->installEventFilter(this);
    this->viewport()->setAcceptDrops(true);
    
    m_dragTimer = new QTimer(this);
    m_dragTimer->setSingleShot(false);
    m_dragTimer->setInterval(0);
    connect(m_dragTimer,SIGNAL(timeout()),this,SLOT(onDragTimer()));
}

//-------------------------------------------------------------------------------------------

QFixedHorizontalScrollArea::~QFixedHorizontalScrollArea()
{
    QPlayerApplication::instance()->removeEventFilter(this);
}

//-------------------------------------------------------------------------------------------

bool QFixedHorizontalScrollArea::eventFilter(QObject *watched,QEvent *event)
{
    if(watched->isWidgetType())
    {
        QWidget *w = dynamic_cast<QWidget *>(watched);
        if(w!=0)
        {
            if(this->isAncestorOf(w))
            {
                switch(event->type())
                {
                    case QEvent::DragMove:
                        {
                            QDragMoveEvent *moveEvent = dynamic_cast<QDragMoveEvent *>(event);
                            if(moveEvent!=0)
                            {
                                // The ratio height, with respect to the view port, of the drag area.
                                const tfloat32 R = 0.25f;
                                
                                // top Y position of the given view port in widget coordinates.
                                tfloat32 v = static_cast<tfloat32>(verticalScrollBar()->value());
                                // height of the view port
                                tfloat32 H = static_cast<tfloat32>(viewport()->size().height());
                                // bounds of up scroll area in viewport coordinates.
                                tfloat32 wC = R * H;
                                // bounds of up scroll area in widget coordinates.
                                tfloat32 D = v + wC;
                                // Y position of mouse in widget coordinates.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                                tfloat32 mY = static_cast<tfloat32>(moveEvent->pos().y());
#else
                                tfloat32 mY = static_cast<tfloat32>(moveEvent->position().y());
#endif
                                // bottom Y position of the given view port in widget coordinates.
                                tfloat32 vH = v + H;
                                // bounds of down scroll area in widget coordinates.
                                tfloat32 E = vH - wC;
                                
                                if(mY < D)
                                {
                                    // Mouse in up scroll area
                                    tfloat32 x = (D - mY) / (D - v);
                                    m_scrollRatio = x * x;
                                    m_direction = e_dragUp;
                                    m_dragTimer->start();
                                }
                                else if(E < mY)
                                {
                                    tfloat32 x = (mY - E) / (vH - E);
                                    m_scrollRatio = x * x;
                                    m_direction = e_dragDown;
                                    m_dragTimer->start();
                                }
                                else
                                {
                                    m_scrollRatio = 0.0f;
                                    m_direction = e_dragNone;
                                    m_dragTimer->stop();
                                }
                            }
                        }
                        break;
                        
                    case QEvent::Drop:
                    case QEvent::DragLeave:
                        m_dragTimer->stop();
                        m_direction = e_dragNone;
                        break;
                    
                    default:
                        break;
                }
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

void QFixedHorizontalScrollArea::resizeEvent(QResizeEvent *e)
{
    QWidget *w = widget();
    if(w!=0)
    {
        w->resize(e->size().width(),w->height());
    }
    QScrollArea::resizeEvent(e);
}

//-------------------------------------------------------------------------------------------

void QFixedHorizontalScrollArea::onDragTimer()
{
    tfloat32 dist = m_scrollRatio * 10.0f;

    if(m_direction==e_dragUp)
    {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - static_cast<int>(dist));
    }
    else if(m_direction==e_dragDown)
    {
        verticalScrollBar()->setValue(verticalScrollBar()->value() + static_cast<int>(dist));
    }
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
