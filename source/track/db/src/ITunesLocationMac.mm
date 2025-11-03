#include "track/db/inc/ITunesLocationMac.h"

#import <Cocoa/Cocoa.h>

#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>

//-------------------------------------------------------------------------------------------

QString ITunesLocationMac::IVLocationHomeDirectory()
{
    QString hDir;

    const struct passwd *passwd = getpwnam([NSUserName() UTF8String]);

    if(passwd!=0)
    {
        NSString *homeDir;
        const char *homeDirC = getpwnam([NSUserName() UTF8String])->pw_dir;
        homeDir = [[NSFileManager defaultManager] stringWithFileSystemRepresentation:homeDirC length:strlen(homeDirC)];
        hDir = QString::fromUtf8([homeDir UTF8String]);
    }
    return hDir;
}

//-------------------------------------------------------------------------------------------

QStringList ITunesLocationMac::lastITunesDirectory()
{
    QStringList iPathList;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    if(pool!=nil)
    {
        NSWorkspace *wkSpace = [[NSWorkspace alloc] init];
        if(wkSpace!=nil)
        {
            CFArrayRef recentLibraries = (CFArrayRef)CFPreferencesCopyAppValue((CFStringRef)@"iTunesRecentDatabases",(CFStringRef)@"com.apple.iApps");
            NSArray *libraries = (NSArray *)(recentLibraries);

            for(NSString* library in libraries)
            {
                NSURL *url = [NSURL URLWithString:library];
                NSString *path = [url path];
                const char *x = [path UTF8String];
                if(x!=0)
                {
                    QString pName = QString::fromUtf8(x);
                    iPathList.append(pName);
                }
            }
            [wkSpace release];
        }
        [pool release];
    }
    return iPathList;
}

//-------------------------------------------------------------------------------------------
