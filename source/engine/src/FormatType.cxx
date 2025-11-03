#include "engine/inc/FormatType.h"

#include <limits>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

bool isLittleEndian()
{
    static bool initF = false;
    static bool res = true;

    if(!initF)
    {
        static tubyte m[4] = {0x01,0x02,0x03,0x04};
        tuint32 *b = reinterpret_cast<tuint32 *>(m);

        if((*b)==0x01020304)
        {
            res = false;
        }
        initF = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint32 to32BitSignedFromLittleEndian(const tchar *mem)
{
    return static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
}

//-------------------------------------------------------------------------------------------

tint32 to32BitSignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    return static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
}

//-------------------------------------------------------------------------------------------

tint32 to32BitSignedFromBigEndian(const tchar *mem)
{
    return static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
}

//-------------------------------------------------------------------------------------------

tint32 to32BitSignedFromBigEndian(const tchar *mem,int offset,int len)
{
    return static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
}

//-------------------------------------------------------------------------------------------

tuint32 to32BitUnsignedFromLittleEndian(const tchar *mem)
{
    tuint32 x;

    if(isLittleEndian())
    {
        const tuint32 *y = reinterpret_cast<const tuint32 *>(mem);
        x = *y;
    }
    else
    {
        const tubyte *y = reinterpret_cast<const tubyte *>(mem);
        x = ((static_cast<tuint32>(y[3]) << 24) & 0xff000000) | ((static_cast<tuint32>(y[2]) << 16) & 0x00ff0000)
          | ((static_cast<tuint32>(y[1]) <<  8) & 0x0000ff00) |  (static_cast<tuint32>(y[0]));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 to32BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    tuint32 x;

    if(mem!=0 && (offset+4)<=len)
    {
        x = to32BitUnsignedFromLittleEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 to32BitUnsignedFromBigEndian(const tchar *mem)
{
    tuint32 x;

    if(isLittleEndian())
    {
        const tubyte *y = reinterpret_cast<const tubyte *>(mem);
        x = ((static_cast<tuint32>(y[0]) << 24) & 0xff000000) | ((static_cast<tuint32>(y[1]) << 16) & 0x00ff0000)
          | ((static_cast<tuint32>(y[2]) <<  8) & 0x0000ff00) |  (static_cast<tuint32>(y[3]));
    }
    else
    {
        const tuint32 *y = reinterpret_cast<const tuint32 *>(mem);
        x = *y;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 to32BitUnsignedFromBigEndian(const tchar *mem,int offset,int len)
{
    tuint32 x;

    if(mem!=0 && (offset+4)<=len)
    {
        x = to32BitUnsignedFromBigEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint64 to64BitUnsignedFromLittleEndian(const tchar* mem)
{
    tuint64 x;

    if(isLittleEndian())
    {
        const tuint64* y = reinterpret_cast<const tuint64*>(mem);
        x = *y;
    }
    else
    {
        const tubyte* y = reinterpret_cast<const tubyte*>(mem);

        x =    ((static_cast<tuint64>(y[7]) << 56) & 0xff00000000000000ULL) |
            ((static_cast<tuint64>(y[6]) << 48) & 0x00ff000000000000ULL) |
            ((static_cast<tuint64>(y[5]) << 40) & 0x0000ff0000000000ULL) |
            ((static_cast<tuint64>(y[4]) << 32) & 0x000000ff00000000ULL) |
            ((static_cast<tuint64>(y[3]) << 24) & 0x00000000ff000000ULL) |
            ((static_cast<tuint64>(y[2]) << 16) & 0x0000000000ff0000ULL) |
            ((static_cast<tuint64>(y[1]) << 8) & 0x000000000000ff00ULL) |
            ((static_cast<tuint64>(y[0])) & 0x00000000000000ffULL);
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint64 to64BitUnsignedFromBigEndian(const tchar* mem)
{
    tuint64 x;

    if(!isLittleEndian())
    {
        const tuint64* y = reinterpret_cast<const tuint64*>(mem);
        x = *y;
    }
    else
    {
        const tubyte* y = reinterpret_cast<const tubyte*>(mem);

        x = ((static_cast<tuint64>(y[0]) << 56) & 0xff00000000000000ULL) |
            ((static_cast<tuint64>(y[1]) << 48) & 0x00ff000000000000ULL) |
            ((static_cast<tuint64>(y[2]) << 40) & 0x0000ff0000000000ULL) |
            ((static_cast<tuint64>(y[3]) << 32) & 0x000000ff00000000ULL) |
            ((static_cast<tuint64>(y[4]) << 24) & 0x00000000ff000000ULL) |
            ((static_cast<tuint64>(y[5]) << 16) & 0x0000000000ff0000ULL) |
            ((static_cast<tuint64>(y[6]) << 8) & 0x000000000000ff00ULL) |
            ((static_cast<tuint64>(y[7])) & 0x00000000000000ffULL);
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 to24BitSignedFromLittleEndian(const tchar *mem)
{
    tint32 x;
    tuint32 t = to24BitUnsignedFromLittleEndian(mem);

    if(t & 0x00800000)
    {
        t |= 0xff000000;
    }
    x = static_cast<tint32>(t);
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 to24BitUnsignedFromLittleEndian(const tchar *mem)
{
    tuint32 x;
    const tubyte *y = reinterpret_cast<const tubyte *>(mem);

    x = (static_cast<tuint32>(y[2] << 16) & 0x00ff0000) |
        (static_cast<tuint32>(y[1] <<  8) & 0x0000ff00) |
        (static_cast<tuint32>(y[0]      ) & 0x000000ff);
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 to24BitSignedFromBigEndian(const tchar *mem)
{
    tint32 x;
    tuint32 t = to24BitUnsignedFromBigEndian(mem);

    if(t & 0x00800000)
    {
        t |= 0xff000000;
    }
    x = static_cast<tint32>(t);
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 to24BitUnsignedFromBigEndian(const tchar *mem)
{
    tuint32 x;
    const tubyte *y = reinterpret_cast<const tubyte *>(mem);

    x = (static_cast<tuint32>(y[0] << 16) & 0x00ff0000) |
        (static_cast<tuint32>(y[1] <<  8) & 0x0000ff00) |
        (static_cast<tuint32>(y[2]      ) & 0x000000ff);
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 to24BitSignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x;

    if(mem!=0 && (offset+3)<=len)
    {
        x = to24BitSignedFromLittleEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 to24BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    tuint32 x;

    if(mem!=0 && (offset+3)<=len)
    {
        x = to24BitUnsignedFromLittleEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 to24BitSignedFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x;

    if(mem!=0 && (offset+3)<=len)
    {
        x = to24BitSignedFromBigEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 to24BitUnsignedFromBigEndian(const tchar *mem,int offset,int len)
{
    tuint32 x;

    if(mem!=0 && (offset+3)<=len)
    {
        x = to24BitUnsignedFromBigEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint16 to16BitSignedFromLittleEndian(const tchar *mem)
{
    return static_cast<tint16>(to16BitUnsignedFromLittleEndian(mem));
}

//-------------------------------------------------------------------------------------------

tuint16 to16BitUnsignedFromLittleEndian(const tchar *mem)
{
    tuint16 x;

    if(isLittleEndian())
    {
        const tuint16 *y = reinterpret_cast<const tuint16 *>(mem);
        x = y[0];
    }
    else
    {
        const tubyte *y = reinterpret_cast<const tubyte *>(mem);
        x = (static_cast<tuint16>(y[1] << 8) & static_cast<tuint16>(0xff00)) | (static_cast<tuint16>(y[0]) & static_cast<tuint16>(0x00ff));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint16 to16BitSignedFromBigEndian(const tchar *mem)
{
    return static_cast<tint16>(to16BitUnsignedFromBigEndian(mem));
}

//-------------------------------------------------------------------------------------------

tuint16 to16BitUnsignedFromBigEndian(const tchar *mem)
{
    tuint16 x;

    if(!isLittleEndian())
    {
        const tuint16 *y = reinterpret_cast<const tuint16 *>(mem);
        x = y[0];
    }
    else
    {
        const tubyte *y = reinterpret_cast<const tubyte *>(mem);
        x = (static_cast<tuint16>(y[0] << 8) & static_cast<tuint16>(0xff00)) | (static_cast<tuint16>(y[1]) & static_cast<tuint16>(0x00ff));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint16 to16BitSignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    return static_cast<tint16>(to16BitUnsignedFromLittleEndian(mem,offset,len));
}

//-------------------------------------------------------------------------------------------

tuint16 to16BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    tuint16 x;

    if(mem!=0 && (offset+2)<=len)
    {
        x = to16BitUnsignedFromLittleEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint16 to16BitSignedFromBigEndian(const tchar *mem,int offset,int len)
{
    return static_cast<tint16>(to16BitUnsignedFromBigEndian(mem,offset,len));
}

//-------------------------------------------------------------------------------------------

tuint16 to16BitUnsignedFromBigEndian(const tchar *mem,int offset,int len)
{
    tuint16 x;

    if(mem!=0 && (offset+2)<=len)
    {
        x = to16BitUnsignedFromBigEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tbyte to8BitSignedFromLittleEndian(const tchar *mem)
{
    return mem[0];
}

//-------------------------------------------------------------------------------------------

tubyte to8BitUnsignedFromLittleEndian(const tchar *mem)
{
    return static_cast<tubyte>(mem[0]);
}

//-------------------------------------------------------------------------------------------

tbyte to8BitSignedFromBigEndian(const tchar *mem)
{
    return mem[0];
}

//-------------------------------------------------------------------------------------------

tubyte to8BitUnsignedFromBigEndian(const tchar *mem)
{
    return static_cast<tubyte>(mem[0]);
}

//-------------------------------------------------------------------------------------------

tbyte to8BitSignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    return static_cast<tbyte>(to8BitUnsignedFromLittleEndian(mem,offset,len));
}

//-------------------------------------------------------------------------------------------

tubyte to8BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len)
{
    tubyte x;

    if(mem!=0 && (offset+1)<=len)
    {
        x = to8BitUnsignedFromLittleEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tbyte to8BitSignedFromBigEndian(const tchar *mem,int offset,int len)
{
    return static_cast<tbyte>(to8BitUnsignedFromBigEndian(mem,offset,len));
}

//-------------------------------------------------------------------------------------------

tubyte to8BitUnsignedFromBigEndian(const tchar *mem,int offset,int len)
{
    tubyte x;

    if(mem!=0 && (offset+1)<=len)
    {
        x = to8BitUnsignedFromBigEndian(&mem[offset]);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------
// 1-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom1Bit(const tchar *mem)
{
    return (to8BitUnsignedFromLittleEndian(mem) & 0x01) ? 1.0f : -1.0f;
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom1Bit(const tchar *mem,int offset,int len)
{
    return (to8BitUnsignedFromLittleEndian(mem,offset,len) & 0x01) ? 1.0f : -1.0f;
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From1Bit(const tchar *mem)
{
    return (to8BitUnsignedFromLittleEndian(mem) & 0x01) ? 1.0 : -1.0;
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From1Bit(const tchar *mem,int offset,int len)
{
    return (to8BitUnsignedFromLittleEndian(mem,offset,len) & 0x01) ? 1.0 : -1.0;
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFromBits(tint32 x,tint N)
{
    tuint32 sMin = static_cast<tuint32>(1 << (N - 1));
    tuint32 sPlus = sMin - 1;
    tfloat32 v;

    if(x & sMin)
    {
        x = 0 - (x | (~sPlus));
        v = static_cast<tfloat32>((-1.0 * static_cast<tfloat64>(x)) / static_cast<tfloat64>(sMin));
    }
    else if(x & sPlus)
    {
        v = static_cast<tfloat32>(static_cast<tfloat64>(x) / static_cast<tfloat64>(sPlus));
    }
    else
    {
        v = 0.0f;
    }
    return v;
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64FromBits(tint32 x,tint N)
{
    tuint32 sMin = static_cast<tuint32>(1 << (N - 1));
    tuint32 sPlus = sMin - 1;
    tfloat64 v;

    if(x & sMin)
    {
        x = 0 - (x | (~sPlus));
        v = (-1.0 * static_cast<tfloat64>(x)) / static_cast<tfloat64>(sMin);
    }
    else if(x & sPlus)
    {
        v = static_cast<tfloat64>(x) / static_cast<tfloat64>(sPlus);
    }
    else
    {
        v = 0.0;
    }
    return v;
}

//-------------------------------------------------------------------------------------------
// 2-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom2Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,2);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom2Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,2);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From2Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,2);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From2Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,2);
}


//-------------------------------------------------------------------------------------------
// 3-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom3Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,3);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom3Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,3);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From3Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,3);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From3Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,3);
}

//-------------------------------------------------------------------------------------------
// 4-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom4Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,4);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom4Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,4);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From4Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,4);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From4Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,4);
}

//-------------------------------------------------------------------------------------------
// 5-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom5Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,5);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom5Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,5);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From5Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,5);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From5Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,5);
}

//-------------------------------------------------------------------------------------------
// 6-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom6Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,6);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom6Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,6);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From6Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,6);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From6Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,6);
}

//-------------------------------------------------------------------------------------------
// 7-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom7Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,7);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom7Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,7);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From7Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,7);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From7Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,7);
}

//-------------------------------------------------------------------------------------------
// 8-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom8Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,8);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom8Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,8);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From8Bits(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,8);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From8Bits(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,8);
}

//-------------------------------------------------------------------------------------------
// 9-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom9BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,9);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom9BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,9);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom9BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,9);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom9BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,9);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From9BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,9);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From9BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,9);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From9BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,9);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From9BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,9);
}

//-------------------------------------------------------------------------------------------
// 10-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom10BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,10);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom10BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,10);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom10BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,10);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom10BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,10);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From10BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,10);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From10BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,10);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From10BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,10);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From10BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,10);
}

//-------------------------------------------------------------------------------------------
// 11-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom11BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,11);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom11BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,11);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom11BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,11);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom11BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,11);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From11BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,11);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From11BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,11);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From11BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,11);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From11BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,11);
}

//-------------------------------------------------------------------------------------------
// 12-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom12BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,12);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom12BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,12);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom12BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,12);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom12BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,12);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From12BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,12);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From12BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,12);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From12BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,12);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From12BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,12);
}

//-------------------------------------------------------------------------------------------
// 13-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom13BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,13);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom13BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,13);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom13BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,13);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom13BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,13);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From13BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,13);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From13BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,13);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From13BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,13);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From13BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,13);
}

//-------------------------------------------------------------------------------------------
// 14-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom14BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,14);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom14BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,14);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom14BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,14);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom14BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,14);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From14BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,14);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From14BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,14);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From14BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,14);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From14BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,14);
}

//-------------------------------------------------------------------------------------------
// 15-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom15BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,15);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom15BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,15);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom15BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,15);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom15BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,15);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From15BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,15);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From15BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,15);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From15BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,15);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From15BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,15);
}

//-------------------------------------------------------------------------------------------
// 16-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom16BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,16);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom16BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,16);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom16BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,16);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom16BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,16);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From16BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,16);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From16BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,16);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From16BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,16);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From16BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to16BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,16);
}

//-------------------------------------------------------------------------------------------
// 17-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom17BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,17);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom17BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,17);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom17BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,17);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom17BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,17);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From17BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,17);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From17BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,17);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From17BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,17);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From17BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,17);
}

//-------------------------------------------------------------------------------------------
// 18-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom18BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,18);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom18BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,18);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom18BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,18);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom18BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,18);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From18BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,18);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From18BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,18);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From18BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,18);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From18BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,18);
}

//-------------------------------------------------------------------------------------------
// 19-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom19BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,19);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom19BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,19);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom19BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,19);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom19BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,19);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From19BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,19);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From19BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,19);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From19BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,19);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From19BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,19);
}

//-------------------------------------------------------------------------------------------
// 20-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom20BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,20);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom20BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,20);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom20BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,20);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom20BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,20);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From20BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,20);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From20BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,20);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From20BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,20);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From20BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,20);
}

//-------------------------------------------------------------------------------------------
// 21-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom21BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,21);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom21BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,21);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom21BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,21);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom21BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,21);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From21BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,21);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From21BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,21);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From21BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,21);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From21BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,21);
}

//-------------------------------------------------------------------------------------------
// 22-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom22BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,22);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom22BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,22);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom22BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,22);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom22BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,22);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From22BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,22);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From22BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,22);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From22BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,22);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From22BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,22);
}

//-------------------------------------------------------------------------------------------
// 23-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom23BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,23);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom23BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,23);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom23BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,23);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom23BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,23);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From23BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,23);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From23BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,23);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From23BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,23);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From23BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,23);
}

//-------------------------------------------------------------------------------------------
// 24-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom24BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,24);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom24BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,24);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom24BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,24);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom24BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,24);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From24BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,24);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From24BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,24);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From24BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,24);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From24BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to24BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,24);
}

//-------------------------------------------------------------------------------------------
// 25-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom25BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,25);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom25BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,25);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom25BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,25);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom25BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,25);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From25BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,25);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From25BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,25);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From25BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,25);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From25BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,25);
}

//-------------------------------------------------------------------------------------------
// 26-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom26BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,26);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom26BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,26);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom26BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,26);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom26BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,26);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From26BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,26);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From26BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,26);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From26BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,26);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From26BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,26);
}

//-------------------------------------------------------------------------------------------
// 27-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom27BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,27);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom27BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,27);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom27BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,27);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom27BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,27);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From27BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,27);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From27BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,27);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From27BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,27);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From27BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,27);
}

//-------------------------------------------------------------------------------------------
// 28-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom28BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,28);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom28BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,28);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom28BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,28);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom28BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,28);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From28BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,28);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From28BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,28);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From28BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,28);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From28BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,28);
}

//-------------------------------------------------------------------------------------------
// 29-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom29BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,29);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom29BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,29);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom29BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,29);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom29BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,29);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From29BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,29);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From29BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,29);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From29BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,29);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From29BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,29);
}

//-------------------------------------------------------------------------------------------
// 30-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom30BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,30);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom30BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,30);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom30BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,30);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom30BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,30);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From30BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,30);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From30BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,30);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From30BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,30);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From30BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,30);
}

//-------------------------------------------------------------------------------------------
// 31-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom31BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSampleFromBits(x,31);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom31BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSampleFromBits(x,31);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom31BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSampleFromBits(x,31);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom31BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSampleFromBits(x,31);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From31BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem));
    return toSample64FromBits(x,31);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From31BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromLittleEndian(mem,offset,len));
    return toSample64FromBits(x,31);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From31BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem));
    return toSample64FromBits(x,31);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From31BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitUnsignedFromBigEndian(mem,offset,len));
    return toSample64FromBits(x,31);
}

//-------------------------------------------------------------------------------------------
// 32-Bit Sample
//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom32BitSignedInteger(tint32 x)
{
    tfloat64 v = static_cast<tfloat64>(x);
    if(v >= 0.0)
    {
        v /= 2147483647.0;
    }
    else
    {
        v /= 2147483648.0;
    }
    return static_cast<tfloat32>(v);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From32BitSignedInteger(tint32 x)
{
    tfloat64 v = static_cast<tfloat64>(x);
    if(v >= 0.0)
    {
        v /= 2147483647.0;
    }
    else
    {
        v /= 2147483648.0;
    }
    return v;
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom32BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(mem));
    return toSampleFrom32BitSignedInteger(x);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom32BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(mem,offset,len));
    return toSampleFrom32BitSignedInteger(x);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom32BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromBigEndian(mem));
    return toSampleFrom32BitSignedInteger(x);
}

//-------------------------------------------------------------------------------------------

tfloat32 toSampleFrom32BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromBigEndian(mem,offset,len));
    return toSampleFrom32BitSignedInteger(x);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From32BitsFromLittleEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(mem));
    return toSample64From32BitSignedInteger(x);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From32BitsFromLittleEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(mem,offset,len));
    return toSample64From32BitSignedInteger(x);
}
//-------------------------------------------------------------------------------------------

tfloat64 toSample64From32BitsFromBigEndian(const tchar *mem)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromBigEndian(mem));
    return toSample64From32BitSignedInteger(x);
}

//-------------------------------------------------------------------------------------------

tfloat64 toSample64From32BitsFromBigEndian(const tchar *mem,int offset,int len)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromBigEndian(mem,offset,len));
    return toSample64From32BitSignedInteger(x);
}

//-------------------------------------------------------------------------------------------

tfloat64 unsignedIntegerToFloat(tuint32 x)
{
    return static_cast<tfloat64>(static_cast<tint32>(x - 0x80000000)) + 2147483648.0;
}

//-------------------------------------------------------------------------------------------

tfloat64 doubleFromExtended(const tbyte *mem)
{
    return doubleFromExtended(reinterpret_cast<const tubyte *>(mem));
}

//-------------------------------------------------------------------------------------------

tfloat64 doubleFromExtended(const tubyte *mem)
{
    tint expon;
    tuint32 hiMant,loMant;
    tfloat64 f;

    expon = static_cast<tint>(((static_cast<tuint32>(mem[0]) << 8) & 0x00007f00) | (static_cast<tuint32>(mem[1]) & 0x000000ff));

    hiMant = ((static_cast<tuint32>(mem[2]) << 24) & 0xff000000) |
             ((static_cast<tuint32>(mem[3]) << 16) & 0x00ff0000) |
             ((static_cast<tuint32>(mem[4]) <<  8) & 0x0000ff00) |
             ((static_cast<tuint32>(mem[5])      ) & 0x000000ff);

    loMant = ((static_cast<tuint32>(mem[6]) << 24) & 0xff000000) |
             ((static_cast<tuint32>(mem[7]) << 16) & 0x00ff0000) |
             ((static_cast<tuint32>(mem[8]) <<  8) & 0x0000ff00) |
             ((static_cast<tuint32>(mem[9])      ) & 0x000000ff);

    if(expon==0 && hiMant==0 && loMant==0)
    {
        f = 0;
    }
    else
    {
        if(expon==0x00007fff)
        {
            f = std::numeric_limits<tfloat64>::quiet_NaN();
        }
        else
        {
            expon -= 16383;
            f  = ::ldexp(unsignedIntegerToFloat(hiMant),expon-=31);
            f += ::ldexp(unsignedIntegerToFloat(loMant),expon-=32);
        }
    }
    return (mem[0] & 0x80) ? -f : f;
}

//-------------------------------------------------------------------------------------------

void shiftLeft32BitsLittleEndian(const tubyte *x,tubyte *y,int s)
{
    static const tubyte maskA[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    static const tubyte maskB[8] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

    int t = 8 - s;
    y[3] =  (x[3] >> s) & maskA[s];
    y[2] = ((x[3] << t) & maskB[s]) | ((x[2] >> s) & maskA[s]);
    y[1] = ((x[2] << t) & maskB[s]) | ((x[1] >> s) & maskA[s]);
    y[0] = ((x[1] << t) & maskB[s]) | ((x[0] >> s) & maskA[s]);
}

//-------------------------------------------------------------------------------------------

void shiftLeft24BitsLittleEndian(const tubyte *x,tubyte *y,int s)
{
    static const tubyte maskA[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    static const tubyte maskB[8] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

    int t = 8 - s;
    y[2] =  (x[2] >> s) & maskA[s];
    y[1] = ((x[2] << t) & maskB[s]) | ((x[1] >> s) & maskA[s]);
    y[0] = ((x[1] << t) & maskB[s]) | ((x[0] >> s) & maskA[s]);
}

//-------------------------------------------------------------------------------------------

void shiftLeft16BitsLittleEndian(const tubyte *x,tubyte *y,int s)
{
    static const tubyte maskA[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    static const tubyte maskB[8] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

    int t = 8 - s;
    y[1] =  (x[1] >> s) & maskA[s];
    y[0] = ((x[1] << t) & maskB[s]) | ((x[0] >> s) & maskA[s]);
}

//-------------------------------------------------------------------------------------------

void shiftLeft32BitsBigEndian(const tubyte *x,tubyte *y,int s)
{
    static const tubyte maskA[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    static const tubyte maskB[8] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

    int t = 8 - s;
    y[0] =  (x[0] >> s) & maskA[s];
    y[1] = ((x[0] << t) & maskB[s]) | ((x[1] >> s) & maskA[s]);
    y[2] = ((x[1] << t) & maskB[s]) | ((x[2] >> s) & maskA[s]);
    y[3] = ((x[2] << t) & maskB[s]) | ((x[3] >> s) & maskA[s]);
}

//-------------------------------------------------------------------------------------------

void shiftLeft24BitsBigEndian(const tubyte *x,tubyte *y,int s)
{
    static const tubyte maskA[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    static const tubyte maskB[8] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

    int t = 8 - s;
    y[0] =  (x[0] >> s) & maskA[s];
    y[1] = ((x[0] << t) & maskB[s]) | ((x[1] >> s) & maskA[s]);
    y[2] = ((x[1] << t) & maskB[s]) | ((x[2] >> s) & maskA[s]);
}

//-------------------------------------------------------------------------------------------

void shiftLeft16BitsBigEndian(const tubyte *x,tubyte *y,int s)
{
    static const tubyte maskA[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    static const tubyte maskB[8] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

    int t = 8 - s;
    y[0] =  (x[0] >> s) & maskA[s];
    y[1] = ((x[0] << t) & maskB[s]) | ((x[1] >> s) & maskA[s]);
}

//-------------------------------------------------------------------------------------------

void shiftLeft8Bits(const tubyte *x,tubyte *y,int s)
{
    static const tubyte maskA[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    y[0] =  (x[0] >> s) & maskA[s];
}

//-------------------------------------------------------------------------------------------

tfloat32 readSampleLittleEndian(const tbyte *mem,int noBits)
{
    return readSampleLittleEndian(reinterpret_cast<const tubyte *>(mem),noBits);
}

//-------------------------------------------------------------------------------------------

tfloat32 readSampleLittleEndian(const tubyte *mem,int noBits)
{
    tfloat32 x;
    tubyte y[4];
    tbyte *z = reinterpret_cast<tbyte *>(y);

    switch(noBits)
    {
        case 1:
            shiftLeft8Bits(mem,y,7);
            x = toSampleFrom1Bit(z);
            break;

        case 2:
            shiftLeft8Bits(mem,y,6);
            x = toSampleFrom2Bits(z);
            break;

        case 3:
            shiftLeft8Bits(mem,y,5);
            x = toSampleFrom3Bits(z);
            break;

        case 4:
            shiftLeft8Bits(mem,y,4);
            x = toSampleFrom4Bits(z);
            break;

        case 5:
            shiftLeft8Bits(mem,y,3);
            x = toSampleFrom5Bits(z);
            break;

        case 6:
            shiftLeft8Bits(mem,y,2);
            x = toSampleFrom6Bits(z);
            break;

        case 7:
            shiftLeft8Bits(mem,y,1);
            x = toSampleFrom7Bits(z);
            break;

        case 8:
            x = toSampleFrom8Bits(reinterpret_cast<const tbyte *>(mem));
            break;

        case 9:
            shiftLeft16BitsLittleEndian(mem,y,7);
            x = toSampleFrom9BitsFromLittleEndian(z);
            break;

        case 10:
            shiftLeft16BitsLittleEndian(mem,y,6);
            x = toSampleFrom10BitsFromLittleEndian(z);
            break;

        case 11:
            shiftLeft16BitsLittleEndian(mem,y,5);
            x = toSampleFrom11BitsFromLittleEndian(z);
            break;

        case 12:
            shiftLeft16BitsLittleEndian(mem,y,4);
            x = toSampleFrom12BitsFromLittleEndian(z);
            break;

        case 13:
            shiftLeft16BitsLittleEndian(mem,y,3);
            x = toSampleFrom13BitsFromLittleEndian(z);
            break;

        case 14:
            shiftLeft16BitsLittleEndian(mem,y,2);
            x = toSampleFrom14BitsFromLittleEndian(z);
            break;

        case 15:
            shiftLeft16BitsLittleEndian(mem,y,1);
            x = toSampleFrom15BitsFromLittleEndian(z);
            break;

        case 16:
            x = toSampleFrom16BitsFromLittleEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 17:
            shiftLeft24BitsLittleEndian(mem,y,7);
            x = toSampleFrom17BitsFromLittleEndian(z);
            break;

        case 18:
            shiftLeft24BitsLittleEndian(mem,y,6);
            x = toSampleFrom18BitsFromLittleEndian(z);
            break;

        case 19:
            shiftLeft24BitsLittleEndian(mem,y,5);
            x = toSampleFrom19BitsFromLittleEndian(z);
            break;

        case 20:
            shiftLeft24BitsLittleEndian(mem,y,4);
            x = toSampleFrom20BitsFromLittleEndian(z);
            break;

        case 21:
            shiftLeft24BitsLittleEndian(mem,y,3);
            x = toSampleFrom21BitsFromLittleEndian(z);
            break;

        case 22:
            shiftLeft24BitsLittleEndian(mem,y,2);
            x = toSampleFrom22BitsFromLittleEndian(z);
            break;

        case 23:
            shiftLeft24BitsLittleEndian(mem,y,1);
            x = toSampleFrom23BitsFromLittleEndian(z);
            break;

        case 24:
            x = toSampleFrom24BitsFromLittleEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 25:
            shiftLeft32BitsLittleEndian(mem,y,7);
            x = toSampleFrom25BitsFromLittleEndian(z);
            break;

        case 26:
            shiftLeft32BitsLittleEndian(mem,y,6);
            x = toSampleFrom26BitsFromLittleEndian(z);
            break;

        case 27:
            shiftLeft32BitsLittleEndian(mem,y,5);
            x = toSampleFrom27BitsFromLittleEndian(z);
            break;

        case 28:
            shiftLeft32BitsLittleEndian(mem,y,4);
            x = toSampleFrom28BitsFromLittleEndian(z);
            break;

        case 29:
            shiftLeft32BitsLittleEndian(mem,y,3);
            x = toSampleFrom29BitsFromLittleEndian(z);
            break;

        case 30:
            shiftLeft32BitsLittleEndian(mem,y,2);
            x = toSampleFrom30BitsFromLittleEndian(z);
            break;

        case 31:
            shiftLeft32BitsLittleEndian(mem,y,1);
            x = toSampleFrom31BitsFromLittleEndian(z);
            break;

        case 32:
        default:
            x = toSampleFrom32BitsFromLittleEndian(reinterpret_cast<const tbyte *>(mem));
            break;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tfloat32 readSampleBigEndian(const tbyte *mem,int noBits)
{
    return readSampleBigEndian(reinterpret_cast<const tubyte *>(mem),noBits);
}

//-------------------------------------------------------------------------------------------

tfloat32 readSampleBigEndian(const tubyte *mem,int noBits)
{
    tfloat32 x;
    tubyte y[4];
    tbyte *z = reinterpret_cast<tbyte *>(y);

    switch(noBits)
    {
        case 1:
            shiftLeft8Bits(mem,y,7);
            x = toSampleFrom1Bit(z);
            break;

        case 2:
            shiftLeft8Bits(mem,y,6);
            x = toSampleFrom2Bits(z);
            break;

        case 3:
            shiftLeft8Bits(mem,y,5);
            x = toSampleFrom3Bits(z);
            break;

        case 4:
            shiftLeft8Bits(mem,y,4);
            x = toSampleFrom4Bits(z);
            break;

        case 5:
            shiftLeft8Bits(mem,y,3);
            x = toSampleFrom5Bits(z);
            break;

        case 6:
            shiftLeft8Bits(mem,y,2);
            x = toSampleFrom6Bits(z);
            break;

        case 7:
            shiftLeft8Bits(mem,y,1);
            x = toSampleFrom7Bits(z);
            break;

        case 8:
            x = toSampleFrom8Bits(reinterpret_cast<const tbyte *>(mem));
            break;

        case 9:
            shiftLeft16BitsBigEndian(mem,y,7);
            x = toSampleFrom9BitsFromBigEndian(z);
            break;

        case 10:
            shiftLeft16BitsBigEndian(mem,y,6);
            x = toSampleFrom10BitsFromBigEndian(z);
            break;

        case 11:
            shiftLeft16BitsBigEndian(mem,y,5);
            x = toSampleFrom11BitsFromBigEndian(z);
            break;

        case 12:
            shiftLeft16BitsBigEndian(mem,y,4);
            x = toSampleFrom12BitsFromBigEndian(z);
            break;

        case 13:
            shiftLeft16BitsBigEndian(mem,y,3);
            x = toSampleFrom13BitsFromBigEndian(z);
            break;

        case 14:
            shiftLeft16BitsBigEndian(mem,y,2);
            x = toSampleFrom14BitsFromBigEndian(z);
            break;

        case 15:
            shiftLeft16BitsBigEndian(mem,y,1);
            x = toSampleFrom15BitsFromBigEndian(z);
            break;

        case 16:
            x = toSampleFrom16BitsFromBigEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 17:
            shiftLeft24BitsBigEndian(mem,y,7);
            x = toSampleFrom17BitsFromBigEndian(z);
            break;

        case 18:
            shiftLeft24BitsBigEndian(mem,y,6);
            x = toSampleFrom18BitsFromBigEndian(z);
            break;

        case 19:
            shiftLeft24BitsBigEndian(mem,y,5);
            x = toSampleFrom19BitsFromBigEndian(z);
            break;

        case 20:
            shiftLeft24BitsBigEndian(mem,y,4);
            x = toSampleFrom20BitsFromBigEndian(z);
            break;

        case 21:
            shiftLeft24BitsBigEndian(mem,y,3);
            x = toSampleFrom21BitsFromBigEndian(z);
            break;

        case 22:
            shiftLeft24BitsBigEndian(mem,y,2);
            x = toSampleFrom22BitsFromBigEndian(z);
            break;

        case 23:
            shiftLeft24BitsBigEndian(mem,y,1);
            x = toSampleFrom23BitsFromBigEndian(z);
            break;

        case 24:
            x = toSampleFrom24BitsFromBigEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 25:
            shiftLeft32BitsBigEndian(mem,y,7);
            x = toSampleFrom25BitsFromBigEndian(z);
            break;

        case 26:
            shiftLeft32BitsBigEndian(mem,y,6);
            x = toSampleFrom26BitsFromBigEndian(z);
            break;

        case 27:
            shiftLeft32BitsBigEndian(mem,y,5);
            x = toSampleFrom27BitsFromBigEndian(z);
            break;

        case 28:
            shiftLeft32BitsBigEndian(mem,y,4);
            x = toSampleFrom28BitsFromBigEndian(z);
            break;

        case 29:
            shiftLeft32BitsBigEndian(mem,y,3);
            x = toSampleFrom29BitsFromBigEndian(z);
            break;

        case 30:
            shiftLeft32BitsBigEndian(mem,y,2);
            x = toSampleFrom30BitsFromBigEndian(z);
            break;

        case 31:
            shiftLeft32BitsBigEndian(mem,y,1);
            x = toSampleFrom31BitsFromBigEndian(z);
            break;

        case 32:
        default:
            x = toSampleFrom32BitsFromBigEndian(reinterpret_cast<const tbyte *>(mem));
            break;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tfloat64 readSample64LittleEndian(const tbyte *mem,int noBits)
{
    return readSample64LittleEndian(reinterpret_cast<const tubyte *>(mem),noBits);
}

//-------------------------------------------------------------------------------------------

tfloat64 readSample64LittleEndian(const tubyte *mem,int noBits)
{
    tfloat64 x;
    tubyte y[4];
    tbyte *z = reinterpret_cast<tbyte *>(y);

    switch(noBits)
    {
        case 1:
            shiftLeft8Bits(mem,y,7);
            x = toSample64From1Bit(z);
            break;

        case 2:
            shiftLeft8Bits(mem,y,6);
            x = toSample64From2Bits(z);
            break;

        case 3:
            shiftLeft8Bits(mem,y,5);
            x = toSample64From3Bits(z);
            break;

        case 4:
            shiftLeft8Bits(mem,y,4);
            x = toSample64From4Bits(z);
            break;

        case 5:
            shiftLeft8Bits(mem,y,3);
            x = toSample64From5Bits(z);
            break;

        case 6:
            shiftLeft8Bits(mem,y,2);
            x = toSample64From6Bits(z);
            break;

        case 7:
            shiftLeft8Bits(mem,y,1);
            x = toSample64From7Bits(z);
            break;

        case 8:
            x = toSample64From8Bits(reinterpret_cast<const tbyte *>(mem));
            break;

        case 9:
            shiftLeft16BitsLittleEndian(mem,y,7);
            x = toSample64From9BitsFromLittleEndian(z);
            break;

        case 10:
            shiftLeft16BitsLittleEndian(mem,y,6);
            x = toSample64From10BitsFromLittleEndian(z);
            break;

        case 11:
            shiftLeft16BitsLittleEndian(mem,y,5);
            x = toSample64From11BitsFromLittleEndian(z);
            break;

        case 12:
            shiftLeft16BitsLittleEndian(mem,y,4);
            x = toSample64From12BitsFromLittleEndian(z);
            break;

        case 13:
            shiftLeft16BitsLittleEndian(mem,y,3);
            x = toSample64From13BitsFromLittleEndian(z);
            break;

        case 14:
            shiftLeft16BitsLittleEndian(mem,y,2);
            x = toSample64From14BitsFromLittleEndian(z);
            break;

        case 15:
            shiftLeft16BitsLittleEndian(mem,y,1);
            x = toSample64From15BitsFromLittleEndian(z);
            break;

        case 16:
            x = toSample64From16BitsFromLittleEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 17:
            shiftLeft24BitsLittleEndian(mem,y,7);
            x = toSample64From17BitsFromLittleEndian(z);
            break;

        case 18:
            shiftLeft24BitsLittleEndian(mem,y,6);
            x = toSample64From18BitsFromLittleEndian(z);
            break;

        case 19:
            shiftLeft24BitsLittleEndian(mem,y,5);
            x = toSample64From19BitsFromLittleEndian(z);
            break;

        case 20:
            shiftLeft24BitsLittleEndian(mem,y,4);
            x = toSample64From20BitsFromLittleEndian(z);
            break;

        case 21:
            shiftLeft24BitsLittleEndian(mem,y,3);
            x = toSample64From21BitsFromLittleEndian(z);
            break;

        case 22:
            shiftLeft24BitsLittleEndian(mem,y,2);
            x = toSample64From22BitsFromLittleEndian(z);
            break;

        case 23:
            shiftLeft24BitsLittleEndian(mem,y,1);
            x = toSample64From23BitsFromLittleEndian(z);
            break;

        case 24:
            x = toSample64From24BitsFromLittleEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 25:
            shiftLeft32BitsLittleEndian(mem,y,7);
            x = toSample64From25BitsFromLittleEndian(z);
            break;

        case 26:
            shiftLeft32BitsLittleEndian(mem,y,6);
            x = toSample64From26BitsFromLittleEndian(z);
            break;

        case 27:
            shiftLeft32BitsLittleEndian(mem,y,5);
            x = toSample64From27BitsFromLittleEndian(z);
            break;

        case 28:
            shiftLeft32BitsLittleEndian(mem,y,4);
            x = toSample64From28BitsFromLittleEndian(z);
            break;

        case 29:
            shiftLeft32BitsLittleEndian(mem,y,3);
            x = toSample64From29BitsFromLittleEndian(z);
            break;

        case 30:
            shiftLeft32BitsLittleEndian(mem,y,2);
            x = toSample64From30BitsFromLittleEndian(z);
            break;

        case 31:
            shiftLeft32BitsLittleEndian(mem,y,1);
            x = toSample64From31BitsFromLittleEndian(z);
            break;

        case 32:
        default:
            x = toSample64From32BitsFromLittleEndian(reinterpret_cast<const tbyte *>(mem));
            break;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tfloat64 readSample64BigEndian(const tbyte *mem,int noBits)
{
    return readSample64BigEndian(reinterpret_cast<const tubyte *>(mem),noBits);
}

//-------------------------------------------------------------------------------------------

tfloat64 readSample64BigEndian(const tubyte *mem,int noBits)
{
    tfloat64 x;
    tubyte y[4];
    tbyte *z = reinterpret_cast<tbyte *>(y);

    switch(noBits)
    {
        case 1:
            shiftLeft8Bits(mem,y,7);
            x = toSample64From1Bit(z);
            break;

        case 2:
            shiftLeft8Bits(mem,y,6);
            x = toSample64From2Bits(z);
            break;

        case 3:
            shiftLeft8Bits(mem,y,5);
            x = toSample64From3Bits(z);
            break;

        case 4:
            shiftLeft8Bits(mem,y,4);
            x = toSample64From4Bits(z);
            break;

        case 5:
            shiftLeft8Bits(mem,y,3);
            x = toSample64From5Bits(z);
            break;

        case 6:
            shiftLeft8Bits(mem,y,2);
            x = toSample64From6Bits(z);
            break;

        case 7:
            shiftLeft8Bits(mem,y,1);
            x = toSample64From7Bits(z);
            break;

        case 8:
            x = toSample64From8Bits(reinterpret_cast<const tbyte *>(mem));
            break;

        case 9:
            shiftLeft16BitsBigEndian(mem,y,7);
            x = toSample64From9BitsFromBigEndian(z);
            break;

        case 10:
            shiftLeft16BitsBigEndian(mem,y,6);
            x = toSample64From10BitsFromBigEndian(z);
            break;

        case 11:
            shiftLeft16BitsBigEndian(mem,y,5);
            x = toSample64From11BitsFromBigEndian(z);
            break;

        case 12:
            shiftLeft16BitsBigEndian(mem,y,4);
            x = toSample64From12BitsFromBigEndian(z);
            break;

        case 13:
            shiftLeft16BitsBigEndian(mem,y,3);
            x = toSample64From13BitsFromBigEndian(z);
            break;

        case 14:
            shiftLeft16BitsBigEndian(mem,y,2);
            x = toSample64From14BitsFromBigEndian(z);
            break;

        case 15:
            shiftLeft16BitsBigEndian(mem,y,1);
            x = toSample64From15BitsFromBigEndian(z);
            break;

        case 16:
            x = toSample64From16BitsFromBigEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 17:
            shiftLeft24BitsBigEndian(mem,y,7);
            x = toSample64From17BitsFromBigEndian(z);
            break;

        case 18:
            shiftLeft24BitsBigEndian(mem,y,6);
            x = toSample64From18BitsFromBigEndian(z);
            break;

        case 19:
            shiftLeft24BitsBigEndian(mem,y,5);
            x = toSample64From19BitsFromBigEndian(z);
            break;

        case 20:
            shiftLeft24BitsBigEndian(mem,y,4);
            x = toSample64From20BitsFromBigEndian(z);
            break;

        case 21:
            shiftLeft24BitsBigEndian(mem,y,3);
            x = toSample64From21BitsFromBigEndian(z);
            break;

        case 22:
            shiftLeft24BitsBigEndian(mem,y,2);
            x = toSample64From22BitsFromBigEndian(z);
            break;

        case 23:
            shiftLeft24BitsBigEndian(mem,y,1);
            x = toSample64From23BitsFromBigEndian(z);
            break;

        case 24:
            x = toSample64From24BitsFromBigEndian(reinterpret_cast<const tbyte *>(mem));
            break;

        case 25:
            shiftLeft32BitsBigEndian(mem,y,7);
            x = toSample64From25BitsFromBigEndian(z);
            break;

        case 26:
            shiftLeft32BitsBigEndian(mem,y,6);
            x = toSample64From26BitsFromBigEndian(z);
            break;

        case 27:
            shiftLeft32BitsBigEndian(mem,y,5);
            x = toSample64From27BitsFromBigEndian(z);
            break;

        case 28:
            shiftLeft32BitsBigEndian(mem,y,4);
            x = toSample64From28BitsFromBigEndian(z);
            break;

        case 29:
            shiftLeft32BitsBigEndian(mem,y,3);
            x = toSample64From29BitsFromBigEndian(z);
            break;

        case 30:
            shiftLeft32BitsBigEndian(mem,y,2);
            x = toSample64From30BitsFromBigEndian(z);
            break;

        case 31:
            shiftLeft32BitsBigEndian(mem,y,1);
            x = toSample64From31BitsFromBigEndian(z);
            break;

        case 32:
        default:
            x = toSample64From32BitsFromBigEndian(reinterpret_cast<const tbyte *>(mem));
            break;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

void write16BitsLittleEndianFromSampleInt16(tint16 v, tchar *mem)
{
    mem[0] = static_cast<tchar>(v & 0x00ff);
    mem[1] = static_cast<tchar>((v >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void write16BitsLittleEndianFromSampleInt24(tint32 v, tchar *mem)
{
    if((v & 0x00000080) && v < 8388607)
    {
        v += 0x00000100;
    }
    mem[0] = static_cast<tchar>((v >> 8) & 0x000000ff);
    mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write16BitsLittleEndianFromSampleInt32(tint32 v, tchar *mem)
{
    if((v & 0x00008000) && v < 2147483647)
    {
        v += 0x00010000;
    }
    mem[0] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[1] = static_cast<tchar>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write16BitsBigEndianFromSampleInt16(tint16 v, tchar *mem)
{
    mem[1] = static_cast<tchar>(v & 0x00ff);
    mem[0] = static_cast<tchar>((v >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void write16BitsBigEndianFromSampleInt24(tint32 v, tchar *mem)
{
    if((v & 0x00000080) && v < 8388607)
    {
        v += 0x00000100;
    }
    mem[1] = static_cast<tchar>((v >> 8) & 0x000000ff);
    mem[0] = static_cast<tchar>((v >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write16BitsBigEndianFromSampleInt32(tint32 v, tchar *mem)
{
    if((v & 0x00008000) && v < 2147483647)
    {
        v += 0x00010000;
    }
    mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[0] = static_cast<tchar>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write24BitsLittleEndianFromSampleInt16(tint16 v,tchar *mem)
{
    mem[0] = 0x00;
    mem[1] = static_cast<tchar>(v & 0x00ff);
    mem[2] = static_cast<tchar>((v >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void write24BitsLittleEndianFromSampleInt24(tint32 v,tchar *mem)
{
    mem[0] = static_cast<tchar>(v & 0x000000ff);
    mem[1] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[2] = static_cast<tchar>((v >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write24BitsLittleEndianFromSampleInt32(tint32 v,tchar *mem)
{
    if((v & 0x00000080) && v < 2147483647)
    {
        v += 0x00000100;
    }
    mem[0] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[2] = static_cast<tchar>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write24BitsBigEndianFromSampleInt16(tint16 v,tchar *mem)
{
    mem[2] = 0x00;
    mem[1] = static_cast<tchar>(v & 0x00ff);
    mem[0] = static_cast<tchar>((v >> 8) & 0x00ff);
}
//-------------------------------------------------------------------------------------------

void write24BitsBigEndianFromSampleInt24(tint32 v,tchar *mem)
{
    mem[2] = static_cast<tchar>(v & 0x000000ff);
    mem[1] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[0] = static_cast<tchar>((v >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write24BitsBigEndianFromSampleInt32(tint32 v,tchar *mem)
{
    if((v & 0x00000080) && v < 2147483647)
    {
        v += 0x00000100;
    }
    mem[2] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[0] = static_cast<tchar>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsLittleEndianFromSampleInt16(tint16 v,tchar *mem)
{
    mem[0] = 0x00;
    mem[1] = 0x00;
    mem[2] = static_cast<tchar>(v & 0x00ff);
    mem[3] = static_cast<tchar>((v >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsLittleEndianFromSampleInt24(tint32 v,tchar *mem)
{
    mem[0] = 0x00;
    mem[1] = static_cast<tchar>(v & 0x000000ff);
    mem[2] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[3] = static_cast<tchar>((v >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsLittleEndianFromSampleInt32(tint32 v,tchar *mem)
{
    mem[0] = static_cast<tchar>(v & 0x000000ff);
    mem[1] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[2] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[3] = static_cast<tchar>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsBigEndianFromSampleInt16(tint16 v,tchar *mem)
{
    mem[3] = 0x00;
    mem[2] = 0x00;
    mem[1] = static_cast<tchar>(v & 0x00ff);
    mem[0] = static_cast<tchar>((v >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsBigEndianFromSampleInt24(tint32 v,tchar *mem)
{
    mem[3] = 0x00;
    mem[2] = static_cast<tchar>(v & 0x000000ff);
    mem[1] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[0] = static_cast<tchar>((v >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsBigEndianFromSampleInt32(tint32 v,tchar *mem)
{
    mem[3] = static_cast<tchar>(v & 0x000000ff);
    mem[2] = static_cast<tchar>((v >>  8) & 0x000000ff);
    mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[0] = static_cast<tchar>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB16FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[0] = static_cast<tchar>(v & 0x00ff);
    mem[1] = static_cast<tchar>((v >> 8) & 0x00ff);
    if(v < 0)
    {
        x[2] = 0xff;
        x[3] = 0xff;
    }
    else
    {
        x[2] = 0x00;
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB16FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    if((v & 0x00000080) && v < 8388607)
    {
        v += 0x00000100;
    }
    mem[0] = static_cast<tchar>((v >> 8) & 0x000000ff);
    mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
    if(v < 0)
    {
        x[2] = 0xff;
        x[3] = 0xff;
    }
    else
    {
        x[2] = 0x00;
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB16FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    if((v & 0x00008000) && v < 2147483647)
    {
        v += 0x00010000;
    }
    mem[0] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[1] = static_cast<tchar>((v >> 24) & 0x000000ff);
    if(v < 0)
    {
        x[2] = 0xff;
        x[3] = 0xff;
    }
    else
    {
        x[2] = 0x00;
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB16FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    mem[3] = static_cast<tchar>(v & 0x00ff);
    mem[2] = static_cast<tchar>((v >> 8) & 0x00ff);
    if(v < 0)
    {
        x[1] = 0xff;
        x[0] = 0xff;
    }
    else
    {
        x[1] = 0x00;
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB16FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    if((v & 0x00000080) && v < 8388607)
    {
        v += 0x00000100;
    }
    mem[3] = static_cast<tchar>((v >> 8) & 0x000000ff);
    mem[2] = static_cast<tchar>((v >> 16) & 0x000000ff);
    if(v < 0)
    {
        x[1] = 0xff;
        x[0] = 0xff;
    }
    else
    {
        x[1] = 0x00;
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB16FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    if((v & 0x00008000) && v < 2147483647)
    {
        v += 0x00010000;
    }
    mem[3] = static_cast<tchar>((v >> 16) & 0x000000ff);
    mem[2] = static_cast<tchar>((v >> 24) & 0x000000ff);
    if(v < 0)
    {
        x[1] = 0xff;
        x[0] = 0xff;
    }
    else
    {
        x[1] = 0x00;
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB18FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    mem[0] = static_cast<tchar>((v << 2)  & 0x00fc);
    mem[1] = static_cast<tchar>((v >> 6)  & 0x00ff);
    mem[2] = static_cast<tchar>((v >> 14) & 0x0003);
    if(v < 0)
    {
        x[2] |= 0xfc;
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB18FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    if((v & 0x00000020) && v < 8388607)
    {
        v += 0x00000040;
    }
    mem[0] = static_cast<tchar>((v >>  6) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 14) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 22) & 0x00000003);
    if(v < 0)
    {
        x[2] |= 0xfc;
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB18FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte* x = reinterpret_cast<tubyte*>(mem);
    if((v & 0x00002000) && v < 2147483647)
    {
        v += 0x00004000;
    }
    mem[0] = static_cast<tchar>((v >> 14) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 22) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 30) & 0x00000003);
    if(v < 0)
    {
        x[2] |= 0xfc;
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB18FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[3] = static_cast<tchar>((v << 2)  & 0x00fc);
    mem[2] = static_cast<tchar>((v >> 6)  & 0x00ff);
    mem[1] = static_cast<tchar>((v >> 14) & 0x0003);
    if(v < 0)
    {
        x[1] |= 0xfc;
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB18FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00000020) && v < 8388607)
    {
        v += 0x00000040;
    }
    mem[3] = static_cast<tchar>((v >>  6) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 14) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 22) & 0x00000003);
    if(v < 0)
    {
        x[1] |= 0xfc;
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB18FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00002000) && v < 2147483647)
    {
        v += 0x00004000;
    }
    mem[3] = static_cast<tchar>((v >> 14) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 22) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 30) & 0x00000003);
    if(v < 0)
    {
        x[1] |= 0xfc;
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB20FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[0] = static_cast<tchar>((v << 4)  & 0x00f0);
    mem[1] = static_cast<tchar>((v >> 4)  & 0x00ff);
    mem[2] = static_cast<tchar>((v >> 12) & 0x000f);
    if(v < 0)
    {
        x[2] |= 0xf0;
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB20FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00000008) && v < 8388607)
    {
        v += 0x00000010;
    }
    mem[0] = static_cast<tchar>((v >>  4) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 12) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 20) & 0x0000000f);
    if(v < 0)
    {
        x[2] |= 0xf0;
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB20FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00000800) && v < 2147483647)
    {
        v += 0x00001000;
    }
    mem[0] = static_cast<tchar>((v >> 12) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 20) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 28) & 0x0000000f);
    if(v < 0)
    {
        x[2] |= 0xf0;
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB20FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[3] = static_cast<tchar>((v << 4)  & 0x00f0);
    mem[2] = static_cast<tchar>((v >> 4)  & 0x00ff);
    mem[1] = static_cast<tchar>((v >> 12) & 0x000f);
    if(v < 0)
    {
        x[1] |= 0xf0;
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB20FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00000008) && v < 8388607)
    {
        v += 0x00000010;
    }
    mem[3] = static_cast<tchar>((v >>  4) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 12) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 20) & 0x0000000f);
    if(v < 0)
    {
        x[1] |= 0xf0;
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB20FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00000800) && v < 2147483647)
    {
        v += 0x00001000;
    }
    mem[3] = static_cast<tchar>((v >> 12) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 20) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 28) & 0x0000000f);
    if(v < 0)
    {
        x[1] |= 0xf0;
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB24FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[0] = 0x00;
    mem[1] = static_cast<tchar>((v) & 0x00ff);
    mem[2] = static_cast<tchar>((v >> 8) & 0x00ff);
    if(v < 0)
    {
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB24FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[0] = static_cast<tchar>((v      ) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >>  8) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 16) & 0x000000ff);
    if(v < 0)
    {
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32LSB24FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00000080) && v < 2147483647)
    {
        v += 0x00000100;
    }
    mem[0] = static_cast<tchar>((v >>  8) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 24) & 0x000000ff);
    if(v < 0)
    {
        x[3] = 0xff;
    }
    else
    {
        x[3] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB24FromSampleInt16(tint16 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[3] = 0x00;
    mem[2] = static_cast<tchar>((v) & 0x00ff);
    mem[1] = static_cast<tchar>((v >> 8) & 0x00ff);
    if(v < 0)
    {
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB24FromSampleInt24(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    mem[3] = static_cast<tchar>((v      ) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >>  8) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 16) & 0x000000ff);
    if(v < 0)
    {
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------

void writeInt32MSB24FromSampleInt32(tint32 v,tchar *mem)
{
    tubyte *x = reinterpret_cast<tubyte *>(mem);
    if((v & 0x00000080) && v < 2147483647)
    {
        v += 0x00000100;
    }
    mem[3] = static_cast<tchar>((v >>  8) & 0x000000ff);
     mem[2] = static_cast<tchar>((v >> 16) & 0x000000ff);
     mem[1] = static_cast<tchar>((v >> 24) & 0x000000ff);
    if(v < 0)
    {
        x[0] = 0xff;
    }
    else
    {
        x[0] = 0x00;
    }
}

//-------------------------------------------------------------------------------------------
/*
        17 0x0001ffff >>  1 0x0001
        18 0x0003ffff >>  2 0x0002
        19 0x0007ffff >>  3 0x0004
        20 0x000fffff >>  4 0x0008
        21 0x001fffff >>  5 0x0010
        22 0x003fffff >>  6 0x0020
        23 0x007fffff >>  7 0x0040
        24 0x00ffffff >>  8 0x0080
        25 0x01ffffff >>  9 0x0100
        26 0x03ffffff >> 10 0x0200
        27 0x07ffffff >> 11 0x0400
        28 0x0fffffff >> 12 0x0800
        29 0x1fffffff >> 13 0x1000
        30 0x3fffffff >> 14 0x2000
        31 0x7fffffff >> 15 0x4000
        32 0xffffffff >> 16 0x8000
*/
//-------------------------------------------------------------------------------------------

tint16 readInt16SampleLittleEndian(const tbyte *mem, tint noBits)
{
    static const tint32 c_mask[16] = {
        0x00000001,
        0x00000002,
        0x00000004,
        0x00000008,
        0x00000010,
        0x00000020,
        0x00000040,
        0x00000080,
        0x00000100,
        0x00000200,
        0x00000400,
        0x00000800,
        0x00001000,
        0x00002000,
        0x00004000,
        0x00008000
    };

    tint16 x;

    if(noBits <= 16)
    {
        if(noBits <= 8)
        {
            x = static_cast<tint16>(to8BitSignedFromLittleEndian(mem));
        }
        else
        {
            x = to16BitSignedFromLittleEndian(mem);
        }
        x <<= 16 - noBits;
    }
    else
    {
        tint32 t, shift;

        if(noBits <= 24)
        {
            t = to24BitSignedFromLittleEndian(mem);
        }
        else
        {
            t = to32BitSignedFromLittleEndian(mem);
        }
        shift = noBits - 16;
        x = static_cast<tint16>(t >> shift);
        if((t & c_mask[shift - 1]) && x < 0x7fff)
        {
            x++;
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint16 readInt16SampleLittleEndian(const tubyte *mem, tint noBits)
{
    return readInt16SampleLittleEndian(reinterpret_cast<const tbyte *>(mem), noBits);
}

//-------------------------------------------------------------------------------------------

tint16 readInt16SampleBigEndian(const tbyte *mem, tint noBits)
{
    static const tint32 c_mask[16] = {
        0x00000001,
        0x00000002,
        0x00000004,
        0x00000008,
        0x00000010,
        0x00000020,
        0x00000040,
        0x00000080,
        0x00000100,
        0x00000200,
        0x00000400,
        0x00000800,
        0x00001000,
        0x00002000,
        0x00004000,
        0x00008000
    };

    tint16 x;

    if(noBits <= 16)
    {
        if(noBits <= 8)
        {
            x = static_cast<tint16>(to8BitSignedFromBigEndian(mem));
        }
        else
        {
            x = to16BitSignedFromBigEndian(mem);
        }
        x <<= 16 - noBits;
    }
    else
    {
        tint32 t, shift;

        if(noBits <= 24)
        {
            t = to24BitSignedFromBigEndian(mem);
        }
        else
        {
            t = to32BitSignedFromBigEndian(mem);
        }
        shift = noBits - 16;
        x = static_cast<tint16>(t >> shift);
        if((t & c_mask[shift - 1]) && x < 0x7fff)
        {
            x++;
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint16 readInt16SampleBigEndian(const tubyte *mem, tint noBits)
{
    return readInt16SampleBigEndian(reinterpret_cast<const tbyte *>(mem), noBits);
}

//-------------------------------------------------------------------------------------------
/*
        25 0x01ffffff >>  1 0x01
        26 0x03ffffff >>  2 0x02
        27 0x07ffffff >>  3 0x04
        28 0x0fffffff >>  4 0x08
        29 0x1fffffff >>  5 0x10
        30 0x3fffffff >>  6 0x20
        31 0x7fffffff >>  7 0x40
        32 0xffffffff >>  8 0x80
*/
//-------------------------------------------------------------------------------------------

tint32 readInt24SampleLittleEndian(const tbyte *mem, tint noBits)
{
    static const tint32 c_mask[8] = {
        0x00000001,
        0x00000002,
        0x00000004,
        0x00000008,
        0x00000010,
        0x00000020,
        0x00000040,
        0x00000080
    };

    tint32 x, t, shift;

    if(noBits <= 24)
    {
        if(noBits <= 8)
        {
            x = static_cast<tint32>(to8BitSignedFromLittleEndian(mem));
        }
        else if(noBits <= 16)
        {
            x = static_cast<tint32>(to16BitSignedFromLittleEndian(mem));
        }
        else
        {
            x = to24BitSignedFromLittleEndian(mem);
        }
        x <<= 24 - noBits;
    }
    else
    {
        t = to32BitSignedFromLittleEndian(mem);
        shift = noBits - 24;
        x = t >> shift;
        if((t & c_mask[shift - 1]) && x < 0x007fffff)
        {
            x++;
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 readInt24SampleLittleEndian(const tubyte *mem, tint noBits)
{
    return readInt24SampleLittleEndian(reinterpret_cast<const tbyte *>(mem), noBits);
}

//-------------------------------------------------------------------------------------------

tint32 readInt24SampleBigEndian(const tbyte *mem, tint noBits)
{
    static const tint32 c_mask[8] = {
        0x00000001,
        0x00000002,
        0x00000004,
        0x00000008,
        0x00000010,
        0x00000020,
        0x00000040,
        0x00000080
    };

    tint32 x, t, shift;

    if(noBits <= 24)
    {
        if(noBits <= 8)
        {
            x = static_cast<tint32>(to8BitSignedFromBigEndian(mem));
        }
        else if(noBits <= 16)
        {
            x = static_cast<tint32>(to16BitSignedFromBigEndian(mem));
        }
        else
        {
            x = to24BitSignedFromBigEndian(mem);
        }
        x <<= 24 - noBits;
    }
    else
    {
        t = to32BitSignedFromBigEndian(mem);
        shift = noBits - 24;
        x = t >> shift;
        if((t & c_mask[shift - 1]) && x < 0x007fffff)
        {
            x++;
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 readInt24SampleBigEndian(const tubyte *mem, tint noBits)
{
    return readInt24SampleBigEndian(reinterpret_cast<const tbyte *>(mem), noBits);
}

//-------------------------------------------------------------------------------------------

tint32 readInt32SampleLittleEndian(const tbyte *mem, tint noBits)
{
    tint32 x;

    if(noBits <= 16)
    {
        if(noBits <= 8)
        {
            x = static_cast<tint32>(to8BitSignedFromLittleEndian(mem));
        }
        else
        {
            x = static_cast<tint32>(to16BitSignedFromLittleEndian(mem));
        }
    }
    else
    {
        if(noBits <= 24)
        {
            x = to24BitSignedFromLittleEndian(mem);
        }
        else
        {
            x = to32BitSignedFromLittleEndian(mem);
        }
    }
    x <<= 32 - noBits;
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 readInt32SampleLittleEndian(const tubyte *mem, tint noBits)
{
    return readInt32SampleLittleEndian(reinterpret_cast<const tbyte *>(mem), noBits);
}

//-------------------------------------------------------------------------------------------

tint32 readInt32SampleBigEndian(const tbyte *mem, tint noBits)
{
    tint32 x;

    if(noBits <= 16)
    {
        if(noBits <= 8)
        {
            x = static_cast<tint32>(to8BitSignedFromBigEndian(mem));
        }
        else
        {
            x = static_cast<tint32>(to16BitSignedFromBigEndian(mem));
        }
    }
    else
    {
        if(noBits <= 24)
        {
            x = to24BitSignedFromBigEndian(mem);
        }
        else
        {
            x = to32BitSignedFromBigEndian(mem);
        }
    }
    x <<= 32 - noBits;
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 readInt32SampleBigEndian(const tubyte *mem, tint noBits)
{
    return readInt32SampleBigEndian(reinterpret_cast<const tbyte *>(mem), noBits);
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
