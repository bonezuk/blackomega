//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/LinuxALSAIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(LinuxALSAIFFactory,LinuxALSAIF)

//-------------------------------------------------------------------------------------------

QSharedPointer<LinuxALSAIF> LinuxALSAIF::m_instance;

//-------------------------------------------------------------------------------------------

LinuxALSAIF::LinuxALSAIF()
{}

//-------------------------------------------------------------------------------------------

LinuxALSAIF::~LinuxALSAIF()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<LinuxALSAIF> LinuxALSAIF::instance(const tchar *factoryName)
{
	if(m_instance.isNull())
	{
        QSharedPointer<LinuxALSAIF> nInstance(LinuxALSAIFFactory::createUnmanaged(factoryName));
		m_instance = nInstance;
	}
	return m_instance;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<LinuxALSAIF> LinuxALSAIF::instance()
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------

void LinuxALSAIF::release()
{
	if(!m_instance.isNull())
	{
		m_instance.reset();
	}
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
