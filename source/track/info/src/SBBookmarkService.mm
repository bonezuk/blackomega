#include "track/info/inc/SBBookmarkService.h"
#include "common/inc/TimeStamp.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include <sys/types.h>
#include <pwd.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(SBBookmarkServiceFactory, SBBookmarkService)

//-------------------------------------------------------------------------------------------

QSharedPointer<SBBookmarkService> SBBookmarkService::m_instance;

//-------------------------------------------------------------------------------------------

SBBookmarkService::SBBookmarkService()
{}

//-------------------------------------------------------------------------------------------

SBBookmarkService::~SBBookmarkService()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<SBBookmarkService> SBBookmarkService::instance()
{
    if(m_instance.data()==0)
    {
        QSharedPointer<SBBookmarkService> pInstance = SBBookmarkServiceFactory::createShared("trackdb");
        m_instance = pInstance;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void SBBookmarkService::release()
{
    QSharedPointer<SBBookmarkService> pEmptyInstance;
    m_instance = pEmptyInstance;
}

//-------------------------------------------------------------------------------------------

QString SBBookmarkService::getHomeDirectory()
{
    QString hDir;
    NSString *homeDir;
    const struct passwd *passwd = getpwnam([NSUserName() UTF8String]);

    if(passwd!=0)
    {
        const char *homeDirC = getpwnam([NSUserName() UTF8String])->pw_dir;
        homeDir = [[NSFileManager defaultManager] stringWithFileSystemRepresentation:homeDirC length:strlen(homeDirC)];
        if(homeDir!=nil)
        {
            hDir = QString::fromUtf8([homeDir UTF8String]);
        }
    }
    return hDir;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
