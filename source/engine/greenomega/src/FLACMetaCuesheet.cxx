#include "engine/greenomega/inc/FLACMetaCuesheet.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------
// FLACMetaCuesheet::Track
//-------------------------------------------------------------------------------------------

FLACMetaCuesheet::Track::Track() : m_isAudio(true),
    m_preEmphasisFlag(false),
    m_trackNumber(0),
    m_trackOffset(0),
    m_ISRC(),
    m_trackIndexes()
{}

//-------------------------------------------------------------------------------------------

FLACMetaCuesheet::Track::Track(const Track& rhs) : m_isAudio(true),
    m_preEmphasisFlag(false),
    m_trackNumber(0),
    m_trackOffset(0),
    m_ISRC(),
    m_trackIndexes()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const FLACMetaCuesheet::Track& FLACMetaCuesheet::Track::operator = (const Track& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void FLACMetaCuesheet::Track::copy(const Track& rhs)
{
    m_isAudio = rhs.m_isAudio;
    m_preEmphasisFlag = rhs.m_preEmphasisFlag;
    m_trackNumber = rhs.m_trackNumber;
    m_trackOffset = rhs.m_trackOffset;
    m_ISRC = rhs.m_ISRC;
    m_trackIndexes = rhs.m_trackIndexes;
}

//-------------------------------------------------------------------------------------------

void FLACMetaCuesheet::Track::read(engine::Sequence *seq)
{
    tint i,noIndex;
    tchar t[13];

    m_trackOffset = seq->read64BitsI();
    m_trackNumber = seq->readBitsI(8);
    for(i=0;i<12;i++)
    {
        t[i] = seq->readBitsI(8);
    }
    t[i] = '\0';
    m_ISRC = QString::fromLatin1(t);
    m_isAudio = (seq->readBitI()) ? false : true;
    m_preEmphasisFlag = (seq->readBitI()) ? true : false;
    seq->readBitsI(6);
    for(i=0;i<13;i++)
    {
        seq->readBitsI(8);
    }
    noIndex = seq->readBitsI(8);
    for(i=0;i<noIndex;i++)
    {
        tuint64 offset = seq->read64BitsI();
        tint pointIndex = seq->readBitsI(8);
        seq->readBitsI(24);
        m_trackIndexes.append(QPair<tuint64,tint>(offset,pointIndex));
    }
}

//-------------------------------------------------------------------------------------------

const bool& FLACMetaCuesheet::Track::isAudio() const
{
    return m_isAudio;
}

//-------------------------------------------------------------------------------------------

const bool& FLACMetaCuesheet::Track::preEmphasisFlag() const
{
    return m_preEmphasisFlag;
}

//-------------------------------------------------------------------------------------------

const tint& FLACMetaCuesheet::Track::trackNumber() const
{
    return m_trackNumber;
}

//-------------------------------------------------------------------------------------------

const tuint64& FLACMetaCuesheet::Track::trackOffset() const
{
    return m_trackOffset;
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaCuesheet::Track::ISRC() const
{
    return m_ISRC;
}

//-------------------------------------------------------------------------------------------

const QVector<QPair<tuint64,tint> >& FLACMetaCuesheet::Track::indexes() const
{
    return m_trackIndexes;
}

//-------------------------------------------------------------------------------------------

tuint64 FLACMetaCuesheet::Track::index(int idx) const
{
    for(tint i=0;i<m_trackIndexes.size();i++)
    {
        if(m_trackIndexes.at(i).second==idx)
        {
            return m_trackIndexes.at(i).first;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

tuint64 FLACMetaCuesheet::Track::index0() const
{
    return index(0);
}

//-------------------------------------------------------------------------------------------

tuint64 FLACMetaCuesheet::Track::index1() const
{
    return index(1);
}

//-------------------------------------------------------------------------------------------
// FLACMetaCuesheet
//-------------------------------------------------------------------------------------------

FLACMetaCuesheet::FLACMetaCuesheet() : FLACMetaBase(e_Cuesheet),
    m_catalogNumber(),
    m_leadInSamples(0),
    m_isCompactDisc(true),
    m_tracks()
{}

//-------------------------------------------------------------------------------------------

FLACMetaCuesheet::~FLACMetaCuesheet()
{}

//-------------------------------------------------------------------------------------------

bool FLACMetaCuesheet::read(engine::Sequence *seq)
{
    int i,noTracks,len;
    tchar t[129];

    len = seq->readBitsI(24) << 3;
    for(i=0;i<128;i++)
    {
        t[i] = seq->readBitsI(8);
    }
    t[128] = '\0';
    m_catalogNumber = QString::fromLatin1(t);
    m_leadInSamples = seq->read64BitsI();
    m_isCompactDisc = (seq->readBitI()) ? true : false;
    if(!seq->seek(7 + (258 * 8)))
    {
        return false;
    }
    noTracks = seq->readBitsI(8);
    for(i=0;i<noTracks;i++)
    {
        Track track;
        track.read(seq);
        m_tracks.append(track);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

const bool& FLACMetaCuesheet::isCompactDisc() const
{
    return m_isCompactDisc;
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaCuesheet::catalogNumber() const
{
    return m_catalogNumber;
}

//-------------------------------------------------------------------------------------------

const tuint64& FLACMetaCuesheet::leadInSamples() const
{
    return m_leadInSamples;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
