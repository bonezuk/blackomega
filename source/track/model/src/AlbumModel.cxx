#include "track/model/inc/AlbumModel.h"
#include "track/model/inc/AlbumTrackModel.h"
#include "track/model/inc/AlbumModelUtilities.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

AlbumModel::AlbumModel() : AbstractTrackModel(),
    m_albums()
{}

//-------------------------------------------------------------------------------------------

AlbumModel::AlbumModel(const TrackModelKey& filterKey) : AbstractTrackModel(filterKey),
    m_albums()
{}

//-------------------------------------------------------------------------------------------

AlbumModel::~AlbumModel()
{}

//-------------------------------------------------------------------------------------------

TrackModelType AlbumModel::type() const
{
    return e_modelAlbums;
}

//-------------------------------------------------------------------------------------------

int AlbumModel::queryImageID(const AlbumModelKey& key) const
{
    int imageID = -1;
    QString cmd;
    db::SQLiteQuerySPtr query = getDBQuery();
    
    cmd  = "SELECT b.imageID";
    cmd += "  FROM album AS a INNER JOIN imagemap AS b ON a.albumID=b.albumID";
    cmd += "    INNER JOIN image AS c ON b.imageID=c.imageID";
    cmd += "  WHERE a.";
    if(key.isAlbum())
    {
        cmd += QString("albumID=%1").arg(key.id());
    }
    else
    {
        cmd += QString("groupID=%1").arg(key.id());
    }
    query->prepare(cmd);
    query->bind(imageID);
    if(!query->next())
    {
        db::SQLiteQuerySPtr aquery = getDBQuery();
        
        cmd  = "SELECT b.imageID";
        cmd += "  FROM album AS a INNER JOIN imagealbummap AS b ON a.albumID=b.albumID";
        cmd += "    INNER JOIN image AS c ON b.imageID=c.imageID";
        cmd += "  WHERE a.";
        if(key.isAlbum())
        {
            cmd += QString("albumID=%1").arg(key.id());
        }
        else
        {
            cmd += QString("groupID=%1").arg(key.id());
        }
        aquery->prepare(cmd);
        aquery->bind(imageID);
        if(!aquery->next())
        {
            imageID = -1;
        }
    }
    return imageID;
}

//-------------------------------------------------------------------------------------------

int AlbumModel::queryAlbumYear(const AlbumModelKey& key) const
{
    int year;
    QString cmd;
    db::SQLiteQuerySPtr query = getDBQuery();
    
    cmd = "SELECT year FROM album WHERE ";
    if(key.isAlbum())
    {
        cmd += QString("albumID=%1").arg(key.id());
    }
    else
    {
        cmd += QString("groupID=%1").arg(key.id());
    }
    query->prepare(cmd);
    query->bind(year);
    if(!query->next())
    {
        year = 0;
    }
    return year;
}

//-------------------------------------------------------------------------------------------

QString AlbumModel::queryAlbumArtist(const AlbumModelKey& key) const
{
    QString artist;
    AlbumModelUtilities utils(key);
    
    if(utils.isVariousArtists())
    {
        artist = "Various Artists";
    }
    else
    {
        artist = utils.artist();
    }
    return artist;
}

//-------------------------------------------------------------------------------------------

QVariant AlbumModel::dataAtIndex(int idx, int columnIndex) const
{
    QVariant dataItem;
    
    if(idx>=0 && idx < m_albums.size() && columnIndex>=0 && columnIndex<5)
    {
        const QPair<AlbumModelKey,QString>& item = m_albums.at(idx);
        if(columnIndex==0)
        {
            dataItem.setValue(item.first.variant());
        }
        else if(columnIndex == 1)
        {
            dataItem.setValue(item.second);
        }
        else if(columnIndex == 2)
        {
            dataItem = queryAlbumArtist(item.first);
        }
        else if(columnIndex == 3)
        {
            dataItem = queryAlbumYear(item.first);
        }
        else if(columnIndex == 4)
        {
            dataItem = queryImageID(item.first);
        }
    }
    return dataItem;
}

//-------------------------------------------------------------------------------------------

QVariant AlbumModel::data(int rowIndex, int columnIndex) const
{
    return dataAtIndex(rowIndex, columnIndex);
}

//-------------------------------------------------------------------------------------------

QVariant AlbumModel::data(int sectionIndex,int rowIndex,int columnIndex) const
{
    QVariant dataItem;

    if(sectionIndex>=0 && sectionIndex<static_cast<int>(m_index.size()))
    {
        if(rowIndex>=0 && rowIndex<numberRowsInSection(sectionIndex))
        {
            int aIndex = rowIndex + m_index.at(sectionIndex);
            dataItem = dataAtIndex(aIndex, columnIndex);
        }
    }
    return dataItem;
}

//-------------------------------------------------------------------------------------------

int AlbumModel::size() const
{
    return m_albums.size();
}

//-------------------------------------------------------------------------------------------

int AlbumModel::numberSections() const
{
    return m_index.size();
}

//-------------------------------------------------------------------------------------------

int AlbumModel::numberRowsInSection(int secIdx) const
{
    if(secIdx>=0 && secIdx<static_cast<int>(m_index.size()))
    {
        int sPosition = m_index.at(secIdx);
        int ePosition = ((secIdx + 1) < static_cast<int>(m_index.size())) ? m_index.at(secIdx+1) : m_albums.size();
        if(sPosition < ePosition)
        {
            return ePosition - sPosition;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

bool AlbumModel::build()
{
    return populate();
}

//-------------------------------------------------------------------------------------------

bool AlbumModel::populate()
{
    QString cmdQ;
    QueryResult results;
    AlbumModelKey albumKey;
    bool res = false;
    
    cmdQ = getQuery(albumKey);
    if(runAlbumQuery(cmdQ,results))
    {
        enumerateSections(results);
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// The enumerateSections is used to scan through the album names.
// Based on the lower case first letter of the trimmed album name
// they are placed into sections defining all album names beginning
// with that associated letter.
//-------------------------------------------------------------------------------------------

void AlbumModel::enumerateSections(const QueryResult& results)
{
    QVector<QChar> alphabet;
    QMap<QChar,int> indexMap;
    QMap<QChar,QMultiMap<QString,int> > sectionMap;
    
    alphabet = getIndexAlphabet();
    buildIndexMap(alphabet,indexMap);
    mapResultsToAlphabetIndex(results,indexMap,alphabet,sectionMap);
    buildModelFromSortedIndex(results,alphabet,sectionMap);
}

//-------------------------------------------------------------------------------------------

void AlbumModel::insertIntoAlbum(QVector<QueryRecord>& recordList)
{
    std::sort(recordList.begin(),recordList.end(),AlbumModel::compareIdenticalAlbumNameLessThan);
    
    for(QVector<QueryRecord>::const_iterator ppI=recordList.begin();ppI!=recordList.end();ppI++)
    {
        const QueryRecord& record = *ppI;
        m_albums.append(QPair<AlbumModelKey,QString>(keyRecordAlbum(record),nameRecordAlbum(record)));
    }
    recordList.clear();
}

//-------------------------------------------------------------------------------------------

void AlbumModel::addToModelForGivenMap(const QueryResult& results,const QMultiMap<QString,int>& map)
{
    QVector<QueryRecord> recordList;

    for(QMultiMap<QString,int>::const_iterator ppI=map.begin();ppI!=map.end();ppI++)
    {
        const QueryRecord& record = results.at(ppI.value());
        
        if(!recordList.isEmpty())
        {
            QString prevIndexName = nameRecordAlbum(recordList.at(0)).trimmed().toLower();
            if(prevIndexName!=ppI.key())
            {
                insertIntoAlbum(recordList);
            }
        }
        recordList.append(record);
    }
    insertIntoAlbum(recordList);
}

//-------------------------------------------------------------------------------------------

void AlbumModel::buildModelFromSortedIndex(const QueryResult& results,const QVector<QChar>& alphabet,const QMap<QChar, QMultiMap<QString,int> >& sectionMap)
{
    m_albums.clear();
    m_index.clear();
    
    for(int i=0;i<alphabet.size();i++)
    {
        QMap<QChar,QMultiMap<QString,int> >::const_iterator ppI;
        
        m_index.push_back(m_albums.size());
        ppI = sectionMap.find(alphabet.at(i));
        if(ppI!=sectionMap.end())
        {
            addToModelForGivenMap(results,ppI.value());
        }
    }
}

//-------------------------------------------------------------------------------------------
// a < b -> true
// a > b -> false
// album < group
// a.album < b.group -> true
// a.group > b.album -> false
//-------------------------------------------------------------------------------------------

bool AlbumModel::compareIdenticalAlbumNameLessThan(const QueryRecord& a,const QueryRecord& b)
{
    AlbumModelKey keyA = keyRecordAlbumStatic(a);
    AlbumModelKey keyB = keyRecordAlbumStatic(b);
    bool res = false;
    
    if(keyA.isGroup())
    {
        if(keyB.isGroup())
        {
            res = (keyA.id() < keyB.id()) ? true : false;
        }
        else
        {
            res = false;
        }
    }
    else
    {
        if(keyB.isAlbum())
        {
            res = (keyA.id() < keyB.id()) ? true : false;
        }
        else
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AlbumModel::buildIndexMap(const QVector<QChar>& alphabet,QMap<QChar,int>& indexMap) const
{
    indexMap.clear();
    for(int i=0;i<alphabet.size();i++)
    {
        indexMap.insert(alphabet.at(i),i);
    }
}

//-------------------------------------------------------------------------------------------

QChar AlbumModel::getFirstCharacter(const QString& name,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet) const
{
    QChar c;
    QString n = name.trimmed();
    
    if(!n.isEmpty())
    {
        c = n.at(0).toLower();
        if(indexMap.find(c)==indexMap.end())
        {
            c = alphabet.at(alphabet.size() - 1);
        }
    }
    else
    {
        c = alphabet.at(alphabet.size() - 1);
    }
    return c;
}

//-------------------------------------------------------------------------------------------

void AlbumModel::mapResultsToAlphabetIndex(const QueryResult& results,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet,QMap<QChar, QMultiMap<QString,int> >& sectionMap) const
{
    int i;

    sectionMap.clear();
    
    for(i=0;i<static_cast<int>(results.size());i++)
    {
        QString name = nameRecordAlbum(results.at(i)).trimmed();
        QChar firstChar = getFirstCharacter(name,indexMap,alphabet);
        QMap<QChar, QMultiMap<QString,int> >::iterator ppJ = sectionMap.find(firstChar);
        if(ppJ==sectionMap.end())
        {
            QMultiMap<QString,int> newMap;
            sectionMap.insert(firstChar,newMap);
            ppJ = sectionMap.find(firstChar);
        }
        QMultiMap<QString,int>& rMap = ppJ.value();
        rMap.insert(name.toLower(),i);
    }
}

//-------------------------------------------------------------------------------------------
// Get the alphabet sequence in unicode format, allows for internationization.
// The default implementation gives the alphabet.
// All alphabet characters are given in lowercase.
// The end character is used to represent all names beginning without a letter in
// the given alphabet.
//-------------------------------------------------------------------------------------------

QVector<QChar> AlbumModel::getIndexAlphabet() const
{
    QVector<QChar> alphabet;
    tchar letter;
    
    letter = 'a';
    while(letter <= 'z')
    {
        alphabet.append(QChar(letter));
        letter++;
    }
    alphabet.append(QChar('#'));
    return alphabet;
}

//-------------------------------------------------------------------------------------------

QString AlbumModel::getQuery(const AlbumModelKey& albumID) const
{
    QString cmdQ;

    cmdQ  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag, ";
    cmdQ += "CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id, ";
    cmdQ += "a.albumname ";
    
    cmdQ += "FROM album AS a ";
    if(m_filterKey.isArtist() || m_filterKey.isGenre())
    {
        cmdQ += "INNER JOIN track AS b ON a.albumID=b.albumID ";
    }
    
    if(!albumID.isAll() || m_filterKey.isArtist() || m_filterKey.isGenre())
    {
        cmdQ += "WHERE ";
    
        if(!albumID.isAll())
        {
            cmdQ += (albumID.isGroup()) ? "a.groupID" : "a.albumID";
            cmdQ += "=" + QString::number(albumID.id()) + " ";
        }
        
        if(m_filterKey.isArtist() || m_filterKey.isGenre())
        {            
            if(m_filterKey.isGenre())
            {
                if(!albumID.isAll())
                {
                    cmdQ += "AND ";
                }

                cmdQ += "b.genreID=" + QString::number(m_filterKey.genre()) + " ";
            }
            
            if(m_filterKey.isArtist())
            {
                QString dbArtist = db::TrackDB::dbString(m_filterKey.artist());
                
                if(!albumID.isAll() || m_filterKey.isGenre())
                {
                    cmdQ += "AND ";
                }
                
                cmdQ += "(b.artist LIKE \'" + dbArtist + "\' OR ";
                cmdQ += "b.originalArtist LIKE \'" + dbArtist + "\' OR ";
                cmdQ += "b.composer LIKE \'" + dbArtist + "\') ";

            }
        }
    }
    
    cmdQ += "ORDER BY a.albumname";

    return cmdQ;
}

//-------------------------------------------------------------------------------------------

QueryRecord AlbumModel::createRecordAlbum(const AlbumModelKey& key,const QString& name) const
{
    QueryRecord r;
    r.push_back(key.isGroup());
    r.push_back(key.id());
    r.push_back(db::TrackDB::dbStringInv(name));
    return r;
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModel::keyRecordAlbum(const QueryRecord& record) const
{
    return keyRecordAlbumStatic(record);
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModel::keyRecordAlbumStatic(const QueryRecord& record)
{
    std::pair<bool,int> key(false,-1);
    if(record.size()>=2)
    {
        key = std::pair<bool,int>(record.at(0).toBool(),record.at(1).toInt());
    }
    AlbumModelKey k(key);
    return k;
}

//-------------------------------------------------------------------------------------------

QString AlbumModel::nameRecordAlbum(const QueryRecord& record) const
{
    return nameRecordAlbumStatic(record);
}

//-------------------------------------------------------------------------------------------

QString AlbumModel::nameRecordAlbumStatic(const QueryRecord& record)
{
    QString name;
    if(record.size()>=3)
    {
        name = record.at(2).toString();
    }
    return name;
}

//-------------------------------------------------------------------------------------------

bool AlbumModel::runAlbumQuery(const QString& cmdQ,QueryResult& results) const
{
    bool res = false;
    
    try
    {
        bool isGroup, isPush;
        int albumID;
        QString albumName;
        QSet<int> pGroups;
        db::SQLiteQuerySPtr query = getDBQuery();
        
        query->prepare(cmdQ);
        query->bind(isGroup);
        query->bind(albumID);
        query->bind(albumName);
        
        while(query->next())
        {
            AlbumModelKey key(std::pair<bool,int>(isGroup,albumID));
            
            isPush = true;
            if(isGroup)
            {
                if(pGroups.find(albumID) == pGroups.end())
                {
                    pGroups.insert(albumID);
                }
                else
                {
                    isPush = false;
                }
            }
            if(isPush)
            {
                results.push_back( createRecordAlbum(key,albumName) );
            }
        }
        res = true;
    }
    catch(const db::SQLiteException&)
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint AlbumModel::findSectionIndex(const QString& albumName)
{
    tint sectionIdx;
    QString name;
    QChar firstChar;
    QVector<QChar> alphabet;
    QMap<QChar,int> indexMap;
    QMap<QChar,int>::iterator ppI;

    alphabet = getIndexAlphabet();
    buildIndexMap(alphabet, indexMap);
    name = albumName.trimmed();
    firstChar = getFirstCharacter(name, indexMap, alphabet);
    ppI = indexMap.find(firstChar);
    if(ppI != indexMap.end())
    {
        sectionIdx = ppI.value();
    }
    else
    {
        sectionIdx = alphabet.size() - 1;
    }
    if(sectionIdx < 0)
    {
        sectionIdx = 0;
    }
    else if(sectionIdx > m_index.size())
    {
        sectionIdx = m_index.size() - 1;
    }
    return sectionIdx;
}

//-------------------------------------------------------------------------------------------

tint AlbumModel::numberOfTracks(const AlbumModelKey& key) const
{
    tint count = 0;
    TrackModelKey filter = filterKey();
    filter.album() = key;
    AlbumTrackModel trackModel(filter);
    if(trackModel.build())
    {
        count = trackModel.size();
    }
    return count;
}

//-------------------------------------------------------------------------------------------

bool AlbumModel::isAlbumListed(const AlbumModelKey& key) const
{
    bool res = false;
    
    for(tint idx = 0; idx < m_albums.size() && !res; idx++)
    {
        const QPair<AlbumModelKey,QString>& item = m_albums.at(idx);
        if(key == item.first)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QVector<tint> AlbumModel::indexForDBInfo(QSharedPointer<db::DBInfo>& dbItem, bool isAdd)
{
    QVector<tint> idxList;

    if(!dbItem.isNull())
    {
        tint idx, noTracks;
        AlbumModelKey key = AlbumModelKey::keyForDBInfo(dbItem);
        
        if(isAdd)
        {
            if(!isAlbumListed(key))
            {
                QString name = dbItem->album().trimmed();
                tint sectionIdx = findSectionIndex(name);
                tint startIdx, endIdx;
                
                startIdx = m_index.at(sectionIdx);
                endIdx = ((startIdx + 1) < m_index.size()) ? m_index.at(sectionIdx + 1) : m_albums.size();
                for(idx = startIdx; idx < endIdx; idx++)
                {
                    const QPair<AlbumModelKey,QString>& item = m_albums.at(idx);
                    if(name.toLower() < item.second.toLower())
                    {
                        break;
                    }
                }
                idxList.append(idx);
            }
        }
        else
        {
            noTracks = numberOfTracks(key);
            if(noTracks <= 1)
            {
                for(idx = 0; idx < m_albums.size(); idx++)
                {
                    const QPair<AlbumModelKey,QString>& item = m_albums.at(idx);
                    if(key == item.first)
                    {
                        idxList.append(idx);
                    }
                }
            }
        }
    }
    return idxList;
}

//-------------------------------------------------------------------------------------------

void AlbumModel::addDBInfo(tint idx, QSharedPointer<db::DBInfo>& dbItem)
{
    if(!dbItem.isNull())
    {
        AlbumModelKey key = AlbumModelKey::keyForDBInfo(dbItem);
        QString name = dbItem->album().trimmed();
        tint sectionIdx = findSectionIndex(name) + 1;
        while(sectionIdx < m_index.size())
        {
            m_index[sectionIdx] += 1;
            sectionIdx++;
        }
        m_albums.insert(idx, QPair<AlbumModelKey,QString>(key, dbItem->album()));
    }
}

//-------------------------------------------------------------------------------------------

void AlbumModel::removeRow(tint idx)
{
    if(idx >= 0 && idx < m_albums.size())
    {
        tint secIdx, startIdx;
        
        for(secIdx = 0; secIdx < m_index.size(); secIdx++)
        {
            startIdx = m_index.at(secIdx);
            if(startIdx > idx)
            {
                m_index[secIdx] -= 1;
            }
        }
        m_albums.removeAt(idx);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namepsace track
} // namespace omega
//-------------------------------------------------------------------------------------------
