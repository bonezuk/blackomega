#include "playerapp/playerapp/inc/OmegaWebServicePLA.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

OmegaWebServicePLA::OmegaWebServicePLA(QSharedPointer<OmegaPLWebInterface>& pPLWebI, QSharedPointer<WebEventRegisterInterface>& pWebEvents, const QString& rootDir) : OmegaWebService(rootDir)
{
    m_pWebInterface = pPLWebI;
    m_pWebEvents = pWebEvents;
}

//-------------------------------------------------------------------------------------------

OmegaWebServicePLA::~OmegaWebServicePLA()
{}

//-------------------------------------------------------------------------------------------

void OmegaWebServicePLA::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "OmegaWebServicePLA::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool OmegaWebServicePLA::setupWebEvents()
{
    return !m_pWebEvents.isNull();
}

//-------------------------------------------------------------------------------------------

void OmegaWebServicePLA::setupPLWebInterface()
{}

//-------------------------------------------------------------------------------------------

void OmegaWebServicePLA::stopEvents()
{
    m_pWebEvents.clear();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
