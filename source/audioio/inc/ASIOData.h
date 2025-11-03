//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_ASIODATA_H
#define __OMEGA_AUDIOIO_ASIODATA_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/RData.h"
#include "audioio/inc/ASIODriver.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT ASIOData : public engine::RData
{
    public:
        ASIOData();
        ASIOData(tint len,tint inChannel,tint outChannel);
        ASIOData(const engine::AData& rhs);
        virtual ~ASIOData();
        
        virtual void *asioData(tint chIdx,tint pIdx);
        virtual const void *asioDataConst(tint chIdx,tint pIdx) const;
        
        virtual void setSampleType(ASIOSampleType t);
        virtual tint getSampleSize() const;
        
        virtual void setVolume(sample_t v);
        virtual void convert();
        virtual void reset();
        
    protected:

        ASIOSampleType m_sampleType;
        void **m_asioDataArray;

        /*
        e_centerChannelIndex = -2, == m_asioAuxDataArray[0] -> m_asioAuxDataArray[-(-2) - 2] = [0]
        e_lfeChannelIndex = -3,    == m_asioAuxDataArray[1] -> m_asioAuxDataArray[-(-3) - 2] = [1]
        [ -( idx ) - m_asioAuxArraySize ]
        */
        void **m_asioAuxDataArray;
        const tint m_asioAuxArraySize = 2;
        
        bool m_convertFlag;
        sample_t m_volume;
        
        tint32 *m_vIntBuffer;
        tint m_vIntBufSize;
        
        virtual void copy(const engine::AData& rhs);
        
        virtual void asioAllocate();
        virtual const void *asioDataI(tint chIdx,tint pIdx) const;
        virtual void *asioDataChannelIndex(tint chIdx);
        
        virtual tint copyToBuffer(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        virtual tint copyToBufferR(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type, bool recursive);
        
        virtual tint32 *volumeIntBuffer();
        virtual void volumeIntUpscale(const sample_t *in, tint32 *out, tint noSamples, engine::CodecDataType type) const;
        virtual void volumeInt16Upscale(const tint16 *in, tint32 *out, tint noSamples) const;
        virtual void volumeInt24Upscale(const tint32 *in, tint32 *out, tint noSamples) const;
        virtual void volumeInt32Upscale(const tint32 *in, tint32 *out, tint noSamples) const;

    private:
    
        sample_t volumeAndClip(sample_t x);
        
        tint copyToBufferInt16LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt16MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt24LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt24MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferFloat32LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferFloat32MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferFloat64LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferFloat64MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32LSB16(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32MSB16(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32LSB18(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32MSB18(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32LSB20(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32MSB20(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32LSB24(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
        tint copyToBufferInt32MSB24(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type);
};

//-------------------------------------------------------------------------------------------

#if defined(SINGLE_FLOAT_SAMPLE)

inline sample_t ASIOData::volumeAndClip(sample_t x)
{
    x *= m_volume;
    if(x < -1.0f)
    {
        x = -1.0f;
    }
    else if(x > 1.0f)
    {
        x = 1.0f;
    }
    return x;
}

#else

inline sample_t ASIOData::volumeAndClip(sample_t x)
{
    x *= m_volume;
    if(x < -1.0)
    {
        x = -1.0;
    }
    else if(x > 1.0)
    {
        x = 1.0;
    }
    return x;
}

#endif

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

