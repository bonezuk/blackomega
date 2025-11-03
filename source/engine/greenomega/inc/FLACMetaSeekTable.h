//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_GREENOMEGA_FLACMETASEEKTABLE_H
#define __OMEGA_GREENOMEGA_FLACMETASEEKTABLE_H
//-------------------------------------------------------------------------------------------

#include <QVector>

#include "engine/greenomega/inc/FLACMetaBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACMetaSeekTable : public FLACMetaBase
{
    public:

        class SeekPoint
        {
            public:
                SeekPoint();
                SeekPoint(const SeekPoint& rhs);

                const SeekPoint& operator = (const SeekPoint& rhs);

                bool isPlaceholder() const;
                const tuint64& sampleNumber() const;
                const tuint64& offset() const;
                const tint& samplesInFrame() const;

                void read(engine::Sequence *seq);

            protected:

                tuint64 m_sampleNumber;
                tuint64 m_offset;
                tint m_samplesInFrame;

                void copy(const SeekPoint& rhs);
        };

    public:
        FLACMetaSeekTable();
        virtual ~FLACMetaSeekTable();

        virtual bool read(engine::Sequence *seq);

        void setFrequency(int f);

        SeekPoint find(common::TimeStamp& t);
        tint findPoints(tuint64 sampleNo,SeekPoint& lPoint,SeekPoint& hPoint);

    protected:

        int m_frequency;
        QVector<SeekPoint> m_seekTable;
};

//-------------------------------------------------------------------------------------------

inline bool FLACMetaSeekTable::SeekPoint::isPlaceholder() const
{
    return (m_sampleNumber==0xffffffffffffffffULL) ? true : false;
}

//-------------------------------------------------------------------------------------------

inline const tuint64& FLACMetaSeekTable::SeekPoint::sampleNumber() const
{
    return m_sampleNumber;
}

//-------------------------------------------------------------------------------------------

inline const tuint64& FLACMetaSeekTable::SeekPoint::offset() const
{
    return m_offset;
}

//-------------------------------------------------------------------------------------------

inline const tint& FLACMetaSeekTable::SeekPoint::samplesInFrame() const
{
    return m_samplesInFrame;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

