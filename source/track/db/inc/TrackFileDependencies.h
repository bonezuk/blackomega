//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_TRACKFILEDEPENDENCIES_H
#define __OMEGA_TRACK_DB_TRACKFILEDEPENDENCIES_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "track/db/inc/Schema.h"

#include <QMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT TrackFileDependencies
{
    public:
    
        typedef enum
        {
            e_Image = 1,
            e_CueSheet,
            e_WavPackCorrection,
            e_NoDependency = 0
        } FileType;
    
        TrackFileDependencies();
        virtual ~TrackFileDependencies();
        
        virtual bool add(const QString& fileName);
        
        virtual bool hasDependency(const QString& fileName) const;
        virtual QSet<QString> dependency(const QString& fileName) const;
        
        virtual QSet<QString> allDependencies() const;
        
    protected:
        
        QSet<QString> m_files;
        // QMap<"directory name",QMap<"file type",QSet<"file name">>
        QMap<QString,QMap<FileType,QSet<QString> > > m_dirCacheMap;

        virtual QSet<QString>& files();
        virtual const QSet<QString>& filesConst() const;

        virtual QMap<QString,QMap<FileType,QSet<QString> > >& dirCacheMap();
        virtual const QMap<QString,QMap<FileType,QSet<QString> > >& dirCacheMapConst() const;
        
        virtual QString getFileExtension(const QString& fileName) const;
        virtual FileType getTypeForFile(const QString& fileName) const;
        virtual void cacheFileBasedOnExtension(const QString& dirName,const QString& fileName);
        virtual bool scanAndCacheDirectory(const QString& dirName);
        
        virtual QString getDirectoryNameFromFile(const QString& fileName) const;
        
        virtual bool isDependencyForType(FileType type,const QString& fileName) const;
        virtual bool isDependencyForImage(const QString& ext) const;
        virtual bool isDependencyForCueSheet(const QString& ext) const;
        virtual bool isDependencyForWavPackCorrection(const QString& ext) const;
        
        virtual QString getDependentImage(const QString& fileName,const QSet<QString>& fSet) const;
        virtual QString getDependentOfName(const QString& fileName,const QSet<QString>& fSet) const;
        
        virtual QString getFileNameWithExtension(const QString& fileName,const QString& ext) const;
        virtual void cacheDependentFileIfExists(const QString& fileName,const QString& ext);

};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

