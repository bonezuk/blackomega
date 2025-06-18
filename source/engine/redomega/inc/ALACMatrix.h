//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACMATRIX_H
#define __OMEGA_ENGINE_REDOMEGA_ALACMATRIX_H
//-------------------------------------------------------------------------------------------

#include "engine/redomega/inc/ALACSequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT ALACMatrix
{
    public:
        ALACMatrix();
        virtual ~ALACMatrix();
        
        void unMix16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, CodecDataType type);
        void unMix20(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, CodecDataType type);
        void unMix24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, 
            tuint16 *shiftUV, tint32 bytesShifted, CodecDataType type);
        void unMix32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted, CodecDataType type);

        void copyPredictorTo16(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples, CodecDataType type);
        void copyPredictorTo20(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples, CodecDataType type);
        void copyPredictorTo24(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples, CodecDataType type);
        void copyPredictorTo24Shift(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted, CodecDataType type);
        void copyPredictorTo32(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, CodecDataType type);
        void copyPredictorTo32Shift(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted, CodecDataType type);

        void clip(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride, CodecDataType type);
        void clipLR(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride, CodecDataType type);


    protected:
        void unMix16Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);
        void unMix16Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);
        void unMix16Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);
        void unMix16Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);

        void unMix20Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);
        void unMix20Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);
        void unMix20Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);
        void unMix20Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes);

        void unMix24Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
            tuint16 *shiftUV, tint32 bytesShifted);
        void unMix24Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
            tuint16 *shiftUV, tint32 bytesShifted);
        void unMix24Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
            tuint16 *shiftUV, tint32 bytesShifted);
        void unMix24Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
            tuint16 *shiftUV, tint32 bytesShifted);

        void unMix32Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted);
        void unMix32Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted);
        void unMix32Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted);
        void unMix32Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted);

        void copyPredictorTo16Int16(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);
        void copyPredictorTo16Int24(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);
        void copyPredictorTo16Int32(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);
        void copyPredictorTo16Float(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);

        void copyPredictorTo24Int16(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);
        void copyPredictorTo24Int24(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);
        void copyPredictorTo24Int32(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);
        void copyPredictorTo24Float(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples);
        
        void copyPredictorTo24ShiftInt16(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);
        void copyPredictorTo24ShiftInt24(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);
        void copyPredictorTo24ShiftInt32(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);
        void copyPredictorTo24ShiftFloat(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);

        void copyPredictorTo20Int16(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples);
        void copyPredictorTo20Int24(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples);
        void copyPredictorTo20Int32(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples);
        void copyPredictorTo20Float(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples);

        void copyPredictorTo32Int16(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples);
        void copyPredictorTo32Int24(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples);
        void copyPredictorTo32Int32(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples);
        void copyPredictorTo32Float(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples);

        void copyPredictorTo32ShiftInt16(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);
        void copyPredictorTo32ShiftInt24(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);
        void copyPredictorTo32ShiftInt32(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);
        void copyPredictorTo32ShiftFloat(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted);

        void clipInt24(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride);
        void clipFloat(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride);

        void clipLRInt24(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride);
        void clipLRFloat(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride);
};

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

