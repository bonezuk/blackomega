#include "network/inc/NetMemory.h"
#include "common/inc/SHA1Digest.h"
#include "common/inc/DiskOps.h"
#include "common/inc/BOParse.h"
#include "track/db/inc/TrackDB.h"
#include "engine/inc/Codec.h"
#include "engine/inc/File.h"
#include "track/db/inc/DBInfo.h"
#include "common/inc/Random.h"

#include <QDir>

#if defined(OMEGA_POSIX)
#include <stdio.h>
#include <dirent.h>
#endif

#if defined(OMEGA_MAC_STORE)
#include "common/inc/SBService.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

TrackDB *TrackDB::m_instance = 0;

//-------------------------------------------------------------------------------------------

TrackDB::TrackDB() : m_db(0),
	m_mountPoints()
{}

//-------------------------------------------------------------------------------------------

TrackDB::~TrackDB()
{
	close();
	m_instance = 0;
}

//-------------------------------------------------------------------------------------------

void TrackDB::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "TrackDB::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

TrackDB *TrackDB::instance(const QString& dbName)
{
	if(m_instance==0)
	{
		m_instance = new TrackDB;
		if(!m_instance->open(dbName))
		{
			delete m_instance;
			m_instance = 0;
		}
		else
		{
			m_instance->clearAudioDevices();
		}
	}
	return m_instance;
}

//-------------------------------------------------------------------------------------------

TrackDB *TrackDB::instance()
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp TrackDB::getModifiedTime(const QString& fileName)
{
	tint fileSize;
	return getModifiedTime(fileName,fileSize);
}

//-------------------------------------------------------------------------------------------

common::TimeStamp TrackDB::getModifiedTime(const QString& fileName,tint& fileSize)
{
	return common::DiskOps::getModifiedTime(fileName,fileSize);
}

//-------------------------------------------------------------------------------------------

bool TrackDB::upgradeDBAsRequired(const QString& dbName)
{
	bool res = true;

	if(common::DiskOps::exist(dbName))
	{
		SQLiteDatabase currentDb;
		
		if(currentDb.open(dbName))
		{
			int currentVersion = dbVersion(&currentDb);
			
			if(currentVersion < 6)
			{
				// No upgrade path so existing functionality removes it
				// to be recreated
				QString err = QString("DB below version 6. No upgrade path for DB file '%1'").arg(dbName);
				printError("upgradeDBAsRequired", err.toUtf8().constData());
				res = false;
			}
			else if(currentVersion < TRACKDB_VERSION)
			{
				Schema schema;
				
				currentDb.close();
				if(!schema.doUpgrade(dbName))
				{
					QString err = QString("Failed to upgrade '%1'").arg(dbName);
					printError("upgradeDBAsRequired", err.toUtf8().constData());
					res = false;
				}
			
				if(!res)
				{
					QString err = QString("Failed to upgrade DB from version %1 to %2. Removing corrupt DB file '%3'")
						.arg(currentVersion - 1).arg(currentVersion).arg(dbName);
					printError("upgradeDBAsRequired", err.toUtf8().constData());
					res = false;
				}
			}
		}
		else
		{
			QString err = QString("Failed to open expected track database '%1'. Removing corrupt DB file").arg(dbName);
			printError("upgradeDBAsRequired", err.toUtf8().constData());
			res = false;
		}
	}
	if(!res)
	{
		omega::common::DiskOps::remove(dbName);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void TrackDB::rebuildMusicDatabase(const QString& dbName)
{
	// TODO - Rescan music directories listed in the old mount points table
	// Also implement backup of mount points in XML settings file (or whatever).
}

//-------------------------------------------------------------------------------------------

bool TrackDB::open(const QString& dbName)
{
	bool res = false;
	
	close();
	
	if(!upgradeDBAsRequired(dbName))
	{
		rebuildMusicDatabase(dbName);
	}
	
	m_db = new SQLiteDatabase;
	if(m_db->open(dbName))
	{
		Schema dbSchema;
		
		if(dbSchema.createDB(m_db))
		{
			QSharedPointer<TrackDBMountPoints> pMounts(new TrackDBMountPoints(m_db));
			m_mountPoints = pMounts;
			res = true;
		}
		else
		{
			printError("open","Error creating database schema");
		}
	}
	else
	{
		printError("open","Failed to open track database");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void TrackDB::close()
{
	if(m_db!=0)
	{
		if(!m_mountPoints.isNull())
		{
			m_mountPoints.clear();
		}
		m_db->close();
		delete m_db;
		m_db = 0;
	}
}

//-------------------------------------------------------------------------------------------

QSharedPointer<TrackDBMountPoints>& TrackDB::mountPoints()
{
	return m_mountPoints;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::add(info::Info *data)
{
	QPair<tint,tint> atID;

	if(data->getFilename().isEmpty())
	{
		printError("add","No file name associated with given track information");
		return false;
	}
	if(getTrackKey(data->getFilename(),atID))
	{
		return true;
	}
	return addInfo(data);
}

//-------------------------------------------------------------------------------------------

bool TrackDB::addInfo(info::Info *data)
{
	tint dirID = -1,albumID,trackID;
	bool res = false;
	
	try
	{
		m_db->exec("SAVEPOINT addTrack");

		albumID = addAlbum(data,dirID);
		if(albumID>=0)
		{
			trackID = addTrack(data,dirID,albumID);
			if(trackID>=0)
			{
				if(addSubtrack(data,albumID,trackID))
				{
					if(addImage(data,albumID,trackID))
					{
						res = true;
					}
					else
					{
						printError("add","Failed to add image data");
					}
				}
				else
				{
					printError("add","Failed to add subtrack data");
				}
			}
			else
			{
				printError("add","Failed to add track");
			}			
		}
		else
		{
			printError("add","Failed to add album for track");	
		}
		
		if(res)
		{
			m_db->exec("RELEASE addTrack");
		}
		else
		{
			m_db->exec("ROLLBACK TO SAVEPOINT addTrack");
		}
	}
	catch(const SQLiteException& e)
	{
		QString errStr;
		m_db->exec("ROLLBACK TO SAVEPOINT addTrack");
		errStr = "Database exception thrown. " + e.error();
		printError("add",errStr.toUtf8().constData());
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

QString TrackDB::getDirectoryName(int dirID)
{
	QString cmdQ, mountName, dirName;
	SQLiteQuery dirQ(m_db);
	
	cmdQ = QString("SELECT c.mountName, a.directoryName FROM directory AS a INNER JOIN dirmount AS b ON a.directoryID=b.dirID INNER JOIN mountpoints AS c ON b.mountID=c.mountID WHERE a.directoryID=%1")
		.arg(dirID);
	dirQ.prepare(cmdQ);
	dirQ.bind(mountName);
	dirQ.bind(dirName);
	if(!dirQ.next())
	{
		SQLiteQuery dirOnlyQ(m_db);
		cmdQ = QString("SELECT directoryName FROM directory WHERE directoryID=%1").arg(dirID);
		dirOnlyQ.prepare(cmdQ);
		dirOnlyQ.bind(dirName);
		if(!dirOnlyQ.next())
		{
			dirName = "";
		}
		else
		{
			dirName = dbStringInv(dirName);
		}
	}
	else
	{
		dirName = dbStringInv(mountName) + dbStringInv(dirName);
	}
	return dirName;
}

//-------------------------------------------------------------------------------------------

tint TrackDB::addAlbum(info::Info *data,tint& dirID)
{
	tint year,albumID = -1,groupID = -1;
	QString albumName;
	QString cmdQ,cmdI;
	SQLiteQuery albumQ(m_db);
	
	dirID = addDirectory(data);
	if(dirID==-1)
	{
		printError("addAlbum","Failed to get directory identifier");
		return -1;
	}
	albumName = dbString(data->albumName());
	if(albumName.isEmpty())
	{
		printError("addAlbum","No album name associated with track information");
		return -1;
	}
	year = (!data->year().isEmpty()) ? data->year().toInt() : 0;
	
	cmdQ = "SELECT albumID FROM album WHERE albumName=\'" + albumName + "\' AND year=" + QString::number(year) + " AND directoryID=" + QString::number(dirID) + ";";
	albumQ.prepare(cmdQ);
	albumQ.bind(albumID);
	if(!albumQ.next())
	{
		SQLiteInsert albumI(m_db);
		cmdI = "INSERT INTO album (albumName, year, directoryID, groupID) VALUES (?,?,?,?)";
		albumI.prepare(cmdI);
		albumI.bind(albumName);
		albumI.bind(year);
		albumI.bind(dirID);
		albumI.bind(groupID);
		if(albumI.next())
		{
			albumID = idOfInserted();
			if(albumID>=0)
			{
				tint gID;
				QString aDir,oDir;
				QVector<tint> albumIDList;
				SQLiteQuery groupQ(m_db),oDirQ(m_db);

				albumIDList.append(albumID);

				oDir = getDirectoryName(dirID);
				if(!oDir.isEmpty())
				{
					int albumDirID;
					
					cmdQ  = "SELECT a.albumID, b.directoryID";
					cmdQ += "  FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
					cmdQ += "  WHERE a.albumName=\'" + albumName + "\'";
					groupQ.prepare(cmdQ);
					groupQ.bind(gID);
					groupQ.bind(albumDirID);
					while(groupQ.next())
					{
						aDir = getDirectoryName(albumDirID);
						if(gID!=albumID)
						{
							QString cDir;
							QVector<QString> dirList;

							dirList.append(oDir);
							dirList.append(aDir);
							cDir = common::DiskOps::commonRoot(dirList);
							if(!cDir.isEmpty())
							{
								QDir commonDir(cDir);
								QVector<QString>::iterator ppL;
								bool found = true;

								cDir = commonDir.absolutePath();
								for(ppL=dirList.begin();ppL!=dirList.end() && found;ppL++)
								{
									QDir aDir(*ppL);
									if(aDir.absolutePath()!=cDir)
									{
										aDir.cdUp();
										if(aDir.absolutePath()!=cDir)
										{
											found = false;
										}
									}
								}
								if(found)
								{
									albumIDList.append(gID);
								}
							}
						}
					}
					if(albumIDList.size()>=2)
					{
						groupAlbums(albumIDList);
					}
				}
			}
			else
			{
				printError("addAlbum","Failed to get inserted album information");
				albumID = -1;
			}
		}
		else
		{
			printError("addAlbum","Error inserting record for album");
			albumID = -1;
		}
	}
	return albumID;
}


//-------------------------------------------------------------------------------------------

tint TrackDB::addTrack(info::Info *data,tint dirID,tint albumID)
{
	tint fileID,trackID = -1;
	QString cmd;
	SQLiteQuery trackPQ(m_db);
	
	fileID = addFile(data,dirID);
	if(fileID==-1)
	{
		printError("addTrack","Could not get file record for track");
		return -1;
	}
	
	cmd = "SELECT trackID FROM track WHERE albumID=" + QString::number(albumID) + " AND fileID=" + QString::number(fileID) + ";";
	trackPQ.prepare(cmd);
	trackPQ.bind(trackID);
	if(!trackPQ.next())
	{
		SQLiteQuery maxQ(m_db);
		
		cmd = "SELECT MAX(trackID) FROM track WHERE albumID=" + QString::number(albumID) + ";";
		maxQ.prepare(cmd);
		maxQ.bind(trackID);
		if(maxQ.next())
		{
			trackID++;
		}
		else
		{
			trackID = 1;
		}
		
		tint genreID,trackNo,discNo;
		tuint64 timeLength;
		QString trackName,artist,originalArtist,composer,comment,copyright,encoder;
		SQLiteInsert trackI(m_db);
		
		trackName = data->title();
		if(trackName.isEmpty())
		{
			data->get(info::e_TagInfo_FileName,trackName);
		}
		trackName = dbString(trackName);
		artist = dbString(data->artist());
		originalArtist = dbString(data->originalArtist());
		composer = dbString(data->composer());
		comment = dbString(data->comment());
		copyright = dbString(data->copyright());
		encoder = dbString(data->encoder());
		timeLength = static_cast<tuint64>(data->length());
		genreID = addGenre(data);
		discNo = (!data->disc().isEmpty()) ? data->disc().toInt() : 1;
		trackNo = (!data->track().isEmpty()) ? data->track().toInt() : 0;

		cmd = "INSERT INTO track VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
		trackI.prepare(cmd);
		trackI.bind(albumID);
		trackI.bind(trackID);
		trackI.bind(fileID);
		trackI.bind(trackName);
		trackI.bind(artist);
		trackI.bind(discNo);
		trackI.bind(trackNo);
		trackI.bind(originalArtist);
		trackI.bind(composer);
		trackI.bind(genreID);
		trackI.bind(comment);
		trackI.bind(copyright);
		trackI.bind(encoder);
		trackI.bind(timeLength);
		if(!trackI.next())
		{
			printError("addTrack","Error inserting track record");
			trackID = -1;
		}
	}
	return trackID;
}

//-------------------------------------------------------------------------------------------

tint TrackDB::addGenre(info::Info *data)
{
	tint genreID = 0;
	
	if(!data->genre().isEmpty())
	{
		QString cmd;
		QString genreName(dbString(data->genre()));
		SQLiteQuery genreQ(m_db);
		
		cmd = "SELECT genreID FROM genre WHERE genreName=\'" + genreName + "\'";
		genreQ.prepare(cmd);
		genreQ.bind(genreID);
		if(!genreQ.next())
		{
			SQLiteQuery maxQ(m_db);
			SQLiteInsert genreI(m_db);
			
			cmd = "SELECT MAX(genreID) FROM genre";
			maxQ.prepare(cmd);
			maxQ.bind(genreID);
			if(maxQ.next())
			{
				genreID++;
			}
			else
			{
				genreID = 1;
			}
			
			cmd = "INSERT INTO genre VALUES (?,?)";
			genreI.prepare(cmd);
			genreI.bind(genreID);
			genreI.bind(genreName);
			if(!genreI.next())
			{
				printError("addGenre","Error inserting genre record");
				genreID = -1;
			}
		}
	}
	else
	{
		genreID = 0;
	}
	return genreID;
}

//-------------------------------------------------------------------------------------------

tint TrackDB::addDirectory(info::Info *data)
{
	return addDirectory(data->directory());
}

//-------------------------------------------------------------------------------------------

QString TrackDB::formatDirectoryPath(const QString& path) const
{
	QString p = path;
	
	if(p.at(p.length() - 1) != QChar('/') && p.at(p.length() - 1) != QChar('\\'))
	{
		p += "/";
	}
	return QDir::toNativeSeparators(p);
}

//-------------------------------------------------------------------------------------------

int TrackDB::getMountDirectoryID(const QString& dName, QString& mName)
{
	int mountID;
	SQLiteQuery mountQ(m_db);
	QString mountName, cmdQ;
	
	cmdQ = "SELECT mountID, mountName FROM mountpoints";
	mountQ.prepare(cmdQ);
	mountQ.bind(mountID);
	mountQ.bind(mountName);
	while(mountQ.next())
	{
		mountName = formatDirectoryPath(mountName);
		
		if(dName.length() >= mountName.length())
		{
			QString n = dName.left(mountName.length());
			if(n == mountName)
			{
				mName = mountName;
				return mountID;
			}
		}
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

QString TrackDB::getMountDirectoryName(const QString& dName)
{
	int mountID;
	QString mountName, mDirName;
	
	mountID = getMountDirectoryID(dName, mountName);
	if(mountID >= 0)
	{
		mDirName = dName.mid(mountName.length());
	}
	return mDirName;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::addMountDirectory(int dirID, const QString& dName)
{
	int mountID;
	QString mountName;
	bool res = false;
	
	mountID = getMountDirectoryID(dName, mountName);
	if(mountID >= 0)
	{
		QString cmdI;
		SQLiteInsert mDirI(m_db);
		
		cmdI = "INSERT INTO dirmount (mountID, dirID) VALUES (?,?)";
		mDirI.prepare(cmdI);
		mDirI.bind(mountID);
		mDirI.bind(dirID);
		if(mDirI.next())
		{
			res = true;
		}
		else
		{
			QString err = QString("Failed to directory mount record mountID=%1, dirID=%2").arg(mountID).arg(dirID);
			printError("addMountDirectory", err.toUtf8().constData());
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tint TrackDB::addDirectory(const QString& dName)
{
	tint dirID = -1;
	QString dirName(dName);
	
	if(!dirName.isEmpty())
	{
		QString cmdQ, cmdI, mDirName, orgName;
		SQLiteQuery dirQ(m_db);
		
		dirName = formatDirectoryPath(dirName);
		
		orgName = dirName;
		mDirName = getMountDirectoryName(dirName);
		if(!mDirName.isEmpty())
		{
			dirName = mDirName;
		}
		
		dirName = dbString(dirName);
		
		cmdQ = "SELECT directoryID FROM directory WHERE directoryName=\'" + dirName + "\';";
		dirQ.prepare(cmdQ);
		dirQ.bind(dirID);
		if(!dirQ.next())
		{
			SQLiteInsert dirI(m_db);
			cmdI = "INSERT INTO directory (directoryName) VALUES (?)";
			dirI.prepare(cmdI);
			dirI.bind(dirName);
			if(dirI.next())
			{
				dirID = idOfInserted();
				if(dirID >= 0)
				{
					if(!mDirName.isEmpty())
					{
						if(!addMountDirectory(dirID, orgName))
						{
							printError("addDirectory","Error in inserting mount point relationship");
							dirID = -1;						
						}
					}
				}
				else
				{
					printError("addDirectory","Error getting ID of inserted directory information");
					dirID = -1;
				}
			}
			else
			{
				printError("addDirectory","Error inserting directory information");
				dirID = -1;
			}
		}
	}
	else
	{
		printError("addDirectory","There is no directory information for track information");
		dirID = -1;
	}
	return dirID;
}

//-------------------------------------------------------------------------------------------

tint TrackDB::addFile(info::Info *data,tint dirID)
{
	tint fileID = -1;
	QString cmdQ,fileName;
	SQLiteQuery fileQ(m_db);
	
	fileName = dbString(data->filename());
	if(!fileName.isEmpty())
	{
		cmdQ = "SELECT fileID FROM file WHERE directoryID=" + QString::number(dirID) + " AND fileName=\'" + fileName + "\';";
		fileQ.prepare(cmdQ);
		fileQ.bind(fileID);
		if(!fileQ.next())
		{
			QString cmd;
			SQLiteQuery maxQ(m_db);
			
			cmd = "SELECT MAX(fileID) FROM file WHERE directoryID=" + QString::number(dirID) + ";";
			maxQ.prepare(cmd);
			maxQ.bind(fileID);
			if(maxQ.next())
			{
				fileID++;
			}
			else
			{
				fileID = 1;
			}
			
			tint codecType,infoType,fileSize;
			tuint64 updateTime;
			QString ext;
			SQLiteInsert fileI(m_db);
			
			ext = engine::Codec::getFileExtension(fileName);
			if(ext=="mp3")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecMp3);
			}
			else if(ext=="m4a" || ext=="m4b")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecMp4);
			}
			else if(ext=="wav")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecWav);
			}
			else if(ext=="ogg")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecVorbis);
			}
			else if(ext=="flac")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecFlac);
			}
			else if(ext=="wma")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecWma);
			}
			else if(ext=="aif" || ext=="aiff")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecAIFF);
			}
			else if(ext=="mpc" || ext=="mp+" || ext=="mpp")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecMPC);
			}
			else if(ext=="ape")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecAPE);
			}
			else if(ext=="wv")
			{
				codecType = static_cast<tint>(engine::Codec::e_codecWavPack);
			}
			else
			{
				codecType = static_cast<tint>(engine::Codec::e_codecUnknown);
			}
			infoType = static_cast<tint>(data->type());
			updateTime = static_cast<tuint64>(getModifiedTime(data->getFilename(),fileSize));
			
			cmd = "INSERT INTO file VALUES (?,?,?,?,?,?,?);";
			fileI.prepare(cmd);
			fileI.bind(dirID);
			fileI.bind(fileID);
			fileI.bind(codecType);
			fileI.bind(infoType);
			fileI.bind(fileName);
			fileI.bind(updateTime);
			fileI.bind(fileSize);
			if(!fileI.next())
			{
				printError("addFile","Error inserting file record");
				fileID = -1;
			}
		}
	}
	else
	{
		printError("addFile","Track information has no associated file name");
	}
	return fileID;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::addImage(info::Info *data,tint albumID,tint trackID)
{
	if(data->isImage())
	{
		tint type,imageID;
		info::Info::ImageFormat format;
		common::Array<tubyte,tubyte> *pImg;
		QString cmdQ,cmdI;
		QSet<tint> idSet;
		QSet<tint>::iterator ppI;
		SQLiteInsert imageMapI(m_db);
		
		for(type=0;type<0x00000015;type++)
		{
			pImg = data->getImageData(static_cast<info::IDTagImageType>(type),format);
			if(pImg!=0)
			{
				QPair<tuint64,tuint64> sig = getSHA1Signature(pImg);
				tint64 sigL = static_cast<tint64>(sig.first);
				tint64 sigH = static_cast<tint64>(sig.second);
				SQLiteQuery imageQ(m_db);
				
				cmdQ = "SELECT imageID FROM image WHERE sha1SignatureH=" + QString::number(sigH) + " AND sha1SignatureL=" + QString::number(sigL) + ";";
				imageQ.prepare(cmdQ);
				imageQ.bind(imageID);
				if(!imageQ.next())
				{
					tint iFormat = static_cast<tint>(format);
					SQLiteInsert imageI(m_db);
					QByteArray iArr(reinterpret_cast<const char *>(pImg->GetData()),pImg->GetSize());
					
					cmdI = "INSERT INTO image (type, format, sha1SignatureH, sha1SignatureL, data) VALUES (?,?,?,?,?);";
					imageI.prepare(cmdI);
					imageI.bind(type);
					imageI.bind(iFormat);
					imageI.bind(sigH);
					imageI.bind(sigL);
					imageI.bind(iArr);
					if(imageI.next())
					{
						SQLiteQuery idQ(m_db);
						
						idQ.prepare(cmdQ);
						idQ.bind(imageID);
						if(idQ.next())
						{
							idSet.insert(imageID);
						}
						else
						{
							printError("addImage","Error getting inserted image record");
							return false;
						}
					}
					else
					{
						printError("addImage","Error inserting image record");
						return false;
					}
				}
				else
				{
					idSet.insert(imageID);
				}
			}
		}
		
		cmdI = "DELETE FROM imagemap WHERE albumID=" + QString::number(albumID) + " AND trackID=" + QString::number(trackID) + ";";
		m_db->exec(cmdI);
		cmdI = "INSERT INTO imagemap VALUES (?,?,?);";
		imageMapI.prepare(cmdI);
		
		for(ppI=idSet.begin();ppI!=idSet.end();ppI++)
		{
			imageID = *ppI;
			imageMapI.bind(albumID);
			imageMapI.bind(trackID);
			imageMapI.bind(imageID);
			if(!imageMapI.next())
			{
				printError("addImage","Error inserting image map record");
				return false;
			}
		}
	}
	else
	{
		return updateAlbumImage(albumID);
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::addSubtrack(info::Info *data,tint albumID,tint trackID)
{
	if(data->isChildren())
	{
		tint i;
		QString cmd;
		QString subtrackName;
		tuint64 timeStart,timeLength;
		SQLiteInsert subtrackI(m_db);

		cmd = "DELETE FROM subtrack WHERE albumID=" + QString::number(albumID) + " AND trackID=" + QString::number(trackID) + ";";
		m_db->exec(cmd);
		
		cmd = "INSERT INTO subtrack VALUES (?,?,?,?,?,?);";
		subtrackI.prepare(cmd);
		
		for(i=0;i<data->noChildren();i++)
		{
			const info::Info::ChildInfo& subtrack = data->child(i);
			
			subtrackName = dbString(subtrack.name());
			timeStart = static_cast<tuint64>(subtrack.startTime());
			timeLength = static_cast<tuint64>(subtrack.length());
			
			subtrackI.bind(albumID);
			subtrackI.bind(trackID);
			subtrackI.bind(i);
			subtrackI.bind(subtrackName);
			subtrackI.bind(timeStart);
			subtrackI.bind(timeLength);
			if(!subtrackI.next())
			{
				printError("addSubtrack","Error inserting subtrack record");
				return false;
			}
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------

QPair<tuint64,tuint64> TrackDB::getSHA1Signature(common::Array<tubyte,tubyte> *pArr)
{
	tuint64 sigL,sigH;
	tbyte sigMem[common::c_SHA1HashSize];
	common::SHA1Digest digest;
	
	if(pArr!=0 && pArr->GetSize()>0)
	{
		digest.input(reinterpret_cast<const tbyte *>(pArr->GetData()),pArr->GetSize());
	}
	digest.getDigestFinal(sigMem,common::c_SHA1HashSize);
	sigL = network::NetMemory::toLong(sigMem,0);
	sigH = network::NetMemory::toLong(sigMem,8);
	return QPair<tuint64,tuint64>(sigL,sigH);
}

//-------------------------------------------------------------------------------------------

bool TrackDB::erase(const QString& fullFileName)
{
	tint albumID,trackID,fileID,dirID;
	bool res = true;
	
	if(getKeysFromFilename(fullFileName,albumID,trackID,dirID,fileID))
	{
		res = eraseTrack(albumID,trackID,dirID,fileID);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::erase(tint albumID,tint trackID)
{
	tint fileID,dirID;
	QString cmd;
	SQLiteQuery idQ(m_db);
	bool res = true;
	
	cmd  = "SELECT a.directoryID, b.fileID";
	cmd += "    FROM album AS a INNER JOIN track AS b ON a.albumID=b.trackID";
	cmd += "    WHERE a.albumID=" + QString::number(albumID) + " AND b.trackID=" + QString::number(trackID) + ";";
	idQ.prepare(cmd);
	idQ.bind(dirID);
	idQ.bind(fileID);
	if(idQ.next())
	{
		res = eraseTrack(albumID,trackID,dirID,fileID);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::eraseTrack(tint albumID,tint trackID,tint dirID,tint fileID)
{
	bool res = true;
	tint i,id,count,imageID;
	QString cmd;
	QString albumIDStr(QString::number(albumID));
	QString trackIDStr(QString::number(trackID));
	QString dirIDStr(QString::number(dirID));
	QString fileIDStr(QString::number(fileID));
	QSet<tint> imageIDSet,delIDSet;
	QSet<tint>::iterator ppI;
		
	try
	{
		m_db->exec("SAVEPOINT deleteTrack");
		
		// delete images
		SQLiteQuery imageInTrackQ(m_db);
		cmd = "SELECT imageID FROM imagemap WHERE albumID=" + albumIDStr + " AND trackID=" + trackIDStr + ";";
		imageInTrackQ.prepare(cmd);
		imageInTrackQ.bind(imageID);
		while(imageInTrackQ.next())
		{
			imageIDSet.insert(imageID);
		}
		if(imageIDSet.size()>0)
		{
			SQLiteQuery imageNoQ(m_db);
			
			cmd = "SELECT imageID, COUNT(*) FROM imagemap WHERE ";
			for(i=0,ppI=imageIDSet.begin();ppI!=imageIDSet.end();ppI++,i++)
			{
				if(i>0)
				{
					cmd += " OR ";
				}
				cmd += "imageID=" + QString::number(*ppI);
			}
			cmd += " GROUP BY imageID;";
			imageNoQ.prepare(cmd);
			imageNoQ.bind(imageID);
			imageNoQ.bind(count);
			while(imageNoQ.next())
			{
				ppI = imageIDSet.find(imageID);
				if(ppI!=imageIDSet.end())
				{
					if(count<2)
					{
						delIDSet.insert(imageID);
					}
					imageIDSet.erase(ppI);
				}
			}
			for(ppI=imageIDSet.begin();ppI!=imageIDSet.end();ppI++)
			{
				delIDSet.insert(*ppI);
			}
			if(delIDSet.size()>0)
			{
				cmd = "DELETE FROM image WHERE ";
				for(i=0,ppI=delIDSet.begin();ppI!=delIDSet.end();ppI++,i++)
				{
					if(i>0)
					{
						cmd += " OR ";
					}
					cmd += "imageID=" + QString::number(*ppI);
				}
				m_db->exec(cmd);
			}
			cmd = "DELETE FROM imagemap WHERE albumID=" + albumIDStr + " AND trackID=" + trackIDStr + ";";
			m_db->exec(cmd);
		}
		
		// delete album - if required
		bool delAlbum = false;
		SQLiteQuery albumCountQ(m_db);
		cmd = "SELECT albumID, COUNT(*) FROM track WHERE albumID=" + albumIDStr + " GROUP BY albumID";
		albumCountQ.prepare(cmd);
		albumCountQ.bind(id);
		albumCountQ.bind(count);
		if(albumCountQ.next())
		{
			if(count<2)
			{
				delAlbum = true;
			}
		}
		else
		{
			delAlbum = true;
		}
		if(delAlbum)
		{
			cmd = "DELETE FROM album WHERE albumID=" + albumIDStr + ";";
			m_db->exec(cmd);
		}
		
		// delete subtracks
		cmd = "DELETE FROM subtrack WHERE albumID=" + albumIDStr + " AND trackID=" + trackIDStr + ";";
		m_db->exec(cmd);
		
		// delete track
		cmd = "DELETE FROM track WHERE albumID=" + albumIDStr + " AND trackID=" + trackIDStr + ";";
		m_db->exec(cmd);
		
		// delete directory - if required
		bool delDir = false;
		SQLiteQuery dirCountQ(m_db);
		cmd = "SELECT directoryID, COUNT(*) FROM file WHERE directoryID=" + dirIDStr + ";";
		dirCountQ.prepare(cmd);
		dirCountQ.bind(id);
		dirCountQ.bind(count);
		if(dirCountQ.next())
		{
			if(count<2)
			{
				delDir = true;
			}
		}
		else
		{
			delDir = true;
		}
		if(delDir)
		{
			cmd = "DELETE FROM directory WHERE directoryID=" + dirIDStr + ";";
			m_db->exec(cmd);
			cmd = "DELETE FROM dirmount WHERE dirID=" + dirIDStr + ";";
			m_db->exec(cmd);
		}
		
		// delete file
		cmd = "DELETE FROM file WHERE directoryID=" + dirIDStr + " AND fileID=" + fileIDStr + ";";
		m_db->exec(cmd);
		
		m_db->exec("RELEASE deleteTrack");
	}
	catch(const SQLiteException& e)
	{
		QString errStr;
		m_db->exec("ROLLBACK TO SAVEPOINT deleteTrack");
		errStr = "Database exception thrown. " + e.error();
		printError("add",errStr.toUtf8().constData());
		res = false;
	}
	return res;	
}

//-------------------------------------------------------------------------------------------

bool TrackDB::getKeysFromFilename(const QString& fullFileName,tint& albumID,tint& trackID)
{
	tint dirID,fileID;
	return getKeysFromFilename(fullFileName,albumID,trackID,dirID,fileID);
}

//-------------------------------------------------------------------------------------------

bool TrackDB::getKeysFromFilename(const QString& fullFileName,tint& albumID,tint& trackID,tint& dirID,tint& fileID)
{
	QString fileName,dirName;
	bool res = false;
	
	if(getFilenameComponents(fullFileName,dirName,fileName))
	{
		int mountID;
		QString cmd, mountName;
		SQLiteQuery idQ(m_db);
		
		dirName = QDir::toNativeSeparators(dirName);
		fileName = QDir::toNativeSeparators(fileName);
		
		mountID = getMountDirectoryID(dirName, mountName);
		if(mountID >= 0)
		{
			dirName = dirName.mid(mountName.length());
		
			cmd  = "SELECT a.albumID, d.trackID, a.directoryID, c.fileID";
			cmd += "  FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
			cmd += "    INNER JOIN file AS c ON b.directoryID=c.directoryID";
			cmd += "    INNER JOIN track AS d ON a.albumID=d.albumID AND c.fileID=d.fileID";
			cmd += "	INNER JOIN dirmount AS e ON b.directoryID=e.dirID";
			cmd += "	INNER JOIN mountpoints AS f ON e.mountID=f.mountID";
		}
		else
		{
			cmd  = "SELECT a.albumID, d.trackID, a.directoryID, c.fileID";
			cmd += "  FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
			cmd += "    INNER JOIN file AS c ON b.directoryID=c.directoryID";
			cmd += "    INNER JOIN track AS d ON a.albumID=d.albumID AND c.fileID=d.fileID";		
		}
		cmd += "  WHERE b.directoryName=\'" + dbString(dirName) + "\' AND c.fileName=\'" + dbString(fileName) + "\';";

		idQ.prepare(cmd);
		idQ.bind(albumID);
		idQ.bind(trackID);
		idQ.bind(dirID);
		idQ.bind(fileID);
		res = (idQ.next()) ? true : false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::getFilenameComponents(const QString& name,QString& dirName,QString& fileName)
{
	tint i;
	
	for(i=name.length()-1;i>=0;i--)
	{
		if(name.at(i)==QChar('/') || name.at(i)==QChar('\\'))
		{
			dirName = name.mid(0,i+1);
			fileName = name.mid(i+1);
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::isUpdateRequired(tint dirID,tint fileID)
{
	tint fileSize;
	tuint64 mTV;
	QString dirName,fileName;
	common::TimeStamp mT;
	QString cmd;
	SQLiteQuery fileQ(m_db);
	bool res = false;
	
	cmd  = "SELECT b.fileName, b.updateTime, b.fileSize";
	cmd += "    FROM directory AS a INNER JOIN file AS b ON a.directoryID=b.directoryID";
	cmd += "    WHERE a.directoryID=" + QString::number(dirID) + " AND b.fileID=" + QString::number(fileID) + ";";
	fileQ.prepare(cmd);
	fileQ.bind(fileName);
	fileQ.bind(mTV);
	fileQ.bind(fileSize);
	if(fileQ.next())
	{
		tint actualFileSize = -1;
		common::TimeStamp actualMT;
		
		dirName = getDirectoryName(dirID);
		fileName = dbStringInv(fileName);

		mT = mTV;
		fileName = dirName + fileName;
		actualMT = getModifiedTime(fileName,actualFileSize);
		if(actualMT>mT || fileSize!=actualFileSize)
		{
			res = true;
		}
	}
	else
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::update(info::Info *data)
{
	bool res = false;
	
	try
	{
		m_db->exec("SAVEPOINT updateTrack");
		
		if(erase(data->getFilename()))
		{
			if(add(data))
			{
				res = true;
			}
			else
			{
				printError("update","Error adding updated track record");
			}
		}
		else
		{
			printError("update","Error removing old track record");
		}
		
		if(res)
		{
			m_db->exec("RELEASE updateTrack");
		}
		else
		{
			m_db->exec("ROLLBACK TO SAVEPOINT updateTrack");
		}
	}
	catch(const SQLiteException& e)
	{
		QString errStr;
		m_db->exec("ROLLBACK TO SAVEPOINT updateTrack");
		errStr = "Database exception thrown. " + e.error();
		printError("update",errStr.toUtf8().constData());
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tint TrackDB::groupAlbums(const QVector<tint>& albumIDList)
{
	tint dirID = -1;
	QString cmdQ,gDirName;
	QMap<tint,QString> albumDirMap;
	QSet<QPair<tint,tint> > trackAlbum;
	QVector<QString> dirList;
	QVector<tint>::const_iterator ppI;
	QSet<QPair<tint,tint> >::iterator ppJ;
	QMap<tint,QString>::iterator ppK;
	QVector<QString>::iterator ppL;
	
	if(albumIDList.size()<2)
	{
		return -1;
	}
	
	for(ppI=albumIDList.begin();ppI!=albumIDList.end();ppI++)
	{
		int dirIDMount;
		QString dirName;
		SQLiteQuery dirQ(m_db);
		
		cmdQ  = "SELECT b.directoryID FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
		cmdQ += "  WHERE a.albumID=" + QString::number(*ppI);
		dirQ.prepare(cmdQ);
		dirQ.bind(dirIDMount);
		if(dirQ.next())
		{
			dirName = getDirectoryName(dirIDMount);
			QDir cDir(dirName);
			dirList.append(cDir.absolutePath());
			albumDirMap.insert(*ppI,dirName);
		}
		else
		{
			return -1;
		}
	}
	
	if(dirList.size()>1)
	{
		QString commonPath = common::DiskOps::commonRoot(dirList);
		
		if(!commonPath.isEmpty())
		{
			QDir commonDir(commonPath);

			commonPath = commonDir.absolutePath();
			for(ppL=dirList.begin();ppL!=dirList.end();ppL++)
			{
				QDir cDir(*ppL);
				if(cDir.absolutePath()!=commonPath)
				{
					cDir.cdUp();
					if(cDir.absolutePath()!=commonPath)
					{
						return -1;
					}
				}
			}
			gDirName = commonPath;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		gDirName = dirList.at(0);
	}
	
	for(ppI=albumIDList.begin();ppI!=albumIDList.end();ppI++)
	{
		tint discNo,trackNo;
		QString dName,fName;
		SQLiteQuery dtQ(m_db);
		
		// as the subsequent search is based on directory name and path is not actual used
		cmdQ  = "SELECT b.discNo,b.trackNo,c.directoryName,d.fileName";
		cmdQ += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
		cmdQ += "    INNER JOIN directory AS c ON a.directoryID=c.directoryID";
		cmdQ += "    INNER JOIN file AS d ON (c.directoryID=d.directoryID AND b.fileID=d.fileID)";
		cmdQ += "  WHERE a.albumID=" + QString::number(*ppI);

		dtQ.prepare(cmdQ);
		dtQ.bind(discNo);
		dtQ.bind(trackNo);
		dtQ.bind(dName);
		dtQ.bind(fName);
		while(dtQ.next())
		{
			QPair<tint,tint> dtPair(discNo,trackNo);
			
			dName = dbStringInv(dName);
			fName = dbStringInv(fName);

			ppJ = trackAlbum.find(dtPair);
			if(ppJ==trackAlbum.end())
			{
				trackAlbum.insert(dtPair);
			}
			else
			{
				tint aDiscNo,aTrackNo;
				QString cmdP;
				SQLiteQuery ctQ(m_db);

				cmdP  = "SELECT d.discNo,d.trackNo";
				cmdP += "  FROM directory AS a INNER JOIN file AS b ON a.directoryID=b.directoryID";
				cmdP += "    INNER JOIN album AS c ON a.directoryID=c.directoryID";
				cmdP += "    INNER JOIN track AS d ON (c.albumID=d.albumID AND b.fileID=d.fileID)";
				cmdP += "  WHERE a.directoryName='" + dbString(dName) + "' AND b.fileName='" + dbString(fName) + "'";
				
				ctQ.prepare(cmdP);
				ctQ.bind(aDiscNo);
				ctQ.bind(aTrackNo);
				while(ctQ.next())
				{
					QPair<tint,tint> aDtPair(aDiscNo,aTrackNo);
					if(dtPair!=aDtPair)
					{
						return -1;
					}
				}
			}
		}
	}
	
	try
	{
		m_db->exec("SAVEPOINT groupAlbums");
		
		for(ppI=albumIDList.begin();ppI!=albumIDList.end() && dirID<0;ppI++)
		{
			SQLiteQuery gAlbumQ(m_db);
			cmdQ = "SELECT groupID FROM album WHERE albumID=" + QString::number(*ppI);
			gAlbumQ.prepare(cmdQ);
			gAlbumQ.bind(dirID);
			gAlbumQ.next();
		}
		
		if(dirID<0)
		{
			dirID = addDirectory(gDirName);
		}
		if(dirID>=0)
		{
			for(ppI=albumIDList.begin();ppI!=albumIDList.end();ppI++)
			{
				QString cmdU;
				cmdU = "UPDATE album SET groupID=" + QString::number(dirID) + " WHERE albumID=" + QString::number(*ppI);
				m_db->exec(cmdU);
			}
		}
		
		if(dirID>=0)
		{
			m_db->exec("RELEASE groupAlbums");
		}
		else
		{
			m_db->exec("ROLLBACK TO SAVEPOINT groupAlbums");
		}
	}
	catch(const SQLiteException& e)
	{
		printError("groupAlbums",e.error().toUtf8().constData());
		m_db->exec("ROLLBACK TO SAVEPOINT groupAlbums");
		dirID = -1;
	}
	return dirID;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::getTrackKey(const QString& fileName,QPair<tint,tint>& pairID)
{
	bool res = false;

	if(!fileName.isEmpty() && common::DiskOps::exist(fileName))
	{
		tint i;
		QString dName,fName;

		for(i=fileName.length()-1;i>=0;i--)
		{
			if(fileName.at(i)==QChar('/') || fileName.at(i)==QChar('\\'))
			{
				dName = fileName.mid(0,i+1);
				fName = fileName.mid(i+1);
				break;
			}
		}
		if(!dName.isEmpty() && !fName.isEmpty())
		{
			tint aID, tID, mountID;
			QString cmdQ, mountName;
			SQLiteQuery atQ(m_db);

			mountID = getMountDirectoryID(dName, mountName);
			cmdQ  = "SELECT a.albumID, d.trackID";
			if(mountID >= 0)
			{
				dName = dName.mid(mountName.length());
				cmdQ += "  FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
				cmdQ += "    INNER JOIN file AS c ON b.directoryID=c.directoryID";
				cmdQ += "    INNER JOIN track AS d ON a.albumID=d.albumID AND c.fileID=d.fileID";
				cmdQ += "	INNER JOIN dirmount AS e ON b.directoryID=e.dirID";
				cmdQ += "	INNER JOIN mountpoints AS f ON e.mountID=f.mountID";
			}
			else
			{
				cmdQ += "  FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
				cmdQ += "    INNER JOIN file AS c ON b.directoryID=c.directoryID";
				cmdQ += "    INNER JOIN track AS d ON a.albumID=d.albumID AND c.fileID=d.fileID";
			}
			cmdQ += "  WHERE b.directoryName=\'" + dbString(dName) + "\' AND c.fileName=\'" + dbString(fName) + "\';";
			
			atQ.prepare(cmdQ);
			atQ.bind(aID);
			atQ.bind(tID);
			if(atQ.next())
			{
				pairID = QPair<tint,tint>(aID,tID);
				res = true;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int TrackDB::idOfInserted()
{
	int id = -1;
	QString cmd = "SELECT last_insert_rowid()";
	SQLiteQuery idQ(m_db);
	idQ.prepare(cmd);
	idQ.bind(id);
	idQ.next();
	return id;
}

//-------------------------------------------------------------------------------------------

QString TrackDB::dbString(const QString& dStr)
{
	int i;
	QStringList x;
	QString s;

	for(i=0;i<dStr.length();i++)
	{
		if(dStr.at(i)==QChar('\''))
		{
			x << QString("&lsquo;");
		}
		else if(dStr.at(i)==QChar('\"'))
		{
			x << QString("&quot;");
		}
		else if(dStr.at(i)==QChar('&'))
		{
			x << QString("&amp;");
		}
		else
		{
			x << QString(dStr.at(i));
		}
	}
	s = x.join("");
	return s;
}

//-------------------------------------------------------------------------------------------

QString TrackDB::dbStringInv(const QString& dStr)
{
	static common::BOParse *dbParser = 0;
	static tint dbPState[3] = {0,0,0};

	if(dbParser==0)
	{
		dbParser = new common::BOParse;
		dbPState[0] = dbParser->String("&lsquo;");
		dbPState[1] = dbParser->String("&quot;");
		dbPState[2] = dbParser->String("&amp;");
	}

	tint start = 0;
	QByteArray dArr,sArr = dStr.toUtf8();
	const tchar *x = sArr.constData();
	common::BO_Parse_Unit *item = dbParser->Lexical(x);

	while(item!=0)
	{
		if(start < item->start)
		{
			dArr.append(&x[start],item->start - start);
		}
		if(item->state==dbPState[0])
		{
			tchar t = '\'';
			dArr.append(&t,1);
		}
		else if(item->state==dbPState[1])
		{
			tchar t = '\"';
			dArr.append(&t,1);
		}
		else if(item->state==dbPState[2])
		{
			tchar t = '&';
			dArr.append(&t,1);
		}
		start = item->start + item->length;
		item = item->next;
	}
	if(start<sArr.size())
	{
		dArr.append(&x[start],sArr.size() - start);
	}

	QString s = QString::fromUtf8(dArr.constData(),dArr.size());
	return s;
}

//-------------------------------------------------------------------------------------------

int TrackDB::dbVersion(SQLiteDatabase *db)
{
	QString cmd;
	int cVersionNo = 1;
	SQLiteQuery projectQ(db);
		
	cmd = "SELECT dbVersion FROM databaseInfo";
	projectQ.prepare(cmd);
	projectQ.bind(cVersionNo);
	projectQ.next();
	return cVersionNo;
}

//-------------------------------------------------------------------------------------------

int TrackDB::dbVersion()
{
	return dbVersion(m_db);
}

//-------------------------------------------------------------------------------------------

bool TrackDB::updateAlbumImage(int albumID)
{
	tint imageID;
	tuint64 dirModifiedT,fileModifiedT;
	bool updateFlag = false,res = true;
	QString curFileName,cmdQ,cmdD,cmdI;
	SQLiteQuery aImageQ(m_db);
	
	cmdQ = "SELECT imageID,fileName,dirModifiedTime,fileModifiedTime FROM imagealbummap WHERE albumID=" + QString::number(albumID);
	aImageQ.prepare(cmdQ);
	aImageQ.bind(imageID);
	aImageQ.bind(curFileName);
	aImageQ.bind(dirModifiedT);
	aImageQ.bind(fileModifiedT);
	if(aImageQ.next())
	{
		common::TimeStamp dirTime(dirModifiedT),fileTime(fileModifiedT);
		QString dName,fName;
		
		curFileName = dbStringInv(curFileName);
		if(curFileName.isEmpty())
		{
			SQLiteQuery aDirQ(m_db);

			cmdQ  = "SELECT CASE WHEN d.mountID IS NULL THEN b.directoryName ELSE (d.mountName || b.directoryName) END directoryName";
			cmdQ += "  FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
			cmdQ += "    LEFT JOIN dirmount AS c ON b.directoryID=c.dirID";
			cmdQ += "    LEFT JOIN mountpoints AS d ON c.mountID=d.mountID";
			cmdQ += "  WHERE a.albumID=" + QString::number(albumID);
			aDirQ.prepare(cmdQ);
			aDirQ.bind(dName);
			if(aDirQ.next())
			{
				common::TimeStamp dTS;
				
				dName = dbStringInv(dName);
				dTS = getModifiedTime(dName);
				if(dTS > dirTime)
				{
					updateFlag = true;
				}
			}
			else
			{
				res = false;
			}
		}
		else if(getFilenameComponents(curFileName,dName,fName))
		{
			common::TimeStamp dTS = getModifiedTime(dName);
			if(dTS > dirTime)
			{
				updateFlag = true;
			}
		}
		else
		{
			res = false;
		}	
	}
	else
	{
		updateFlag = true;
		imageID = -2;
	}
	
	if(updateFlag && res)
	{
		try
		{
			QString albumName,dirName,imageName;
			SQLiteQuery albumNameQ(m_db);
			
			m_db->exec("SAVEPOINT updateAlbumImage");

			cmdQ  = "SELECT a.albumName, CASE WHEN d.mountID IS NULL THEN b.directoryName ELSE (d.mountName || b.directoryName) END directoryName";
			cmdQ += "  FROM album AS a INNER JOIN directory AS b ON a.directoryID=b.directoryID";
			cmdQ += "    LEFT JOIN dirmount AS c ON b.directoryID=c.dirID";
			cmdQ += "    LEFT JOIN mountpoints AS d ON c.mountID=d.mountID";
			cmdQ += "  WHERE a.albumID=" + QString::number(albumID);

			albumNameQ.prepare(cmdQ);
			albumNameQ.bind(albumName);
			albumNameQ.bind(dirName);
			if(albumNameQ.next())
			{
				common::TimeStamp nDirTime,nFileTime;
				
				albumName = dbStringInv(albumName);
				dirName = dbStringInv(dirName);
				
				nDirTime = getModifiedTime(dirName);
				
				imageName = loadDirectoryImage(dirName,albumName);
				if(!imageName.isEmpty())
				{
					bool updateImageFlag = false;
					
					nFileTime = getModifiedTime(imageName);
					if(imageName!=curFileName)
					{
						updateImageFlag = true;
					}
					else
					{
						if(nFileTime > fileModifiedT)
						{
							updateImageFlag = true;
						}
					}
					
					if(updateImageFlag)
					{
						if(imageID>=0)
						{
							cmdD = "DELETE FROM image WHERE imageID=" + QString::number(imageID);
							m_db->exec(cmdD);
						}
						imageID = addImageFile(imageName);
					}
				}
				else
				{
					if(imageID>=0)
					{
						cmdD = "DELETE FROM image WHERE imageID=" + QString::number(imageID);
						m_db->exec(cmdD);
					}
					imageID = -1;
				}
				
				if(updateFlag)
				{
					cmdD = "DELETE FROM imagealbummap WHERE albumID=" + QString::number(albumID);
					m_db->exec(cmdD);
					if(imageID>-2)
					{
						tuint64 nDirT = static_cast<tuint64>(nDirTime);
						tuint64 nFileT = static_cast<tuint64>(nFileTime);
						SQLiteInsert iAlbumI(m_db);
						
						imageName = dbString(imageName);
						cmdI = "INSERT INTO imagealbummap VALUES (?,?,?,?,?)";
						iAlbumI.prepare(cmdI);
						iAlbumI.bind(albumID);
						iAlbumI.bind(imageID);
						iAlbumI.bind(imageName);
						iAlbumI.bind(nDirT);
						iAlbumI.bind(nFileT);
						if(!iAlbumI.next())
						{
							res = false;
						}
					}
				}
			}
		
			if(res)
			{
				m_db->exec("RELEASE updateAlbumImage");
			}
			else
			{
				m_db->exec("ROLLBACK TO SAVEPOINT updateAlbumImage");
			}
		}
		catch(const SQLiteException& e)
		{
			QString errStr;
			m_db->exec("ROLLBACK TO SAVEPOINT updateAlbumImage");
			errStr = "Database exception thrown. " + e.error();
			printError("updateAlbumImage",errStr.toUtf8().constData());
			res = false;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int TrackDB::addImageFile(const QString& fileName)
{
	info::Info::ImageFormat format;
	QString ext;
	
	ext = engine::Codec::getFileExtension(fileName);
	if(ext=="jpeg" || ext=="jpg")
	{
		format = info::Info::e_imageJPEG;
	}
	else if(ext=="gif")
	{
		format = info::Info::e_imageGIF;
	}
	else if(ext=="png")
	{
		format = info::Info::e_imagePNG;
	}
	else
	{
		return -1;
	}

#if defined(OMEGA_MAC_STORE)
	common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
	if(sbBookmark->has(fileName,true))
	{
		sbBookmark->checkOut(fileName,true);
	}
#endif

	{
		QImage fImage(fileName);
		if(fImage.isNull())
		{
			return -1;
		}
	}
	
	tint imageID = -1;
	engine::File f;
	
	if(f.open(fileName))
	{
		common::Array<tubyte,tubyte> *pImg = new common::Array<tubyte,tubyte>();
		tint len = f.length();
		
		pImg->SetSize(len);
		if(f.read(reinterpret_cast<tchar *>(pImg->GetData()),len)==len)
		{
			tint type = static_cast<tint>(info::e_TagImage_CoverFront);
			tint iFormat = static_cast<tint>(format);
			QString cmdI;
			QByteArray iArr(reinterpret_cast<const char *>(pImg->GetData()),pImg->GetSize());
			QPair<tuint64,tuint64> sig = getSHA1Signature(pImg);
			tint64 sigL = static_cast<tint64>(sig.first);
			tint64 sigH = static_cast<tint64>(sig.second);
			SQLiteInsert imageI(m_db);
			
			cmdI = "INSERT INTO image (type, format, sha1SignatureH, sha1SignatureL, data) VALUES (?,?,?,?,?);";
			imageI.prepare(cmdI);
			imageI.bind(type);
			imageI.bind(iFormat);
			imageI.bind(sigL);
			imageI.bind(sigH);
			imageI.bind(iArr);
			if(imageI.next())
			{
				imageID = idOfInserted();
			}
		}
		f.close();
		delete pImg;
	}
	
#if defined(OMEGA_MAC_STORE)
	sbBookmark->checkIn(fileName,true);
#endif
	
	return imageID;
}

//-------------------------------------------------------------------------------------------

tint TrackDB::levenshteinDistance(const QString& a,const QString& b) const
{
	tint dist;
	tint m = a.length();
	tint n = b.length();
	
	if(m>0 && n>0)
	{
		tint i,j;
		tint *d = new tint [m * n];
		
		// (i,j) = i + (j * m)
		
		for(i=0;i<m;i++)
		{
			d[i] = i;
		}
		for(j=0;j<n;j++)
		{
			d[0+(j*m)] = j;
		}
		
		for(j=1;j<n;j++)
		{
			for(i=1;i<m;i++)
			{
				if(a.at(i)==b.at(j))
				{
					d[i+(j*m)] = d[(i-1)+((j-1)*m)];
				}
				else
				{
					tint dV,iV,sV,v;
					
					dV = d[(i-1)+(j*m)] + 1; // deletion
					iV = d[i+((j-1)*m)] + 1; // insertion
					sV = d[(i-1)+((j-1)*m)] + 1; // subtitution
					
					v = dV;
					if(v > iV)
					{
						v = iV;
					}
					if(v > sV)
					{
						v = sV;
					}
					d[i+(j*m)] = v;
				}
			}
		}
		dist = d[(m-1)+((n-1)*m)];
		
		delete [] d;
	}
	else
	{
		if(m>0)
		{
			dist = m;
		}
		else if(n>0)
		{
			dist = n;
		}
		else
		{
			dist = 0;
		}
	}
	return dist;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

QString TrackDB::loadDirectoryImage(const QString& dirName,const QString& albumName) const
{
	QString qdName,dName,imageName;
	
	if(!dirName.isEmpty())
	{
		LPCWSTR wStr;
		struct _stat fileStat;
		
		dName = formatDirectoryPath(dirName);
		
		wStr = reinterpret_cast<LPCWSTR>(dName.utf16());
		if(::_wstat(wStr,&fileStat)==0)
		{
			if(S_IFDIR & fileStat.st_mode)
			{
				tint cDistance = 0x40000000;
				HANDLE h;
				WIN32_FIND_DATAW fData;
				QStringList picNameList;
				QStringList::iterator ppI;
				
				::memset(&fData,0,sizeof(WIN32_FIND_DATAW));
				qdName = dName + "\\*";
				wStr = reinterpret_cast<LPCWSTR>(qdName.utf16());
				h = ::FindFirstFileW(wStr,&fData);
				if(h!=INVALID_HANDLE_VALUE)
				{
					do
					{
						QString cName(QString::fromUtf16(reinterpret_cast<const char16_t *>(fData.cFileName)));
						
						if(cName!="." && cName!="..")
						{
							QString fName = dName + "\\" + cName;
							wStr = reinterpret_cast<LPCWSTR>(fName.utf16());
							if(::_wstat(wStr,&fileStat)==0)
							{
								if((S_IFREG & fileStat.st_mode) && fileStat.st_size<250000)
								{
									QString ext = engine::Codec::getFileExtension(fName);
									
									if(ext=="jpeg" || ext=="jpg" || ext=="gif" || ext=="png")
									{
										picNameList.append(fName);
									}
								}
							}
						}
					} while(::FindNextFileW(h,&fData));
					::FindClose(h);
					
					for(ppI=picNameList.begin();ppI!=picNameList.end();ppI++)
					{
						tint nDist;
						const QString& fName = *ppI;
						QFileInfo fInfo(fName);
						QString bName = fInfo.baseName();
						
						nDist = levenshteinDistance(bName,albumName);
						if(nDist < cDistance)
						{
							imageName = fName;
							cDistance = nDist;
						}
					}
				}
			}
		}
	}
	return imageName;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

QString TrackDB::loadDirectoryImage(const QString& dirName,const QString& albumName) const
{
	QString dName,imageName;
	
	if(!dirName.isEmpty())
	{
		struct stat fileStat;
		
		dName = formatDirectoryPath(dirName);

		if(::stat(dName.toUtf8().constData(),&fileStat)==0)
		{
			if(S_IFDIR & fileStat.st_mode)
			{
				tint cDistance = 0x40000000;
				DIR *h;
				struct dirent *entry;
				QStringList picNameList;
				QStringList::iterator ppI;

#if defined(OMEGA_MAC_STORE)
				bool checkOutFlag;
				common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
				if(sbBookmark->has(dName,true))
				{
					checkOutFlag = sbBookmark->checkOut(dName,true);
				}
				else
				{
					checkOutFlag = false;
				}
#endif
				
				h = ::opendir(dName.toUtf8().constData());
				if(h!=0)
				{
					while(entry=::readdir(h),entry!=0)
					{
						QString cName(QString::fromUtf8(entry->d_name));
						
						if(cName!="." && cName!="..")
						{
							QString fName = dName + "/" + cName;
							
							if(::stat(fName.toUtf8().constData(),&fileStat)==0)
							{
								if((S_IFREG & fileStat.st_mode) && fileStat.st_size<250000)
								{
									QString ext = engine::Codec::getFileExtension(fName);
									
									if(ext=="jpeg" || ext=="jpg" || ext=="gif" || ext=="png")
									{
										picNameList.append(fName);
									}	
								}
							}
						}
					}
					::closedir(h);

					for(ppI=picNameList.begin();ppI!=picNameList.end();ppI++)
					{
						tint nDist;
						const QString& fName = *ppI;
						QFileInfo fInfo(fName);
						QString bName = fInfo.baseName();
						
						nDist = levenshteinDistance(bName,albumName);
						if(nDist < cDistance)
						{
							imageName = fName;
							cDistance = nDist;
						}
					}
				}
					
#if defined(OMEGA_MAC_STORE)					
				if(checkOutFlag)
				{
					sbBookmark->checkIn(dName,true);
				}	
#endif
			}
		}
	}
	return imageName;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QMap<tint, QString> TrackDB::playlists()
{
	SQLiteQuery playlistQ(m_db);
	QMap<tint, QString> plMap;
	
	try
	{
		tint id;
		QString name, cmdQ;
		
		cmdQ = "SELECT playListID, name FROM playlistInfo ORDER BY playListID";
		playlistQ.prepare(cmdQ);
		playlistQ.bind(id);
		playlistQ.bind(name);
		while(playlistQ.next())
		{
			name = dbStringInv(name);
			plMap.insert(id, name);
		}
	}
	catch(const SQLiteException& e)
	{
		printError("playlists",e.error().toUtf8().constData());
		plMap.clear();
	}
	return plMap;
}

//-------------------------------------------------------------------------------------------

QString TrackDB::playlist(int playlistID)
{
	QString name;
	SQLiteQuery playlistQ(m_db);
	QMap<tint, QString> plMap;
	
	try
	{
		QString name, cmdQ;
		
		cmdQ = "SELECT name FROM playlistInfo WHERE playListID=" + QString::number(playlistID);
		playlistQ.prepare(cmdQ);
		playlistQ.bind(name);
		if(playlistQ.next())
		{
			name = dbStringInv(name);
		}
		else
		{
			name = QString();
		}
	}
	catch(const SQLiteException& e)
	{
		printError("playlist",e.error().toUtf8().constData());
		name = QString();
	}
	return name;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::loadPlaylist(int playlistID, QVector<PlaylistTuple>& pList)
{
	bool res = true;
	
	pList.clear();
	try
	{
		int position, albumID, trackID, subtrackID;
		tuint64 itemID;
		QVector<PlaylistTuple> trackDBList;
		SQLiteQuery playQ(m_db);
		QString cmdQ;
		
		cmdQ = "SELECT position, albumID, trackID, subtrackID, itemID FROM playlist WHERE playlistID=" + QString::number(playlistID) + " ORDER BY position";
		playQ.prepare(cmdQ);
		playQ.bind(position);
		playQ.bind(albumID);
		playQ.bind(trackID);
		playQ.bind(subtrackID);
		playQ.bind(itemID);
		while(playQ.next())
		{
			PlaylistTuple t;
			t.albumID = albumID;
			t.trackID = trackID;
			t.subtrackID = subtrackID;
			t.itemID = itemID;
			pList.append(t);
		}
	}
	catch(const SQLiteException& e)
	{
		printError("loadPlaylist", e.error().toUtf8().constData());
		res = false;
	}
	return res;	
}

//-------------------------------------------------------------------------------------------

bool TrackDB::loadPlaylist(int playlistID, QVector<QPair<info::InfoSPtr, tint> >& pList)
{
	QVector<PlaylistTuple> trackDBList;
	bool res = false;
	
	pList.clear();
	res = loadPlaylist(playlistID, trackDBList);
	if(res)
	{
		for(QVector<PlaylistTuple>::iterator ppI=trackDBList.begin();ppI!=trackDBList.end();ppI++)
		{
			PlaylistTuple& t = *ppI;
			info::InfoSPtr info = DBInfo::readInfo(t.albumID, t.trackID);
			pList.append(QPair<track::info::InfoSPtr,int>(info, t.subtrackID));
		}	
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void TrackDB::clearPlaylistOp(int playlistID)
{
	QString cmdD;
	
	cmdD = "DELETE FROM playlistInfo WHERE playlistID=" + QString::number(playlistID);
	m_db->exec(cmdD);
	cmdD = "DELETE FROM playlist WHERE playlistID=" + QString::number(playlistID);
	m_db->exec(cmdD);
}

//-------------------------------------------------------------------------------------------

void TrackDB::clearPlaylist(int playlistID)
{
	try
	{
		m_db->exec("SAVEPOINT clearPlaylist");
		
		clearPlaylistOp(playlistID);
		
		m_db->exec("RELEASE clearPlaylist");
	}
	catch(const SQLiteException&)
	{
		m_db->exec("ROLLBACK TO SAVEPOINT clearPlaylist");
	}
}

//-------------------------------------------------------------------------------------------

void TrackDB::getDBInfoListFromPlaylist(QVector<QPair<info::InfoSPtr, tint> >& pList, QVector<QPair<DBInfoSPtr, tint> >& pDBList)
{
	pDBList.clear();
	
	for(QVector<QPair<info::InfoSPtr, tint> >::iterator ppI = pList.begin(); ppI != pList.end(); ppI++)
	{
		const QPair<info::InfoSPtr, tint>& item = *ppI;
		DBInfoSPtr dbInfo = item.first.dynamicCast<track::db::DBInfo>();
		if(dbInfo.isNull())
		{
			QPair<tint,tint> atID;
			
			if(!getTrackKey(item.first->getFilename(), atID))
			{
				if(!add(item.first.data()))
				{
					continue;
				}
			}
			dbInfo = DBInfo::readInfo(item.first->getFilename()).dynamicCast<DBInfo>();
			if(!dbInfo.isNull())
			{
				pDBList.append(QPair<DBInfoSPtr, tint>(dbInfo, item.second));
			}
		}
		else
		{
			pDBList.append(QPair<DBInfoSPtr, tint>(dbInfo, item.second));
		}
	}
}

//-------------------------------------------------------------------------------------------

int TrackDB::savePlaylistOp(int playlistID, const QString& name, const QVector<PlaylistTuple>& pList)
{
	int i, maxPlaylistID;
	QString cmdI, cmdQ, playlistName;
	SQLiteQuery playlistQ(m_db);
	SQLiteInsert playI(m_db), infoI(m_db);
	QVector<PlaylistTuple>::const_iterator ppI;
	
	try
	{
		m_db->exec("SAVEPOINT savePlaylist");
	
		if(playlistID >= 0)
		{
			clearPlaylistOp(playlistID);
		}
		else
		{
			cmdQ = "SELECT MAX(playlistID) FROM playlistInfo";
			playlistQ.prepare(cmdQ);
			playlistQ.bind(maxPlaylistID);
			if(playlistQ.next())
			{
				playlistID = maxPlaylistID;
			}
			else
			{
				playlistID = 0;
			}
		}

		cmdI = "INSERT INTO playlistInfo VALUES (?,?)";
		playlistName = dbString(name);
		infoI.prepare(cmdI);
		infoI.bind(playlistID);
		infoI.bind(playlistName);
		if(infoI.next())
		{
			cmdI = "INSERT INTO playlist VALUES (?,?,?,?,?,?)";
			playI.prepare(cmdI);
			for(i=0, ppI=pList.constBegin(); ppI != pList.constEnd() && playlistID >= 0; ppI++, i++)
			{
				PlaylistTuple item = *ppI;
				
				if(item.itemID == 0)
				{
					item.itemID = newPlaylistItemID();
				}
				playI.bind(playlistID);
				playI.bind(i);
				playI.bind(item.albumID);
				playI.bind(item.trackID);
				playI.bind(item.subtrackID);
				playI.bind(item.itemID);
				if(!playI.next())
				{
					printError("savePlaylistOp", "Error insert into playlist table");
					playlistID = -1;
				}
			}
		}
		else
		{
			printError("savePlaylistOp", "Error insert into playlistInfo table");
			playlistID = -1;
		}
		
		if(playlistID >= 0)
		{
			m_db->exec("RELEASE savePlaylist");
		}
		else
		{
			m_db->exec("ROLLBACK TO SAVEPOINT savePlaylist");
		}
	}
	catch(const SQLiteException& e)
	{
		printError("replacePlaylist", e.error().toUtf8().constData());
		m_db->exec("ROLLBACK TO SAVEPOINT savePlaylist");
		playlistID = -1;
	}
	return playlistID;
}

//-------------------------------------------------------------------------------------------

int TrackDB::savePlaylistOp(int playlistID, const QString& name, const QVector<QPair<DBInfoSPtr, tint> >& pList)
{
	QVector<QPair<DBInfoSPtr, tint> >::const_iterator ppI;
	QVector<PlaylistTuple> tupleList;
	
	for(ppI = pList.begin(); ppI != pList.end(); ppI++)
	{
		PlaylistTuple t;
		const QPair<DBInfoSPtr, tint>& item = *ppI;
		t.albumID = item.first->albumID();
		t.trackID = item.first->trackID();
		t.subtrackID = item.second;
		t.itemID = 0;
		tupleList.append(t);
	}
	return savePlaylistOp(playlistID, name, tupleList);
}

//-------------------------------------------------------------------------------------------

int TrackDB::replacePlaylist(int playlistID, const QString& name, QVector<QPair<info::InfoSPtr, tint> >& pList)
{
	QVector<QPair<DBInfoSPtr, tint> > pDBList;
	getDBInfoListFromPlaylist(pList, pDBList);
	return savePlaylistOp(playlistID, name, pDBList);
}

//-------------------------------------------------------------------------------------------

int TrackDB::replacePlaylist(int playlistID, const QString& name, const QVector<PlaylistTuple>& pList)
{
	return savePlaylistOp(playlistID, name, pList);
}

//-------------------------------------------------------------------------------------------

int TrackDB::savePlaylist(const QString& name, QVector<QPair<info::InfoSPtr, tint> >& pList)
{	
	return replacePlaylist(-1, name, pList);
}

//-------------------------------------------------------------------------------------------

int TrackDB::savePlaylist(const QString& name, QVector<PlaylistTuple>& pList)
{	
	return replacePlaylist(-1, name, pList);
}

//-------------------------------------------------------------------------------------------

tuint64 TrackDB::newPlaylistItemID()
{
	return newPlaylistItemID(m_db);
}

//-------------------------------------------------------------------------------------------

tuint64 TrackDB::newPlaylistItemID(SQLiteDatabase *db)
{
	tuint64 a, itemID;
	QString cmd;
	
	while(true)
	{
		itemID = common::Random::instance()->randomUInt64();
		if(itemID)
		{
			SQLiteQuery idQ(db);
			cmd = QString("SELECT itemID FROM playlist WHERE itemID=%1").arg(itemID);
			idQ.prepare(cmd);
			idQ.bind(a);
			if(!idQ.next())
			{
				break;
			}
		}
	}
	return itemID;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::isPlaylist(int playlistID)
{
	int plID;
	SQLiteQuery playlistQ(m_db);
	QString cmdQ = QString("SELECT playListID, name FROM playlistInfo WHERE playListID=%1").arg(playlistID);
	playlistQ.prepare(cmdQ);
	playlistQ.bind(plID);
	return (playlistQ.next()) ? true : false;
}

//-------------------------------------------------------------------------------------------

int TrackDB::refIDOfAudioDevice(const QString& deviceID)
{
	int refID = -1;
	SQLiteQuery devQ(m_db);
	QString cmdQ = QString("SELECT referenceID FROM audiodevice WHERE deviceID='%1'").arg(dbString(deviceID));
	devQ.prepare(cmdQ);
	devQ.bind(refID);
	if(!devQ.next())
	{
		refID = -1;
	}
	return refID;
}

//-------------------------------------------------------------------------------------------

void TrackDB::removeAudioDevice(const audioio::AOQueryDevice::Device& dev)
{
	int refID = refIDOfAudioDevice(dev.idConst());
	if(refID >= 0)
	{
		QString cmd;
		
		cmd = QString("DELETE FROM audiodevice WHERE referenceID=%1").arg(refID);
		m_db->exec(cmd);
		cmd = QString("DELETE FROM audiofrequency WHERE referenceID=%1").arg(refID);
		m_db->exec(cmd);
		cmd = QString("DELETE FROM audiochannel WHERE referenceID=%1").arg(refID);
		m_db->exec(cmd);
	}
}

//-------------------------------------------------------------------------------------------

bool TrackDB::insertAudioDevice(const audioio::AOQueryDevice::Device& dev)
{
	QString cmdI;
	bool res = true;
	
	QString deviceID = dbString(dev.idConst());
	QString deviceName = dbString(dev.name());
	SQLiteInsert devI(m_db);
	cmdI = "INSERT INTO audiodevice (deviceID, deviceName) VALUES (?,?)";
	devI.prepare(cmdI);
	devI.bind(deviceID);
	devI.bind(deviceName);
	if(devI.next())
	{
		int refID = idOfInserted();
		const QSet<int>& freqs = dev.frequencies();
		for(QSet<int>::const_iterator ppI = freqs.begin(); ppI != freqs.end() && res; ppI++)
		{	
			int freq = *ppI;
			SQLiteInsert freqI(m_db);
			cmdI = "INSERT INTO audiofrequency (referenceID, frequency) VALUES (?,?)";
			freqI.prepare(cmdI);
			freqI.bind(refID);
			freqI.bind(freq);
			if(!freqI.next())
			{
				printError("insertAudioDevice", "Failed to insert audio device frequency record");
				res = false;
			}
		}
	}
	else
	{
		printError("insertAudioDevice", "Failed to insert audio device record");
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::saveAudioDevice(const audioio::AOQueryDevice::Device& dev)
{
	bool res = false;
	
	try
	{
		m_db->exec("SAVEPOINT saveAudioDevice");
		
		removeAudioDevice(dev);
		res = insertAudioDevice(dev);
		
		if(res)
		{
			m_db->exec("RELEASE saveAudioDevice");
		}
		else
		{
			m_db->exec("ROLLBACK TO SAVEPOINT saveAudioDevice");
		}
	}
	catch(const SQLiteException& e)
	{
		printError("saveAudioDevice", e.error().toUtf8().constData());
		m_db->exec("ROLLBACK TO SAVEPOINT saveAudioDevice");
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDB::restoreAudioDevice(const QString& deviceID, audioio::AOQueryDevice::Device& dev)
{
	bool res = false;
	int refID = refIDOfAudioDevice(deviceID);
	if(refID >= 0)
	{
		dev.clear();
		
		QString deviceID, deviceName, cmdQ;
		SQLiteQuery devQ(m_db);
		cmdQ = QString("SELECT deviceID, deviceName FROM audiodevice WHERE referenceID=%1").arg(refID);
		devQ.prepare(cmdQ);
		devQ.bind(deviceID);
		devQ.bind(deviceName);
		if(devQ.next())
		{
			dev.id() = dbStringInv(deviceID);
			dev.name() = dbStringInv(deviceName);
			
			int freq;
			SQLiteQuery freqQ(m_db);
			cmdQ = QString("SELECT frequency FROM audiofrequency WHERE referenceID=%1").arg(refID);
			freqQ.prepare(cmdQ);
			freqQ.bind(freq);
			while(freqQ.next())
			{
				dev.addFrequency(freq);
			}
			
			int chIndex;
			QStringList chNames;
			QString chName;
			SQLiteQuery chQ(m_db);
			cmdQ = QString("SELECT channelIndex, name FROM audiochannel WHERE referenceID=%1 ORDER BY channelIndex").arg(refID);
			chQ.prepare(cmdQ);
			chQ.bind(chIndex);
			chQ.bind(chName);
			while(chQ.next())
			{
				chName = dbStringInv(chName);
				chNames.append(chName);
			}
			dev.setNoChannels(chNames.size());
			
			res = true;
		}
		else
		{
			printError("restoreAudioDevice", "Failed to find expected audio device record");
		}
	}
	return res;
}
//-------------------------------------------------------------------------------------------

void TrackDB::clearAudioDevices()
{
	QString cmd;
	
	try
	{
		m_db->exec("SAVEPOINT clearAudioDevices");

		cmd = QString("DELETE FROM audiodevice");
		m_db->exec(cmd);
		cmd = QString("DELETE FROM audiofrequency");
		m_db->exec(cmd);
		cmd = QString("DELETE FROM audiochannel");
		m_db->exec(cmd);	

        m_db->exec("RELEASE clearAudioDevices");
	}
	catch(const SQLiteException& e)
	{
		printError("clearAudioDevices", e.error().toUtf8().constData());
		m_db->exec("ROLLBACK TO SAVEPOINT clearAudioDevices");
	}
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
