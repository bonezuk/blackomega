#include "track/info/inc/DirInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

QMap<QString,DirInfo> DirInfo::m_cacheMap;
DirInfo DirInfo::m_nullDir;
QFileInfo DirInfo::m_nullFile;

//-------------------------------------------------------------------------------------------

DirInfo::DirInfo() : m_name(),
    m_files()
{}

//-------------------------------------------------------------------------------------------

DirInfo::DirInfo(const QString& name) : m_name(name),
    m_files()
{}

//-------------------------------------------------------------------------------------------

DirInfo::DirInfo(const DirInfo& info) : m_name(),
    m_files()
{
    copy(info);
}

//-------------------------------------------------------------------------------------------

DirInfo& DirInfo::directory(const tchar *name)
{
    QString n(name);
    return directory(n);
}

//-------------------------------------------------------------------------------------------

DirInfo& DirInfo::directory(const QString& dName)
{
    QDir d(dName);
    QString name(d.canonicalPath());
    
    if(!name.isEmpty())
    {
        QMap<QString,DirInfo>::iterator ppI;
        
        ppI = m_cacheMap.find(name);
        if(ppI==m_cacheMap.end())
        {
            DirInfo info(name);
            
            if(info.read())
            {
                ppI = m_cacheMap.insert(name,info);
                return ppI.value();
            }
        }
        else
        {
            return ppI.value();
        }
    }
    return m_nullDir;
}

//-------------------------------------------------------------------------------------------

const DirInfo& DirInfo::operator = (const DirInfo& info)
{
    if(this!=&info)
    {
        copy(info);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void DirInfo::copy(const DirInfo& info)
{
    m_name = info.m_name;
    m_files = info.m_files;
}

//-------------------------------------------------------------------------------------------

bool DirInfo::read()
{
    QDir d(m_name);
    bool res;
    
    if(d.exists())
    {
        tint i;
        QFileInfoList infoList(d.entryInfoList());
        QFileInfoList::iterator ppI;
        Map::iterator ppJ;
        
        for(ppI=infoList.begin();ppI!=infoList.end();++ppI)
        {
            QFileInfo& info = *ppI;
            m_files.insert(info.completeBaseName(),QPair<QFileInfo,tint>(info,0));
        }
        for(i=0,ppJ=m_files.begin();ppJ!=m_files.end();++i,++ppJ)
        {
            ppJ.value().second = i;
        }
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool DirInfo::isNull() const
{
    return (m_name.isEmpty()) ? true : false;
}

//-------------------------------------------------------------------------------------------

const QString& DirInfo::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

const DirInfo::Map& DirInfo::list() const
{
    return m_files;
}

//-------------------------------------------------------------------------------------------

bool DirInfo::exist(const tchar *name) const
{
    QString n(name);
    return exist(n);
}

//-------------------------------------------------------------------------------------------

bool DirInfo::exist(const QString& name) const
{
    return (m_files.find(name)!=m_files.end()) ? true : false;
}

//-------------------------------------------------------------------------------------------

const QFileInfo& DirInfo::info(const tchar *name) const
{
    QString n(name);
    return info(n);
}

//-------------------------------------------------------------------------------------------

const QFileInfo& DirInfo::info(const QString& name) const
{
    Map::const_iterator ppI;
    
    ppI = m_files.find(name);
    if(ppI!=m_files.end())
    {
        return ppI.value().first;
    }
    else
    {
        return m_nullFile;
    }
}

//-------------------------------------------------------------------------------------------

tint DirInfo::index(const tchar *name) const
{
    QString n(name);
    return index(n);
}

//-------------------------------------------------------------------------------------------

tint DirInfo::index(const QString& name) const
{
    Map::const_iterator ppI;
    
    ppI = m_files.find(name);
    if(ppI!=m_files.end())
    {
        return ppI.value().second;
    }
    else
    {
        return -1;
    }    
}

//-------------------------------------------------------------------------------------------

void DirInfo::refresh()
{
    m_files.clear();
    if(!read())
    {
        m_name = "";
    }
}
        
//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
