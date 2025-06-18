#include "gtest/gtest.h"

#include "engine/inc/OmegaDCT64.h"
#include "common/inc/Random.h"

using namespace omega;
using namespace engine;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(OmegaDCT,MDCT1024)
{
    const tfloat64 c_Tolerance = 0.0000001;
    
    const tint c_N = 1024;
    
    int i,k,n;
    tfloat64 *input = new tfloat64 [c_N * 2];
    tfloat64 *formulaOutput = new tfloat64 [c_N];
    tfloat64 *formulaInverse = new tfloat64 [c_N * 2];
    
    common::Random *random = common::Random::instance();
    random->reset();
    
    for(i=0;i<c_N * 2;i++)
    {
        input[i] = random->randomReal1();
    }
    
    tfloat64 NF = static_cast<tfloat64>(c_N);
    
    // MDCT
    for(k=0;k<c_N;k++)
    {
        tfloat64 kF = static_cast<tfloat64>(k);
        tfloat64 X = 0.0;
        
        for(n=0;n<c_N * 2;n++)
        {
            tfloat64 nF = static_cast<tfloat64>(n);
            tfloat64 d = (c_PI_D / NF) * (nF + 0.5 + (NF / 2.0)) * (kF + 0.5);
            X += input[n] * cos(d);
        }
        formulaOutput[k] = X;
    }
    
    OmegaDCT64 *dct = OmegaDCT64::get(c_N * 2);

    tfloat64 *actualOutput = dct->MDCT(input);

    for(k=0;k<c_N;k++)
    {
        EXPECT_NEAR(formulaOutput[k],actualOutput[k],c_Tolerance);
    }


    // Inverse MDCT
    for(n=0;n<c_N * 2;n++)
    {
        tfloat64 nF = static_cast<tfloat64>(n);
        tfloat64 y = 0.0;
        
        for(k=0;k<c_N;k++)
        {
            tfloat64 kF = static_cast<tfloat64>(k);
            tfloat64 d = (c_PI_D / NF) * (nF + 0.5 + (NF / 2.0)) * (kF + 0.5);
            y += formulaOutput[k] * cos(d);
        }
        formulaInverse[n] = y;
    }        
    
    tfloat64 *actualInverse = dct->InverseMDCT(actualOutput);
    
    for(k=0;k<c_N * 2;k++)
    {
        EXPECT_NEAR(formulaInverse[k],actualInverse[k],c_Tolerance);
    }
    
    OmegaDCT64::stop();
    
    delete [] input;
    delete [] formulaOutput;
    delete [] formulaInverse;
}

//-------------------------------------------------------------------------------------------
