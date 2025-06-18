//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_ALBUMMODELKEY_H
#define __OMEGA_TRACK_MODEL_ALBUMMODELKEY_H
//-------------------------------------------------------------------------------------------

#include "track/model/inc/TrackModelDLL.h"
#include "track/db/inc/DBInfo.h"

#include <QString>
#include <QVariant>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT AlbumModelKey
{
    public:
        AlbumModelKey();
        AlbumModelKey(const std::pair<bool,int>& albumID);
        AlbumModelKey(const AlbumModelKey& key);
        AlbumModelKey(const QVariant& v);
        
        const AlbumModelKey& operator = (const QVariant& v);
        const AlbumModelKey& operator = (const AlbumModelKey& key);
        
        bool isNull() const;
        
        bool isGroup() const;
        bool isAlbum() const;
        bool isAll() const;
        
        const int& id() const;
        
        void setAsGroup();
        void setAsAlbum();
        
        QVariant variant() const;
        
        static tint groupIDFromDBInfo(QSharedPointer<db::DBInfo>& pDBInfo);
        static AlbumModelKey keyForDBInfo(QSharedPointer<db::DBInfo>& pDBInfo);
        
        friend TRACK_MODEL_EXPORT bool operator == (const AlbumModelKey& a,const AlbumModelKey& b);
        friend TRACK_MODEL_EXPORT bool operator != (const AlbumModelKey& a,const AlbumModelKey& b);
        
    protected:
    
        bool m_group;
        int m_id;
        
        bool compare(const AlbumModelKey& b) const;
        void copy(const QVariant& v);
};


//-------------------------------------------------------------------------------------------

inline bool AlbumModelKey::isGroup() const
{
    return m_group;
}

//-------------------------------------------------------------------------------------------

inline bool AlbumModelKey::isAlbum() const
{
    return (m_group) ? false : true;
}

//-------------------------------------------------------------------------------------------

inline bool AlbumModelKey::isAll() const
{
    return (m_id==-1) ? true : false;
}

//-------------------------------------------------------------------------------------------

inline bool AlbumModelKey::isNull() const
{
    return (m_id==-1 && !m_group) ? true : false;
}

//-------------------------------------------------------------------------------------------

inline const int& AlbumModelKey::id() const
{
    return m_id;
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

