//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_ABSTRACTPROGRESSINTERFACE_H
#define __OMEGA_COMMON_ABSTRACTPROGRESSINTERFACE_H
//-------------------------------------------------------------------------------------------

#include <QObject>

#include "common/inc/CommonFunctions.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT AbstractProgressInterface : public QObject
{
    public:
        AbstractProgressInterface(QObject *parent);
        virtual ~AbstractProgressInterface();

        virtual void setProgress(tfloat32 v) = 0;
        virtual tfloat32 getProgress() = 0;
        virtual bool isCancelled() = 0;

        virtual bool isActive() = 0;
        virtual void activate(bool useReference = false) = 0;
        virtual void deactivate(bool useReference = false) = 0;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
