//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_SANDBOX_BASE_H
#define __OMEGA_COMMON_SANDBOX_BASE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/SBService.h"
#include "widget/inc/WidgetDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace widget
{
//-------------------------------------------------------------------------------------------

class WIDGET_EXPORT SBServiceBase : public common::SBService
{
    public:
        Q_OBJECT

    public:
        SBServiceBase(QObject *parent = 0);
        virtual ~SBServiceBase();

        virtual void loadDirDialog(QObject *parent,const QString& title,const QString& dirName);
        virtual void loadFilesDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter);
        virtual void saveFileDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter);

        virtual void *allocatePool();
        virtual void releasePool(void *pPtr);

    protected:

        virtual QString getHomeDirectory();
        virtual QString getTempDirectory();
        virtual QString getApplicationDataDirectory();
};

//-------------------------------------------------------------------------------------------
} // namespace widget
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

