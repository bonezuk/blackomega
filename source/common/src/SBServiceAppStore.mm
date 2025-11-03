//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

#include "common/inc/SBService.h"

#if defined(OMEGA_MAC_STORE)
#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>

#include <QDir>

#import <Foundation/Foundation.h>

#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QString SBBookmark::getFilename(const QString& fileName)
{
    QString oName;

    if(!fileName.isEmpty())
    {
        char *realHome = getpwuid(getuid())->pw_dir;
        QString hDir = QString::fromUtf8(realHome);

        if(hDir.at(hDir.length()-1)!=QChar('/'))
        {
            hDir += "/";
        }
        hDir += "Music";
        hDir = QDir::toNativeSeparators(hDir);
        if(hDir.length()<=fileName.length())
        {
            if(hDir==fileName.mid(0,hDir.length()))
            {
                NSString *contHome = NSHomeDirectory();
                if(contHome!=nil)
                {
                    const char *x = [contHome UTF8String];
                    QString cDir = QString::fromUtf8(x);
                    if(cDir.at(cDir.length()-1)!=QChar('/'))
                    {
                        cDir += "/";
                    }
                    cDir += "Music";
                    if(fileName.length()>hDir.length())
                    {
                        cDir += fileName.mid(hDir.length());
                    }
                    oName = QDir::toNativeSeparators(cDir);
                }
                else
                {
                    oName = fileName;
                }
            }
            else
            {
                oName = fileName;
            }
        }
        else
        {
            oName = fileName;
        }
    }
    return oName;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
