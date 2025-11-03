//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_DBINFO_H
#define __OMEGA_TRACK_DB_DBINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"
#include "track/db/inc/TrackDB.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT DBInfo : public info::Info
{
    public:
        DBInfo(const QString& fileName);
        DBInfo(const QString& fileName, bool isUpdate);
        virtual ~DBInfo();
        
        static QSharedPointer<info::Info> readInfo(int albumID,int trackID);
        static QSharedPointer<info::Info> readInfo(const QString& fileName, bool isUpdate = true);
        
        tint albumID() const;
        tint trackID() const;
        
        virtual bool isValid() const;
        virtual bool isUpdateRequired() const;

        virtual bool isChildren() const;
        virtual tint noChildren() const;
        virtual const ChildInfo& child(tint idx) const;
        
        virtual bool isImage() const;
        virtual info::ImageInfoArray *getImageData(ImageFormat& format) const;
        virtual info::ImageInfoArray *getImageData(info::IDTagImageType type,info::Info::ImageFormat& format) const;
        virtual int getImageID() const;
        
        virtual QString directoryGroup() const;
        
    protected:
        
        tint m_albumID;
        tint m_trackID;
        bool m_isUpdate;
        bool m_updateFlag;
        QString m_directoryGroup;
        
        QList<ChildInfo> m_chapters;
        
        mutable bool m_imageFlag;
        mutable QMap<info::IDTagImageType,QPair<ImageFormat,info::ImageInfoArray *> > m_imageMap;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual bool loadRecord();
        virtual bool loadImages() const;
        
        tuint64 generateHashID(tint dirID, tint fileID);
        tuint64 loadOrGenerateHashID(tint dirID, tint fileID);
};

//-------------------------------------------------------------------------------------------

inline tint DBInfo::albumID() const
{
    return m_albumID;
}

//-------------------------------------------------------------------------------------------

inline tint DBInfo::trackID() const
{
    return m_trackID;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
