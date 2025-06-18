#include "engine/whiteomega/inc/ALACAtomContainer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

ALACAtomContainer::ALACAtomContainer() : redomega::ALACContainer()
{}


//-------------------------------------------------------------------------------------------

ALACAtomContainer::~ALACAtomContainer()
{}

//-------------------------------------------------------------------------------------------

bool ALACAtomContainer::initWithAtom(Atom *a)
{
    bool res = false;
    
    if(a!=0 && a->m_tracks.size()>0)
    {
        Atom::Track *t = a->m_tracks.at(0);
        
        if(t->m_type==Atom::Track::e_track_alac)
        {
            tint formatFlags = 1;

            if(t->m_alacBitDepth==16)
            {
                formatFlags = 1;
            }
            else if(t->m_alacBitDepth==20)
            {
                formatFlags = 2;
            }
            else if(t->m_alacBitDepth==24)
            {
                formatFlags = 3;
            }
            else if(t->m_alacBitDepth==32)
            {
                formatFlags = 4;
            }

            m_description.sampleRate() = static_cast<tfloat64>(t->m_alacSampleRate);
            m_description.formatID() = ATOM_ALAC_ID;
            m_description.formatFlags() = formatFlags;
            m_description.bytesPerPacket() =  t->m_alacFrameLength * t->m_alacNumChannels * (t->m_alacBitDepth >> 3);
            m_description.framesPerPacket() = t->m_alacFrameLength;
            m_description.bytesPerFrame() = t->m_alacNumChannels * (t->m_alacBitDepth >> 3);
            m_description.channelsPerFrame() = t->m_alacNumChannels;
            m_description.bitsPerChannel() = t->m_alacBitDepth;
            
            m_config.frameLength() = t->m_alacFrameLength;
            m_config.compatibleVersion() = t->m_alacCompatibleVersion;
            m_config.bitDepth() = t->m_alacBitDepth;
            m_config.pb() = t->m_alacPb;
            m_config.mb() = t->m_alacMb;
            m_config.kb() = t->m_alacKb;
            m_config.numChannels() = t->m_alacNumChannels;
            m_config.maxRun() = t->m_alacMaxRun;
            m_config.maxFrameBytes() = t->m_alacMaxFrameBytes;
            m_config.avgBitRate() = t->m_alacAvgBitRate;
            m_config.sampleRate() = t->m_alacSampleRate;
            
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
