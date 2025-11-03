//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_FORMATDESCRIPTION_H
#define __OMEGA_AUDIOIO_FORMATDESCRIPTION_H
//-------------------------------------------------------------------------------------------

#include <QVector>
#include <QSet>

#include "audioio/inc/AudioIODLL.h"
#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

#define NUMBER_MAXFREQUENCIES 18
#define NUMBER_MAXCHANNELS 8

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT FormatDescription
{
    public:

        typedef enum
        {
            e_DataFloatSingle = 1,
            e_DataFloatDouble,
            e_DataSignedInteger,
            e_DataUnsignedInteger
        } DataType;

    public:
        FormatDescription();
        FormatDescription(DataType type,tint noBits,tint noChannels,tint freq);
        FormatDescription(DataType type,tint noBits,tint noChannels,tint freq,bool littleEndian);
        FormatDescription(const FormatDescription& rhs);

        const FormatDescription& operator = (const FormatDescription& rhs);

        const DataType& typeOfData() const;
        bool setTypeOfData(DataType type);

        const tint& bits() const;
        bool setNumberOfBits(tint noBits);
        tint bitsIndex() const;
        bool setBitsIndex(tint idx);

        const tint& channels() const;
        bool setNumberOfChannels(tint noChannels);
        tint channelsIndex() const;
        bool setChannelsIndex(tint idx);

        const tint& frequency() const;
        bool setFrequency(tint freq);
        tint frequencyIndex() const;
        bool setFrequencyIndex(tint idx);

        bool isLittleEndian() const;
        bool isBigEndian() const;
        void setEndian(bool littleEndian);

        friend AUDIOIO_EXPORT bool operator == (const FormatDescription& a,const FormatDescription& b);
        friend AUDIOIO_EXPORT bool operator != (const FormatDescription& a,const FormatDescription& b);

        static QSet<tint> setOfFrequencies();

        QString description() const;

    protected:

        DataType m_dataType;
        tint m_bits;
        tint m_channels;
        tint m_frequency;
        bool m_isLittleEndian;

        void copy(const FormatDescription& rhs);
        bool isEqual(const FormatDescription& rhs) const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
