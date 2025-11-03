#include "network/inc/Resource.h"
#include "track/db/inc/ITunesLocation.h"
#include "common/inc/QBIOStreamDevice.h"
#include "common/inc/DiskOps.h"
#include "common/inc/DiskIF.h"

#include <QSettings>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

#if defined(OMEGA_MACOSX)
#include "track/db/inc/ITunesLocationMac.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

ITunesLocation::ITunesLocation()
{}

//-------------------------------------------------------------------------------------------

ITunesLocation::~ITunesLocation()
{}

//-------------------------------------------------------------------------------------------

bool ITunesLocation::isValidPList(const QString& fileName) const
{
    int state = 0;
    
    common::BIOStream ioStream(common::e_BIOStream_FileRead);
    if(ioStream.open(fileName))
    {
        int count = 0;
        common::QBIOStreamDevice file(&ioStream);
        
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream textStream(&file);
        
            while(!textStream.atEnd() && count<10 && state<2)
            {
                QString line = textStream.readLine();
                if(!line.isEmpty())
                {
                    if(state==0 && isLineXMLHeader(line))
                    {
                        state = 1;
                    }
                    else if(state==1 && isLinePListHeader(line))
                    {
                        state = 2;
                    }
                }
                count++;
            }
        }
        
        ioStream.close();
    }
    return (state==2);
}

//-------------------------------------------------------------------------------------------

bool ITunesLocation::isLineXMLHeader(const QString& line) const
{
    QRegExp reg("^(\\s)?(<\\?)(\\s)?(xml)(.)*(\\?>)(\\s)?$",Qt::CaseInsensitive);
    return (reg.indexIn(line)==0);
}

//-------------------------------------------------------------------------------------------

bool ITunesLocation::isLinePListHeader(const QString& line) const
{
    QRegExp reg("^(\\s)?(<)(\\s)?(plist)(.)*(>)(\\s)?$",Qt::CaseInsensitive);
    return (reg.indexIn(line)==0);
}

//-------------------------------------------------------------------------------------------

QSet<int> ITunesLocation::getLocationIDSet()
{
    QSet<int> idSet;
    QSettings settings;
    
    settings.beginGroup("iTunesLocation");
    if(settings.contains("id"))
    {
        QStringList idList = settings.value("id").toStringList();
        for(QStringList::const_iterator ppI=idList.begin();ppI!=idList.end();ppI++)
        {
            int num;
            bool ok = true;
            const QString& pNum = *ppI;
            
            num = pNum.toInt(&ok);
            if(ok)
            {
                idSet.insert(num);
            }
        }
    }
    settings.endGroup();
    
    return idSet;
}

//-------------------------------------------------------------------------------------------

int ITunesLocation::getLocationNextID()
{
    int nID = 1;
    QStringList idList;
    QSettings settings;
    
    settings.beginGroup("iTunesLocation");
    if(settings.contains("id"))
    {
        bool loop = false;
        
        idList = settings.value("id").toStringList();
        for(QStringList::const_iterator ppI=idList.begin();ppI!=idList.end();ppI++)
        {
            int num;
            bool ok = true;
            const QString& pNum = *ppI;
            
            num = pNum.toInt(&ok);
            if(ok && num>=nID)
            {
                nID = num + 1;
            }
        }
        
        while(loop)
        {
            QString dirName = "location" + QString::number(nID);
            if(settings.contains(dirName))
            {
                nID++;
            }
            else
            {
                loop = false;
            }
        }
    }

    idList.append(QString::number(nID));
    QVariant vSList(idList);
    settings.setValue("id",vSList);
    settings.endGroup();

    return nID;
}

//-------------------------------------------------------------------------------------------

int ITunesLocation::addDirectoryLocation(const QString& pos)
{
    int nextID = 0;
    QString nextLocation;
    QSettings settings;
    
    if(!pos.isEmpty())
    {
        QString posName = "D:" + pos;
        nextID = getLocationNextID();
        nextLocation = "location" + QString::number(nextID);
        settings.beginGroup("iTunesLocation");
        settings.setValue(nextLocation,posName);
        settings.endGroup();
    }
    return nextID;
}

//-------------------------------------------------------------------------------------------

int ITunesLocation::addFileLocation(const QString& pos)
{
    int nextID = 0;
    QString nextLocation;
    QSettings settings;
    
    if(!pos.isEmpty())
    {
        QString posName = "F:" + pos;
        nextID = getLocationNextID();
        nextLocation = "location" + QString::number(nextID);
        settings.beginGroup("iTunesLocation");
        settings.setValue(nextLocation,posName);
        settings.endGroup();
    }
    return nextID;
}

//-------------------------------------------------------------------------------------------

bool ITunesLocation::updateFileLocation(int ID,const QString& pos)
{
    QSettings settings;
    bool res = false;
    
    settings.beginGroup("iTunesLocation");
    if(ID>0 && !pos.isEmpty())
    {
        QString locName = "location" + QString::number(ID);
        if(settings.contains(locName))
        {
            QString fName = "F:" + pos;
            settings.setValue(locName,fName);
            res = true;
        }
    }
    settings.endGroup();
    
    return res;
}

//-------------------------------------------------------------------------------------------

QString ITunesLocation::getDirectoryLocation(int ID)
{
    QSettings settings;
    QString lName;
    
    settings.beginGroup("iTunesLocation");
    if(ID>0)
    {
        QString locName = "location" + QString::number(ID);
        if(settings.contains(locName))
        {
            QString posName = settings.value(locName).toString();
            
            if(posName.left(2)=="D:")
            {
                lName = posName.mid(2);
            }
            else if(posName.left(2)=="F:")
            {
                posName = posName.mid(2);
                for(int i=posName.length()-1;i>=0;i--)
                {
                    if(posName.at(i)==QChar('/') || posName.at(i)==QChar('\\'))
                    {
                        posName = posName.mid(0,i);
                        break;
                    }
                }
                lName = posName;
            }
        }
    }
    settings.endGroup();
    return lName;
}

//-------------------------------------------------------------------------------------------

QString ITunesLocation::getFileLocation(int ID)
{
    QSettings settings;
    QString lName;
    
    settings.beginGroup("iTunesLocation");
    if(ID>0)
    {
        QString locName = "location" + QString::number(ID);
        if(settings.contains(locName))
        {
            QString posName = settings.value(locName).toString();
            
            if(posName.left(2)=="F:")
            {
                lName = posName.mid(2);
            }
        }
    }
    settings.endGroup();
    return lName;
}

//-------------------------------------------------------------------------------------------

void ITunesLocation::clearLocation(int ID)
{
    QSettings settings;
    QString lName;
    
    settings.beginGroup("iTunesLocation");
    if(ID>0)
    {
        QString locName = "location" + QString::number(ID);
        settings.remove(locName);
        
        QSet<int>::iterator ppI;
        QSet<int> idSet = getLocationIDSet();
        ppI = idSet.find(ID);
        if(ppI!=idSet.end())
        {
            QStringList idList;
            
            idSet.erase(ppI);
            for(ppI=idSet.begin();ppI!=idSet.end();ppI++)
            {
                idList.append(QString::number(*ppI));
            }
            
            QVariant vSList(idList);
            settings.setValue("id",vSList);
        }
    }
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

void ITunesLocation::clearLocations()
{
    QSettings settings;
    QSet<int>::iterator ppI;
    QSet<int> idSet = getLocationIDSet();
    
    settings.beginGroup("iTunesLocation");
    for(ppI=idSet.begin();ppI!=idSet.end();ppI++)
    {
        QString locName = "location" + QString::number(*ppI);
        settings.remove(locName);
    }
    settings.remove("id");
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

int ITunesLocation::sizeLocations()
{
    return getLocationIDSet().size();
}

//-------------------------------------------------------------------------------------------

bool ITunesLocation::isITunesXMLName(const QString& fileName) const
{
    int i;
    QString name;
    bool res = false;
    
    for(i=fileName.length()-1;i>=0 && name.isEmpty();i--)
    {
        if(fileName.at(i)==QChar('/') || fileName.at(i)==QChar('\\'))
        {
            name = fileName.mid(i+1);
        }
    }
    if(name.isEmpty())
    {
        name = fileName;
    }
    
    if(name.compare("iTunes Library.xml",Qt::CaseInsensitive)==0)
    {
        res = true;
    }
    else if(name.compare("iTunes Music Library.xml",Qt::CaseInsensitive)==0)
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32) || defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

QStringList ITunesLocation::defaultITunesDirectory()
{
    QString iTuneLib;
    QStringList iTuneList;

    QStringList hDirList;
#if QT_VERSION >= 0x050000
    hDirList = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
#endif
    if(hDirList.size()>0)
    {
        iTuneLib = hDirList.at(0);
    }

    if(!iTuneLib.isEmpty())
    {
        QString hDir;
        
        if(iTuneLib.at(iTuneLib.size()-1)!=QChar('/') && iTuneLib.at(iTuneLib.size()-1)!=QChar('\\'))
        {
            iTuneLib += "/";
        }
        hDir = iTuneLib + "iTunes";
        hDir = QDir::toNativeSeparators(hDir);

        iTuneList.append(hDir);
    }
    return iTuneList;
}

//-------------------------------------------------------------------------------------------
#else
#if !defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

QStringList ITunesLocation::defaultITunesDirectory()
{
    return ITunesLocationMac::lastITunesDirectory();
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

QStringList ITunesLocation::defaultITunesDirectory()
{
    QStringList iTuneList;
    QString hDir = ITunesLocationMac::IVLocationHomeDirectory();
    
    if(hDir.at(hDir.length()-1)!=QChar('/'))
    {
        hDir += "/";
    }
    hDir = hDir + "Music/iTunes";
    iTuneList.append(hDir);
    
    return iTuneList;
}

//-------------------------------------------------------------------------------------------
#endif
#endif
//-------------------------------------------------------------------------------------------

common::TimeStamp ITunesLocation::getModifiedFileTime(const QString& fileName)
{
    return common::DiskOps::getModifiedTime(fileName);
}

//-------------------------------------------------------------------------------------------

QString ITunesLocation::findITunesDBInDirectory(const QString& dirName)
{
    return findITunesDBInDirectory(dirName,0,0);
}

//-------------------------------------------------------------------------------------------

QString ITunesLocation::findITunesDBInDirectory(const QString& dirName,CancelScanFunction cancelFunc,void *pUserData)
{
    int progressCount = 0;
    return findITunesDBInDirectoryImpl(dirName,cancelFunc,pUserData,progressCount);
}

//-------------------------------------------------------------------------------------------

QString ITunesLocation::findITunesDBInDirectoryImpl(const QString& dirName,CancelScanFunction cancelFunc,void *pUserData,int& progressCount)
{
    QString dbFileName;
    common::DiskIFSPtr pDisk = common::DiskIF::instance();
    
    if(!pDisk.isNull() && pDisk->isDirectory(dirName))
    {
        bool res = true;
        common::DiskIF::DirHandle h = pDisk->openDirectory(dirName);
        
        if(h!=common::DiskIF::invalidDirectory())
        {
            QString name;
            QStringList dirList,nameList;
            
            while(name=pDisk->nextDirectoryEntry(h),!name.isEmpty() && res)
            {
                QString fullName = common::DiskOps::mergeName(dirName,name);
                
                if(pDisk->isFile(fullName))
                {
                    if(isITunesXMLName(fullName) && isValidPList(fullName))
                    {
                        nameList.append(fullName);
                    }
                }
                else if(pDisk->isDirectory(fullName))
                {
                    dirList.append(fullName);
                }
                
                if(cancelFunc!=0)
                {
                    progressCount++;
                    if((progressCount % 100)==0)
                    {
                        if(!cancelFunc(pUserData))
                        {
                            res = false;
                        }
                    }
                }
            }
            pDisk->closeDirectory(h);
            
            if(res)
            {
                if(nameList.size() > 0)
                {
                    common::TimeStamp uTime = 0;
                    
                    for(QStringList::const_iterator ppI=nameList.begin();ppI!=nameList.end();ppI++)
                    {
                        common::TimeStamp mTime = getModifiedFileTime(*ppI);
                        if(uTime==0 || mTime > uTime)
                        {
                            dbFileName = *ppI;
                        }
                    }
                }
                else
                {
                    for(QStringList::const_iterator ppI=dirList.begin();ppI!=dirList.end() && dbFileName.isEmpty();ppI++)
                    {
                        dbFileName = findITunesDBInDirectoryRecursive(*ppI,cancelFunc,pUserData,progressCount);
                        if(cancelFunc!=0)
                        {
                            if(!cancelFunc(pUserData))
                            {
                                dbFileName = "";
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return dbFileName;    
}

//-------------------------------------------------------------------------------------------

QString ITunesLocation::findITunesDBInDirectoryRecursive(const QString& dirName,CancelScanFunction cancelFunc,void *pUserData,int& progressCount)
{
    return findITunesDBInDirectoryImpl(dirName,cancelFunc,pUserData,progressCount);
}

//-------------------------------------------------------------------------------------------

void ITunesLocation::getFileAndDirectoryLocationsFromSettings(QSet<QString>& confDirSet,QSet<QString>& confFileSet)
{
    QSet<int>::iterator ppI;
    QSet<int> IDs = getLocationIDSet();
    
    confDirSet.clear();
    confFileSet.clear();
    
    for(ppI=IDs.begin();ppI!=IDs.end();ppI++)
    {
        int ID = *ppI;
        QString confName = getFileLocation(ID);
        if(confName.isEmpty())
        {
            confName = getDirectoryLocation(ID);
            if(!confName.isEmpty())
            {
                confDirSet.insert(confName);
            }
        }
        else
        {
            confFileSet.insert(confName);
        }
    }
}

//-------------------------------------------------------------------------------------------

QStringList ITunesLocation::validateDirectorySet(const QSet<QString>& dirSet)
{
    QString fileName;
    QStringList dbFileList;
    QSet<int> IDs;
    QSet<QString>::const_iterator ppI;
    QSet<int>::const_iterator ppJ;
    
    IDs = getLocationIDSet();
    
    for(ppI=dirSet.begin();ppI!=dirSet.end();ppI++)
    {
        const QString& dirName = *ppI;
        
        fileName = findITunesDBInDirectory(dirName);
        if(!fileName.isEmpty())
        {
            for(ppJ=IDs.begin();ppJ!=IDs.end();ppJ++)
            {
                int ID = *ppJ;
                QString confName = getDirectoryLocation(ID);
                if(confName==dirName && getFileLocation(ID).isEmpty())
                {
                    updateFileLocation(ID,fileName);
                }
            }
            dbFileList.append(fileName);
        }
    }
    return dbFileList;
}


//-------------------------------------------------------------------------------------------

QStringList ITunesLocation::validateFileSet(const QSet<QString>& fileSet)
{
    QStringList dbFileList;
    QSet<QString>::const_iterator ppI;
    
    for(ppI=fileSet.begin();ppI!=fileSet.end();ppI++)
    {
        const QString& fileName = *ppI;
        
        if(isITunesXMLName(fileName) && isValidPList(fileName))
        {
            dbFileList.append(fileName);
        }
    }
    return dbFileList;
}

//-------------------------------------------------------------------------------------------

QStringList ITunesLocation::getDefaultITuneDBs()
{
    QString fileName;
    QStringList dirList,dbFileList;
    QStringList::const_iterator ppI;
    
    dirList = defaultITunesDirectory();
    for(ppI=dirList.begin();ppI!=dirList.end();ppI++)
    {
        if(common::DiskIF::instance()->isDirectory(*ppI))
        {
            fileName = findITunesDBInDirectory(*ppI);
            if(!fileName.isEmpty())
            {
                addFileLocation(fileName);
                dbFileList.append(fileName);
            }
        }
        else
        {
            fileName = *ppI;
            if(isITunesXMLName(fileName) && isValidPList(fileName))
            {
                addFileLocation(fileName);
                dbFileList.append(fileName);
            }
        }
    }
    return dbFileList;
}

//-------------------------------------------------------------------------------------------

QStringList ITunesLocation::getITunesConfigFileNames()
{
    QStringList dbFileList;
    QSet<QString> confDirSet,confFileSet;
    
    getFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);
    dbFileList  = validateFileSet(confFileSet);
    dbFileList += validateDirectorySet(confDirSet);
    
    if(dbFileList.isEmpty())
    {
        dbFileList = getDefaultITuneDBs();
    }
    return dbFileList;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------

