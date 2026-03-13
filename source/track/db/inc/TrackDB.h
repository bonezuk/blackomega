//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_TRACKDB_H
#define __OMEGA_TRACK_DB_TRACKDB_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"
#include "track/db/inc/Schema.h"
#include "track/db/inc/SQLiteInsert.h"
#include "track/db/inc/TrackDBMountPoints.h"

#include "audioio/inc/AOQueryDevice.h"

#include <QSharedPointer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class DBInfo;

typedef QSharedPointer<DBInfo> DBInfoSPtr;

//-------------------------------------------------------------------------------------------

typedef struct s_PlaylistTuple
{
	tint albumID;
	tint trackID;
	tint subtrackID;
	tuint64 itemID;
} PlaylistTuple;

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT TrackDB
{
	public:
		friend class DBInfo;
	
	public:
		~TrackDB();
		
		static TrackDB *instance();
		static TrackDB *instance(const QString& dbName);
		
		static common::TimeStamp getModifiedTime(const QString& fileName);
		static common::TimeStamp getModifiedTime(const QString& fileName,tint& fileSize);

		bool open(const QString& dbName);
		void close();
			
		bool add(info::Info *data);
		bool addInfo(info::Info *data);
		
		bool erase(const QString& fileName);
		bool erase(tint albumID,tint trackID);
		
		bool isUpdateRequired(tint dirID,tint fileID);
		bool update(info::Info *data);
		
		static QString dbString(const QString& dStr);
		static QString dbStringInv(const QString& dStr);
		
		SQLiteDatabase *db();
		
		tint groupAlbums(const QVector<tint>& albumIDList);

		bool getTrackKey(const QString& fileName,QPair<tint,tint>& pairID);
		
		int idOfInserted();
		
		int dbVersion();
		static int dbVersion(SQLiteDatabase *db);
		
		QSharedPointer<TrackDBMountPoints>& mountPoints();
		
		QMap<tint, QString> playlists();
		bool isPlaylist(int playlistID);
		QString playlist(int playlistID);
		bool loadPlaylist(int playlistID, QVector<PlaylistTuple>& pList);
		bool loadPlaylist(int playlistID, QVector<QPair<info::InfoSPtr, tint> >& pList);
		int savePlaylist(const QString& name, QVector<QPair<info::InfoSPtr, tint> >& pList);
		int savePlaylist(const QString& name, QVector<PlaylistTuple>& pList);
		int replacePlaylist(int playlistID, const QString& name, QVector<QPair<info::InfoSPtr, tint> >& pList);
		int replacePlaylist(int playlistID, const QString& name, const QVector<PlaylistTuple>& pList);
		void clearPlaylist(int playlistID);
		
		tuint64 newPlaylistItemID();
		static tuint64 newPlaylistItemID(SQLiteDatabase *db);

		bool getKeysFromFilename(const QString& fullFileName,tint& albumID,tint& trackID);
		bool getKeysFromFilename(const QString& fullFileName,tint& albumID,tint& trackID,tint& dirID,tint& fileID);
		
		QString getDirectoryName(int dirID);
		QString formatDirectoryPath(const QString& path) const;
		
		bool saveAudioDevice(const audioio::AOQueryDevice::Device& dev);
		bool restoreAudioDevice(const QString& deviceID, audioio::AOQueryDevice::Device& dev);
		void clearAudioDevices();
		
	protected:
		
		static TrackDB *m_instance;
		
		SQLiteDatabase *m_db;
		QSharedPointer<TrackDBMountPoints> m_mountPoints;
		
		TrackDB();
		
		void printError(const tchar *strR,const tchar *strE) const;

		bool upgradeDBAsRequired(const QString& dbName);
		void rebuildMusicDatabase(const QString& dbName);
		
		tint addAlbum(info::Info *data,tint& dirID);
		tint addTrack(info::Info *data,tint dirID,tint albumID);
		tint addGenre(info::Info *data);
		tint addDirectory(info::Info *data);
		tint addDirectory(const QString& dName);
		tint addFile(info::Info *data,tint dirID);
		bool addImage(info::Info *data,tint albumID,tint trackID);
		int addImageFile(const QString& fileName);
		bool addSubtrack(info::Info *data,tint albumID,tint trackID);
		QPair<tuint64,tuint64> getSHA1Signature(common::Array<tubyte,tubyte> *pArr);
		
		bool eraseTrack(tint albumID,tint trackID,tint dirID,tint fileID);
		bool getFilenameComponents(const QString& name,QString& dirName,QString& fileName);
		
		bool updateAlbumImage(int albumID);
		tint levenshteinDistance(const QString& a,const QString& b) const;
		QString loadDirectoryImage(const QString& dirName,const QString& albumName) const;
		
		int savePlaylistOp(int playlistID, const QString& name, const QVector<QPair<DBInfoSPtr, tint> >& pList);
		int savePlaylistOp(int playlistID, const QString& name, const QVector<PlaylistTuple>& pList);
		void getDBInfoListFromPlaylist(QVector<QPair<info::InfoSPtr, tint> >& pList, QVector<QPair<DBInfoSPtr, tint> >& pDBList);
		void clearPlaylistOp(int playlistID);
		
		int getMountDirectoryID(const QString& dName, QString& mName);
		QString getMountDirectoryName(const QString& dName);
		bool addMountDirectory(int dirID, const QString& dName);
		
		int refIDOfAudioDevice(const QString& deviceID);
		void removeAudioDevice(const audioio::AOQueryDevice::Device& dev);
		bool insertAudioDevice(const audioio::AOQueryDevice::Device& dev);
};

//-------------------------------------------------------------------------------------------

inline SQLiteDatabase *TrackDB::db()
{
	return m_db;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

