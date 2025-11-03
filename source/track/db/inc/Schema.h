//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SCHEMA_H
#define __OMEGA_TRACK_DB_SCHEMA_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/SQLiteDatabase.h"
#include "track/db/inc/SQLiteQuery.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

#define TRACKDB_VERSION 7

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT Schema
{
    public:
        typedef bool (*SchemaUpgradeFunc)(SQLiteDatabase *orgDB, SQLiteDatabase *newDB);

    public:
        Schema();
        virtual ~Schema();
        
        virtual bool createDB(SQLiteDatabase *db);
        virtual bool doUpgrade(const QString& orgTrackDBFileName);
        
    protected:
    
        SQLiteDatabase *m_db;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        static bool isTableDefinedOps(SQLiteDatabase *db, const QString& tableName);
        virtual bool isTableDefined(const QString& tableName);
        
        virtual void createDatabaseInfo();
        
        virtual void createAlbum();
        virtual void copyAlbum(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createTrack();
        virtual void copyTrack(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createSubtrack();
        virtual void copySubtrack(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createGenre();
        virtual void copyGenre(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createImage();
        virtual void copyImage(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createImageMap();
        virtual void copyImageMap(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createImageAlbumMap();
        virtual void copyImageAlbumMap(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createDirectory();
        virtual void copyDirectory(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createFile();
        virtual void copyFile(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createPlayList();
        virtual void copyPlayList(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createPlayListInfo();
        virtual void copyPlayListInfo(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createSandBoxURL();
        virtual void copySandBoxURL(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createFileHash();
        virtual void copyFileHash(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createMountPoints();
        static void createMountPointsOps(SQLiteDatabase *db);
        virtual void copyMountPoints(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);
        
        virtual void createDirectoryMount();
        virtual void copyDirectoryMount(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);

        virtual void createAudioDevice();
        virtual void copyAudioDevice(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);

        virtual void createAudioFrequency();
        virtual void copyAudioFrequency(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);

        virtual void createAudioChannel();
        virtual void copyAudioChannel(SQLiteDatabase *srcDB, SQLiteDatabase *destDB);


        virtual bool writeDatabaseVersion(int versionNo);
        
        virtual QString tempUpgradeDBFileName(const QString& orgTrackDBFileName);
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

