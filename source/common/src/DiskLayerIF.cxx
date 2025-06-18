#include "common/inc/DiskLayerIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(DiskIFFactory,DiskIF, \
        DiskLayerIFFactory,DiskLayerIF, \
        "disk",false)

//-------------------------------------------------------------------------------------------

DiskLayerIF::DiskLayerIF() : DiskIF(),
    m_mutex(),
    m_dirHandleMap()
{}

//-------------------------------------------------------------------------------------------

DiskLayerIF::~DiskLayerIF()
{
#if defined(OMEGA_WIN32)
    QMap<DiskIF::DirHandle,QPair<QString,WIN32_FIND_DATAW> >::iterator ppI;
#elif defined(OMEGA_POSIX)
    QSet<DiskIF::DirHandle>::iterator ppI;
#endif

    m_mutex.lock();
    while(!m_dirHandleMap.isEmpty())
    {
        ppI = m_dirHandleMap.begin();
#if defined(OMEGA_WIN32)
        DiskLayerIF::closeDirectory(ppI.key());
#elif defined(OMEGA_POSIX)
        DiskLayerIF::closeDirectory(*ppI);
#endif
    }
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

QString DiskLayerIF::directoryName(const QString& name) const
{
    QString dName;
    
    if(!name.isEmpty())
    {
        dName = name;
        if(dName.at(dName.length()-1)==QChar('/') || dName.at(dName.length()-1)==QChar('\\'))
        {
            dName = dName.mid(0,dName.length()-1);
        }
        dName = common::DiskOps::toNativeSeparators(dName);
    }
    return dName;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool DiskLayerIF::isFile(const QString& name) const
{
    bool res = false;
    
    if(!name.isEmpty())
    {
        QString fName = common::DiskOps::toNativeSeparators(name);
        LPCWSTR wStr = reinterpret_cast<LPCWSTR>(fName.utf16());
        struct _stat fileStat;
        if(::_wstat(wStr,&fileStat)==0)
        {
            if(fileStat.st_mode & S_IFREG)
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool DiskLayerIF::isDirectory(const QString& name) const
{
    bool res = false;
    
    if(!name.isEmpty())
    {
        QString dName = directoryName(name);
        LPCWSTR wStr = reinterpret_cast<LPCWSTR>(dName.utf16());
        struct _stat fileStat;
        if(::_wstat(wStr,&fileStat)==0)
        {
            if(fileStat.st_mode & S_IFDIR)
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool DiskLayerIF::isFile(const QString& name) const
{
    bool res = false;
    
    if(!name.isEmpty())
    {
        QString fName = common::DiskOps::toNativeSeparators(name);
        struct stat fileStat;
        if(::stat(fName.toUtf8().constData(),&fileStat)==0)
        {
            if(fileStat.st_mode & S_IFREG)
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool DiskLayerIF::isDirectory(const QString& name) const
{
    bool res = false;
    
    if(!name.isEmpty())
    {
        QString dName = directoryName(name);
        struct stat fileStat;
        if(::stat(dName.toUtf8().constData(),&fileStat)==0)
        {
            if(fileStat.st_mode & S_IFDIR)
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

DiskIF::DirHandle DiskLayerIF::openDirectory(const QString& name) const
{
    DirHandle h = c_invalidDirectoryHandle;

    if(isDirectory(name))
    {
        WIN32_FIND_DATAW fData;
        QString dName = directoryName(name);
        QString qdName = dName + "\\*";
        LPCWSTR wStr = reinterpret_cast<LPCWSTR>(qdName.utf16());
        ::memset(&fData,0,sizeof(WIN32_FIND_DATAW));
        h = ::FindFirstFileW(wStr,&fData);
        if(h!=c_invalidDirectoryHandle)
        {
            m_mutex.lock();
            m_dirHandleMap.insert(h,QPair<QString,WIN32_FIND_DATAW>(dName,fData));
            m_mutex.unlock();
        }
    }
    return h;
}

//-------------------------------------------------------------------------------------------

QString DiskLayerIF::nextDirectoryEntry(DirHandle h) const
{
    QString fName;
    
    if(h!=c_invalidDirectoryHandle)
    {
        QMap<DiskIF::DirHandle,QPair<QString,WIN32_FIND_DATAW> >::iterator ppI;
    
        m_mutex.lock();
        ppI = m_dirHandleMap.find(h);
        if(ppI!=m_dirHandleMap.end())
        {
            QPair<QString,WIN32_FIND_DATAW>& d = ppI.value();
            
            if(d.second.cFileName!=0)
            {
                WIN32_FIND_DATAW fData;            
                QString cName(QString::fromUtf16(reinterpret_cast<const char16_t *>(d.second.cFileName)));

                ::memset(&fData,0,sizeof(WIN32_FIND_DATAW));
                if(::FindNextFileW(h,&fData))
                {
                    d.second = fData;
                }
                else
                {
                    ::memset(&(d.second),0,sizeof(WIN32_FIND_DATAW));
                }

                if(cName=="." || cName=="..")
                {
                    cName = nextDirectoryEntry(h);
                }
                
                fName = cName;
            }
        }
        m_mutex.unlock();
    }
    return fName;
}

//-------------------------------------------------------------------------------------------

void DiskLayerIF::closeDirectory(DirHandle h) const
{
    if(h!=c_invalidDirectoryHandle)
    {
        QMap<DiskIF::DirHandle,QPair<QString,WIN32_FIND_DATAW> >::iterator ppI;
        
        m_mutex.lock();
        ppI = m_dirHandleMap.find(h);
        if(ppI!=m_dirHandleMap.end())
        {
            ::FindClose(h);
            m_dirHandleMap.erase(ppI);
        }
        m_mutex.unlock();
    }
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

DiskIF::DirHandle DiskLayerIF::openDirectory(const QString& name) const
{
    DirHandle h = c_invalidDirectoryHandle;

    if(isDirectory(name))
    {
        QString dName = directoryName(name);
        h = ::opendir(dName.toUtf8().constData());
        if(h!=0)
        {
            m_mutex.lock();
            m_dirHandleMap.insert(h);
            m_mutex.unlock();
        }
    }
    return h;
}

//-------------------------------------------------------------------------------------------

QString DiskLayerIF::nextDirectoryEntry(DirHandle h) const
{
    QString fName;
    
    if(h!=c_invalidDirectoryHandle)
    {
        QSet<DiskIF::DirHandle>::iterator ppI;
        
        m_mutex.lock();
        ppI = m_dirHandleMap.find(h);
        if(ppI!=m_dirHandleMap.end())
        {
            struct dirent *entry;
            DiskIF::DirHandle h = *ppI;
            
            entry = ::readdir(h);
            if(entry!=0)
            {
                QString cName(QString::fromUtf8(entry->d_name));
                
                if(cName=="." || cName=="..")
                {
                    cName = nextDirectoryEntry(h);
                }
                fName = cName;
            }
        }
        m_mutex.unlock();
    }
    return fName;
}

//-------------------------------------------------------------------------------------------

void DiskLayerIF::closeDirectory(DirHandle h) const
{
    if(h!=c_invalidDirectoryHandle)
    {
        QSet<DiskIF::DirHandle>::iterator ppI;
        
        m_mutex.lock();
        ppI = m_dirHandleMap.find(h);
        if(ppI!=m_dirHandleMap.end())
        {
            ::closedir(h);
            m_dirHandleMap.erase(ppI);
        }
        m_mutex.unlock();
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

