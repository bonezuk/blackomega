//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACDECODER_H
#define __OMEGA_ENGINE_REDOMEGA_ALACDECODER_H
//-------------------------------------------------------------------------------------------

#include "engine/redomega/inc/ALACSequence.h"
#include "engine/redomega/inc/ALACContainer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT ALACDecoder
{
    public:
        typedef enum
        {
            e_idSCE = 0,    // single channel element
            e_idCPE = 1,    // channel pair element
            e_idCCE = 2,    // coupling channel element
            e_idLFE = 3,    // lfe channel element
            e_idDSE = 4,    // data stream element
            e_idPCE = 5,    // program config element
            e_idFIL = 6,    // fill element
            e_idEND = 7        // end element
        } PayloadType;
        
    public:
        ALACDecoder(ALACContainer *container);
        virtual ~ALACDecoder();
        
        bool init();
        
        tint decode(ALACSequence *seq, sample_t *mem, tint len, CodecDataType type);
        
    protected:
        
        ALACContainer *m_container;
        
        tint m_channelIndex;
        tint m_activeElements;
        
        tint32 *m_mixBufferU;
        tint32 *m_mixBufferV;
        tint32 *m_predictor;
        tuint16 *m_shiftBuffer;

#if defined(OMEGA_ALAC_COMPARE)
        tint m_frameCounter;
        tint m_dynamicCounter;
#endif

        tint decodeSCE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        tint decodeCPE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        tint decodeCCE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        tint decodeLFE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        tint decodeDSE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        tint decodePCE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        tint decodeFIL(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        tint decodeEND(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type);
        
        tint signOfInt(tint i);
        void dynamicPredictor(tint *pc,tint *out,tint num,tint16 *coefs,tint numActive,tuint chanBits,tuint denShift);
};

//-------------------------------------------------------------------------------------------

inline tint ALACDecoder::signOfInt(tint i)
{
    tint n = static_cast<tint>(static_cast<tuint32>(0-i) >> 31);
    return n | (i >> 31);
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------


