//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_EXCEPTION_H
#define __OMEGA_COMMON_EXCEPTION_H
//-------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "common/inc/CommonTypes.h"
#include "common/inc/BString.h"
#include "common/inc/DLong.h"
#include <QString>


//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT Exception
{
    public:
        Exception(const tchar *msg);
        Exception(const BString& msg);
        Exception(const QString& msg);
        Exception(const tchar *sObj,const tchar *sFunc,const tchar *msg);
        Exception(const tchar *sObj,const tchar *sFunc,const BString& msg);
        
        virtual const BString& error() const;
    
    protected:
    
        BString m_error;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
