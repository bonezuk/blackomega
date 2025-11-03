#include "common/inc/SBService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// SBService
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(SBServiceFactory,SBService)

//-------------------------------------------------------------------------------------------

SBService::SBService(QObject *parent) : QObject(parent)
{}

//-------------------------------------------------------------------------------------------

SBService::~SBService()
{}

//-------------------------------------------------------------------------------------------

QString SBService::homeDirectory()
{
    SBServicePtr sService = SBServiceFactory::createShared(SBFACTORYKEY);
    return sService->getHomeDirectory();
}

//-------------------------------------------------------------------------------------------

QString SBService::tempDirectory()
{
    SBServicePtr sService = SBServiceFactory::createShared(SBFACTORYKEY);
    return sService->getTempDirectory();
}

//-------------------------------------------------------------------------------------------

QString SBService::applicationDataDirectory()
{
    SBServicePtr sService = SBServiceFactory::createShared(SBFACTORYKEY);
    return sService->getApplicationDataDirectory();
}

//-------------------------------------------------------------------------------------------
// SBBookmark
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(SBBookmarkFactory,SBBookmark)

//-------------------------------------------------------------------------------------------

SBBookmark::SBBookmark()
{}

//-------------------------------------------------------------------------------------------

SBBookmark::~SBBookmark()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<SBBookmark> SBBookmark::get()
{
    QSharedPointer<SBBookmark> sbBookmark = SBBookmarkFactory::createShared("trackdb");
    return sbBookmark;
}

//-------------------------------------------------------------------------------------------

bool SBBookmark::isReadOnly(const QString& fileName)
{
    int i;
    QString ext;
    
    for(i=fileName.length()-1;i>=0;i--)
    {
        if(fileName.at(i)==QChar('.'))
        {
            ext = fileName.mid(i+1).trimmed().toLower();
            if(ext==".m3u" || ext==".pls" || ext==".m3u8" || ext==".xspf")
            {
                return false;
            }
        }
    }
    return true;    
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
