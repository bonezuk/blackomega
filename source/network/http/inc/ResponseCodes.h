//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_HTTP_RESPONSECODES_H
#define __OMEGA_NETWORK_HTTP_RESPONSECODES_H
//-------------------------------------------------------------------------------------------

#include "network/http/inc/HTTPDLL.h"

#include <QString>
#include <QMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

class HTTP_EXPORT ResponseCodes
{
    public:
        static const QString& reason(tint code);
    
    protected:
        static ResponseCodes *m_instance;
    
        ResponseCodes();
        
        QString m_blank;
        QMap<tint,QString> m_reasonMap;
};

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

