//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_OMEGADCT_H
#define __OMEGA_ENGINE_OMEGADCT_H
//-------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <memory.h>

#include "common/inc/Allocation.h"
#include "common/inc/BOTree.h"
#include "engine/inc/EngineDLL.h"

#include <QMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
// Cosine definitions
//-------------------------------------------------------------------------------------------

#define D_Cos_1Pi4 (0.70710678118654752440084436210485f)
#define D_Cos_3Pi4 (-0.70710678118654752440084436210485f)

#define D_Cos_1Pi8 (0.92387953251128675612818318939679f)
#define D_Cos_3Pi8 (0.3826834323650897717284599840304f)

#define D_Delta_0Pi4 (0.50979557910415916894193980398784f)
#define D_Delta_1Pi4 (0.60134488693504528054372182390922f)
#define D_Delta_2Pi4 (0.89997622313641570463850954094189f)
#define D_Delta_3Pi4 (2.5629154477415061787960862961777f)

#define D_Delta_0Pi8 (0.50241928618815570551167011928012f)
#define D_Delta_1Pi8 (0.52249861493968888062857531905669f)
#define D_Delta_2Pi8 (0.56694403481635770368053791515488f)
#define D_Delta_3Pi8 (0.646821783359990129548360111652f)
#define D_Delta_4Pi8 (0.78815462345125022473398248719742f)
#define D_Delta_5Pi8 (1.0606776859903474713404517472331f)
#define D_Delta_6Pi8 (1.7224470982383339278159153641566f)
#define D_Delta_7Pi8 (5.1011486186891638581062454923454f)

#define D_Delta_0Pi16 (0.50060299823519630134550410676638f)
#define D_Delta_1Pi16 (0.50547095989754365998444458560696f)
#define D_Delta_2Pi16 (0.51544730992262454697495130564925f)
#define D_Delta_3Pi16 (0.53104259108978417447573393235719f)
#define D_Delta_4Pi16 (0.55310389603444452782938083813705f)
#define D_Delta_5Pi16 (0.58293496820613387367383070125262f)
#define D_Delta_6Pi16 (0.62250412303566481615725615676281f)
#define D_Delta_7Pi16 (0.67480834145500574602596871104104f)
#define D_Delta_8Pi16 (0.74453627100229844977698119197295f)
#define D_Delta_9Pi16 (0.83934964541552703873926374662543f)
#define D_Delta_10Pi16 (0.97256823786196069369768941405256f)
#define D_Delta_11Pi16 (1.1694399334328849551557702840422f)
#define D_Delta_12Pi16 (1.4841646163141662772433269374281f)
#define D_Delta_13Pi16 (2.0577810099534115508565544797104f)
#define D_Delta_14Pi16 (3.4076084184687187857011913334591f)
#define D_Delta_15Pi16 (10.190008123548056811212109201036f)

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT OmegaDCT
{
    public:
        OmegaDCT(int N);
        virtual ~OmegaDCT();

        static OmegaDCT *get(tint N);
        static void stop();

        tfloat32 *input();
        const tfloat32 *input() const;

        tfloat32 *output();
        const tfloat32 *output() const;

        tfloat32 *TypeII(tfloat32 *x);
        tfloat32 *TypeIII(tfloat32 *x);
        tfloat32 *TypeIV(tfloat32 *x);

        tfloat32 *MDCT(tfloat32 *x);
        tfloat32 *InverseMDCT(tfloat32 *x);

        void WMDCT(tfloat32 *x,tfloat32 *X,tint offset);
        void WInverseMDCT(tfloat32 *x,tfloat32 *X,tint offset);

        void VSInverseMDCT(tfloat32 *x,tfloat32 *X);

    protected:

        static QMap<tint,OmegaDCT *> m_DCTCollection;

        static tint m_DCTCounter;
        static common::Allocation m_DCTAllocation;

        static tfloat32 *m_DCTCoefficients_IntelSIMD;
        static tuint32 *m_DCTMasks_IntelSIMD;

#if defined(OMEGA_INTEL)
        static bool isIntelSIMD();
        static void startIntelSIMD();
        static void finishIntelSIMD();
#endif

        common::Allocation m_alloc;

        bool m_isSIMD;
        tint m_N;
        tfloat32 *m_x;
        tfloat32 *m_X;
        tfloat32 *m_Y;
        tint m_offsetY;
        tfloat32 **m_D4FactorArray;

        tfloat32 m_halfN;

        bool isMod2() const;
        bool isMod2(int N) const;

        int mod2() const;
        int mod2(int N) const;

        void init();
        void free();

        tfloat64 dctD4Factor(int k,int N) const;

        void Type2(tfloat32 *x,tfloat32 *X,int N);
        void Type2(tfloat32 *x,tfloat32 *X,int N,int lN);
        void Type3(tfloat32 *x,tfloat32 *X,int N);
        void Type3(tfloat32 *x,tfloat32 *X,int N,int lN);
        void Type4(tfloat32 *x,tfloat32 *X,int N);
        void Type4(tfloat32 *x,tfloat32 *X,int N,int lN);

#if defined(OMEGA_INTEL)
        void Type2_IntelSIMD(tfloat32 *x,tfloat32 *X,int N);
        void Type2_IntelSIMD(tfloat32 *x,tfloat32 *X,int N,int lN);
        void Type3_IntelSIMD(tfloat32 *x,tfloat32 *X,int N);
        void Type3_IntelSIMD(tfloat32 *x,tfloat32 *X,int N,int lN);
        void Type4_IntelSIMD(tfloat32 *x,tfloat32 *X,int N);
        void Type4_IntelSIMD(tfloat32 *x,tfloat32 *X,int N,int lN);
#endif

        void T2L16(tfloat32 *x,tfloat32 *X);
        void T3L16(tfloat32 *x,tfloat32 *X);
        void T4L16(tfloat32 *x,tfloat32 *X);

#if defined(OMEGA_INTEL)
        void T2L16_IntelSIMD(tfloat32 *x,tfloat32 *X,tfloat32 *y);
        void T3L16_IntelSIMD(tfloat32 *x,tfloat32 *X,tfloat32 *y);
        void T4L16_IntelSIMD(tfloat32 *x,tfloat32 *X,tfloat32 *y);
#endif
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
