//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERAPP_OMEGAWEBSERVICEPLA_H
#define __PLAYERAPP_PLAYERAPP_OMEGAWEBSERVICEPLA_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/OmegaWebService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class OmegaWebServicePLA : public OmegaWebService
{
    public:
        OmegaWebServicePLA(QSharedPointer<OmegaPLWebInterface>& pPLWebI, QSharedPointer<WebEventRegisterInterface>& pWebEvents, const QString& rootDir);
        virtual ~OmegaWebServicePLA();
        
    protected:
        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual bool setupWebEvents();
        virtual void setupPLWebInterface();
        virtual void stopEvents();
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
