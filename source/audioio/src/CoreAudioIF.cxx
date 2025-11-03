#include "audioio/inc/CoreAudioIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(CoreAudioIFFactory,CoreAudioIF)

//-------------------------------------------------------------------------------------------

QSharedPointer<CoreAudioIF> CoreAudioIF::m_instance;

//-------------------------------------------------------------------------------------------

CoreAudioIF::CoreAudioIF()
{}

//-------------------------------------------------------------------------------------------

CoreAudioIF::~CoreAudioIF()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<CoreAudioIF> CoreAudioIF::instance(const tchar *factoryName)
{
    if(m_instance.isNull())
    {
        QSharedPointer<CoreAudioIF> nInstance(CoreAudioIFFactory::createUnmanaged(factoryName));
        m_instance = nInstance;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<CoreAudioIF> CoreAudioIF::instance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void CoreAudioIF::release()
{
    if(!m_instance.isNull())
    {
        m_instance.clear();
    }
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
