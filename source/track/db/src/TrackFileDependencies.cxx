#include "track/db/inc/TrackFileDependencies.h"
#include "common/inc/DiskOps.h"
#include "common/inc/DiskLayerIF.h"
#include "engine/inc/Codec.h"
#include "common/inc/JaroWinklerDistance.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

TrackFileDependencies::TrackFileDependencies() : m_files(),
    m_dirCacheMap()
{}

//-------------------------------------------------------------------------------------------

TrackFileDependencies::~TrackFileDependencies()
{}

//-------------------------------------------------------------------------------------------

QString TrackFileDependencies::getFileExtension(const QString& fileName) const
{
    return engine::Codec::getFileExtension(fileName).trimmed();
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependencies::scanAndCacheDirectory(const QString& dirName)
{
    QMap<QString,QMap<FileType,QSet<QString> > >::iterator ppI;
    bool res;

    ppI = dirCacheMap().find(dirName);
    if(ppI==dirCacheMap().end())
    {
        common::DiskIFSPtr pDiskIF = common::DiskIF::instance();

        if(pDiskIF->isDirectory(dirName))
        {
            common::DiskIF::DirHandle h;

            h = pDiskIF->openDirectory(dirName);
            if(pDiskIF->isValidDirectory(h))
            {
                QString name;

                while(name=pDiskIF->nextDirectoryEntry(h),!name.isEmpty())
                {
                    QString fullName = common::DiskOps::mergeName(dirName,name);
                    if(pDiskIF->isFile(fullName))
                    {
                        cacheFileBasedOnExtension(dirName,name);
                    }
                }
                pDiskIF->closeDirectory(h);

                ppI = dirCacheMap().find(dirName);
                if(ppI==dirCacheMap().end())
                {
                    QMap<FileType,QSet<QString> > fileMap;
                    dirCacheMap().insert(dirName,fileMap);
                }

                res = true;
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

TrackFileDependencies::FileType TrackFileDependencies::getTypeForFile(const QString& fileName) const
{
    FileType type;
    QString ext = getFileExtension(fileName);

    if(ext=="jpeg" || ext=="jpg" || ext=="gif" || ext=="png")
    {
        type = e_Image;
    }
    else if(ext=="cue")
    {
        type = e_CueSheet;
    }
    else if(ext=="wvc")
    {
        type = e_WavPackCorrection;
    }
    else
    {
        type = e_NoDependency;
    }
    return type;
}

//-------------------------------------------------------------------------------------------

QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >& TrackFileDependencies::dirCacheMap()
{
    return m_dirCacheMap;
}

//-------------------------------------------------------------------------------------------

const QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >& TrackFileDependencies::dirCacheMapConst() const
{
    return m_dirCacheMap;
}

//-------------------------------------------------------------------------------------------

void TrackFileDependencies::cacheFileBasedOnExtension(const QString& dirName,const QString& fileName)
{
    FileType type = getTypeForFile(fileName);

    if(type!=e_NoDependency)
    {
        QMap<QString,QMap<FileType,QSet<QString> > >::iterator ppI;

        ppI = dirCacheMap().find(dirName);
        if(ppI==dirCacheMap().end())
        {
            QMap<FileType,QSet<QString> > newTypeMap;
            dirCacheMap().insert(dirName,newTypeMap);
            ppI = dirCacheMap().find(dirName);
        }
        QMap<FileType,QSet<QString> >& typeMap = ppI.value();
        QMap<FileType,QSet<QString> >::iterator ppJ;

        ppJ = typeMap.find(type);
        if(ppJ==typeMap.end())
        {
            QSet<QString> newFileSet;
            typeMap.insert(type,newFileSet);
            ppJ = typeMap.find(type);
        }
        QSet<QString>& fileSet = ppJ.value();
        QSet<QString>::iterator ppK;

        ppK = fileSet.find(fileName);
        if(ppK==fileSet.end())
        {
            fileSet.insert(fileName);
        }
    }
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependencies::getDirectoryNameFromFile(const QString& fileName) const
{
    int i;
    QString dirName;

    for(i=fileName.size()-1;i>=0 && dirName.isEmpty();i--)
    {
        if(fileName.at(i)==QChar('/') || fileName.at(i)==QChar('\\'))
        {
            dirName = fileName.mid(0,i);
        }
    }
    return dirName;
}

//-------------------------------------------------------------------------------------------

QSet<QString>& TrackFileDependencies::files()
{
    return m_files;
}

//-------------------------------------------------------------------------------------------

const QSet<QString>& TrackFileDependencies::filesConst() const
{
    return m_files;
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependencies::getFileNameWithExtension(const QString& fileName,const QString& ext) const
{
    int i;
    QString baseName,fullName;

    for(i=fileName.length()-1;i>=0 && baseName.isEmpty();i--)
    {
        if(fileName.at(i)==QChar('.'))
        {
            baseName = fileName.mid(0,i);
        }
    }
    if(i<0)
    {
        baseName = fileName;
    }
    fullName = baseName + "." + ext;
    return fullName;
}

//-------------------------------------------------------------------------------------------

void TrackFileDependencies::cacheDependentFileIfExists(const QString& fileName,const QString& ext)
{
    common::DiskIFSPtr pDiskIF = common::DiskIF::instance();
    QString dName = getFileNameWithExtension(fileName,ext);

    if(pDiskIF->isFile(dName))
    {
        int i;

        for(i=dName.length()-1;i>=0;i--)
        {
            if(dName.at(i)==QChar('/') || dName.at(i)==QChar('\\'))
            {
                break;
            }
        }
        if(i>=0)
        {
            QString dirName = dName.mid(0,i);
            QString fName = dName.mid(i+1);

            if(!dirName.isEmpty() && !fName.isEmpty())
            {
                cacheFileBasedOnExtension(dirName,fName);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependencies::add(const QString& fileName)
{
    bool res;
    QSet<QString>::iterator ppI;

    ppI = files().find(fileName);
    if(ppI==files().end())
    {
        common::DiskIFSPtr pDiskIF = common::DiskIF::instance();

        if(pDiskIF->isFile(fileName))
        {
            QString dirName = getDirectoryNameFromFile(fileName);
            if(scanAndCacheDirectory(dirName))
            {
                res = true;
            }
            else
            {
                if(isDependencyForType(e_CueSheet,fileName))
                {
                    cacheDependentFileIfExists(fileName,"cue");
                }
                if(isDependencyForType(e_WavPackCorrection,fileName))
                {
                    cacheDependentFileIfExists(fileName,"wvc");
                }
            }
            files().insert(fileName);
            res = true;
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependencies::hasDependency(const QString& fileName) const
{
    return (dependency(fileName).size() > 0);
}

//-------------------------------------------------------------------------------------------

QSet<QString> TrackFileDependencies::dependency(const QString& fileName) const
{
    QSet<QString> fSet;
    QMap<QString,QMap<FileType,QSet<QString> > >::const_iterator ppI;
    QString dirName = getDirectoryNameFromFile(fileName);

    ppI = dirCacheMapConst().find(dirName);
    if(ppI!=dirCacheMapConst().end())
    {
        const QMap<FileType,QSet<QString> >& typeMap = ppI.value();

        for(QMap<FileType,QSet<QString> >::const_iterator ppJ=typeMap.begin();ppJ!=typeMap.end();++ppJ)
        {
            if(isDependencyForType(ppJ.key(),fileName))
            {
                QString dName;

                switch(ppJ.key())
                {
                    case e_Image:
                        dName = getDependentImage(fileName,ppJ.value());
                        break;

                    case e_CueSheet:
                    case e_WavPackCorrection:
                    default:
                        dName = getDependentOfName(fileName,ppJ.value());
                        break;
                }
                if(!dName.isEmpty())
                {
                    dName = common::DiskOps::mergeName(dirName,dName);
                    fSet.insert(dName);
                }
            }
        }
    }
    return fSet;
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependencies::getDependentImage(const QString& fileName,const QSet<QString>& fSet) const
{
    QString baseName,imageFileName;
    QFileInfo fInfo(fileName);
    tfloat64 maxDistance = -100.0;

    baseName = fInfo.baseName();

    for(QSet<QString>::const_iterator ppI=fSet.begin();ppI!=fSet.end();++ppI)
    {
        QFileInfo sInfo(*ppI);
        QString sBase = sInfo.baseName();
        tfloat64 nDistance = common::JaroWinklerDistance::distance(baseName,sBase);
        if(nDistance > maxDistance)
        {
            imageFileName = *ppI;
            maxDistance = nDistance;
        }
    }
    return imageFileName;
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependencies::getDependentOfName(const QString& fileName,const QSet<QString>& fSet) const
{
    QString dFileName;
    QFileInfo fInfo(fileName);
    QString baseName = fInfo.baseName();

    for(QSet<QString>::const_iterator ppI=fSet.begin();ppI!=fSet.end() && dFileName.isEmpty();++ppI)
    {
        QFileInfo sInfo(*ppI);
        QString sBase = sInfo.baseName();
        if(baseName.toLower().trimmed()==sBase.toLower().trimmed())
        {
            dFileName = *ppI;
        }
    }
    return dFileName;
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependencies::isDependencyForType(FileType type,const QString& fileName) const
{
    QString ext = getFileExtension(fileName);
    bool res;

    switch(type)
    {
        case e_Image:
            res = isDependencyForImage(ext);
            break;

        case e_CueSheet:
            res = isDependencyForCueSheet(ext);
            break;

        case e_WavPackCorrection:
            res = isDependencyForWavPackCorrection(ext);
            break;

        case e_NoDependency:
        default:
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependencies::isDependencyForImage(const QString& ext) const
{
    return (ext=="mp3" || ext=="m4a" || ext=="m4b" || ext=="ogg" || ext=="wav" || ext=="flac" || ext=="aif" || ext=="aiff" || ext=="mpc" || ext=="mp+" || ext=="mpp" || ext=="ape" || ext=="wv");
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependencies::isDependencyForCueSheet(const QString& ext) const
{
    return (ext=="wav" || ext=="flac" || ext=="aif" || ext=="aiff" || ext=="mpc" || ext=="mp+" || ext=="mpp" || ext=="ape" || ext=="wv");
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependencies::isDependencyForWavPackCorrection(const QString& ext) const
{
    return (ext=="wv");
}

//-------------------------------------------------------------------------------------------

QSet<QString> TrackFileDependencies::allDependencies() const
{
    QSet<QString> fSet;
    QSet<QString>::const_iterator ppI,ppJ,ppK;

    for(ppI=filesConst().begin();ppI!=filesConst().end();++ppI)
    {
        QSet<QString> fileSet = dependency(*ppI);
        for(ppJ=fileSet.begin();ppJ!=fileSet.end();++ppJ)
        {
            ppK = fSet.find(*ppJ);
            if(ppK==fSet.end())
            {
                fSet.insert(*ppJ);
            }
        }
    }
    return fSet;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
