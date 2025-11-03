//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_FORMATDESCRIPTIONUTILS_H
#define __OMEGA_AUDIOIO_FORMATDESCRIPTIONUTILS_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/FormatDescription.h"
#include "audioio/inc/FormatsSupported.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT FormatDescriptionUtils
{
    public:
        static int *closestFrequencyOrder(int freq);

        static bool findClosestFormatType(const FormatDescription& format,const FormatsSupported& support,FormatDescription& closeFormat);

    protected:
        FormatDescriptionUtils();

        static bool isBitIndexFloatingPoint(tint bitIndex);
        static void closestBitOrder(tint bitIndex,QVector<tint>& hList,QVector<tint>& lList);
        static void findClosestFormatTypeChannelList(const FormatDescription& format, bool isAscending, QVector<tint>& chList);
        static bool findClosestFormatTypeCase(const FormatDescription& format, const FormatsSupported& support, FormatDescription& closeFormat, const QVector<tint>& listA, const QVector<tint>& listB, bool isBitFirst);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
