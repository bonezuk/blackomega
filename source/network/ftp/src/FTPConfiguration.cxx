#include "network/ftp/inc/FTPConfiguration.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------

FTPConfiguration::FTPConfiguration() : m_rootPath(),
    m_port(23045)
{}

//-------------------------------------------------------------------------------------------

FTPConfiguration::~FTPConfiguration()
{}

//-------------------------------------------------------------------------------------------

const QString& FTPConfiguration::rootPath() const
{
    return m_rootPath;
}

//-------------------------------------------------------------------------------------------

void FTPConfiguration::setRootPath(const QString& p)
{
    m_rootPath = p;
}

//-------------------------------------------------------------------------------------------

tint FTPConfiguration::passivePort() const
{
    return m_port;
}

//-------------------------------------------------------------------------------------------

void FTPConfiguration::setPassivePort(tint p)
{
    m_port = p;
}

//-------------------------------------------------------------------------------------------

bool FTPConfiguration::isUser(const QString& username,const QString& password)
{
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
