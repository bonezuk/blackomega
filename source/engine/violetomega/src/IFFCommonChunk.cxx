#include "engine/violetomega/inc/IFFCommonChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFCommonChunkFactory,IFFCommonChunk, \
                            "COMM",false)

//-------------------------------------------------------------------------------------------

IFFCommonChunk::IFFCommonChunk() : IFFChunk(),
    m_noChannels(0),
    m_noSampleFrames(0),
    m_sampleSize(0),
    m_sampleRate(1.0),
    m_formatType(IFFCommonChunk::e_PCM_Integer)
{}

//-------------------------------------------------------------------------------------------

IFFCommonChunk::~IFFCommonChunk()
{}

//-------------------------------------------------------------------------------------------

const tint& IFFCommonChunk::noChannels() const
{
    return m_noChannels;
}

//-------------------------------------------------------------------------------------------

const tint& IFFCommonChunk::noSampleFrames() const
{
    return m_noSampleFrames;
}

//-------------------------------------------------------------------------------------------

const tint& IFFCommonChunk::sampleSize() const
{
    return m_sampleSize;
}

//-------------------------------------------------------------------------------------------

const tfloat64& IFFCommonChunk::sampleRate() const
{
    return m_sampleRate;
}

//-------------------------------------------------------------------------------------------

const IFFCommonChunk::PCMFormatType& IFFCommonChunk::formatType() const
{
    return m_formatType;
}

//-------------------------------------------------------------------------------------------

bool IFFCommonChunk::scan()
{
    bool res = false;
    
    if(m_file!=0 && size()>=18 && filePositionToStart())
    {
        tbyte mem[18];
        
        if(m_file->read(mem,18)==18)
        {
            m_noChannels = static_cast<tint>(read16BitSigned(mem));
            m_noSampleFrames = static_cast<tint>(read32BitUnsigned(&mem[2]));
            m_sampleSize = static_cast<tint>(read16BitSigned(&mem[6]));
            m_sampleRate = doubleFromExtended(&mem[8]);
            if((m_sampleSize>=1 && m_sampleSize<=32) || m_sampleSize == 64)
            {
                if(m_noChannels>=1 && m_noChannels<=6)
                {
                    res = true;
                }
            }

            tint remain = size() - 18;
            if(remain >= 4 && m_file->read(mem,4)==4)
            {
                IFFID compressionID = read32BitUnsigned(mem);
                if(compressionID == IFF_ID('f', 'l', '3', '2') || compressionID == IFF_ID('F', 'L', '3', '2'))
                {
                    if(m_sampleSize == sizeof(tfloat32) * 8)
                    {
                        m_formatType = e_PCM_Float32;
                    }
                    else
                    {
                        res = false;
                    }
                }
                else if(compressionID == IFF_ID('f', 'l', '6', '4') || compressionID == IFF_ID('F', 'L', '6', '4'))
                {
                    if(m_sampleSize == sizeof(tfloat64) * 8)
                    {
                        m_formatType = e_PCM_Float64;
                    }
                    else
                    {
                        res = false;
                    }
                }
                else if(compressionID == IFF_ID('n', 'o', 'n', 'e') || compressionID == IFF_ID('N', 'O', 'N', 'E'))
                {}
                else
                {
                    // Unknown or unsupported compression scheme in AIFF
                    res = false;
                }
            }
        }
    }
    return res;
}


//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

