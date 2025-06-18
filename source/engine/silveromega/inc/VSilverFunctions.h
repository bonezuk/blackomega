//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERFUNCTIONS_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERFUNCTIONS_H
//-------------------------------------------------------------------------------------------

#include <math.h>
#include "common/inc/BString.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

tint inline iLog(tuint v)
{
    tint r = 0;
    
    while(v)
    {
        r++;
        v >>= 1;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

tint inline iLog(tint v)
{
    if(v<0)
    {
        v = -v;
    }
    return iLog(static_cast<tuint>(v));
}

//-------------------------------------------------------------------------------------------

tint inline iLog2(tuint v)
{
    tint r = 0;
    
    if(v)
    {
        --v;
    }
    while(v)
    {
        r++;
        v >>= 1;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

tint inline iLog2(tint v)
{
    if(v<0)
    {
        v = -v;
    }
    return iLog2(static_cast<tuint>(v));
}

//-------------------------------------------------------------------------------------------

tint inline iCount(tint v)
{
    tint r = 0;
    
    while(v)
    {
        r += v & 0x00000001;
        v = static_cast<tint>(static_cast<tuint>(v) >> 1);
    }
    return r;
}

//-------------------------------------------------------------------------------------------

tint inline rint(tint x)
{
    return static_cast<tint>(floor(static_cast<tfloat32>(x) + 0.5f));
}

//-------------------------------------------------------------------------------------------

tint inline rint(tfloat32 x)
{
    return static_cast<tint>(floor(x + 0.5f));
}

//-------------------------------------------------------------------------------------------

tint inline rint(tfloat64 x)
{
    return static_cast<tint>(floor(x + 0.5));
}

//-------------------------------------------------------------------------------------------

const tuint c_VQFExp = 10;
const tuint c_VQFMan = 21;
const tuint c_VQFExpBias = 768;

//-------------------------------------------------------------------------------------------

tfloat32 inline float32unpack(tuint x)
{
    tfloat64 mant = static_cast<tfloat64>(x & 0x001fffff);
    tuint sign = x & 0x80000000;
    tuint exp = (x & 0x7fe00000) >> c_VQFMan;
    
    if(sign)
    {
        mant = -mant;
    }
    return static_cast<tfloat32>(::ldexp(mant,exp-(c_VQFMan-1)-c_VQFExpBias));
}

//-------------------------------------------------------------------------------------------

tint inline vorbis_ftoi(tfloat64 f)
{
    return static_cast<tint>(f + 0.5);
}

//-------------------------------------------------------------------------------------------

const tint c_cosLookupSize = 128;
extern const tfloat32 g_cosLookup[];

tfloat32 inline lookCos(tfloat32 a)
{
    tfloat64 d = static_cast<tfloat64>(a) * (0.31830989 * static_cast<tfloat32>(c_cosLookupSize));
    tint i = vorbis_ftoi(d - 0.5);
    return g_cosLookup[i] + (static_cast<tfloat32>(d) - static_cast<tfloat32>(i)) * (g_cosLookup[i+1] - g_cosLookup[i]);
}

//-------------------------------------------------------------------------------------------

const tint c_invsqLookupSize = 32;
extern const tfloat32 g_invsqLookup[];

tfloat32 inline lookInvSQ(tfloat32 a)
{
    tfloat64 d = a * (2.0f * static_cast<tfloat32>(c_invsqLookupSize)) - static_cast<tfloat32>(c_invsqLookupSize);
    tint i = vorbis_ftoi(d - 0.5);
    return g_invsqLookup[i] + (static_cast<tfloat32>(d) - static_cast<tfloat32>(i)) * (g_invsqLookup[i+1] - g_invsqLookup[i]);
}

//-------------------------------------------------------------------------------------------

const tint c_invsq2ExpLookupMin = -32;
const tint c_invsq2ExpLookupMax =  32;
extern const tfloat32 g_invsq2ExpLookup[];

tfloat32 inline lookInvSQ2Exp(tint a)
{
    return g_invsq2ExpLookup[a - c_invsq2ExpLookupMin];
}

//-------------------------------------------------------------------------------------------

const tint c_fromDB_LookupSize = 35;
const tint c_fromDB2_LookupSize2 = 32;
const tint c_fromDB_Shift = 5;
const tint c_fromDB2_Shift = 3;
const tint c_fromDB2_Mask = 31;
extern const tfloat32 g_fromDBLookup[];
extern const tfloat32 g_fromDB2Lookup[];

tfloat32 inline lookFromDB(tfloat32 a)
{
    tint i = vorbis_ftoi(a * (static_cast<tfloat32>(-(1 << c_fromDB2_Shift))) - 0.5f);
    
    if(i<0)
    {
        return 1.0f;
    }
    else
    {
        if(i>=(c_fromDB_LookupSize << c_fromDB_Shift))
        {
            return 0.0f;
        }
        else
        {
            return g_fromDBLookup[i >> c_fromDB_Shift] * g_fromDB2Lookup[i & c_fromDB2_Mask];
        }
    }
}

//-------------------------------------------------------------------------------------------

const tfloat32 c_PI = 3.141592653589793238464264338832795f;

const tfloat32 c_PI3_8 = 0.38268343236508977175f;
const tfloat32 c_PI2_8 = 0.70710678118654752441f;
const tfloat32 c_PI1_8 = 0.92387953251128675613f;


//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

