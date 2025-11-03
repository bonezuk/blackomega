#include "network/inc/Resource.h"
#include "player/inc/ITunesConfig.h"
#include "track/db/inc/PlaylistAbstractIO.h"
#include "common/inc/DiskOps.h"

#include <QProcess>
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDesktopServices>

#if defined(OMEGA_MAC_STORE)
#include "common/inc/SBService.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

ITunesConfig::ITunesConfig(QObject *parent) : common::ProcessService(parent),
    m_mutex(),
    m_musicFolder(),
    m_fileMaps(),
    m_playlistMaps(),
    m_lastUpdateMap(),
    m_playlistWidget(0),
    m_collectionMenu(0),
    m_actionSet()
{
    m_editFlag = false;
    QObject::connect(this,SIGNAL(onUpdate()),this,SLOT(onUpdateMenus()));
    QObject::connect(this,SIGNAL(onNoUpdate()),this,SLOT(onNoUpdateMenus()));
}

//-------------------------------------------------------------------------------------------

ITunesConfig::~ITunesConfig()
{}

//-------------------------------------------------------------------------------------------

void ITunesConfig::setPlaylistWidget(QPlaylistWidget *plWidget,QMenu *contextMenu)
{
    m_playlistWidget = plWidget;
    m_collectionMenu = contextMenu;
}

//-------------------------------------------------------------------------------------------

QString ITunesConfig::getMusicDirectory()
{
    QString mDir;

    m_mutex.lock();
    if(!m_musicFolder.isEmpty())
    {
        mDir = m_musicFolder;
    }
    m_mutex.unlock();
    if(mDir.isEmpty())
    {
        QStringList mDirList;
#if QT_VERSION >= 0x050000
        mDirList = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
#else
        mDirList += QDir::homePath();
#endif
        if(mDirList.size()>0)
        {
            mDir = mDirList.at(0);
        }
    }
    return mDir;
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::onUpdateMenus()
{
    buildCollectionMenu();
    m_collectionMenu->setEnabled(true);
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::onNoUpdateMenus()
{
    m_collectionMenu->setEnabled(false);
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::buildCollectionMenu()
{
    if(m_collectionMenu!=0)
    {
        int i;
        QMap<QString,QList<QPair<QString,QList<int> > > >::const_iterator ppI;

        clearCollectionMenu();

        QMenu *albumMenu = m_collectionMenu->addMenu("Albums");
        QMenu *artistMenu = m_collectionMenu->addMenu("Artists");
        QMenu *playlistMenu = m_collectionMenu->addMenu("Playlists");

        QObject::connect(albumMenu,SIGNAL(triggered(QAction*)),this,SLOT(onSelectAlbum(QAction*)));
        QObject::connect(artistMenu,SIGNAL(triggered(QAction*)),this,SLOT(onSelectArtist(QAction*)));
        QObject::connect(playlistMenu,SIGNAL(triggered(QAction*)),this,SLOT(onSelectPlaylist(QAction*)));

        m_mutex.lock();
        // playlists
        for(i=0,ppI=m_playlistMaps.begin();ppI!=m_playlistMaps.end();ppI++,i++)
        {
            const QList<QPair<QString,QList<int> > >& playlists = ppI.value();
            QList<QPair<QString,QList<int> > >::const_iterator ppJ;

            if(i>0)
            {
                playlistMenu->addSeparator();
            }
            for(ppJ=playlists.begin();ppJ!=playlists.end();ppJ++)
            {
                const QPair<QString,QList<int> >& pl = *ppJ;
                QAction *a = new QAction(pl.first,this);
                a->setData(ppI.key());
                playlistMenu->addAction(a);
            }
        }
        // albums
        {
            QMap<QString,QVector<QPair<QString,int> > >::iterator ppJ;
            for(ppJ=m_albumMap.begin();ppJ!=m_albumMap.end();ppJ++)
            {
                QString album = ppJ.key();
                QAction *a = new QAction(album,this);
                a->setData(album);
                albumMenu->addAction(a);
            }
        }
        // artists
        {
            QMap<QString,QVector<QPair<QString,int> > >::iterator ppJ;
            for(ppJ=m_artistMap.begin();ppJ!=m_artistMap.end();ppJ++)
            {
                QString artist = ppJ.key();
                QAction *a = new QAction(artist,this);
                a->setData(artist);
                artistMenu->addAction(a);
            }
        }
        m_mutex.unlock();
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::clearCollectionMenu()
{
    QSet<QAction *>::iterator ppI;

    m_collectionMenu->clear();
    for(ppI=m_actionSet.begin();ppI!=m_actionSet.end();ppI++)
    {
        QAction *a = *ppI;
        delete a;
    }
    m_actionSet.clear();
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::onSelectPlaylist(QAction *action)
{
    QStringList fileList;

    m_mutex.lock();
    if(action!=0)
    {
        QMap<QString,QMap<tint,track::info::InfoSPtr> >::iterator ppL = m_fileMaps.find(action->data().toString());
        if(ppL!=m_fileMaps.end())
        {
            QMap<tint,track::info::InfoSPtr>& fileMap = ppL.value();
            QMap<QString,QList<QPair<QString,QList<int> > > >::iterator ppI = m_playlistMaps.find(action->data().toString());
            if(ppI!=m_playlistMaps.end())
            {
                QList<QPair<QString,QList<int> > >& playlists = ppI.value();
                QList<QPair<QString,QList<int> > >::iterator ppJ;
                for(ppJ=playlists.begin();ppJ!=playlists.end();ppJ++)
                {
                    QPair<QString,QList<int> >& playlistPair = *ppJ;
                    if(playlistPair.first==action->text())
                    {
                        QList<int>& pList = playlistPair.second;
                        QList<int>::const_iterator ppK;

                        for(ppK=pList.begin();ppK!=pList.end();ppK++)
                        {
                            QMap<tint,track::info::InfoSPtr>::iterator ppM = fileMap.find(*ppK);
                            if(ppM!=fileMap.end())
                            {
                                fileList.append(ppM.value()->getFilename());
                            }
                        }
                    }
                }
            }
        }
    }
    m_mutex.unlock();
    if(!fileList.isEmpty() && m_playlistWidget!=0)
    {
#if defined(OMEGA_MAC_STORE)
        fileList = processFileListForAppSandbox(fileList);
#endif
        if(!fileList.isEmpty())
        {
            m_playlistWidget->addFiles(fileList,m_playlistWidget->lastItem(),false);
        }
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::onSelectAlbum(QAction *action)
{
    int i;
    QStringList fileList;

    m_mutex.lock();
    if(action!=0)
    {
        QMap<QString,QVector<QPair<QString,int> > >::iterator ppI = m_albumMap.find(action->data().toString());
        if(ppI!=m_albumMap.end())
        {
            QVector<QPair<QString,int> >& albumList = ppI.value();
            for(i=0;i<albumList.size();i++)
            {
                QPair<QString,int> tID = albumList.at(i);
                QMap<QString,QMap<tint,track::info::InfoSPtr> >::iterator ppK = m_fileMaps.find(tID.first);
                if(ppK!=m_fileMaps.end())
                {
                    QMap<tint,track::info::InfoSPtr>::iterator ppL = ppK.value().find(tID.second);
                    if(ppL!=ppK.value().end())
                    {
                        fileList.append(ppL.value()->getFilename());
                    }
                }
            }
        }
    }
    m_mutex.unlock();
    if(!fileList.isEmpty() && m_playlistWidget!=0)
    {
#if defined(OMEGA_MAC_STORE)
        fileList = processFileListForAppSandbox(fileList);
#endif
        if(!fileList.isEmpty())
        {
            m_playlistWidget->addFiles(fileList,m_playlistWidget->lastItem());
        }
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::onSelectArtist(QAction *action)
{
    int i;
    QStringList fileList;

    m_mutex.lock();
    if(action!=0)
    {
        QMap<QString,QVector<QPair<QString,int> > >::iterator ppI = m_artistMap.find(action->data().toString());
        if(ppI!=m_artistMap.end())
        {
            QVector<QPair<QString,int> >& artistList = ppI.value();
            for(i=0;i<artistList.size();i++)
            {
                QPair<QString,int> tID = artistList.at(i);
                QMap<QString,QMap<tint,track::info::InfoSPtr> >::iterator ppK = m_fileMaps.find(tID.first);
                if(ppK!=m_fileMaps.end())
                {
                    QMap<tint,track::info::InfoSPtr>::iterator ppL = ppK.value().find(tID.second);
                    if(ppL!=ppK.value().end())
                    {
                        fileList.append(ppL.value()->getFilename());
                    }
                }
            }
        }
    }
    m_mutex.unlock();
    if(!fileList.isEmpty() && m_playlistWidget!=0)
    {
#if defined(OMEGA_MAC_STORE)
        fileList = processFileListForAppSandbox(fileList);
#endif
        if(!fileList.isEmpty())
        {
            m_playlistWidget->addFiles(fileList,m_playlistWidget->lastItem());
        }
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::startEdit()
{
    m_editFlag = true;
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::stopEdit()
{
    m_editFlag = false;
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::process()
{
    track::db::ITunesLocation location;
    QStringList libLocList;
    QStringList::iterator ppI;

    if(m_editFlag)
    {
        return;
    }

    libLocList = location.getITunesConfigFileNames();

    if(!libLocList.isEmpty())
    {
        bool clear = false;

        if(m_lastUpdateMap.size()==libLocList.size())
        {
            for(ppI=libLocList.begin();ppI!=libLocList.end();ppI++)
            {
                if(m_lastUpdateMap.find(*ppI)==m_lastUpdateMap.end())
                {
                    clear = true;
                }
            }
        }
        else
        {
            clear = true;
        }

        if(clear)
        {
            m_mutex.lock();
            m_fileMaps.clear();
            m_playlistMaps.clear();
            m_albumMap.clear();
            m_artistMap.clear();
            m_lastUpdateMap.clear();
            m_mutex.unlock();

            for(ppI=libLocList.begin();ppI!=libLocList.end();ppI++)
            {
                const QString& libFilename = *ppI;
                if(common::DiskOps::exist(libFilename))
                {
                    parseDatabase(libFilename);
                }
            }
        }
        else
        {
            for(ppI=libLocList.begin();ppI!=libLocList.end();ppI++)
            {
                const QString& libFilename = *ppI;
                if(common::DiskOps::exist(libFilename))
                {
                    bool updateFlag = false;

                    m_mutex.lock();
                    {
                        QMap<QString,common::TimeStamp>::iterator ppI = m_lastUpdateMap.find(libFilename);
                        if(ppI!=m_lastUpdateMap.end())
                        {
                            if(ppI.value() < common::DiskOps::getModifiedTime(libFilename))
                            {
                                updateFlag = true;
                            }
                        }
                        else
                        {
                            updateFlag = true;
                        }
                    }
                    m_mutex.unlock();

                    if(updateFlag)
                    {
                        parseDatabase(libFilename);
                    }
                }
            }
        }
    }
    else
    {
        emit onNoUpdate();
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::getDBDictionary(QMap<QString,QSet<QString> >& iTunesDict)
{
    QSet<QString> dict;

    iTunesDict.clear();

    dict.insert("Application Version");
    dict.insert("Music Folder");
    dict.insert("Tracks");
    dict.insert("Playlists");
    iTunesDict.insert("",dict);

    dict.clear();
    dict.insert("Track ID");
    dict.insert("Location");
    dict.insert("Name");
    dict.insert("Album");
    dict.insert("Artist");
    dict.insert("Album Artist");
    dict.insert("Composer");
    dict.insert("Genre");
    dict.insert("Track Number");
    dict.insert("Disc Number");
    iTunesDict.insert("Tracks",dict);

    dict.clear();
    dict.insert("Name");
    dict.insert("Playlist ID");
    dict.insert("Playlist Items");
    iTunesDict.insert("Playlists",dict);

    dict.clear();
    dict.insert("Track ID");
    iTunesDict.insert("Playlist Items",dict);
}

//-------------------------------------------------------------------------------------------
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//-------------------------------------------------------------------------------------------

QString ITunesConfig::getDataItem(const QVariant& v)
{
    QString d;

    switch(v.type())
    {
        case QVariant::String:
            d = v.toString();
            break;

        case QVariant::Int:
            d = QString::number(v.toInt());
            break;

        case QVariant::Double:
            d = QString::number(v.toDouble(),'f',8);
            break;

        default:
            break;
    }
    return d;
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

QString ITunesConfig::getDataItem(const QVariant& v)
{
    QString d;

    switch(v.metaType().id())
    {
        case QMetaType::Type::QString:
            d = v.toString();
            break;

        case QMetaType::Type::Int:
            d = QString::number(v.toInt());
            break;

        case QMetaType::Type::Double:
            d = QString::number(v.toDouble(),'f',8);
            break;

        default:
            break;
    }
    return d;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QString ITunesConfig::parseURL(const QString& pUrl)
{
    int i,j,state = 0;
    QString oStr;

    for(i=0;i<pUrl.length();i++)
    {
        switch(state)
        {
            case 0:
                if(pUrl.at(i)==QChar('%'))
                {
                    state = 1;
                }
                else
                {
                    oStr += pUrl.at(i);
                }
                break;

            case 1:
            case 2:
                {
                    tchar x = pUrl.at(i).toLatin1();
                    if((x>='0' && x<='9') || (x>='a' && x<='f') || (x>='A' && x<='F'))
                    {
                        state++;
                        if(state>2)
                        {
                            tchar a[2],b;

                            a[0] = pUrl.at(i-1).toLatin1();
                            a[1] = pUrl.at(i).toLatin1();
                            for(j=0;j<2;j++)
                            {
                                if(a[j]>='0' && a[j]<='9')
                                {
                                    a[j] = a[j] - '0';
                                }
                                else if(a[j]>='a' && a[j]<='f')
                                {
                                    a[j] = (a[j] - 'a') + 0x0a;
                                }
                                else if(a[j]>='A' && a[j]<='F')
                                {
                                    a[j] = (a[j] - 'A') + 0x0a;
                                }
                            }
                            b = ((a[0] << 4) & 0xf0) | (a[1] & 0x0f);
                            oStr += QChar(b);
                            state = 0;
                        }
                    }
                    else
                    {
                        i -= state;
                        state = 0;
                    }
                }
        }
    }
    return oStr;
}

//-------------------------------------------------------------------------------------------

QString ITunesConfig::getURLFilename(const QString& uPath)
{
    QString fPath;

    if(!uPath.isEmpty())
    {
        xmlURIPtr rURI = xmlCreateURI();
        if(rURI!=0)
        {
            if(xmlParseURIReference(rURI,uPath.toUtf8().constData())==0)
            {
                xmlChar *refS = xmlSaveUri(rURI);
                if(refS!=0)
                {
                    QString n;
                    QString fN = QString::fromUtf8(reinterpret_cast<const char *>(refS));
                    QUrl fUrl(fN);
                    fUrl.setHost("");
                    n = parseURL(fUrl.toLocalFile());
                    n = QDir::toNativeSeparators(n);
                    if(!n.isEmpty())
                    {
                        if(n.at(n.size()-1)==QChar('/') || n.at(n.size()-1)==QChar('\\'))
                        {
                            n = n.mid(0,n.size()-1);
                        }
                        if(common::DiskOps::exist(n))
                        {
                            fPath = n;
                        }
                    }
                    xmlFree(refS);
                }
            }
            xmlFreeURI(rURI);
        }
    }
    return fPath;
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::processTracks(const QVariant& tRoot,QMap<int,track::info::InfoSPtr>& trackMap)
{
    trackMap.clear();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if(tRoot.type()==QVariant::Map)
#else
    if(tRoot.metaType().id()==QMetaType::Type::QVariantMap)
#endif
    {
        QMap<QString,QVariant> tMap = tRoot.toMap();
        QMap<QString,QVariant>::iterator ppI,ppJ,ppK;

        for(ppI=tMap.begin();ppI!=tMap.end();ppI++)
        {
            const QVariant& dV = ppI.value();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            if(dV.type()==QVariant::Map)
#else
            if(dV.metaType().id()==QMetaType::Type::QVariantMap)
#endif
            {
                QMap<QString,QVariant> dMap = dV.toMap();
                ppJ = dMap.find("Track ID");
                ppK = dMap.find("Location");
                if(ppJ!=dMap.end() && ppK!=dMap.end())
                {
                    int trackID = ppJ.value().toInt();
                    QString location = getURLFilename(getDataItem(ppK.value()));
                    if(!location.isEmpty())
                    {
                        if(track::info::Info::isSupported(location))
                        {
                            track::info::InfoSPtr pInfo(new track::info::Info);

                            pInfo->setFilename(location);
                            ppJ = dMap.find("Name");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->title() = ppJ.value().toString();
                            }
                            ppJ = dMap.find("Album");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->album() = ppJ.value().toString();
                            }
                            ppJ = dMap.find("Artist");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->artist() = ppJ.value().toString();
                            }
                            ppJ = dMap.find("Album Artist");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->originalArtist() = ppJ.value().toString();
                            }
                            ppJ = dMap.find("Composer");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->composer() = ppJ.value().toString();
                            }
                            ppJ = dMap.find("Genre");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->genre() = ppJ.value().toString();
                            }
                            ppJ = dMap.find("Track Number");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->track() = ppJ.value().toString();
                            }
                            ppJ = dMap.find("Disc Number");
                            if(ppJ!=dMap.end())
                            {
                                pInfo->disc() = ppJ.value().toString();
                            }

                            trackMap.insert(trackID,pInfo);
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::processPlaylist(const QVariant& pRoot,QString& name,QList<int>& pList)
{
    pList.clear();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if(pRoot.type()==QVariant::Map)
#else
    if(pRoot.metaType().id()==QMetaType::Type::QVariantMap)
#endif
    {
        QMap<QString,QVariant> rMap = pRoot.toMap();
        QMap<QString,QVariant>::iterator ppI,ppK;

        ppI = rMap.find("Name");
        if(ppI!=rMap.end())
        {
            name = ppI.value().toString();
            ppI = rMap.find("Playlist Items");
            if(ppI!=rMap.end())
            {
                const QVariant& vA = ppI.value();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                if(vA.type()==QVariant::List)
#else
                if(vA.metaType().id()==QMetaType::Type::QVariantList)
#endif
                {
                    QList<QVariant> aList = vA.toList();
                    QList<QVariant>::iterator ppJ;

                    for(ppJ=aList.begin();ppJ!=aList.end();ppJ++)
                    {
                        const QVariant& dV = *ppJ;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                        if(dV.type()==QVariant::Map)
#else
                        if(dV.metaType().id()==QMetaType::Type::QVariantMap)
#endif
                        {
                            QMap<QString,QVariant> dMap = dV.toMap();
                            ppK = dMap.find("Track ID");
                            if(ppK!=dMap.end())
                            {
                                const QVariant& tV = ppK.value();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                                if(tV.type()==QVariant::Int)
#else
                                if(tV.metaType().id()==QMetaType::Type::Int)
#endif
                                {
                                    pList.append(tV.toInt());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::processPlaylists(const QVariant& pRoot,QList<QPair<QString,QList<int> > >& pLists)
{
    pLists.clear();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if(pRoot.type()==QVariant::List)
#else
    if(pRoot.metaType().id()==QMetaType::Type::QVariantList)
#endif
    {
        QList<QVariant> aList = pRoot.toList();
        QList<QVariant>::iterator ppI;

        for(ppI=aList.begin();ppI!=aList.end();ppI++)
        {
            QString name;
            QList<int> tList;
            const QVariant& v = *ppI;

            processPlaylist(v,name,tList);
            if(!name.isEmpty() && !tList.isEmpty())
            {
                pLists.append(QPair<QString,QList<int> >(name,tList));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::parseDatabase(const QString& fileName)
{
    QString versionDb,musicFolderLocation;
    QMap<int,track::info::InfoSPtr> trackMap;
    QList<QPair<QString,QList<int> > > playlistMap;
    QMap<QString,QSet<QString> > iTunesDict;
    QList<QPair<QString,QList<int> > >::iterator ppJ;
    QMap<int,track::info::InfoSPtr>::iterator ppL;

    getDBDictionary(iTunesDict);

    track::info::PListXMLParser pListParser(iTunesDict);
    if(pListParser.process(fileName))
    {
        tint i;
        const QList<QVariant>& rootList = pListParser.data();

        for(i=0;i<rootList.size();i++)
        {
            const QVariant& v = rootList.at(i);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            if(v.type()==QVariant::Map)
#else
            if(v.metaType().id()==QMetaType::Type::QVariantMap)
#endif
            {
                QMap<QString,QVariant> dMap = v.toMap();
                QMap<QString,QVariant>::iterator ppI;

                ppI = dMap.find("Application Version");
                if(ppI!=dMap.end())
                {
                    versionDb = getDataItem(ppI.value());
                }
                ppI = dMap.find("Music Folder");
                if(ppI!=dMap.end())
                {
                    musicFolderLocation = getURLFilename(getDataItem(ppI.value()));
                    if(!musicFolderLocation.isEmpty())
                    {
                        m_mutex.lock();
                        m_musicFolder = musicFolderLocation;
                        m_mutex.unlock();
                    }
                }
                ppI = dMap.find("Tracks");
                if(ppI!=dMap.end())
                {
                    processTracks(ppI.value(),trackMap);
                }
                ppI = dMap.find("Playlists");
                if(ppI!=dMap.end())
                {
                    processPlaylists(ppI.value(),playlistMap);
                }
            }
        }
    }

    for(ppJ=playlistMap.begin();ppJ!=playlistMap.end();)
    {
        QPair<QString,QList<int> >& pl = *ppJ;
        QList<int>& pList = pl.second;
        QList<int>::iterator ppK = pList.begin();

        while(ppK!=pList.end())
        {
            ppL = trackMap.find(*ppK);
            if(ppL!=trackMap.end())
            {
                ppK++;
            }
            else
            {
                ppK = pList.erase(ppK);
            }
        }

        if(!pList.isEmpty())
        {
            ppJ++;
        }
        else
        {
            ppJ = playlistMap.erase(ppJ);
        }
    }

    updateConfig(fileName,trackMap,playlistMap);
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::updateConfig(const QString& fName,const QMap<int,track::info::InfoSPtr>& trackMap,const QList<QPair<QString,QList<int> > >& playlistMap)
{
    QMap<QString,QMap<tint,track::info::InfoSPtr> >::iterator ppI;
    QMap<QString,QList<QPair<QString,QList<int> > > >::iterator ppJ;
    QMap<QString,common::TimeStamp>::iterator ppK;

    m_mutex.lock();
    ppI = m_fileMaps.find(fName);
    if(ppI!=m_fileMaps.end())
    {
        m_fileMaps.erase(ppI);
    }
    m_fileMaps.insert(fName,trackMap);

    ppJ = m_playlistMaps.find(fName);
    if(ppJ!=m_playlistMaps.end())
    {
        m_playlistMaps.erase(ppJ);
    }
    m_playlistMaps.insert(fName,playlistMap);

    ppK = m_lastUpdateMap.find(fName);
    if(ppK!=m_lastUpdateMap.end())
    {
        m_lastUpdateMap.erase(ppK);
    }
    m_lastUpdateMap.insert(fName,common::DiskOps::getModifiedTime(fName));

    buildAlbumConfig();
    buildArtistConfig();

    m_mutex.unlock();

    emit onUpdate();
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::buildAlbumConfig()
{
    QMap<QString,QMap<tint,track::info::InfoSPtr> >::iterator ppI;
    QMap<QString,QVector<QPair<QString,int> > >::iterator ppK;

    m_albumMap.clear();
    for(ppI=m_fileMaps.begin();ppI!=m_fileMaps.end();ppI++)
    {
        QMap<tint,track::info::InfoSPtr>& tMap = ppI.value();
        QMap<tint,track::info::InfoSPtr>::iterator ppJ;

        for(ppJ=tMap.begin();ppJ!=tMap.end();ppJ++)
        {
            track::info::InfoSPtr pInfo(ppJ.value());
            if(!pInfo->album().isEmpty())
            {
                const QString& album = pInfo->album();
                bool skip = false;

                ppK = m_albumMap.find(album);
                if(ppK==m_albumMap.end())
                {
                    QVector<QPair<QString,int> > v;
                    m_albumMap.insert(album,v);
                    ppK = m_albumMap.find(album);
                }
                else
                {
                    QVector<QPair<QString,int> >& albumList = ppK.value();
                    if(!albumList.isEmpty() && albumList.at(0).first!=ppI.key())
                    {
                        skip = true;
                    }
                }

                if(ppK!=m_albumMap.end() && !skip)
                {
                    ppK.value().append(QPair<QString,int>(ppI.key(),ppJ.key()));
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void ITunesConfig::buildArtistConfig()
{
    QMap<QString,QVector<QPair<QString,int> > >& aMap = m_artistMap;
    QMap<QString,QMap<tint,track::info::InfoSPtr> >::iterator ppI;
    QMap<QString,QVector<QPair<QString,int> > >::iterator ppK;

    aMap.clear();
    for(ppI=m_fileMaps.begin();ppI!=m_fileMaps.end();ppI++)
    {
        QMap<tint,track::info::InfoSPtr>& tMap = ppI.value();
        QMap<tint,track::info::InfoSPtr>::iterator ppJ;

        for(ppJ=tMap.begin();ppJ!=tMap.end();ppJ++)
        {
            track::info::InfoSPtr pInfo(ppJ.value());
            if(!pInfo->artist().isEmpty())
            {
                const QString& artist = pInfo->artist();
                ppK = aMap.find(artist);
                if(ppK!=aMap.end())
                {
                    QVector<QPair<QString,int> >& v = ppK.value();
                    v.append(QPair<QString,int>(ppI.key(),ppJ.key()));
                }
                else
                {
                    QVector<QPair<QString,int> > v;
                    v.append(QPair<QString,int>(ppI.key(),ppJ.key()));
                    aMap.insert(artist,v);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

QStringList ITunesConfig::processFileListForAppSandbox(const QStringList& fileList)
{
    QStringList accessFileList,outFileList;
    QStringList::const_iterator ppI;
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();

    for(ppI=fileList.begin();ppI!=fileList.end();ppI++)
    {
        const QString& name = *ppI;
        if(!sbBookmark->has(name,true))
        {
            accessFileList.append(name);
        }
        else
        {
            outFileList.append(name);
        }
    }

    if(accessFileList.size() > 0)
    {
        QSet<QString> pathSet = common::CommonDirectoriesForFiles::find(accessFileList);
        QStringList pathList(pathSet.begin(), pathSet.end());

        widget::ImportPlaylistDialog importDialog(m_playlistWidget->parentWidget());
        importDialog.setDirectories(pathList);
        importDialog.setModal(true);
        importDialog.exec();

        if(importDialog.result()==QDialog::Accepted)
        {
            for(ppI=fileList.begin();ppI!=fileList.end();ppI++)
            {
                const QString& name = *ppI;
                if(sbBookmark->has(name,true))
                {
                    outFileList.append(name);
                }
            }
        }
    }

    return outFileList;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void showHelp()
{
    QUrl helpUrl("https://www.blackomega.co.uk/help/index.html", QUrl::TolerantMode);
    QDesktopServices::openUrl(helpUrl);
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
