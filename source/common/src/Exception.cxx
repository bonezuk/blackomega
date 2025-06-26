#include "common/inc/Exception.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

Exception::Exception(const tchar *msg) : m_error(msg)
{}

//-------------------------------------------------------------------------------------------

Exception::Exception(const BString& msg) : m_error(msg)
{}

//-------------------------------------------------------------------------------------------

Exception::Exception(const QString& msg) : m_error()
{
    m_error = msg.toLatin1().constData();
}

//-------------------------------------------------------------------------------------------

Exception::Exception(const tchar *sObj,const tchar *sFunc,const tchar *msg) : m_error()
{
    m_error << sObj << "::" << sFunc << " - " << msg;
}

//-------------------------------------------------------------------------------------------

Exception::Exception(const tchar *sObj,const tchar *sFunc,const BString& msg)
{
    m_error << sObj << "::" << sFunc << " - " << msg;
}

//-------------------------------------------------------------------------------------------

const BString& Exception::error() const
{
    return m_error;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
