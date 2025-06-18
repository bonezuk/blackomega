//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_HYBRID_H
#define __OMEGA_ENGINE_BLACKOMEGA_HYBRID_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/blackomega/inc/Stereo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------
#if defined(SINGLE_FLOAT_SAMPLE)
//-------------------------------------------------------------------------------------------

#define COS_6_1    0.866025403784f
#define COS_6_2    0.500000000000f

#define C_0  0.923879532511f
#define C_1  0.382683432651f
#define C_2  1.306562964876f
#define C_3 -0.541196100146f
#define C_0_MINUS -0.923879532511f
#define C_1_MINUS -0.382683432651f

#define DCT36_1_0       0.984807753012f
#define DCT36_1_1       0.866025403784f
#define DCT36_1_2       0.642787609687f
#define DCT36_1_3       0.342020143326f
#define DCT36_1_4       -0.000000000000f
#define DCT36_1_5       -0.342020143326f
#define DCT36_1_6       -0.642787609687f
#define DCT36_1_7       -0.866025403784f
#define DCT36_1_8       -0.984807753012f

#define DCT36_2_0       0.939692620786f
#define DCT36_2_1       0.500000000000f
#define DCT36_2_2       -0.173648177667f
#define DCT36_2_3       -0.766044443119f
#define DCT36_2_4       -1.000000000000f
#define DCT36_2_5       -0.766044443119f
#define DCT36_2_6       -0.173648177667f
#define DCT36_2_7       0.500000000000f
#define DCT36_2_8       0.939692620786f

#define DCT36_3_0       0.866025403784f
#define DCT36_3_1       -0.000000000000f
#define DCT36_3_2       -0.866025403784f
#define DCT36_3_3       -0.866025403784f
#define DCT36_3_4       0.000000000000f
#define DCT36_3_5       0.866025403784f
#define DCT36_3_6       0.866025403784f
#define DCT36_3_7       -0.000000000000f
#define DCT36_3_8       -0.866025403784f

#define DCT36_4_0       0.766044443119f
#define DCT36_4_1       -0.500000000000f
#define DCT36_4_2       -0.939692620786f
#define DCT36_4_3       0.173648177667f
#define DCT36_4_4       1.000000000000f
#define DCT36_4_5       0.173648177667f
#define DCT36_4_6       -0.939692620786f
#define DCT36_4_7       -0.500000000000f
#define DCT36_4_8       0.766044443119f

#define DCT36_5_0       0.642787609687f
#define DCT36_5_1       -0.866025403784f
#define DCT36_5_2       -0.342020143326f
#define DCT36_5_3       0.984807753012f
#define DCT36_5_4       -0.000000000000f
#define DCT36_5_5       -0.984807753012f
#define DCT36_5_6       0.342020143326f
#define DCT36_5_7       0.866025403784f
#define DCT36_5_8       -0.642787609687f

#define DCT36_6_0       0.500000000000f
#define DCT36_6_1       -1.000000000000f
#define DCT36_6_2       0.500000000000f
#define DCT36_6_3       0.500000000000f
#define DCT36_6_4       -1.000000000000f
#define DCT36_6_5       0.500000000000f
#define DCT36_6_6       0.500000000000f
#define DCT36_6_7       -1.000000000000f
#define DCT36_6_8       0.500000000000f

#define DCT36_7_0       0.342020143326f
#define DCT36_7_1       -0.866025403784f
#define DCT36_7_2       0.984807753012f
#define DCT36_7_3       -0.642787609687f
#define DCT36_7_4       0.000000000000f
#define DCT36_7_5       0.642787609687f
#define DCT36_7_6       -0.984807753012f
#define DCT36_7_7       0.866025403784f
#define DCT36_7_8       -0.342020143326f

#define DCT36_8_0       0.173648177667f
#define DCT36_8_1       -0.500000000000f
#define DCT36_8_2       0.766044443119f
#define DCT36_8_3       -0.939692620786f
#define DCT36_8_4       1.000000000000f
#define DCT36_8_5       -0.939692620786f
#define DCT36_8_6       0.766044443119f
#define DCT36_8_7       -0.500000000000f
#define DCT36_8_8       0.173648177667f

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

#define COS_6_1    0.866025403784
#define COS_6_2    0.500000000000

#define C_0  0.923879532511
#define C_1  0.382683432651
#define C_2  1.306562964876
#define C_3 -0.541196100146
#define C_0_MINUS -0.923879532511
#define C_1_MINUS -0.382683432651

#define DCT36_1_0       0.984807753012
#define DCT36_1_1       0.866025403784
#define DCT36_1_2       0.642787609687
#define DCT36_1_3       0.342020143326
#define DCT36_1_4       -0.000000000000
#define DCT36_1_5       -0.342020143326
#define DCT36_1_6       -0.642787609687
#define DCT36_1_7       -0.866025403784
#define DCT36_1_8       -0.984807753012

#define DCT36_2_0       0.939692620786
#define DCT36_2_1       0.500000000000
#define DCT36_2_2       -0.173648177667
#define DCT36_2_3       -0.766044443119
#define DCT36_2_4       -1.000000000000
#define DCT36_2_5       -0.766044443119
#define DCT36_2_6       -0.173648177667
#define DCT36_2_7       0.500000000000
#define DCT36_2_8       0.939692620786

#define DCT36_3_0       0.866025403784
#define DCT36_3_1       -0.000000000000
#define DCT36_3_2       -0.866025403784
#define DCT36_3_3       -0.866025403784
#define DCT36_3_4       0.000000000000
#define DCT36_3_5       0.866025403784
#define DCT36_3_6       0.866025403784
#define DCT36_3_7       -0.000000000000
#define DCT36_3_8       -0.866025403784

#define DCT36_4_0       0.766044443119
#define DCT36_4_1       -0.500000000000
#define DCT36_4_2       -0.939692620786
#define DCT36_4_3       0.173648177667
#define DCT36_4_4       1.000000000000
#define DCT36_4_5       0.173648177667
#define DCT36_4_6       -0.939692620786
#define DCT36_4_7       -0.500000000000
#define DCT36_4_8       0.766044443119

#define DCT36_5_0       0.642787609687
#define DCT36_5_1       -0.866025403784
#define DCT36_5_2       -0.342020143326
#define DCT36_5_3       0.984807753012
#define DCT36_5_4       -0.000000000000
#define DCT36_5_5       -0.984807753012
#define DCT36_5_6       0.342020143326
#define DCT36_5_7       0.866025403784
#define DCT36_5_8       -0.642787609687

#define DCT36_6_0       0.500000000000
#define DCT36_6_1       -1.000000000000
#define DCT36_6_2       0.500000000000
#define DCT36_6_3       0.500000000000
#define DCT36_6_4       -1.000000000000
#define DCT36_6_5       0.500000000000
#define DCT36_6_6       0.500000000000
#define DCT36_6_7       -1.000000000000
#define DCT36_6_8       0.500000000000

#define DCT36_7_0       0.342020143326
#define DCT36_7_1       -0.866025403784
#define DCT36_7_2       0.984807753012
#define DCT36_7_3       -0.642787609687
#define DCT36_7_4       0.000000000000
#define DCT36_7_5       0.642787609687
#define DCT36_7_6       -0.984807753012
#define DCT36_7_7       0.866025403784
#define DCT36_7_8       -0.342020143326

#define DCT36_8_0       0.173648177667
#define DCT36_8_1       -0.500000000000
#define DCT36_8_2       0.766044443119
#define DCT36_8_3       -0.939692620786
#define DCT36_8_4       1.000000000000
#define DCT36_8_5       -0.939692620786
#define DCT36_8_6       0.766044443119
#define DCT36_8_7       -0.500000000000
#define DCT36_8_8       0.173648177667

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT Hybrid
{
    public:
        Hybrid();
        ~Hybrid();
        
        void set(GrInfoS *p,tint n);
        
        void transform(sample_t *in,sample_t *out,tint sb);
        void swap();
    
    protected:
    
        common::Allocation m_alloc;
        
        GrInfoS *m_gr;
        GrInfoS *m_grA;
        GrInfoS *m_grB;
        
        sample_t *m_prev;
        sample_t m_win[4][36];
        
        sample_t m_inv36Out[36];
        sample_t m_inv12Out[12];
        
        tint m_curGr;

        void init();
        
        void invDCT(sample_t *in,tint sb);
        
        void dct12(sample_t *x,sample_t *y);
        
        void dct36(sample_t *x,sample_t *y);
};

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
