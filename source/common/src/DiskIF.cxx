#include "common/inc/DiskIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QSharedPointer<DiskIF> DiskIF::m_instance;


#if defined(OMEGA_WIN32)
const DiskIF::DirHandle DiskIF::c_invalidDirectoryHandle = INVALID_HANDLE_VALUE;
#elif defined(OMEGA_POSIX)
const DiskIF::DirHandle DiskIF::c_invalidDirectoryHandle = 0;
#endif

ABSTRACT_FACTORY_CLASS_IMPL(DiskIFFactory,DiskIF)

//-------------------------------------------------------------------------------------------

DiskIF::DiskIF()
{}

//-------------------------------------------------------------------------------------------

DiskIF::~DiskIF()
{}

//-------------------------------------------------------------------------------------------

DiskIF::DirHandle DiskIF::invalidDirectory()
{
    return c_invalidDirectoryHandle;
}

//-------------------------------------------------------------------------------------------

bool DiskIF::isValidDirectory(DirHandle h)
{
    return (h!=invalidDirectory());
}

//-------------------------------------------------------------------------------------------

QSharedPointer<DiskIF> DiskIF::instance(const tchar *factoryName)
{
    if(m_instance.data()==0)
    {
        QSharedPointer<DiskIF> nInstance(DiskIFFactory::createUnmanaged(factoryName));
        m_instance = nInstance;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<DiskIF> DiskIF::instance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void DiskIF::release()
{
    QSharedPointer<DiskIF> eInstance;
    m_instance = eInstance;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
