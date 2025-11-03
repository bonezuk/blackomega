#include "common/inc/XMLLibIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(XMLLibIFFactory,XMLLibIF)

//-------------------------------------------------------------------------------------------

QSharedPointer<XMLLibIF> XMLLibIF::m_instance;

//-------------------------------------------------------------------------------------------

XMLLibIF::XMLLibIF()
{}

//-------------------------------------------------------------------------------------------

XMLLibIF::~XMLLibIF()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<XMLLibIF> XMLLibIF::instance(const QString& factoryKey)
{
    if(m_instance.data()==0)
    {
        m_instance = XMLLibIFFactory::createShared(factoryKey);
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<XMLLibIF> XMLLibIF::instance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void XMLLibIF::release()
{
    QSharedPointer<XMLLibIF> eInstance;
    m_instance = eInstance;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
