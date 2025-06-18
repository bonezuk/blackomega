    //-------------------------------------------------------------------------------------------
#ifndef __OMEGA_GREENOMEGA_FLACMETACUESHEET_H
#define __OMEGA_GREENOMEGA_FLACMETACUESHEET_H
//-------------------------------------------------------------------------------------------

#include "engine/greenomega/inc/FLACMetaBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACMetaCuesheet : public FLACMetaBase
{
    public:
        class Track;
        
    public:
        FLACMetaCuesheet();
        virtual ~FLACMetaCuesheet();
        
        virtual bool read(engine::Sequence *seq);
        
        const bool& isCompactDisc() const;
        const QString& catalogNumber() const;
        const tuint64& leadInSamples() const;
        const QVector<Track>& tracks() const;
        
    protected:
        
        QString m_catalogNumber;
        tuint64 m_leadInSamples;
        bool m_isCompactDisc;
        QVector<Track> m_tracks;
};

//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACMetaCuesheet::Track
{
    public:
        Track();
        Track(const Track& rhs);
                
        const Track& operator = (const Track& rhs);
                
        void read(engine::Sequence *seq);
                
        const bool& isAudio() const;
        const bool& preEmphasisFlag() const;
        const tint& trackNumber() const;
        const tuint64& trackOffset() const;
        const QString& ISRC() const;
        const QVector<QPair<tuint64,tint> >& indexes() const;
        tuint64 index0() const;
        tuint64 index1() const;
                
    protected:

        bool m_isAudio;
        bool m_preEmphasisFlag;
        tint m_trackNumber;                
        tuint64 m_trackOffset;
        QString m_ISRC;
        QVector<QPair<tuint64,tint> > m_trackIndexes;
                
        void copy(const Track& rhs);
        tuint64 index(int idx) const;
};

//-------------------------------------------------------------------------------------------

inline const QVector<FLACMetaCuesheet::Track>& FLACMetaCuesheet::tracks() const
{
    return m_tracks;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

