#include "track/model/inc/AlbumModelUtilities.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

AlbumModelUtilities::AlbumModelUtilities() : m_albumID(),
    m_filterKey(),
    m_artist(),
    m_composer()
{}

//-------------------------------------------------------------------------------------------

AlbumModelUtilities::AlbumModelUtilities(const AlbumModelKey& albumID) : m_albumID(albumID),
    m_filterKey(),
    m_artist(),
    m_composer()
{}

//-------------------------------------------------------------------------------------------

AlbumModelUtilities::AlbumModelUtilities(const AlbumModelKey& albumID,const TrackModelKey& key) : m_albumID(albumID),
    m_filterKey(key),
    m_artist(),
    m_composer()
{}    

//-------------------------------------------------------------------------------------------

AlbumModelUtilities::~AlbumModelUtilities()
{}

//-------------------------------------------------------------------------------------------

const AlbumModelKey& AlbumModelUtilities::id() const
{
    return m_albumID;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::getArtistTypeString(int artistType) const
{
    QString type;
    
    switch(artistType)
    {
        case 1:
            type = "originalArtist";
            break;
            
        case 2:
            type = "composer";
            break;
        
        case 0:
        default:
            type = "artist";
    }
    return type;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::getQueryForAlbumArtist() const
{
    AlbumModelKey key(m_albumID);
    QString cmdQ;
    
    if(key.isAll())
    {
        key.setAsAlbum();
    }
    
    cmdQ  = getQueryPartForAlbumArtist(0,key);
    cmdQ += " UNION ";
    cmdQ += getQueryPartForAlbumArtist(1,key);
    cmdQ += " UNION ";
    cmdQ += getQueryPartForAlbumArtist(2,key);
    cmdQ += " ORDER BY groupFlag, albumID, artistCount DESC";
    return cmdQ;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::getQueryPartForAlbumArtist(int artistType,const AlbumModelKey& key) const
{
    QString artist = getArtistTypeString(artistType);
    QString type = (key.isGroup()) ? "group" : "album";
    QString cmdQ;
    
    cmdQ  = "SELECT ";
    cmdQ += (key.isGroup()) ? "1" : "0";
    cmdQ += " AS groupFlag, b." + type + "ID AS albumID, a.";
    cmdQ += artist + " AS artist, COUNT(a." + artist + ") AS artistCount ";
    cmdQ += "FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID ";
    cmdQ += "WHERE b.";
    if(!key.isAll())
    {
        cmdQ += type + "ID=" + QString::number(key.id());
    }
    else
    {
        cmdQ += "groupID";
        cmdQ += (key.isGroup()) ? ">=0" : "=-1";
    }
    cmdQ += " AND a." + artist + " NOT LIKE \'\' ";
    cmdQ += getQueryConditionForFilter();
    cmdQ += "GROUP BY b." + type + "ID, a." + artist + " ";
    
    if(key.isAlbum() && key.isAll())
    {
        AlbumModelKey gKey(key);
        gKey.setAsGroup();
        cmdQ += "UNION " + getQueryPartForAlbumArtist(artistType,gKey);
    }
    return cmdQ;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::getQueryConditionForFilter() const
{
    QString cond;
    
    if(m_filterKey.isGenre() || m_filterKey.isArtist())
    {
        cond = " AND ";
        if(m_filterKey.isGenre())
        {
            cond += "a.genreID=" + QString::number(m_filterKey.genre()) + " ";
            if(m_filterKey.isArtist())
            {
                cond += " AND ";
            }
        }
        if(m_filterKey.isArtist())
        {
            QString dbArtist = db::TrackDB::dbString(m_filterKey.artist());
        
            cond += "(a.artist LIKE \'" + dbArtist + "\' OR ";
            cond += "a.originalArtist LIKE \'" + dbArtist + "\' OR ";
            cond += "a.composer LIKE \'" + dbArtist + "\')";
        }
    }
    return cond;
}

//-------------------------------------------------------------------------------------------

QueryRecord AlbumModelUtilities::createRecordAlbumArtist(const AlbumModelKey& key,const QString& artist,int artistCount) const
{
    QueryRecord r;
    r.push_back(key.isGroup());
    r.push_back(key.id());
    r.push_back(artist);
    r.push_back(artistCount);
    return r;
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModelUtilities::keyAlbumArtistRecord(const QueryRecord& record) const
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

QString AlbumModelUtilities::artistAlbumArtistRecord(const QueryRecord& record) const
{
    QString artist;
    if(record.size()>=3)
    {
        artist = record.at(2).toString();
    }
    return artist;
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilities::countAlbumArtistRecord(const QueryRecord& record) const
{
    int artistCount = 0;
    if(record.size()>=4)
    {
        artistCount = record.at(3).toInt();
    }
    return artistCount;
}

//-------------------------------------------------------------------------------------------

db::SQLiteQuerySPtr AlbumModelUtilities::getDBQuery() const
{
    db::SQLiteQuerySPtr pDB(new db::SQLiteQuery(db::TrackDB::instance()->db()));
    return pDB;
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilities::runAlbumArtistQuery(const QString& cmdQ,QueryResult& results) const
{
    bool res = false;
    
    try
    {
        bool isGroup;
        int albumID,artistCount;
        QString artist;
        db::SQLiteQuerySPtr query = getDBQuery();
        
        query->prepare(cmdQ);
        query->bind(isGroup);
        query->bind(albumID);
        query->bind(artist);
        query->bind(artistCount);
        
        while(query->next())
        {
            AlbumModelKey key(std::pair<bool,int>(isGroup,albumID));
            results.push_back( createRecordAlbumArtist(key,artist,artistCount) );
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

QString AlbumModelUtilities::primaryArtist(const QueryResult& results) const
{
    QString primary;

    if(!results.empty())
    {
        int count = 0,total = 0;
        QMap<QString,int> aMap;
        QMap<QString,int>::iterator ppJ;
        QueryResult::const_iterator ppI;
    
        for(ppI=results.begin();ppI!=results.end();ppI++)
        {
            const QueryRecord& record = *ppI;
            int artistCount = countAlbumArtistRecord(record);
            QString artist = artistAlbumArtistRecord(record);
            ppJ = aMap.find(artist);
            if(ppJ!=aMap.end())
            {
                ppJ.value() += artistCount;
            }
            else
            {
                aMap.insert(artist,artistCount);
            }
            total += artistCount;
        }
        
        if(total > 0)
        {
            for(ppJ=aMap.begin();ppJ!=aMap.end();ppJ++)
            {
                tfloat32 ratio = static_cast<tfloat32>(ppJ.value()) / static_cast<tfloat32>(total);
                if(ratio >= 0.45f)
                {
                    primary = (!count) ? ppJ.key() : QString::fromUtf8("Various Artists");
                    count++;
                }
            }
            if(primary.isEmpty())
            {
                if(aMap.size()>1)
                {
                    primary = "Various Artists";
                }
                else
                {
                    primary = aMap.begin().key();
                }
            }
        }
    }
    return primary;
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilities::isVariousArtists() const
{
    return (artist()=="Various Artists") ? true : false;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::artist() const
{
    if(m_artist.isEmpty())
    {
        QString cmdQ;
        QueryResult results;

        cmdQ = getQueryForAlbumArtist();
        if(runAlbumArtistQuery(cmdQ,results))
        {
            m_artist = primaryArtist(results);
        }
    }
    return m_artist;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::composer() const
{
    if(m_composer.isEmpty())
    {
        QString cmdQ;
        QueryResult results;
    
        cmdQ = getQueryPartForAlbumArtist(2,m_albumID);
        if(runAlbumArtistQuery(cmdQ,results))
        {
            m_composer = primaryArtist(results);
        }
    }
    return m_composer;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::getQueryForAlbumImage() const
{
    QString query;

    query = "SELECT ";
    query += (m_albumID.isGroup()) ? "1" : "0";
    query += " AS groupFlag, ";
    query += (m_albumID.isGroup()) ? "d.groupID" : "a.albumID";
    query += " AS albumID, b.imageID, COUNT(b.imageID) AS usage, b.type AS type ";
    query += "FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID) ";
    query += "INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID) ";
    if(m_albumID.isGroup())
    {
        query += "INNER JOIN album AS d ON (a.albumID=d.albumID) ";
        query += "WHERE d.groupID=" + QString::number(m_albumID.id()) + " ";
    }
    else
    {
        query += "WHERE c.albumID=" + QString::number(m_albumID.id()) + " ";
    }
    query += getQueryConditionForFilter() + " ";
    if(m_albumID.isGroup())
    {
        query += "GROUP BY d.groupID, b.imageID ORDER BY d.groupID, b.imageID";
    }
    else
    {
        query += "GROUP BY a.albumID, b.imageID ORDER BY a.albumID, b.imageID";
    }
    return query;
}

//-------------------------------------------------------------------------------------------

QueryRecord AlbumModelUtilities::createRecordAlbumImage(const AlbumModelKey& key,int imageID,int imageCount,int imageType) const
{
    QueryRecord r;
    r.push_back(key.isGroup());
    r.push_back(key.id());
    r.push_back(imageID);
    r.push_back(imageCount);
    r.push_back(imageType);
    return r;
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModelUtilities::keyAlbumImageRecord(const QueryRecord& record) const
{
    return keyAlbumImageRecordStatic(record);
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModelUtilities::keyAlbumImageRecordStatic(const QueryRecord& record)
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

int AlbumModelUtilities::imageIDAlbumImageRecord(const QueryRecord& record) const
{
    return imageIDAlbumImageRecordStatic(record);
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilities::imageIDAlbumImageRecordStatic(const QueryRecord& record)
{
    int imageID = -1;
    if(record.size()>=3)
    {
        imageID = record.at(2).toInt();
    }
    return imageID;
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilities::imageCountAlbumImageRecord(const QueryRecord& record) const
{
    return imageCountAlbumImageRecordStatic(record);
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilities::imageCountAlbumImageRecordStatic(const QueryRecord& record)
{
    int imageCount = 0;
    if(record.size()>=4)
    {
        imageCount = record.at(3).toInt();
    }
    return imageCount;
}

//-------------------------------------------------------------------------------------------

info::IDTagImageType AlbumModelUtilities::imageTypeAlbumImageRecord(const QueryRecord& record) const
{
    return imageTypeAlbumImageRecordStatic(record);
}

//-------------------------------------------------------------------------------------------

info::IDTagImageType AlbumModelUtilities::imageTypeAlbumImageRecordStatic(const QueryRecord& record)
{
    info::IDTagImageType type = info::e_TagImage_Other;
    if(record.size()>=5)
    {
        int tValue = record.at(4).toInt();
        if(tValue>=0 && tValue<=0x14)
        {
            type = static_cast<info::IDTagImageType>(tValue);
        }
    }
    return type;
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilities::runAlbumImageQuery(const QString& cmdQ,QueryResult& results) const
{
    bool res = false;
    
    try
    {
        bool isGroup;
        int albumID,imageID,imageCount,imageType;
        db::SQLiteQuerySPtr query = getDBQuery();
        
        query->prepare(cmdQ);
        query->bind(isGroup);
        query->bind(albumID);
        query->bind(imageID);
        query->bind(imageCount);
        query->bind(imageType);
        
        while(query->next())
        {
            AlbumModelKey key(std::pair<bool,int>(isGroup,albumID));
            results.push_back( createRecordAlbumImage(key,imageID,imageCount,imageType) );
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

void AlbumModelUtilities::sortImageListByPreferredType(QueryResult& results) const
{
    std::sort(results.begin(),results.end(),AlbumModelUtilities::compareImageResults);
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilities::compareImageResults(const QueryRecord& a,const QueryRecord& b)
{
    bool res;
    info::IDTagImageType aType = imageTypeAlbumImageRecordStatic(a);
    info::IDTagImageType bType = imageTypeAlbumImageRecordStatic(b);
    
    if(aType!=bType)
    {
        res = preferedImageType(aType,bType);
    }
    else
    {
        int aCount = imageCountAlbumImageRecordStatic(a);
        int bCount = imageCountAlbumImageRecordStatic(b);
        if(aCount!=bCount)
        {
            res = (aCount > bCount) ? true : false;
        }
        else
        {
            int aImageID = imageIDAlbumImageRecordStatic(a);
            int bImageID = imageIDAlbumImageRecordStatic(b);
            res = (aImageID <= bImageID) ? true : false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// true -> a is prefered ; false -> b is prefered
//-------------------------------------------------------------------------------------------

bool AlbumModelUtilities::preferedImageType(const info::IDTagImageType& a,const info::IDTagImageType& b)
{
    static const int c_IDOrder[22] = { 
        0x03, 0x0A, 0x05, 0x06, 0x07, 0x0F,
        0x04, 0x07, 0x08, 0x09, 0x0B, 0x0C,
        0x0D, 0x0E, 0x12, 0x13, 0x14, 0x01,
        0x00, 0x02, 0x10, 0x11
    };
    
    int i,aIdx = -1,bIdx = -1;
    int aV = static_cast<int>(a);
    int bV = static_cast<int>(b);
    bool res = true;
    
    for(i=0;i<22;i++)
    {
        if(aV==c_IDOrder[i])
        {
            aIdx = i;
        }
        if(bV==c_IDOrder[i])
        {
            bIdx = i;
        }
    }
    
    if(aIdx>=0 && bIdx>=0)
    {
        res = (aIdx <= bIdx) ? true : false;
    }
    else if(aIdx<0)
    {
        res = (bIdx<0) ? true : false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QImage *AlbumModelUtilities::albumArt(int iWidth,int iHeight) const
{
    ImageRepositarySPtr pImageRep = ImageRepositary::instance();
    QString cmdQ;
    QueryResult qResults;
    QImage *img = 0;
    
    cmdQ = getQueryForAlbumImage();
    if(runAlbumImageQuery(cmdQ,qResults))
    {
        if(qResults.size() > 0)
        {
            int imageID;
            
            sortImageListByPreferredType(qResults);
            imageID = imageIDAlbumImageRecord(qResults.at(0));
            img = pImageRep->getImage(imageID,iWidth,iHeight);
        }
    }
    if(img==0)
    {
        img = pImageRep->getReference(iWidth,iHeight);
    }
    return img;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilities::getQueryForYear() const
{
    QString query;
    query += "SELECT ";
    query += (m_albumID.isGroup()) ? "avg(year) AS year" : "year";
    query += " FROM album WHERE ";
    query += (m_albumID.isGroup()) ? "groupID" : "albumID";
    query += "=" + QString::number(m_albumID.id());
    return query;
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilities::year() const
{
    int yearV = 0;
    
    try
    {
        QString cmdQ = getQueryForYear();
        db::SQLiteQuerySPtr query = getDBQuery();
        
        query->prepare(cmdQ);
        query->bind(yearV);
        if(!query->next())
        {
            yearV = 0;
        }
    }
    catch(const db::SQLiteException&)
    {
        yearV = 0;
    }
    return yearV;
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
