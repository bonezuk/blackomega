//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_WEBEVENTREGISTERINTERFACE_H
#define __PLAYERAPP_PLAYERCOMMON_WEBEVENTREGISTERINTERFACE_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "playerapp/playercommon/inc/HTTPFileTransfer.h"
#include "playerapp/playercommon/inc/OmegaWebInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT WebEventRegisterInterface
{
    public:
        WebEventRegisterInterface();
        virtual ~WebEventRegisterInterface();
        
        virtual bool registerConnection(network::http::HTTPReceive *recieve) = 0;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

