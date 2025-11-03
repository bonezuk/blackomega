#include "track/model/inc/AlbumTrackModel.h"
#include "common/inc/DiskOps.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------
// AlbumTrackModel::Record
//-------------------------------------------------------------------------------------------

AlbumTrackModel::Record::Record() : m_groupID(-1),
    m_albumID(-1),
    m_trackID(-1),
    m_subtrackID(-1),
    m_discNo(-1),
    m_trackNo(-1),
    m_albumName(),
    m_trackName(),
    m_fileName(),
    m_length(),
    m_artist(),
    m_originalArtist(),
    m_composer()
{}

//-------------------------------------------------------------------------------------------

AlbumTrackModel::Record::Record(const Record& rhs) : m_groupID(-1),
    m_albumID(-1),
    m_trackID(-1),
    m_subtrackID(-1),
    m_discNo(-1),
    m_trackNo(-1),
    m_albumName(),
    m_trackName(),
    m_fileName(),
    m_length(),
    m_artist(),
    m_originalArtist(),
    m_composer()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const AlbumTrackModel::Record& AlbumTrackModel::Record::operator = (const Record& rhs)
{
    if(this != &rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void AlbumTrackModel::Record::copy(const Record& rhs)
{
    m_groupID = rhs.m_groupID;
    m_albumID = rhs.m_albumID;
    m_trackID = rhs.m_trackID;
    m_subtrackID = rhs.m_subtrackID;
    m_discNo = rhs.m_discNo;
    m_trackNo = rhs.m_trackNo;
    m_albumName = rhs.m_albumName;
    m_trackName = rhs.m_trackName;
    m_fileName = rhs.m_fileName;
    m_length = rhs.m_length;
    m_artist = rhs.m_artist;
    m_originalArtist = rhs.m_originalArtist;
    m_composer = rhs.m_composer;
}

//-------------------------------------------------------------------------------------------

bool AlbumTrackModel::Record::isGroup() const
{
    return (m_groupID >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

const tint& AlbumTrackModel::Record::groupID() const
{
    return m_groupID;
}

//-------------------------------------------------------------------------------------------

const tint& AlbumTrackModel::Record::albumID() const
{
    return m_albumID;
}

//-------------------------------------------------------------------------------------------

const tint& AlbumTrackModel::Record::trackID() const
{
    return m_trackID;
}

//-------------------------------------------------------------------------------------------

const tint& AlbumTrackModel::Record::subtrackID() const
{
    return m_subtrackID;
}

//-------------------------------------------------------------------------------------------

const tint& AlbumTrackModel::Record::discNo() const
{
    return m_discNo;
}

//-------------------------------------------------------------------------------------------

const tint& AlbumTrackModel::Record::trackNo() const
{
    return m_trackNo;
}

//-------------------------------------------------------------------------------------------

const QString& AlbumTrackModel::Record::albumName() const
{
    return m_albumName;
}

//-------------------------------------------------------------------------------------------

const QString& AlbumTrackModel::Record::trackName() const
{
    return m_trackName;
}

//-------------------------------------------------------------------------------------------

const QString& AlbumTrackModel::Record::fileName() const
{
    return m_fileName;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AlbumTrackModel::Record::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

const QString& AlbumTrackModel::Record::artist() const
{
    return m_artist;
}

//-------------------------------------------------------------------------------------------

const QString& AlbumTrackModel::Record::originalArtist() const
{
    return m_originalArtist;
}

//-------------------------------------------------------------------------------------------

const QString& AlbumTrackModel::Record::composer() const
{
    return m_composer;
}

//-------------------------------------------------------------------------------------------

void AlbumTrackModel::Record::set(tint vGroupID, tint vAlbumID, tint vTrackID, tint vSubtrackID,
                 tint vDiscNo, tint vTrackNo, const QString& vAlbumName, const QString& vTrackName, const QString& vFileName,
                 const QString& vArtist, const QString& vOrgArtist, const QString& vComposer, common::TimeStamp& vLength)
{
    m_groupID = vGroupID;
    m_albumID = vAlbumID;
    m_trackID = vTrackID;
    m_subtrackID = vSubtrackID;
    m_discNo = vDiscNo;
    m_trackNo = vTrackNo;
    m_albumName = vAlbumName;
    m_trackName = vTrackName;
    m_fileName = vFileName;
    m_length = vLength;
    m_artist = vArtist;
    m_originalArtist = vOrgArtist;
    m_composer = vComposer;
}

//-------------------------------------------------------------------------------------------
// AlbumTrackModel
//-------------------------------------------------------------------------------------------

AlbumTrackModel::AlbumTrackModel() : AbstractTrackModel()
{}

//-------------------------------------------------------------------------------------------

AlbumTrackModel::AlbumTrackModel(const TrackModelKey& filterKey) : AbstractTrackModel(filterKey)
{}

//-------------------------------------------------------------------------------------------

AlbumTrackModel::~AlbumTrackModel()
{}

//-------------------------------------------------------------------------------------------

void AlbumTrackModel::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "AlbumTrackModel::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

TrackModelType AlbumTrackModel::type() const
{
    return e_modelAlbumTracks;
}

//-------------------------------------------------------------------------------------------

QVariant AlbumTrackModel::data(int rowIndex, int columnIndex) const
{
    QVariant v;

    if(rowIndex >= 0 && rowIndex < m_tracks.size())
    {
        const Record& r = m_tracks.at(rowIndex);
        ColumnType clType = static_cast<ColumnType>(columnIndex);

        switch(clType)
        {
            case e_isGroup:
                v = QVariant(r.isGroup());
                break;
            case e_groupID:
                v = QVariant(r.groupID());
                break;
            case e_albumID:
                v = QVariant(r.albumID());
                break;
            case e_trackID:
                v = QVariant(r.trackID());
                break;
            case e_subtrackID:
                v = QVariant(r.subtrackID());
                break;
            case e_discNo:
                v = QVariant(r.discNo());
                break;
            case e_trackNo:
                v = QVariant(r.trackNo());
                break;
            case e_albumName:
                v = QVariant(r.albumName());
                break;
            case e_trackName:
                v = QVariant(r.trackName());
                break;
            case e_fileName:
                v = QVariant(r.fileName());
                break;
            case e_length:
                v = QVariant(static_cast<tfloat64>(r.length()));
                break;
            case e_artist:
                v = QVariant(r.artist());
                break;
            case e_originalArtist:
                v = QVariant(r.originalArtist());
                break;
            case e_composer:
                v = QVariant(r.composer());
                break;
            default:
                break;
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

QVariant AlbumTrackModel::data(int sectionIndex,int rowIndex,int columnIndex) const
{
    QVariant v;

    if(!sectionIndex)
    {
        v = data(rowIndex, columnIndex);
    }
    return v;
}

//-------------------------------------------------------------------------------------------

int AlbumTrackModel::size() const
{
    return m_tracks.size();
}

//-------------------------------------------------------------------------------------------

int AlbumTrackModel::numberSections() const
{
    return 1;
}

//-------------------------------------------------------------------------------------------

int AlbumTrackModel::numberRowsInSection(int secIdx) const
{
    return (!secIdx) ? m_tracks.size() : 0;
}

//-------------------------------------------------------------------------------------------

bool AlbumTrackModel::build()
{
    return populate();
}

//-------------------------------------------------------------------------------------------

bool AlbumTrackModel::populate()
{
    bool res = false;

    try
    {
        tint flag, gaID, groupID, albumID, trackID, subtrackID, dirID;
        tint discNo, trackNo;
        tuint64 tLen;
        common::TimeStamp tLength;
        QString albumName, trackName, directoryName, fileName;
        QString artist, originalArtist, composer;
        QString cmdQ = getQuery(false);
        db::SQLiteQuerySPtr trackQ = getDBQuery();

        m_tracks.clear();

        trackQ->prepare(cmdQ);
        trackQ->bind(flag);
        trackQ->bind(gaID);
        trackQ->bind(groupID);
        trackQ->bind(albumID);
        trackQ->bind(trackID);
        trackQ->bind(subtrackID);
        trackQ->bind(discNo);
        trackQ->bind(trackNo);
        trackQ->bind(albumName);
        trackQ->bind(trackName);
        trackQ->bind(dirID);
        trackQ->bind(fileName);
        trackQ->bind(tLen);
        trackQ->bind(artist);
        trackQ->bind(originalArtist);
        trackQ->bind(composer);

        while(trackQ->next())
        {
            QString fName;
            albumName = db::TrackDB::dbStringInv(albumName);
            trackName = db::TrackDB::dbStringInv(trackName);
            directoryName = db::TrackDB::instance()->getDirectoryName(dirID);
            artist = db::TrackDB::dbStringInv(artist);
            originalArtist = db::TrackDB::dbStringInv(originalArtist);
            composer = db::TrackDB::dbStringInv(composer);
            fileName = db::TrackDB::dbStringInv(fileName);
            fName = common::DiskOps::mergeName(directoryName, fileName);
            fName = QDir::toNativeSeparators(fName);
            tLength = tLen;
            if(common::DiskOps::exist(fName))
            {
                Record r;
                r.set(groupID, albumID, trackID, subtrackID, discNo, trackNo, albumName, trackName, fName,
                    artist, originalArtist, composer, tLength);
                m_tracks.append(r);
            }
            else
            {
                QString err = QString("Expected file '%1' referenced in database not found").arg(fName);
                printError("populate", err.toUtf8().constData());
            }
        }
        res = true;
    }
    catch(const db::SQLiteException& e)
    {
        QString err = QString("SQL exception in populating model. %1").arg(e.error());
        printError("populate", err.toUtf8().constData());
        res = false;
    }
    return res;
}


//-------------------------------------------------------------------------------------------

const AlbumModelKey& AlbumTrackModel::getAlbumID() const
{
    return m_filterKey.album();
}

//-------------------------------------------------------------------------------------------

void AlbumTrackModel::applyAnd(QString& cmd, bool isAnd) const
{
    if(isAnd)
    {
        cmd += "AND ";
    }
}

//-------------------------------------------------------------------------------------------

QString AlbumTrackModel::getQuery(bool isIDOnly) const
{
    bool isAnd = false;
    QString cmd;

    cmd  = "SELECT ";
    cmd += "CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag, ";
    cmd += "CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id, ";
    cmd += "a.groupID, a.albumID, b.trackID, ";
    cmd += "CASE WHEN e.subtrackID IS NULL THEN -1 ELSE e.subtrackID END AS subtrackID ";
    cmd += ", b.discNo, b.trackNo ";
    if(!isIDOnly)
    {
        cmd += ",a.albumName, ";
        cmd += "CASE WHEN e.subtrackID IS NULL THEN b.trackName ELSE e.subtrackName END AS trackName, ";
        cmd += "c.directoryID, d.fileName, ";
        cmd += "b.timeLength, b.artist, b.originalArtist, b.composer ";
    }
    cmd += "FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID ";
    cmd += "INNER JOIN directory AS c ON a.directoryID=c.directoryID ";
    cmd += "INNER JOIN file AS d ON a.directoryID=d.directoryID AND b.fileID=d.fileID ";
    cmd += "LEFT JOIN subtrack AS e ON a.albumID=e.albumID AND b.trackID=e.trackID ";

    if(!m_filterKey.isEmpty())
    {
        cmd += "WHERE ";

        if(!m_filterKey.album().isAll())
        {
            cmd += (m_filterKey.album().isGroup()) ? "a.groupID" : "a.albumID";
            cmd += "=" + QString::number(m_filterKey.album().id()) + " ";
            isAnd = true;
        }
        if(m_filterKey.isTrack())
        {
            applyAnd(cmd, isAnd);
            cmd += "b.trackID=" + QString::number(m_filterKey.track()) + " ";
            isAnd = true;
        }
        if(m_filterKey.isSubtrack())
        {
            applyAnd(cmd, isAnd);
            cmd += "e.subtrackID=" + QString::number(m_filterKey.subtrack()) + " ";
            isAnd = true;
        }
        if(m_filterKey.isGenre())
        {
            applyAnd(cmd, isAnd);
            cmd += "b.genreID=" + QString::number(m_filterKey.genre()) + " ";
            isAnd = true;
        }
        if(m_filterKey.isArtist())
        {
            QString dbArtist = db::TrackDB::dbString(m_filterKey.artist());
            applyAnd(cmd, isAnd);
            cmd += "(b.artist LIKE \'" + db::TrackDB::dbString(dbArtist) + "\' OR ";
            cmd += "b.originalArtist LIKE \'" + db::TrackDB::dbString(dbArtist) + "\' OR ";
            cmd += "b.composer LIKE \'" + db::TrackDB::dbString(dbArtist) + "\') ";
        }
    }
    cmd += "ORDER BY flag, id, b.discNo, b.trackNo, e.subtrackID";
    return cmd;
}

//-------------------------------------------------------------------------------------------

QVector<QPair<tint, tint> > AlbumTrackModel::indexForDBInfo(QSharedPointer<db::DBInfo>& dbItem, bool isAdd)
{
    QVector<QPair<tint, tint> > idxList;

    if(!dbItem.isNull())
    {
        if(isAdd)
        {
            try
            {
                tint idx, flag, keyID, groupID, albumID, trackID, subtrackID;
                tint discNo, trackNo;
                QString cmdQ = getQuery(true);
                db::SQLiteQuerySPtr trackQ = getDBQuery();

                trackQ->prepare(cmdQ);
                trackQ->bind(flag);
                trackQ->bind(keyID);
                trackQ->bind(groupID);
                trackQ->bind(albumID);
                trackQ->bind(trackID);
                trackQ->bind(subtrackID);
                trackQ->bind(discNo);
                trackQ->bind(trackNo);

                idx = 0;
                while(trackQ->next())
                {
                    if(albumID == dbItem->albumID() && trackID == dbItem->trackID())
                    {
                        idxList.append(QPair<tint, tint>(idx, subtrackID));
                    }
                    idx++;
                }
            }
            catch(const db::SQLiteException& e)
            {
                QString err = QString("Exception on SQL. %1").arg(e.error());
                printError("indexForDBInfo", err.toUtf8().constData());
            }
        }
        else
        {
            for(tint idx = 0; idx < m_tracks.size(); idx++)
            {
                const Record& r = m_tracks.at(idx);
                if(r.albumID() == dbItem->albumID() && r.trackID() == dbItem->trackID())
                {
                    idxList.append(QPair<tint, tint>(idx, r.subtrackID()));
                }
            }
        }
    }
    return idxList;
}

//-------------------------------------------------------------------------------------------

void AlbumTrackModel::addDBInfo(tint idx, tint subtrackID, QSharedPointer<db::DBInfo>& dbItem)
{
    tint groupID;
    Record r;

    groupID = AlbumModelKey::groupIDFromDBInfo(dbItem);
    r.set(groupID, dbItem->albumID(), dbItem->trackID(), subtrackID, dbItem->disc().toInt(), dbItem->track().toInt(),
        dbItem->album(), dbItem->title(), dbItem->getFilename(), dbItem->artist(), dbItem->originalArtist(), dbItem->composer(), dbItem->length());
    if(idx >= 0 && idx < m_tracks.size())
    {
        m_tracks.insert(idx, r);
    }
    else
    {
        m_tracks.append(r);
    }
}

//-------------------------------------------------------------------------------------------

void AlbumTrackModel::removeRow(tint idx)
{
    if(idx >= 0 && idx < m_tracks.size())
    {
        m_tracks.removeAt(idx);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------

