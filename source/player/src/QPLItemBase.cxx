#include "player/inc/QPLItemBase.h"
#include "player/inc/QPlaylistWidget.h"
#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPLItemBase::QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,QSharedPointer<track::info::Info>& info) : QObject(playListW),
    m_playListWidget(playListW),
    m_parentItem(parent),
    m_info(info),
    m_children(),
    m_expandFlag(false),
    m_viewPosition(0),
    m_viewHeight(0),
    m_height(0),
    m_selectFlag(false),
    m_imageMap(),
    m_treeNodeRect(),
    m_subTrackIndex(-1)
{
    if(m_parentItem!=0)
    {
        m_parentItem->addChild(this);
    }
}

//-------------------------------------------------------------------------------------------

QPLItemBase::QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,tint childIdx,QSharedPointer<track::info::Info>& info) : QObject(playListW),
    m_playListWidget(playListW),
    m_parentItem(parent),
    m_info(info),
    m_children(),
    m_expandFlag(false),
    m_viewPosition(0),
    m_viewHeight(0),
    m_height(0),
    m_selectFlag(false),
    m_imageMap(),
    m_treeNodeRect(),
    m_subTrackIndex(-1)
{
    if(m_parentItem!=0)
    {
        m_parentItem->insertChild(childIdx,this);
    }
}

//-------------------------------------------------------------------------------------------

QPLItemBase::QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,QSharedPointer<track::info::Info>& info,tint subTIndex) : QObject(playListW),
    m_playListWidget(playListW),
    m_parentItem(parent),
    m_info(info),
    m_children(),
    m_expandFlag(false),
    m_viewPosition(0),
    m_viewHeight(0),
    m_height(0),
    m_selectFlag(false),
    m_imageMap(),
    m_treeNodeRect(),
    m_subTrackIndex(subTIndex)
{
    if(m_parentItem!=0)
    {
        m_parentItem->addChild(this);
    }
}

//-------------------------------------------------------------------------------------------

QPLItemBase::QPLItemBase(QPlaylistWidget *playListW,QPLItemBase *parent,tint childIdx,QSharedPointer<track::info::Info>& info,tint subTIndex) : QObject(playListW),
    m_playListWidget(playListW),
    m_parentItem(parent),
    m_info(info),
    m_children(),
    m_expandFlag(false),
    m_viewPosition(0),
    m_viewHeight(0),
    m_height(0),
    m_selectFlag(false),
    m_imageMap(),
    m_treeNodeRect(),
    m_subTrackIndex(subTIndex)
{
    if(m_parentItem!=0)
    {
        m_parentItem->insertChild(childIdx,this);
    }
}

//-------------------------------------------------------------------------------------------

QPLItemBase::~QPLItemBase()
{
    QMap<QPair<tint,tint>,QPair<QImage *,QImage *> >::iterator ppI;
    
    for(ppI=m_imageMap.begin();ppI!=m_imageMap.end();ppI++)
    {
        QImage *img = ppI.value().first;
        delete img;
        img = ppI.value().second;
        if(img!=0)
        {
            delete img;
        }
    }
    m_imageMap.clear();
    
    while(m_children.size()>0)
    {
        QPLItemBase *cItem = m_children[0];
        delete cItem;
    }
    if(m_parentItem!=0)
    {
        m_parentItem->delChild(this);
    }
}

//-------------------------------------------------------------------------------------------

bool QPLItemBase::isExpanded() const
{
    return m_expandFlag;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<track::info::Info>& QPLItemBase::info()
{
    return m_info;
}

//-------------------------------------------------------------------------------------------

qreal QPLItemBase::paintOffset(const QPointF& pos,QPainter *painter)
{
    QRect r(0,0,0,0);
    m_treeNodeRect = r;
    return 0.0;
}

//-------------------------------------------------------------------------------------------

bool QPLItemBase::isVisible() const
{
    if(m_parentItem!=0)
    {
        if(m_parentItem->isVisible())
        {
            return (m_parentItem->isExpanded()) ? true : false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::addChild(QPLItemBase *item)
{
    if(item!=0)
    {
        m_children.append(item);
        item->m_parentItem = this;
    }
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::insertChild(int idx,QPLItemBase *item)
{
    if(item!=0)
    {
        if(idx<0)
        {
            idx = 0;
        }
        if(idx<m_children.size())
        {
            m_children.insert(idx,item);
        }
        else
        {
            m_children.append(item);
        }
        item->m_parentItem = this;
    }
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::delChild(QPLItemBase *item)
{
    QVector<QPLItemBase *>::iterator ppI;
    
    for(ppI=m_children.begin();ppI!=m_children.end();ppI++)
    {
        QPLItemBase *cItem = *ppI;
        if(cItem==item)
        {
            m_children.erase(ppI);
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::delChildren()
{
    m_children.clear();
}

//-------------------------------------------------------------------------------------------

tint QPLItemBase::depth() const
{
    tint depth = 0;
    QPLItemBase *parent = m_parentItem;
    
    while(parent!=0)
    {
        depth++;
        parent = parent->m_parentItem;
    }
    return depth;
}

//-------------------------------------------------------------------------------------------

QImage *QPLItemBase::getImage(tint w,tint h,bool greyFlag)
{
    QPair<tint,tint> idx(w,h);
    QMap<QPair<tint,tint> , QPair<QImage *,QImage *> >::iterator ppI;
    QImage *img = 0;
    
    ppI = m_imageMap.find(idx);
    if(ppI==m_imageMap.end())
    {
        if(m_info->isImage())
        {
            QImage *oImg;
            track::info::Info::ImageFormat iFormat;
            track::info::ImageInfoArray *pArr;
            
            pArr = m_info->getImageData(iFormat);
            if(pArr!=0)
            {
                QByteArray qArr(reinterpret_cast<const tchar *>(pArr->GetData()),pArr->GetSize());
                QBuffer qBuffer(&qArr);
                QString format;
                bool res = false;
                
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
                oImg = new QImage;
                qBuffer.open(QIODevice::ReadOnly);
                if(!format.isEmpty())
                {
                    res = oImg->load(&qBuffer,format.toLatin1().constData());
                    if(!res)
                    {
                        res = oImg->load(&qBuffer,0);
                    }
                }
                else
                {
                    res = oImg->load(&qBuffer,0);
                }
                
                if(res)
                {
                    img = new QImage(oImg->scaled(QSize(w,h),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                    if(greyFlag)
                    {
                        QImage *gImg = m_playListWidget->convertImageToGreyscale(img);
                        m_imageMap.insert(idx,QPair<QImage *,QImage *>(img,gImg));
                        img = gImg;
                    }
                    else
                    {
                        m_imageMap.insert(idx,QPair<QImage *,QImage *>(img,0));
                    }
                }
                delete oImg;            
            }
        }
        
        if(img==0)
        {
            img = m_playListWidget->noTrackImage(w,h,greyFlag);
        }
    }
    else
    {
        img = ppI.value().first;
        if(greyFlag)
        {
            if(ppI.value().second==0)
            {
                img = m_playListWidget->convertImageToGreyscale(img);
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

QString QPLItemBase::displayString(const QString& org,qreal width,QFontMetricsF& fm)
{
    QString dis;
    
    if(fm.horizontalAdvance(org)>width && org.length()>3)
    {
        tint min,max,mid;
        QString midS;
        
        min = 1;
        max = org.length() - 3;
        do
        {
            mid = (min + max) / 2;
            midS = org.mid(0,mid) + "...";
            if(fm.horizontalAdvance(midS) > width)
            {
                max = mid - 1;
            }
            else
            {
                midS = org.mid(0,mid+1) + "...";
                if(fm.horizontalAdvance(midS) > width)
                {
                    min = max;
                }
                else
                {
                    min = mid + 1;
                }
            }
        } while(min<max);
        
        dis = org.mid(0,min) + "...";
    }
    else
    {
        dis = org;
    }
    return dis;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp QPLItemBase::lengthOfTrack()
{
    if(isSubTrack())
    {
        return m_info->child(subTrackIndex()).length();
    }
    else
    {
        return m_info->length();
    }
}

//-------------------------------------------------------------------------------------------

QString QPLItemBase::getTrackLength()
{
    return getTrackLength(lengthOfTrack());
}

//-------------------------------------------------------------------------------------------

QString QPLItemBase::getTrackLength(const common::TimeStamp& ts)
{
    return m_playListWidget->getTrackLength(ts);
}

//-------------------------------------------------------------------------------------------

QFont& QPLItemBase::getDarkFont()
{
    return m_playListWidget->getDarkFont();
}

//-------------------------------------------------------------------------------------------

QFont& QPLItemBase::getLightFont()
{
    return m_playListWidget->getLightFont();
}

//-------------------------------------------------------------------------------------------

QFont& QPLItemBase::getMediumFont()
{
    return m_playListWidget->getMediumFont();
}

//-------------------------------------------------------------------------------------------

QFontMetricsF& QPLItemBase::getDarkFontMetric()
{
    return m_playListWidget->getDarkFontMetric();
}

//-------------------------------------------------------------------------------------------

QFontMetricsF& QPLItemBase::getLightFontMetric()
{
    return m_playListWidget->getLightFontMetric();
}

//-------------------------------------------------------------------------------------------

QFontMetricsF& QPLItemBase::getMediumFontMetric()
{
    return m_playListWidget->getMediumFontMetric();
}

//-------------------------------------------------------------------------------------------

const QFontMetricsF& QPLItemBase::getDarkFontMetric() const
{
    return m_playListWidget->getDarkFontMetric();
}

//-------------------------------------------------------------------------------------------

const QFontMetricsF& QPLItemBase::getLightFontMetric() const
{
    return m_playListWidget->getLightFontMetric();
}

//-------------------------------------------------------------------------------------------

const QFontMetricsF& QPLItemBase::getMediumFontMetric() const
{
    return m_playListWidget->getMediumFontMetric();
}

//-------------------------------------------------------------------------------------------

qreal QPLItemBase::getTimeColumnWidth()
{
    return m_playListWidget->getTimeColumnWidth();
}

//-------------------------------------------------------------------------------------------

qreal QPLItemBase::getTrackColumnWidth()
{
    return m_playListWidget->getTrackColumnWidth();
}

//-------------------------------------------------------------------------------------------

tint QPLItemBase::width() const
{
    return m_playListWidget->paintWidth();
}

//-------------------------------------------------------------------------------------------

QImage& QPLItemBase::treeExpandImage()
{
    return *(m_playListWidget->treeExpandImage());
}

//-------------------------------------------------------------------------------------------

QImage& QPLItemBase::treeCollapseImage()
{
    return *(m_playListWidget->treeCollapseImage());
}

//-------------------------------------------------------------------------------------------

QImage& QPLItemBase::noTrackImage()
{
    return *(m_playListWidget->noTrackImage());
}

//-------------------------------------------------------------------------------------------

QPen& QPLItemBase::blackPen()
{
    return m_playListWidget->blackPen();
}

//-------------------------------------------------------------------------------------------

QPen& QPLItemBase::grayPen()
{
    return m_playListWidget->grayPen();
}

//-------------------------------------------------------------------------------------------

QPen& QPLItemBase::blackDisablePen()
{
    return m_playListWidget->blackDisablePen();
}

//-------------------------------------------------------------------------------------------

QPen& QPLItemBase::grayDisablePen()
{
    return m_playListWidget->grayDisablePen();
}

//-------------------------------------------------------------------------------------------

tint QPLItemBase::childIndexOf(QPLItemBase *cItem)
{
    return m_children.indexOf(cItem);
}

//-------------------------------------------------------------------------------------------

bool QPLItemBase::isInsideTreeNode(const QPoint& p)
{
    if(m_treeNodeRect.width()>0)
    {
        return m_treeNodeRect.contains(p);
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::setExpanded(bool flag)
{
    m_expandFlag = flag;
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::paintBorder(const QRectF& rect,QPainter *painter)
{
    tint idx;
    QPen nPen(grayPen());
    QPen sPen(QColor(132,172,221));
    QPen tPenA(QColor(0,0,0,128)),tPenB(QColor(0,0,0,60));
    QList<QPLItemBase *>& viewList = m_playListWidget->m_viewList;
    
    idx = viewList.indexOf(this);
    if(idx>=0)
    {
        bool selectA,selectB;
        
        if(!m_selectFlag)
        {
            if(idx>0)
            {
                selectA = viewList.at(idx-1)->isSelected();
            }
            else
            {
                selectA = false;
            }
            if((idx + 1)<viewList.size())
            {
                selectB = viewList.at(idx+1)->isSelected();
            }
            else
            {
                selectB = false;
            }
        }
        else
        {
            selectA = true;
            selectB = true;
        }
        
        painter->setPen(m_selectFlag ? sPen : nPen);
        painter->drawLine(rect.left(),rect.top(),rect.left(),rect.bottom());
        painter->drawLine(rect.right(),rect.top(),rect.right(),rect.bottom());
        painter->setPen(selectA ? sPen : nPen);
        painter->drawLine(rect.left(),rect.top(),rect.right(),rect.top());
        painter->setPen(selectB ? sPen : nPen);
        painter->drawLine(rect.left(),rect.bottom(),rect.right(),rect.bottom());

        if(m_playListWidget->isDragPreItem(this))
        {
            painter->setPen(tPenA);
            painter->drawLine(rect.left(),rect.bottom(),rect.right(),rect.bottom());
            painter->drawLine(rect.left(),rect.bottom()-1,rect.right(),rect.bottom()-1);
            painter->setPen(tPenB);
            painter->drawLine(rect.left(),rect.bottom()-2,rect.right(),rect.bottom()-2);
        }
        else if(m_playListWidget->isDragPostItem(this))
        {
            painter->setPen(tPenA);
            painter->drawLine(rect.left(),rect.top(),rect.right(),rect.top());
            painter->drawLine(rect.left(),rect.top()-1,rect.right(),rect.top()-1);
            painter->setPen(tPenB);
            painter->drawLine(rect.left(),rect.top()-2,rect.right(),rect.top()-2);
        }
    }
    else
    {
        painter->setPen(m_selectFlag ? sPen : nPen);
        painter->drawRect(rect);
    }
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::setSelected(bool v)
{
    tint i;
    
    for(i=0;i<m_children.size();i++)
    {
        m_children[i]->setSelected(v);
    }
    m_selectFlag = v;
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPLItemBase::prevSibling()
{
    tint idx;
    QPLItemBase *sItem = 0;
    
    if(m_parentItem!=0)
    {
        idx = m_parentItem->m_children.indexOf(this) - 1;
        if(idx>=0 && idx<m_parentItem->m_children.size())
        {
            sItem = m_parentItem->m_children[idx];
        }
    }
    else
    {
        idx = m_playListWidget->m_playRootList.indexOf(this) - 1;
        if(idx>=0 && idx<m_playListWidget->m_playRootList.size())
        {
            sItem = m_playListWidget->m_playRootList[idx];
        }
    }
    return sItem;
}

//-------------------------------------------------------------------------------------------

QPLItemBase *QPLItemBase::nextSibling()
{
    tint idx;
    QPLItemBase *sItem = 0;
    
    if(m_parentItem!=0)
    {
        idx = m_parentItem->m_children.indexOf(this) + 1;
        if(idx>=0 && idx<m_parentItem->m_children.size())
        {
            sItem = m_parentItem->m_children[idx];
        }
    }
    else
    {
        idx = m_playListWidget->m_playRootList.indexOf(this) + 1;
        if(idx>=0 && idx<m_playListWidget->m_playRootList.size())
        {
            sItem = m_playListWidget->m_playRootList[idx];
        }
    }
    return sItem;
}

//-------------------------------------------------------------------------------------------

QImage *QPLItemBase::getImageIcon()
{
    tint wPic,hPic;
    QImage *img;
    
    if(type()==e_Album || type()==e_Single)
    {
        wPic = static_cast<tint>(::floor(static_cast<qreal>(height()) * 1.112));
        hPic = height() - 1;
        img = getImage(wPic,hPic,false);
        return img;
    }
    else if(parent()!=0)
    {
        return parent()->getImageIcon();
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

bool QPLItemBase::isDisabled()
{
    return m_playListWidget->m_progressControl->isActive();
}

//-------------------------------------------------------------------------------------------

bool QPLItemBase::isRetina() const
{
    bool retinaFlag = false;

    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        retinaFlag = true;
    }
    return retinaFlag;
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage) const
{
    QPixmap pixMap = QPixmap::fromImage(*pImage);
#if QT_VERSION >= 0x050000
    pixMap.setDevicePixelRatio(2.0);
#endif
    painter->drawPixmap(pos,pixMap);
}

//-------------------------------------------------------------------------------------------

void QPLItemBase::paintRetinaImage(QPainter *painter,QPointF pos,QImage *pImage) const
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
