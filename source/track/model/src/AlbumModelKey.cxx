#include "track/model/inc/AlbumModelKey.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

AlbumModelKey::AlbumModelKey() : m_group(false),
    m_id(-1)
{}

//-------------------------------------------------------------------------------------------

AlbumModelKey::AlbumModelKey(const std::pair<bool,int>& albumID) : m_group(albumID.first),
    m_id(albumID.second)
{}

//-------------------------------------------------------------------------------------------

AlbumModelKey::AlbumModelKey(const AlbumModelKey& key) : m_group(key.m_group),
    m_id(key.m_id)
{}

//-------------------------------------------------------------------------------------------

AlbumModelKey::AlbumModelKey(const QVariant& v) : m_group(false),
    m_id(-1)
{
    copy(v);
}

//-------------------------------------------------------------------------------------------

const AlbumModelKey& AlbumModelKey::operator = (const QVariant& v)
{
    copy(v);
    return *this;
}

//-------------------------------------------------------------------------------------------

const AlbumModelKey& AlbumModelKey::operator = (const AlbumModelKey& key)
{
    if(this!=&key)
    {
        m_group = key.m_group;
        m_id = key.m_id;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void AlbumModelKey::copy(const QVariant& v)
{
    bool res = false;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if(v.canConvert(QMetaType::QVariantList))
#else
    if(v.canConvert(QMetaType(QMetaType::QVariantList)))
#endif
    {
        QList<QVariant> vList = v.toList();
        if(vList.size()==2)
        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            if(vList.at(0).canConvert(QMetaType::Bool) && vList.at(1).canConvert(QMetaType::Int))
#else
            if(vList.at(0).canConvert(QMetaType(QMetaType::Bool)) && vList.at(1).canConvert(QMetaType(QMetaType::Int)))
#endif
            {
                m_group = vList.at(0).toBool();
                m_id = vList.at(1).toInt();
                res = true;
            }
        }
    }
    if(!res)
    {
        m_group = false;
        m_id = -1;
    }
}

//-------------------------------------------------------------------------------------------

QVariant AlbumModelKey::variant() const
{
    QList<QVariant> vList;
    vList.append(QVariant(m_group));
    vList.append(QVariant(m_id));
    QVariant v(vList);
    return v;
}

//-------------------------------------------------------------------------------------------

void AlbumModelKey::setAsGroup()
{
    m_group = true;
}

//-------------------------------------------------------------------------------------------

void AlbumModelKey::setAsAlbum()
{
    m_group = false;
}

//-------------------------------------------------------------------------------------------

bool AlbumModelKey::compare(const AlbumModelKey& b) const
{
    return (m_group==b.m_group && m_id==b.m_id) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const AlbumModelKey& a,const AlbumModelKey& b)
{
    return a.compare(b);
}

//-------------------------------------------------------------------------------------------

bool operator != (const AlbumModelKey& a,const AlbumModelKey& b)
{
    return (!a.compare(b)) ? true : false;
}

//-------------------------------------------------------------------------------------------

tint AlbumModelKey::groupIDFromDBInfo(QSharedPointer<db::DBInfo>& pDBInfo)
{
    tint groupID;
    QString cmd = QString("SELECT groupID FROM album WHERE albumID=%1").arg(pDBInfo->albumID());
    db::SQLiteQuery query(db::TrackDB::instance()->db());    
    query.prepare(cmd);
    query.bind(groupID);
    if(!query.next())
    {
        groupID = -1;
    }
    return groupID;
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModelKey::keyForDBInfo(QSharedPointer<db::DBInfo>& pDBInfo)
{
    AlbumModelKey key;
    
    if(!pDBInfo.isNull())
    {
        tint groupID = groupIDFromDBInfo(pDBInfo);
        if(groupID >= 0)
        {
            AlbumModelKey k(std::pair<bool,int>(true, groupID));
            key = k;
        }
        else
        {
            AlbumModelKey k(std::pair<bool,int>(false, pDBInfo->albumID()));
            key = k;
        }
    }
    return key;
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
