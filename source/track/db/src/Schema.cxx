#include "track/db/inc/Schema.h"
#include "track/db/inc/SQLiteInsert.h"
#include "track/db/inc/TrackDB.h"
#include "common/inc/DiskOps.h"

#include <QFile>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

Schema::Schema() : m_db(0)
{}

//-------------------------------------------------------------------------------------------

Schema::~Schema()
{}

//-------------------------------------------------------------------------------------------

void Schema::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Schema::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool Schema::isTableDefinedOps(SQLiteDatabase *db, const QString& tableName)
{
    QString cmd;
    SQLiteQuery qTable(db);

    cmd = "SELECT name FROM sqlite_master WHERE type=\"table\" AND name=\"" + tableName + "\"";
    qTable.prepare(cmd);
    return (qTable.next()) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool Schema::isTableDefined(const QString& tableName)
{
    return isTableDefinedOps(m_db, tableName);
}

//-------------------------------------------------------------------------------------------

bool Schema::createDB(SQLiteDatabase *db)
{
    bool res = true;

    if(db!=0)
    {
        try
        {
            m_db = db;
            createDatabaseInfo();
            if(writeDatabaseVersion(TRACKDB_VERSION))
            {
                createAlbum();
                createTrack();
                createSubtrack();
                createGenre();
                createImage();
                createImageMap();
                createImageAlbumMap();
                createDirectory();
                createFile();
                createPlayList();
                createPlayListInfo();
                createSandBoxURL();
                createMountPoints();
                createFileHash();
                createDirectoryMount();
                createAudioDevice();
                createAudioFrequency();
                createAudioChannel();
            }
        }
        catch(const SQLiteException& e)
        {
            printError("Schema",e.error().toLatin1().constData());
            res = false;
        }
    }
    else
    {
        printError("Schema","No database given to define");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void Schema::createMountPointsOps(SQLiteDatabase *db)
{
    if(!isTableDefinedOps(db, "mountpoints"))
    {
        QString cmd;

        cmd  = "CREATE TABLE mountpoints (";
        cmd += "  mountID INTEGER PRIMARY KEY AUTOINCREMENT,";
        cmd += "  mountName TEXT NOT NULL";
        cmd += ");";
        db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createMountPoints()
{
    createMountPointsOps(m_db);
}

//-------------------------------------------------------------------------------------------

void Schema::copyMountPoints(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int mountID;
    QString mountName;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "mountID, mountName";
    cmd = "SELECT " + items + " FROM mountpoints";
    srcQ.prepare(cmd);
    srcQ.bind(mountID);
    srcQ.bind(mountName);

    cmd = "INSERT INTO mountpoints (" + items + ") VALUES (?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(mountID);
        destI.bind(mountName);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createDirectoryMount()
{
    if(!isTableDefined("dirmount"))
    {
        QString cmd;

        cmd  = "CREATE TABLE dirmount (";
        cmd += "  mountID INTEGER NOT NULL,";
        cmd += "  dirID INTEGER PRIMARY KEY";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyDirectoryMount(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int mountID, dirID;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "mountID, dirID";

    cmd = "SELECT "    + items + " FROM dirmount";
    srcQ.prepare(cmd);
    srcQ.bind(mountID);
    srcQ.bind(dirID);

    cmd = "INSERT INTO dirmount (" + items + ") VALUES (?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(mountID);
        destI.bind(dirID);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createAlbum()
{
    if(!isTableDefined("album"))
    {
        QString cmd;

        cmd  = "CREATE TABLE album (";
        cmd += "  albumID INTEGER PRIMARY KEY AUTOINCREMENT,";
        cmd += "  albumName TEXT NOT NULL COLLATE NOCASE,";
        cmd += "  year INTEGER NOT NULL,";
        cmd += "  directoryID INTEGER NOT NULL,";
        cmd += "  groupID INTEGER NOT NULL";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX albumNameIndex ON album (albumName COLLATE NOCASE);";
        m_db->exec(cmd);
        cmd = "CREATE INDEX albumLookupIndex ON album (albumName COLLATE NOCASE, year, directoryID);";
        m_db->exec(cmd);
    }
}


//-------------------------------------------------------------------------------------------

void Schema::copyAlbum(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int albumID, year, directoryID, groupID;
    QString albumName;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "albumID, albumName, year, directoryID, groupID";
    cmd = "SELECT " + items + " FROM album";
    srcQ.prepare(cmd);
    srcQ.bind(albumID);
    srcQ.bind(albumName);
    srcQ.bind(year);
    srcQ.bind(directoryID);
    srcQ.bind(groupID);

    cmd = "INSERT INTO album (" + items + ") VALUES (?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(albumID);
        destI.bind(albumName);
        destI.bind(year);
        destI.bind(directoryID);
        destI.bind(groupID);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createTrack()
{
    if(!isTableDefined("track"))
    {
        QString cmd;

        cmd  = "CREATE TABLE track (";
        cmd += "  albumID INTEGER NOT NULL,";
        cmd += "  trackID INTEGER NOT NULL,";
        cmd += "  fileID INTEGER NOT NULL,";
        cmd += "  trackName TEXT NOT NULL COLLATE NOCASE,";
        cmd += "  artist TEXT NOT NULL COLLATE NOCASE,";
        cmd += "  discNo INTEGER NOT NULL,";
        cmd += "  trackNo INTEGER NOT NULL,";
        cmd += "  originalArtist TEXT NOT NULL COLLATE NOCASE,";
        cmd += "  composer TEXT NOT NULL COLLATE NOCASE,";
        cmd += "  genreID INTEGER NOT NULL,";
        cmd += "  comment TEXT NOT NULL,";
        cmd += "  copyright TEXT NOT NULL,";
        cmd += "  encoder TEXT NOT NULL,";
        cmd += "  timeLength INTEGER NOT NULL,";
        cmd += "  PRIMARY KEY(albumID,trackID)";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX trackNameIndex ON track (trackName COLLATE NOCASE);";
        m_db->exec(cmd);
        cmd = "CREATE INDEX artistNameIndex ON track (artist COLLATE NOCASE);";
        m_db->exec(cmd);
        cmd = "CREATE INDEX originalArtistNameIndex ON track (originalArtist COLLATE NOCASE);";
        m_db->exec(cmd);
        cmd = "CREATE INDEX composerNameIndex ON track (composer COLLATE NOCASE);";
        m_db->exec(cmd);
        cmd = "CREATE INDEX fileTrackIndex ON track (albumID,fileID);";
        m_db->exec(cmd);
        cmd = "CREATE INDEX genreTrackIndex ON track (genreID);";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyTrack(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int albumID, trackID, fileID, discNo, trackNo, genreID;
    QString trackName, artist, originalArtist, composer, comment, copyright, encoder;
    tuint64 timeLength;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "albumID, trackID, fileID, trackName, artist, discNo, trackNo, originalArtist, composer, genreID, comment, copyright, encoder, timeLength";

    cmd  = "SELECT " + items + " FROM track";
    srcQ.prepare(cmd);
    srcQ.bind(albumID);
    srcQ.bind(trackID);
    srcQ.bind(fileID);
    srcQ.bind(trackName);
    srcQ.bind(artist);
    srcQ.bind(discNo);
    srcQ.bind(trackNo);
    srcQ.bind(originalArtist);
    srcQ.bind(composer);
    srcQ.bind(genreID);
    srcQ.bind(comment);
    srcQ.bind(copyright);
    srcQ.bind(encoder);
    srcQ.bind(timeLength);

    cmd = "INSERT INTO track (" + items + ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(albumID);
        destI.bind(trackID);
        destI.bind(fileID);
        destI.bind(trackName);
        destI.bind(artist);
        destI.bind(discNo);
        destI.bind(trackNo);
        destI.bind(originalArtist);
        destI.bind(composer);
        destI.bind(genreID);
        destI.bind(comment);
        destI.bind(copyright);
        destI.bind(encoder);
        destI.bind(timeLength);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createSubtrack()
{
    if(!isTableDefined("subtrack"))
    {
        QString cmd;

        cmd  = "CREATE TABLE subtrack (";
        cmd += "  albumID INTEGER NOT NULL,";
        cmd += "  trackID INTEGER NOT NULL,";
        cmd += "  subtrackID INTEGER NOT NULL,";
        cmd += "  subtrackName TEXT NOT NULL COLLATE NOCASE,";
        cmd += "  timeStart INTEGER NOT NULL,";
        cmd += "  timeLength INTEGER NOT NULL,";
        cmd += "  PRIMARY KEY(albumID,trackID,subtrackID)";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX subtrackNameIndex ON subtrack (subtrackName COLLATE NOCASE);";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copySubtrack(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int albumID, trackID, subtrackID;
    QString subtrackName;
    tuint64 timeStart, timeLength;

    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "albumID, trackID, subtrackID, subtrackName, timeStart, timeLength";

    cmd = "SELECT "    + items + " FROM subtrack";
    srcQ.prepare(cmd);
    srcQ.bind(albumID);
    srcQ.bind(trackID);
    srcQ.bind(subtrackID);
    srcQ.bind(subtrackName);
    srcQ.bind(timeStart);
    srcQ.bind(timeLength);

    cmd = "INSERT INTO subtrack (" + items + ") VALUES (?,?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(albumID);
        destI.bind(trackID);
        destI.bind(subtrackID);
        destI.bind(subtrackName);
        destI.bind(timeStart);
        destI.bind(timeLength);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createGenre()
{
    if(!isTableDefined("genre"))
    {
        QString cmd;

        cmd  = "CREATE TABLE genre (";
        cmd += "  genreID INTEGER NOT NULL,";
        cmd += "  genreName TEXT NOT NULL COLLATE NOCASE,";
        cmd += "  PRIMARY KEY(genreID)";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX genreNameIndex ON genre (genreName COLLATE NOCASE);";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyGenre(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int genreID;
    QString genreName;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "genreID, genreName";
    cmd = "SELECT " + items + " FROM genre";
    srcQ.prepare(cmd);
    srcQ.bind(genreID);
    srcQ.bind(genreName);

    cmd = "INSERT INTO genre (" + items + ") VALUES (?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(genreID);
        destI.bind(genreName);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createImage()
{
    if(!isTableDefined("image"))
    {
        QString cmd;

        cmd  = "CREATE TABLE image (";
        cmd += "  imageID INTEGER PRIMARY KEY AUTOINCREMENT,";
        cmd += "  type INTEGER NOT NULL,";
        cmd += "  format INTEGER NOT NULL,";
        cmd += "  sha1SignatureH INTEGER NOT NULL,";
        cmd += "  sha1SignatureL INTEGER NOT NULL,";
        cmd += "  data BLOB";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX imageSignatureIndex ON image (sha1SignatureH,sha1SignatureL)";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyImage(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int imageID, type, format;
    tint64 sha1SignatureH, sha1SignatureL;
    QByteArray data;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "imageID, type, format, sha1SignatureH, sha1SignatureL, data";
    cmd = "SELECT " + items + " FROM image";
    srcQ.prepare(cmd);
    srcQ.bind(imageID);
    srcQ.bind(type);
    srcQ.bind(format);
    srcQ.bind(sha1SignatureH);
    srcQ.bind(sha1SignatureL);
    srcQ.bind(data);

    cmd = "INSERT INTO image (" + items + ") VALUES (?,?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(imageID);
        destI.bind(type);
        destI.bind(format);
        destI.bind(sha1SignatureH);
        destI.bind(sha1SignatureL);
        destI.bind(data);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createImageAlbumMap()
{
    if(!isTableDefined("imagealbummap"))
    {
        QString cmd;

        cmd  = "CREATE TABLE imagealbummap (";
        cmd += "  albumID INTEGER NOT NULL,";
        cmd += "  imageID INTEGER NOT NULL,";
        cmd += "  fileName TEXT NOT NULL,";
        cmd += "  dirModifiedTime INTEGER NOT NULL,";
        cmd += "  fileModifiedTime INTEGER NOT NULL,";
        cmd += "  PRIMARY KEY(albumID)";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyImageAlbumMap(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int albumID, imageID;
    QString fileName;
    tuint64 dirModifiedTime, fileModifiedTime;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "albumID, imageID, fileName, dirModifiedTime, fileModifiedTime";
    cmd = "SELECT " + items + " FROM imagealbummap";
    srcQ.prepare(cmd);
    srcQ.bind(albumID);
    srcQ.bind(imageID);
    srcQ.bind(fileName);
    srcQ.bind(dirModifiedTime);
    srcQ.bind(fileModifiedTime);

    cmd = "INSERT INTO imagealbummap (" + items + ") VALUES (?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(albumID);
        destI.bind(imageID);
        destI.bind(fileName);
        destI.bind(dirModifiedTime);
        destI.bind(fileModifiedTime);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createImageMap()
{
    if(!isTableDefined("imagemap"))
    {
        QString cmd;

        cmd  = "CREATE TABLE imagemap (";
        cmd += "  albumID INTEGER NOT NULL,";
        cmd += "  trackID INTEGER NOT NULL,";
        cmd += "  imageID INTEGER NOT NULL,";
        cmd += "  PRIMARY KEY(albumID,trackID,imageID)";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX imageIDIndex ON imagemap (imageID)";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyImageMap(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int albumID, trackID, imageID;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "albumID, trackID, imageID";
    cmd = "SELECT " + items + " FROM imagemap";
    srcQ.prepare(cmd);
    srcQ.bind(albumID);
    srcQ.bind(trackID);
    srcQ.bind(imageID);

    cmd = "INSERT INTO imagemap (" + items + ") VALUES (?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(albumID);
        destI.bind(trackID);
        destI.bind(imageID);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createDirectory()
{
    if(!isTableDefined("directory"))
    {
        QString cmd;

        cmd  = "CREATE TABLE directory (";
        cmd += "  directoryID INTEGER PRIMARY KEY AUTOINCREMENT,";
        cmd += "  directoryName TEXT NOT NULL UNIQUE";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX directoryNameIndex ON directory (directoryName)";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyDirectory(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int directoryID;
    QString directoryName;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "directoryID, directoryName";
    cmd = "SELECT " + items + " FROM directory";
    srcQ.prepare(cmd);
    srcQ.bind(directoryID);
    srcQ.bind(directoryName);

    cmd = "INSERT INTO directory (" + items + ") VALUES (?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(directoryID);
        destI.bind(directoryName);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createFile()
{
    if(!isTableDefined("file"))
    {
        QString cmd;

        cmd  = "CREATE TABLE file (";
        cmd += "  directoryID INTEGER NOT NULL,";
        cmd += "  fileID INTEGER NOT NULL,";
        cmd += "  codecType INTEGER NOT NULL,";
        cmd += "  infoType INTEGER NOT NULL,";
        cmd += "  fileName TEXT NOT NULL,";
        cmd += "  updateTime INTEGER NOT NULL,";
        cmd += "  fileSize INTEGER NOT NULL,";
        cmd += "  PRIMARY KEY(directoryID,fileID)";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX fileNameIndex ON file (fileName)";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyFile(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int directoryID, fileID, codecType, infoType, fileSize;
    QString fileName;
    tuint64 updateTime;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "directoryID, fileID, codecType, infoType, fileName, updateTime, fileSize";
    cmd = "SELECT " + items + " FROM file";
    srcQ.prepare(cmd);
    srcQ.bind(directoryID);
    srcQ.bind(fileID);
    srcQ.bind(codecType);
    srcQ.bind(infoType);
    srcQ.bind(fileName);
    srcQ.bind(updateTime);
    srcQ.bind(fileSize);

    cmd = "INSERT INTO file (" + items + ") VALUES (?,?,?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(directoryID);
        destI.bind(fileID);
        destI.bind(codecType);
        destI.bind(infoType);
        destI.bind(fileName);
        destI.bind(updateTime);
        destI.bind(fileSize);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createPlayList()
{
    if(!isTableDefined("playlist"))
    {
        QString cmd;

        cmd  = "CREATE TABLE playlist (";
        cmd += "  playListID INTEGER NOT NULL,";
        cmd += "  position INTEGER NOT NULL,";
        cmd += "  albumID INTEGER NOT NULL,";
        cmd += "  trackID INTEGER NOT NULL,";
        cmd += "  subtrackID INTEGER NOT NULL,";
        cmd += "  itemID INTEGER NOT NULL UNIQUE,";
        cmd += "  PRIMARY KEY(playListID,position)";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX playlistItemIndex ON playlist (itemID)";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyPlayList(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int srcDBVersion, destDBVersion;
    int playlistID, position, albumID, trackID, subtrackID;
    tuint64 itemID;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    srcDBVersion = TrackDB::dbVersion(srcDB);
    destDBVersion = TrackDB::dbVersion(destDB);

    items = "playListID, position, albumID, trackID, subtrackID";
    if(srcDBVersion > 6)
    {
        items += ", itemID";
    }
    cmd = "SELECT " + items + " FROM playlist";
    srcQ.prepare(cmd);
    srcQ.bind(playlistID);
    srcQ.bind(position);
    srcQ.bind(albumID);
    srcQ.bind(trackID);
    srcQ.bind(subtrackID);
    if(srcDBVersion > 6)
    {
        srcQ.bind(itemID);
    }

    if(destDBVersion > 6)
    {
        items += ", itemID";
    }
    cmd = "INSERT INTO playlist (" + items;
    cmd += (destDBVersion > 6) ? ") VALUES (?,?,?,?,?,?)" : ") VALUES (?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(playlistID);
        destI.bind(position);
        destI.bind(albumID);
        destI.bind(trackID);
        destI.bind(subtrackID);
        if(destDBVersion > 6 && srcDBVersion <= 6)
        {
            itemID = TrackDB::newPlaylistItemID(destDB);
        }
        if(destDBVersion > 6)
        {
            destI.bind(itemID);
        }
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createPlayListInfo()
{
    if(!isTableDefined("playlistInfo"))
    {
        QString cmd;

        cmd  = "CREATE TABLE playlistInfo (";
        cmd += "  playListID INTEGER NOT NULL,";
        cmd += "  name TEXT NOT NULL,";
        cmd += "  PRIMARY KEY(playListID)";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyPlayListInfo(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int playListID;
    QString name;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "playListID, name";
    cmd = "SELECT " + items + " FROM playlistInfo";
    srcQ.prepare(cmd);
    srcQ.bind(playListID);
    srcQ.bind(name);

    cmd = "INSERT INTO playlistInfo (" + items + ") VALUES (?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(playListID);
        destI.bind(name);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createDatabaseInfo()
{
    if(!isTableDefined("databaseInfo"))
    {
        QString cmd;

        cmd  = "CREATE TABLE databaseInfo (";
        cmd += "  dbVersion INTEGER NOT NULL";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createSandBoxURL()
{
    if(!isTableDefined("sandBoxURL"))
    {
        QString cmd;

        cmd  = "CREATE TABLE sandBoxURL (";
        cmd += "  url TEXT NOT NULL,";
        cmd += "  docUrl TEXT NOT NULL,";
        cmd += "  access INTEGER NOT NULL,";
        cmd += "  accessTime INTEGER NOT NULL,";
        cmd += "  bookmark BLOB,";
        cmd += "  PRIMARY KEY(url,docUrl)";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copySandBoxURL(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int access;
    QString url, docUrl;
    tuint64 accessTime;
    QByteArray bookmark;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "url, docUrl, access, accessTime, bookmark";
    cmd = "SELECT " + items + " FROM sandBoxURL";
    srcQ.prepare(cmd);
    srcQ.bind(url);
    srcQ.bind(docUrl);
    srcQ.bind(access);
    srcQ.bind(accessTime);
    srcQ.bind(bookmark);

    cmd = "INSERT INTO sandBoxURL (" + items + ") VALUES (?,?,?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(url);
        destI.bind(docUrl);
        destI.bind(access);
        destI.bind(accessTime);
        destI.bind(bookmark);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createFileHash()
{
    if(!isTableDefined("fileHash"))
    {
        QString cmd;

        cmd  = "CREATE TABLE fileHash (";
        cmd += "    directoryID INTEGER NOT NULL,";
        cmd += "    fileID INTEGER NOT NULL,";
        cmd += "    hashID INTEGER NOT NULL,";
        cmd += "    PRIMARY KEY(directoryID, fileID)";
        cmd += ");";
        m_db->exec(cmd);
        cmd = "CREATE INDEX fileHashIDIndex ON fileHash (hashID);";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyFileHash(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int directoryID, fileID;
    tuint64 hashID;
    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "directoryID, fileID, hashID";
    cmd = "SELECT " + items + " FROM fileHash";
    srcQ.prepare(cmd);
    srcQ.bind(directoryID);
    srcQ.bind(fileID);
    srcQ.bind(hashID);

    cmd = "INSERT INTO fileHash (" + items + ") VALUES (?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(directoryID);
        destI.bind(fileID);
        destI.bind(hashID);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------
/*
CREATE TABLE audiodevice (
    // Used as unique reference key for records in audiofrequency and audiochannel tables
    referenceID INTEGER PRIMARY KEY AUTOINCREMENT,
    // The device ID name as given by AOQueryDevice
    deviceID TEXT NOT NULL UNIQUE,
    // Human readable device name as given by AOQueryDevice
    deviceName TEXT NOT NULL
);

CREATE TABLE audiofrequency (
    // Points to device record in audiodevice table
    referenceID INTEGER NOT NULL,
    // The frequency supported by the referenced device
    frequency INTEGER NOT NULL,
);

CREATE TABLE audiochannel {
    // Points to device record in audiodevice table
    referenceID INTEGER NOT NULL,
    // Index of channel
    index INTEGER NOT NULL,
    // Name of channel
    name TEXT NOT NULL,
    PRIMARY KEY(referenceID, index)
};
*/
//-------------------------------------------------------------------------------------------

void Schema::createAudioDevice()
{
    if(!isTableDefined("audiodevice"))
    {
        QString cmd;

        cmd  = "CREATE TABLE audiodevice (";
        cmd += "  referenceID INTEGER PRIMARY KEY AUTOINCREMENT,";
        cmd += "  deviceID TEXT NOT NULL UNIQUE,";
        cmd += "  deviceName TEXT NOT NULL";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyAudioDevice(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int referenceID;
    QString deviceID, deviceName;

    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "referenceID, deviceID, deviceName";

    cmd = "SELECT "    + items + " FROM audiodevice";
    srcQ.prepare(cmd);
    srcQ.bind(referenceID);
    srcQ.bind(deviceID);
    srcQ.bind(deviceName);

    cmd = "INSERT INTO audiodevice (" + items + ") VALUES (?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(referenceID);
        destI.bind(deviceID);
        destI.bind(deviceName);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createAudioFrequency()
{
    if(!isTableDefined("audiofrequency"))
    {
        QString cmd;

        cmd  = "CREATE TABLE audiofrequency (";
        cmd += "  referenceID INTEGER NOT NULL,";
        cmd += "  frequency INTEGER NOT NULL,";
        cmd += "  PRIMARY KEY(referenceID, frequency)";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyAudioFrequency(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int referenceID, frequency;

    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "referenceID, frequency";

    cmd = "SELECT "    + items + " FROM audiofrequency";
    srcQ.prepare(cmd);
    srcQ.bind(referenceID);
    srcQ.bind(frequency);

    cmd = "INSERT INTO audiofrequency (" + items + ") VALUES (?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(referenceID);
        destI.bind(frequency);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

void Schema::createAudioChannel()
{
    if(!isTableDefined("audiochannel"))
    {
        QString cmd;

        cmd  = "CREATE TABLE audiochannel (";
        cmd += "  referenceID INTEGER NOT NULL,";
        cmd += "  channelIndex INTEGER NOT NULL,";
        cmd += "  name TEXT NOT NULL,";
        cmd += "  PRIMARY KEY(referenceID, channelIndex)";
        cmd += ");";
        m_db->exec(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void Schema::copyAudioChannel(SQLiteDatabase *srcDB, SQLiteDatabase *destDB)
{
    int referenceID, index;
    QString name;

    QString cmd, items;
    SQLiteQuery srcQ(srcDB);
    SQLiteInsert destI(destDB);

    items = "referenceID, channelIndex, name";

    cmd = "SELECT "    + items + " FROM audiochannel";
    srcQ.prepare(cmd);
    srcQ.bind(referenceID);
    srcQ.bind(index);
    srcQ.bind(name);

    cmd = "INSERT INTO audiochannel (" + items + ") VALUES (?,?,?)";
    destI.prepare(cmd);

    while(srcQ.next())
    {
        destI.bind(referenceID);
        destI.bind(index);
        destI.bind(name);
        destI.next();
    }
}

//-------------------------------------------------------------------------------------------

bool Schema::writeDatabaseVersion(int versionNo)
{
    QString cmd;
    bool res = false;

    try
    {
        int cVersionNo;
        SQLiteInsert projectI(m_db);
        SQLiteQuery projectQ(m_db);

        m_db->exec("SAVEPOINT writeDatabaseVersion");

        cmd = "SELECT dbVersion FROM databaseInfo";
        projectQ.prepare(cmd);
        projectQ.bind(cVersionNo);
        if(projectQ.next())
        {
            if(cVersionNo==versionNo)
            {
                res = true;
            }
            else
            {
                QString errMsg;
                errMsg = "Current project database is version " + QString::number(cVersionNo) + " while version " + QString::number(versionNo) + " was expected.";
                printError("writeProjectVersion",errMsg.toLatin1().constData());
            }
        }
        else
        {
            cmd = "INSERT INTO databaseInfo VALUES (?)";
            projectI.prepare(cmd);
            projectI.bind(versionNo);
            projectI.next();
            res = true;
        }

        if(res)
        {
            m_db->exec("RELEASE writeDatabaseVersion");
        }
        else
        {
            m_db->exec("ROLLBACK TO SAVEPOINT writeDatabaseVersion");
        }
    }
    catch(const SQLiteException& e)
    {
        m_db->exec("ROLLBACK TO SAVEPOINT writeDatabaseVersion");
        printError("writeDatabaseVersion",e.error().toLatin1().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString Schema::tempUpgradeDBFileName(const QString& orgTrackDBFileName)
{
    int i;
    QString name;

    for(i=orgTrackDBFileName.length()-1;i>=0 && name.isEmpty();i--)
    {
        if(orgTrackDBFileName.at(i)==QChar('.'))
        {
            name = orgTrackDBFileName.mid(0,i) + "_upgrade.db";
        }
    }
    return name;
}

//-------------------------------------------------------------------------------------------

bool Schema::doUpgrade(const QString& orgTrackDBFileName)
{
    bool res = false;

    QString newTrackDBFileName = tempUpgradeDBFileName(orgTrackDBFileName);
    if(!newTrackDBFileName.isEmpty())
    {
        if(common::DiskOps::exist(newTrackDBFileName))
        {
            common::DiskOps::remove(newTrackDBFileName);
        }

        try
        {
            SQLiteDatabase srcDB, destDB;

            if(srcDB.open(orgTrackDBFileName))
            {
                if(destDB.open(newTrackDBFileName))
                {
                    if(createDB(&destDB))
                    {
                        copyAlbum(&srcDB, &destDB);
                        copyTrack(&srcDB, &destDB);
                        copySubtrack(&srcDB, &destDB);
                        copyGenre(&srcDB, &destDB);
                        copyImage(&srcDB, &destDB);
                        copyImageMap(&srcDB, &destDB);
                        copyImageAlbumMap(&srcDB, &destDB);
                        copyDirectory(&srcDB, &destDB);
                        copyFile(&srcDB, &destDB);
                        copyPlayList(&srcDB, &destDB);
                        copyPlayListInfo(&srcDB, &destDB);
                        copySandBoxURL(&srcDB, &destDB);
                        copyFileHash(&srcDB, &destDB);
                        copyMountPoints(&srcDB, &destDB);
                        copyDirectoryMount(&srcDB, &destDB);
                        copyAudioDevice(&srcDB, &destDB);
                        copyAudioFrequency(&srcDB, &destDB);
                        copyAudioChannel(&srcDB, &destDB);
                        res = true;
                    }
                    else
                    {
                        QString err = QString("Failed to create database schema for '%1'").arg(newTrackDBFileName);
                        printError("doUpgrade", err.toUtf8().constData());
                    }
                }
                else
                {
                    QString err = QString("Failed to create upgrade database '%1'").arg(newTrackDBFileName);
                    printError("doUpgrade", err.toUtf8().constData());
                }
            }
            else
            {
                QString err = QString("Failed to open old database '%1'").arg(orgTrackDBFileName);
                printError("doUpgrade", err.toUtf8().constData());
            }
        }
        catch(const SQLiteException& e)
        {
            QString err = QString("Failed to upgrade database '%1' to '%2'. Database exception - %3")
                .arg(orgTrackDBFileName).arg(newTrackDBFileName).arg(e.error());
            printError("doUpgrade", err.toUtf8().constData());
            res = false;
        }

        if(res)
        {
            common::DiskOps::remove(orgTrackDBFileName);
            QFile nDBFile(newTrackDBFileName);
            if(!nDBFile.rename(orgTrackDBFileName))
            {
                res = false;
            }
        }
        else
        {
            common::DiskOps::remove(newTrackDBFileName);
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
