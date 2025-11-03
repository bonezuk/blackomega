//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_IOTIMESTAMP_H
#define __OMEGA_AUDIOIO_IOTIMESTAMP_H
//-------------------------------------------------------------------------------------------

#include "common/inc/TimeStamp.h"
#include "audioio/inc/FormatDescription.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IOTimeStamp
{
    public:
        IOTimeStamp();
        IOTimeStamp(bool bValid,const common::TimeStamp& t);
        IOTimeStamp(const IOTimeStamp& rhs);

        const IOTimeStamp& operator = (const IOTimeStamp& rhs);

        virtual bool isValid() const;
        virtual const common::TimeStamp& time() const;

        friend AUDIOIO_EXPORT bool operator == (const IOTimeStamp& a,const IOTimeStamp& b);
        friend AUDIOIO_EXPORT bool operator != (const IOTimeStamp& a,const IOTimeStamp& b);

    protected:
        bool m_valid;
        common::TimeStamp m_time;

        virtual void copy(const IOTimeStamp& rhs);
        virtual bool isEqual(const IOTimeStamp& rhs) const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

