#include "track/db/inc/DBInfo.h"
#include "common/inc/BIOBufferedStream.h"

#include <QDir>
#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

DBInfo::DBInfo(const QString& fileName) : info::Info(),
    m_albumID(-1),
    m_trackID(-1),
    m_isUpdate(true),
    m_updateFlag(false),
    m_chapters(),
    m_imageFlag(false),
    m_imageMap()
{
    m_fileName = QDir::toNativeSeparators(fileName);
    if(!loadRecord())
    {
        m_albumID = -1;
        m_trackID = -1;
    }
}

//-------------------------------------------------------------------------------------------

DBInfo::DBInfo(const QString& fileName, bool isUpdate) : info::Info(),
    m_albumID(-1),
    m_trackID(-1),
    m_isUpdate(isUpdate),
    m_updateFlag(false),
    m_chapters(),
    m_imageFlag(false),
    m_imageMap()
{
    m_fileName = QDir::toNativeSeparators(fileName);
    if(!loadRecord())
    {
        m_albumID = -1;
        m_trackID = -1;
    }
}

//-------------------------------------------------------------------------------------------

DBInfo::~DBInfo()
{
    QMap<info::IDTagImageType,QPair<ImageFormat,info::ImageInfoArray *> >::iterator ppI;

    for(ppI=m_imageMap.begin();ppI!=m_imageMap.end();ppI++)
    {
        info::ImageInfoArray *pArr = ppI.value().second;
        delete pArr;
    }
    m_imageMap.clear();
}

//-------------------------------------------------------------------------------------------

void DBInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "DBInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<info::Info> DBInfo::readInfo(int albumID,int trackID)
{
    QSharedPointer<info::Info> trackInfo;
    SQLiteDatabase *db = TrackDB::instance()->db();

    if(db!=0)
    {
        QString cmdQ,dirName,fileName;
        SQLiteQuery trackQ(db);

        cmdQ  = "SELECT CASE WHEN f.mountID IS NULL THEN c.directoryName ELSE (f.mountName || c.directoryName) END directoryName, d.fileName";
        cmdQ += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
        cmdQ += "    INNER JOIN directory AS c ON a.directoryID=c.directoryID";
        cmdQ += "    INNER JOIN file AS d ON (c.directoryID=d.directoryID AND b.fileID=d.fileID)";
        cmdQ += "    LEFT JOIN dirmount AS e ON c.directoryID=e.dirID";
        cmdQ += "    LEFT JOIN mountpoints AS f ON e.mountID=f.mountID";
        cmdQ += "  WHERE a.albumID=" + QString::number(albumID) + " AND b.trackID=" + QString::number(trackID);

        trackQ.prepare(cmdQ);
        trackQ.bind(dirName);
        trackQ.bind(fileName);
        if(trackQ.next())
        {
            QString fName;

            dirName = TrackDB::dbStringInv(dirName);
            fileName = TrackDB::dbStringInv(fileName);

            if(dirName.at(dirName.length()-1)==QChar('/') || dirName.at(dirName.length()-1)==QChar('\\'))
            {
                fName = dirName + fileName;
            }
            else
            {
                fName = dirName + "/" + fileName;
            }
            fName = QDir::toNativeSeparators(fName);
            trackInfo = readInfo(fName);
        }
    }
    return trackInfo;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<info::Info> DBInfo::readInfo(const QString& fileName, bool isUpdate)
{
    bool flag = true,updateFlag = false;
    QSharedPointer<info::Info> trackInfo;
    TrackDB *trackDB = TrackDB::instance();

    {
        QString ssFileDir = ":/surround/Resources/surround/";
        if(ssFileDir.length() < fileName.length())
        {
            if(ssFileDir==fileName.mid(0,ssFileDir.length()))
            {
                QSharedPointer<info::Info> ssInfo = info::InfoFactory::createShared("speaker");
                if(ssInfo.data()!=0)
                {
                    ssInfo->setFilename(fileName);
                    if(!ssInfo->title().isEmpty())
                    {
                        return ssInfo;
                    }
                }
            }
        }
    }

    if(trackDB!=0)
    {
        QSharedPointer<DBInfo> dbItem(new DBInfo(fileName, isUpdate));

        if(dbItem->isValid())
        {
            if(!dbItem->isUpdateRequired())
            {
                trackInfo = dbItem.dynamicCast<info::Info>();
                flag = false;
            }
            else
            {
                updateFlag = true;
            }
        }
        else if(dbItem->isUpdateRequired())
        {
            updateFlag = true;
        }
    }

    if(flag)
    {
        common::BIOBufferedStream *ioFile = new common::BIOBufferedStream(common::e_BIOStream_FileRead);

        if(ioFile->open(fileName))
        {
            QSharedPointer<info::Info> tInfo = info::Info::readInfo(ioFile);

            if(trackDB!=0 && tInfo.data()!=0)
            {
                if(updateFlag)
                {
                    trackDB->update(tInfo.data());
                }
                else
                {
                    trackDB->add(tInfo.data());
                }

                {
                    QSharedPointer<DBInfo> dbItem(new DBInfo(fileName));
                    if(dbItem.data()!=0)
                    {
                        trackInfo = dbItem.dynamicCast<info::Info>();
                    }
                    else
                    {
                        trackInfo = tInfo;
                    }
                }
            }
            else
            {
                trackInfo = tInfo;
            }
            ioFile->close();
        }
        delete ioFile;
    }

    QCoreApplication::processEvents(QEventLoop::AllEvents);

    return trackInfo;
}

//-------------------------------------------------------------------------------------------

bool DBInfo::isValid() const
{
    return (m_albumID!=-1 && m_trackID!=-1) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool DBInfo::isUpdateRequired() const
{
    return m_updateFlag;
}

//-------------------------------------------------------------------------------------------

bool DBInfo::isChildren() const
{
    return (m_chapters.size()>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

tint DBInfo::noChildren() const
{
    return m_chapters.size();
}

//-------------------------------------------------------------------------------------------

const info::Info::ChildInfo& DBInfo::child(tint idx) const
{
    return m_chapters.at(idx);
}

//-------------------------------------------------------------------------------------------

bool DBInfo::isImage() const
{
    if(loadImages())
    {
        return (m_imageMap.size()>0);
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

info::ImageInfoArray *DBInfo::getImageData(info::Info::ImageFormat& format) const
{
    static info::IDTagImageType typeOrder[13] = {
        info::e_TagImage_CoverFront,
        info::e_TagImage_LeadArtist,
        info::e_TagImage_Artist,
        info::e_TagImage_Conductor,
        info::e_TagImage_Band,
        info::e_TagImage_Lyricist,
        info::e_TagImage_Illustration,
        info::e_TagImage_BandLogo,
        info::e_TagImage_StudioLogo,
        info::e_TagImage_CoverBack,
        info::e_TagImage_Composer,
        info::e_TagImage_Performance,
        info::e_TagImage_Other};

    tint i;
    info::ImageInfoArray *pArr = 0;

    if(loadImages())
    {
        for(i=0;i<13 && pArr==0;i++)
        {
            pArr = getImageData(typeOrder[i],format);
        }
    }
    return pArr;
}

//-------------------------------------------------------------------------------------------

info::ImageInfoArray *DBInfo::getImageData(info::IDTagImageType type,info::Info::ImageFormat& format) const
{
    info::ImageInfoArray *pArr = 0;
    QMap<info::IDTagImageType,QPair<info::Info::ImageFormat,info::ImageInfoArray *> >::const_iterator ppI;

    if(loadImages())
    {
        ppI = m_imageMap.find(type);
        if(ppI!=m_imageMap.end())
        {
            const QPair<info::Info::ImageFormat,info::ImageInfoArray *>& p = ppI.value();
            format = p.first;
            pArr = p.second;
        }
    }
    return pArr;
}

//-------------------------------------------------------------------------------------------

bool DBInfo::loadRecord()
{
    TrackDB *trackDB = TrackDB::instance();
    bool res = false;

    if(trackDB!=0)
    {
        tint dirID,fileID;

        if(trackDB->getKeysFromFilename(m_fileName,m_albumID,m_trackID,dirID,fileID))
        {
            if(!m_isUpdate || !trackDB->isUpdateRequired(dirID,fileID))
            {
                tint infoType,year,trackNo,discNo,groupID;
                tuint64 timeLength = 0;
                QString cmd;
                SQLiteQuery trackQ(trackDB->m_db);

                cmd  = "SELECT d.infoType, b.artist, b.trackName, a.albumName, a.year, b.comment, e.genreName,";
                cmd += "        b.trackNo, b.discNo, b.composer, b.originalArtist, b.copyright, b.encoder, b.timeLength, a.groupID";
                cmd += "    FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
                cmd += "        INNER JOIN directory AS c ON a.directoryID=c.directoryID";
                cmd += "        INNER JOIN file AS d ON c.directoryID=d.directoryID AND b.fileID=d.fileID";
                cmd += "        OUTER LEFT JOIN genre AS e ON b.genreID=e.genreID";
                cmd += "    WHERE a.albumID=" + QString::number(m_albumID) + " AND b.trackID=" + QString::number(m_trackID);
                trackQ.prepare(cmd);
                trackQ.bind(infoType);
                trackQ.bind(m_Artist);
                trackQ.bind(m_Title);
                trackQ.bind(m_Album);
                trackQ.bind(year);
                trackQ.bind(m_Comment);
                trackQ.bind(m_Genre);
                trackQ.bind(trackNo);
                trackQ.bind(discNo);
                trackQ.bind(m_Composer);
                trackQ.bind(m_OriginalArtist);
                trackQ.bind(m_Copyright);
                trackQ.bind(m_Encoder);
                trackQ.bind(timeLength);
                trackQ.bind(groupID);
                if(trackQ.next())
                {
                    tuint64 subtrackID,timeStart;
                    QString subtrackName;
                    SQLiteQuery subtrackQ(trackDB->m_db);

                    m_Artist = TrackDB::dbStringInv(m_Artist);
                    m_Title = TrackDB::dbStringInv(m_Title);
                    m_Album = TrackDB::dbStringInv(m_Album);
                    m_Comment = TrackDB::dbStringInv(m_Comment);
                    m_Genre = TrackDB::dbStringInv(m_Genre);
                    m_Composer = TrackDB::dbStringInv(m_Composer);
                    m_OriginalArtist = TrackDB::dbStringInv(m_OriginalArtist);
                    m_Copyright = TrackDB::dbStringInv(m_Copyright);
                    m_Encoder = TrackDB::dbStringInv(m_Encoder);

                    m_type = static_cast<Type>(infoType);
                    m_Year = QString::number(year);
                    m_Track = QString::number(trackNo);
                    m_Disc = QString::number(discNo);
                    m_length = timeLength;

                    cmd = "SELECT subtrackID, subtrackName, timeStart, timeLength FROM subtrack WHERE albumID=" + QString::number(m_albumID) + " AND trackID=" + QString::number(m_trackID) + " ORDER BY subtrackID;";
                    subtrackQ.prepare(cmd);
                    subtrackQ.bind(subtrackID);
                    subtrackQ.bind(subtrackName);
                    subtrackQ.bind(timeStart);
                    subtrackQ.bind(timeLength);
                    while(subtrackQ.next())
                    {
                        info::Info::ChildInfo subtrack;
                        subtrack.name() = TrackDB::dbStringInv(subtrackName);
                        subtrack.startTime() = timeStart;
                        subtrack.length() = timeLength;
                        m_chapters.append(subtrack);
                    }

                    m_hash = loadOrGenerateHashID(dirID, fileID);

                    res = true;
                }
                else
                {
                    printError("loadRecord","Failed to get expected track record");
                    m_updateFlag = true;
                }
            }
            else
            {
                m_updateFlag = true;
            }
        }
    }
    else
    {
        printError("loadRecord","No track database available");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int DBInfo::getImageID() const
{
    TrackDB *trackDB = TrackDB::instance();
    tint imageID,iType,iFormat;
    QString cmd;
    SQLiteQuery imageQ(trackDB->m_db);

    cmd  = "SELECT a.imageID, b.type, b.format";
    cmd += "    FROM imagemap AS a INNER JOIN image AS b ON a.imageID=b.imageID";
    cmd += "    WHERE a.albumID=" + QString::number(m_albumID) + " AND a.trackID=" + QString::number(m_trackID) + ";";
    imageQ.prepare(cmd);
    imageQ.bind(imageID);
    imageQ.bind(iType);
    imageQ.bind(iFormat);
    if(!imageQ.next())
    {
        SQLiteQuery aImageQ(trackDB->m_db);

        cmd  = "SELECT b.imageID";
        cmd += "  FROM imagealbummap AS a INNER JOIN image AS b ON a.imageID=b.imageID";
        cmd += "  WHERE a.albumID=" + QString::number(m_albumID);
        aImageQ.prepare(cmd);
        aImageQ.bind(imageID);
        if(!aImageQ.next())
        {
            imageID = -1;
        }
    }
    return imageID;
}

//-------------------------------------------------------------------------------------------

bool DBInfo::loadImages() const
{
    TrackDB *trackDB = TrackDB::instance();

    if(!m_imageFlag && trackDB!=0)
    {
        tint imageID,iType,iFormat;
        QByteArray iData;
        QString cmd;
        SQLiteQuery imageQ(trackDB->m_db);

        cmd  = "SELECT a.imageID, b.type, b.format, b.data";
        cmd += "    FROM imagemap AS a INNER JOIN image AS b ON a.imageID=b.imageID";
        cmd += "    WHERE a.albumID=" + QString::number(m_albumID) + " AND a.trackID=" + QString::number(m_trackID) + ";";
        imageQ.prepare(cmd);
        imageQ.bind(imageID);
        imageQ.bind(iType);
        imageQ.bind(iFormat);
        imageQ.bind(iData);
        while(imageQ.next())
        {
            if(iData.size()>0)
            {
                info::IDTagImageType tagType = static_cast<info::IDTagImageType>(iType);
                ImageFormat imageFormat = static_cast<ImageFormat>(iFormat);
                info::ImageInfoArray *pArr = new info::ImageInfoArray();
                pArr->AppendRaw(iData.data(),iData.size());
                m_imageMap.insert(tagType,QPair<ImageFormat,info::ImageInfoArray *>(imageFormat,pArr));
            }
        }
        if(m_imageMap.isEmpty())
        {
            if(trackDB->updateAlbumImage(m_albumID))
            {
                SQLiteQuery aImageQ(trackDB->m_db);

                cmd  = "SELECT b.type, b.format, b.data";
                cmd += "  FROM imagealbummap AS a INNER JOIN image AS b ON a.imageID=b.imageID";
                cmd += "  WHERE a.albumID=" + QString::number(m_albumID);
                aImageQ.prepare(cmd);
                aImageQ.bind(iType);
                aImageQ.bind(iFormat);
                aImageQ.bind(iData);
                if(aImageQ.next())
                {
                    info::IDTagImageType tagType = static_cast<info::IDTagImageType>(iType);
                    ImageFormat imageFormat = static_cast<ImageFormat>(iFormat);
                    info::ImageInfoArray *pArr = new info::ImageInfoArray();
                    pArr->AppendRaw(iData.data(),iData.size());
                    m_imageMap.insert(tagType,QPair<ImageFormat,info::ImageInfoArray *>(imageFormat,pArr));
                }
            }
        }
        m_imageFlag = true;
    }
    return m_imageFlag;
}

//-------------------------------------------------------------------------------------------

QString DBInfo::directoryGroup() const
{
    tint dirID;
    QString cmdQ,dGroup;
    SQLiteQuery dgQ(TrackDB::instance()->m_db);

    cmdQ  = "SELECT b.directoryID";
    cmdQ += "  FROM album AS a INNER JOIN directory AS b ON a.groupID=b.directoryID";
    cmdQ += "  WHERE a.albumID=" + QString::number(m_albumID) + " AND a.groupID>=0";
    dgQ.prepare(cmdQ);
    dgQ.bind(dirID);
    if(dgQ.next())
    {
        dGroup = TrackDB::instance()->getDirectoryName(dirID);
        return dGroup;
    }
    else
    {
        return info::Info::directoryGroup();
    }
}

//-------------------------------------------------------------------------------------------

tuint64 DBInfo::generateHashID(tint dirID, tint fileID)
{
    tuint64 hashID = 0;
    SQLiteDatabase *db = TrackDB::instance()->db();

    try
    {
        bool res = false;
        QString cmd;
        SQLiteInsert hashI(db);
        common::BIOBufferedStream fileIO(common::e_BIOStream_FileRead);

        db->exec("SAVEPOINT generateHashID");
        if(fileIO.open(m_fileName))
        {
            hashID = Info::calculateELFHash(&fileIO);

            cmd = "INSERT INTO fileHash VALUES (?, ?, ?);";
            hashI.prepare(cmd);
            hashI.bind(dirID);
            hashI.bind(fileID);
            hashI.bind(hashID);
            if(hashI.next())
            {
                res = true;
            }
            else
            {
                printError("generateHashID", "Failed to insert hash record");
            }
            fileIO.close();
        }
        else
        {
            QString err = QString("Failed to open file '%1'").arg(m_fileName);
            printError("generateHashID", err.toUtf8().constData());
        }
        if(res)
        {
            db->exec("RELEASE generateHashID");
        }
        else
        {
            db->exec("ROLLBACK TO SAVEPOINT generateHashID");
        }
    }
    catch(const SQLiteException& e)
    {
        QString errStr;
        db->exec("ROLLBACK TO SAVEPOINT generateHashID");
        errStr = "Database exception thrown. " + e.error();
        printError("generateHashID",errStr.toUtf8().constData());
    }
    return hashID;
}

//-------------------------------------------------------------------------------------------

tuint64 DBInfo::loadOrGenerateHashID(tint dirID, tint fileID)
{
    tuint64 hashID = 0;
    QString cmd;
    SQLiteDatabase *db = TrackDB::instance()->db();

    try
    {
        SQLiteQuery hashQ(db);

        cmd  = "SELECT a.hashID FROM fileHash AS a ";
        cmd += "WHERE a.directoryID=" + QString::number(dirID) + " AND a.fileID=" + QString::number(fileID);
        hashQ.prepare(cmd);
        hashQ.bind(hashID);
        if(!hashQ.next() && m_isUpdate)
        {
            hashID = generateHashID(dirID, fileID);
        }
    }
    catch(const SQLiteException& e)
    {
        QString errStr;
        errStr = "Database exception thrown. " + e.error();
        printError("loadOrGenerateHashID",errStr.toUtf8().constData());
    }
    return hashID;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
