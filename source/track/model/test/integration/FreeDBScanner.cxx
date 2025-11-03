#include "network/inc/Resource.h"
#include "common/inc/DiskOps.h"
#include "track/info/inc/XMCDInfo.h"
#include "track/db/inc/TrackDB.h"
#include "common/inc/BIOBufferedStream.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

class DBDirectoryScanner
{
    public:
        DBDirectoryScanner();
        virtual ~DBDirectoryScanner();
        
        virtual tint64 countFiles(const QString& dirName) const;
        virtual void processDirectory(const QString& dirName,tint64& currentCount,tint64 totalCount) const;
        
    protected:
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void updateCountFilesProgress(tint64 count) const;
        virtual void updateProgress(tint64 current,tint64 total) const;
        
        virtual void processFile(const QString& fileName) const = 0;
};

//-------------------------------------------------------------------------------------------

class FreeDBScanner : public DBDirectoryScanner
{
    public:
        FreeDBScanner();
        virtual ~FreeDBScanner();
        
    protected:
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void processFile(const QString& fileName) const;
};

//-------------------------------------------------------------------------------------------

class MusicDBScanner : public DBDirectoryScanner
{
    public:
        MusicDBScanner();
        virtual ~MusicDBScanner();
        
    protected:    

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void processFile(const QString& fileName) const;
};

//-------------------------------------------------------------------------------------------
// DBDirectoryScanner
//-------------------------------------------------------------------------------------------

DBDirectoryScanner::DBDirectoryScanner()
{}

//-------------------------------------------------------------------------------------------

DBDirectoryScanner::~DBDirectoryScanner()
{}

//-------------------------------------------------------------------------------------------

void DBDirectoryScanner::printError(const tchar *strR,const tchar *strE) const
{
    fprintf(stdout,"DBDirectoryScanner::%s - %s.\n",strR,strE);
}

//-------------------------------------------------------------------------------------------

void DBDirectoryScanner::updateCountFilesProgress(tint64 count) const
{
    tuint32 t = static_cast<tuint32>(count & 0x00000000ffffffffULL);

    if(!(t & 0x0000007f))
    {
        QChar c;
    
        t = (t >> 7) & 0x00000003;
        switch(t)
        {
            case 0:
                c = QChar('/');
                break;
            case 1:
                c = QChar('-');
                break;
            case 2:
                c = QChar('\\');
                break;
            case 3:
            default:
                c = QChar('|');
                break;
        }
        QString line = QString::fromLatin1("Counting Files... ") + c;
        printf("%s\r",line.toLatin1().constData());
    }
}

//-------------------------------------------------------------------------------------------

void DBDirectoryScanner::updateProgress(tint64 current,tint64 total) const
{
    tfloat64 percent = (static_cast<tfloat64>(current) * 100.0) / static_cast<tfloat64>(total);
    printf("Processed (%u of %u) - %.1f%%\r",static_cast<tuint32>(current),static_cast<tuint32>(total),percent);
}

//-------------------------------------------------------------------------------------------

tint64 DBDirectoryScanner::countFiles(const QString& dirName) const
{
    tint64 count = 0;
    common::DiskIFSPtr pDisk = common::DiskIF::instance();
    common::DiskIF::DirHandle h = pDisk->openDirectory(dirName);
    
    if(h!=common::DiskIF::invalidDirectory())
    {
        QString name;
        QStringList dirNameList;
        
        while(name=pDisk->nextDirectoryEntry(h),!name.isEmpty())
        {
            QString fullName = common::DiskOps::mergeName(dirName,name);
            
            if(pDisk->isDirectory(fullName))
            {
                dirNameList.append(fullName);
            }
            else if(pDisk->isFile(fullName))
            {
                count++;
                updateCountFilesProgress(count);
            }
        }
        pDisk->closeDirectory(h);
        
        for(QStringList::iterator ppI=dirNameList.begin();ppI!=dirNameList.end();ppI++)
        {
            count += countFiles(*ppI);
        }
    }
    return count;
}

//-------------------------------------------------------------------------------------------

void DBDirectoryScanner::processDirectory(const QString& dirName,tint64& currentCount,tint64 totalCount) const
{
    common::DiskIFSPtr pDisk = common::DiskIF::instance();
    common::DiskIF::DirHandle h = pDisk->openDirectory(dirName);
    
    if(h!=common::DiskIF::invalidDirectory())
    {
        QString name;
        QStringList dirNameList;
        
        while(name=pDisk->nextDirectoryEntry(h),!name.isEmpty())
        {
            QString fullName = common::DiskOps::mergeName(dirName,name);
            
            if(pDisk->isDirectory(fullName))
            {
                dirNameList.append(fullName);
            }
            else if(pDisk->isFile(fullName))
            {
                processFile(fullName);
                currentCount++;
                updateProgress(currentCount,totalCount);
            }
        }
        pDisk->closeDirectory(h);
        
        for(QStringList::iterator ppI=dirNameList.begin();ppI!=dirNameList.end();ppI++)
        {
            processDirectory(*ppI,currentCount,totalCount);
        }
    }
    else
    {
        QString errorStr;
        errorStr = "Unable to scan directory \'" + dirName + "\'";
        printError("processDirectory",errorStr.toUtf8().constData());
    }
}

//-------------------------------------------------------------------------------------------
// FreeDBScanner
//-------------------------------------------------------------------------------------------

FreeDBScanner::FreeDBScanner() : DBDirectoryScanner()
{}

//-------------------------------------------------------------------------------------------

FreeDBScanner::~FreeDBScanner()
{}

//-------------------------------------------------------------------------------------------

void FreeDBScanner::printError(const tchar *strR,const tchar *strE) const
{
    fprintf(stdout,"FreeDBScanner::%s - %s.\n",strR,strE);
}

//-------------------------------------------------------------------------------------------

void FreeDBScanner::processFile(const QString& fileName) const
{
    common::BIOStream *iFile = new common::BIOStream(common::e_BIOStream_FileRead);
    
    if(iFile->open(fileName))
    {
        QVector<track::info::InfoSPtr> infoList = track::info::XMCDInfo::readXMCD(iFile);
        
        if(!infoList.isEmpty())
        {
            QVector<track::info::InfoSPtr>::iterator ppI;
            track::db::TrackDB *db = track::db::TrackDB::instance();
            
            for(ppI=infoList.begin();ppI!=infoList.end();ppI++)
            {
                track::info::InfoSPtr pInfo = *ppI;
                db->addInfo(pInfo.data());
            }
        }
        iFile->close();
    }
    delete iFile;
}

//-------------------------------------------------------------------------------------------
// MusicDBScanner
//-------------------------------------------------------------------------------------------

MusicDBScanner::MusicDBScanner() : DBDirectoryScanner()
{}

//-------------------------------------------------------------------------------------------

MusicDBScanner::~MusicDBScanner()
{}

//-------------------------------------------------------------------------------------------

void MusicDBScanner::printError(const tchar *strR,const tchar *strE) const
{
    fprintf(stdout,"MusicDBScanner::%s - %s.\n",strR,strE);
}

//-------------------------------------------------------------------------------------------

void MusicDBScanner::processFile(const QString& fileName) const
{
    if(track::info::Info::isSupported(fileName))
    {
        common::BIOBufferedStream *iFile = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
        if(iFile->open(fileName))
        {
            track::info::InfoSPtr pInfo = track::info::Info::readInfo(iFile);
            if(pInfo.data()!=0)
            {
                track::db::TrackDB *db = track::db::TrackDB::instance();
                db->add(pInfo.data());
            }
            iFile->close();
        }
        delete iFile;
    }
}

//-------------------------------------------------------------------------------------------

TEST(FreeDBScanner,run)
{
    common::DiskIFSPtr diskIF = common::DiskIF::instance("disk");

#if defined(OMEGA_WIN32)
    QString freeDBDirectory = "C:\\Temp\\FreeDB";
    QString trackDBFile = "C:\\Temp\\freedb.omega.db";
#else
    QString freeDBDirectory = "~/Temp/freedb";
    QString trackDBFile = "~/Temp/freedb.omega.db";
#endif

    if(common::DiskOps::exist(trackDBFile))
    {
        common::DiskOps::deleteFile(trackDBFile);
    }

    track::db::TrackDB *db = track::db::TrackDB::instance(trackDBFile);
    if(db!=0)
    {
        FreeDBScanner scanner;
    
        fprintf(stdout,"Generating TrackDB SQLite Omega music database file- %s\n",trackDBFile.toUtf8().constData());
        fprintf(stdout,"Scanning FreeDB directory- %s\n",freeDBDirectory.toUtf8().constData());
        
        tint64 currentFileCount = 0;
        tint64 totalFileCount = scanner.countFiles(freeDBDirectory);
        
        try
        {
            scanner.processDirectory(freeDBDirectory,currentFileCount,totalFileCount);
        }
        catch(const track::db::SQLiteException& e)
        {
            fprintf(stdout,"DB Errror. %s\n",e.error().toUtf8().constData());
            FAIL();
        }

        delete db;
    }
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(MusicDBScanner,run)
{
    common::DiskIFSPtr diskIF = common::DiskIF::instance("disk");

#if defined(OMEGA_WIN32)
    QString freeDBDirectory = "C:\\Music";
    QString trackDBFile = "C:\\Temp\\musicdb.omega.db";
#else
    QString freeDBDirectory = "~/Temp/freedb";
    QString trackDBFile = "~/Temp/freedb.omega.db";
#endif

    if(common::DiskOps::exist(trackDBFile))
    {
        common::DiskOps::deleteFile(trackDBFile);
    }

    track::db::TrackDB *db = track::db::TrackDB::instance(trackDBFile);
    if(db!=0)
    {
        MusicDBScanner scanner;
    
        fprintf(stdout,"Generating TrackDB SQLite Omega music database file- %s\n",trackDBFile.toUtf8().constData());
        fprintf(stdout,"Scanning music directory- %s\n",freeDBDirectory.toUtf8().constData());
        
        tint64 currentFileCount = 0;
        tint64 totalFileCount = scanner.countFiles(freeDBDirectory);
        
        try
        {
            scanner.processDirectory(freeDBDirectory,currentFileCount,totalFileCount);
        }
        catch(const track::db::SQLiteException& e)
        {
            fprintf(stdout,"DB Errror. %s\n",e.error().toUtf8().constData());
            FAIL();
        }

        delete db;
    }
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------
