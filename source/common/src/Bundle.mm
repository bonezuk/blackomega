#import <Foundation/Foundation.h>

#include "common/inc/Bundle.h"

#include <QDir>
#include <QString>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QString Bundle::pluginDirectory()
{
    QString pDir;
    NSBundle *bundle = [NSBundle mainBundle];
    if(bundle!=nil)
    {
        NSString *nPath = [bundle bundlePath];
        if(nPath!=nil)
        {
            QString bPath = QString::fromUtf8([nPath UTF8String]);
            if(!bPath.isEmpty())
            {
                bPath = QDir::toNativeSeparators(bPath);
                if(bPath.at(bPath.length()-1)==QChar('/'))
                {
                    bPath = bPath.mid(0,bPath.length()-1);
                }
                pDir = bPath + "/Contents/Plugins";
            }
        }
    }
    return pDir;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
