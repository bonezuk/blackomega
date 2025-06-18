//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_HTTPFILETRANSFER_H
#define __PLAYERAPP_PLAYERCOMMON_HTTPFILETRANSFER_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "network/inc/Resource.h"
#include "network/inc/Controller.h"
#include "network/http/inc/HTTPService.h"
#include "network/http/inc/HTTPServer.h"
#include "network/http/inc/HTTPConnection.h"
#include "network/http/inc/HTTPUtilities.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT HTTPFileTransfer
{
    public:
        HTTPFileTransfer(const QString& rootDir);
        virtual ~HTTPFileTransfer();
        
        virtual void process(network::http::HTTPReceive *recieve);
        
    private:
        QString m_rootDir;
        
        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual bool isValid(network::http::HTTPReceive *recieve) const;
        virtual void postErrorResponse(int code, network::http::HTTPReceive *recieve);
        virtual void transferFile(const QString& fileName, network::http::HTTPReceive *recieve, network::http::Unit& hdr);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
