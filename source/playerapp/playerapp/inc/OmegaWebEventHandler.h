//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERAPP_OMEGAWEBEVENTHANDLER_H
#define __PLAYERAPP_PLAYERAPP_OMEGAWEBEVENTHANDLER_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/HTTPEventBroker.h"
#include "playerapp/playercommon/inc/WebEventInterface.h"
#include "playerapp/playercommon/inc/WebEventRegisterInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class OmegaWebEventHandler : public WebEventInterface, public WebEventRegisterInterface
{
    public:
        OmegaWebEventHandler();
        virtual ~OmegaWebEventHandler();
        
        virtual bool registerConnection(network::http::HTTPReceive *recieve);
        
    protected:
    
        HTTPEventBroker m_broker;
    
        virtual void printError(const char *strR, const char *strE) const;
        virtual void sendEvent(const QJsonDocument& doc);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
