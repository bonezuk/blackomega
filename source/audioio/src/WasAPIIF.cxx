#include "audioio/inc/WasAPIIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------
// WasAPIIF
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(WasAPIIFFactory,WasAPIIF)

//-------------------------------------------------------------------------------------------

QSharedPointer<WasAPIIF> WasAPIIF::m_instance;
bool WasAPIIF::m_exclusiveAccess = false;

//-------------------------------------------------------------------------------------------

WasAPIIF::WasAPIIF() : QObject()
{}

//-------------------------------------------------------------------------------------------

WasAPIIF::~WasAPIIF()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<WasAPIIF> WasAPIIF::instance(const tchar *factoryName)
{
	if(m_instance.isNull())
	{
		QSharedPointer<WasAPIIF> nInstance(WasAPIIFFactory::createUnmanaged(factoryName));
		
		if(nInstance->init())
		{
			m_instance = nInstance;
		}
	}
	return m_instance;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<WasAPIIF> WasAPIIF::instance()
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------

void WasAPIIF::release()
{
	if(!m_instance.isNull())
	{
		m_instance->done();
		m_instance.reset();
	}
}

//-------------------------------------------------------------------------------------------
// WasAPIDevice
//-------------------------------------------------------------------------------------------

WasAPIDevice::WasAPIDevice() : QObject()
{}

//-------------------------------------------------------------------------------------------

WasAPIDevice::~WasAPIDevice()
{}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
