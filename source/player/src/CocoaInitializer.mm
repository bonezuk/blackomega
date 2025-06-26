#include "player/inc/CocoaInitializer.h"

#include <Foundation/NSAutoreleasePool.h>
#include <AppKit/NSApplication.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------
// CocoaInitializer::Private
//-------------------------------------------------------------------------------------------

class CocoaInitializer::Private
{
    public:
        NSAutoreleasePool *m_pool;    
};

//-------------------------------------------------------------------------------------------

CocoaInitializer::CocoaInitializer()
{
    d = new Private;
    d->m_pool = [[NSAutoreleasePool alloc] init];
}

//-------------------------------------------------------------------------------------------

CocoaInitializer::~CocoaInitializer()
{
    [d->m_pool release];
    delete d;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
