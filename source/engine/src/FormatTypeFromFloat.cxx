#include "engine/inc/FormatTypeFromFloat.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

void write8BitsFromSample(tfloat32 v,tchar *mem)
{
    write8BitsFromSample(static_cast<tfloat64>(v),mem);
}

//-------------------------------------------------------------------------------------------

void write8BitsFromSample(tfloat64 v,tchar *mem)
{
    mem[0] = static_cast<tchar>(sampleTo8BitsInteger(v));
}

//-------------------------------------------------------------------------------------------

void write16BitsLittleEndianFromSample(tfloat32 v,tchar *mem)
{
    write16BitsLittleEndianFromSample(static_cast<tfloat64>(v),mem);
}

//-------------------------------------------------------------------------------------------

void write16BitsLittleEndianFromSample(tfloat64 v,tchar *mem)
{
    tint x = sampleTo16BitsInteger(v);
    tubyte *y = reinterpret_cast<tubyte *>(mem);
    y[1] = static_cast<tubyte>((x >> 8) & 0x000000ff);
    y[0] = static_cast<tubyte>(x & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write16BitsBigEndianFromSample(tfloat32 v,tchar *mem)
{
    write16BitsBigEndianFromSample(static_cast<tfloat64>(v),mem);
}

//-------------------------------------------------------------------------------------------

void write16BitsBigEndianFromSample(tfloat64 v,tchar *mem)
{
    tint x = sampleTo16BitsInteger(v);
    tubyte *y = reinterpret_cast<tubyte *>(mem);
    y[0] = static_cast<tubyte>((x >> 8) & 0x000000ff);
    y[1] = static_cast<tubyte>(x & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write24BitsLittleEndianFromSample(tfloat32 v,tchar *mem)
{
    write24BitsLittleEndianFromSample(static_cast<tfloat64>(v),mem);
}

//-------------------------------------------------------------------------------------------

void write24BitsLittleEndianFromSample(tfloat64 v,tchar *mem)
{
    tint x = sampleTo24BitsInteger(v);
    tubyte *y = reinterpret_cast<tubyte *>(mem);
    y[2] = static_cast<tubyte>((x >> 16) & 0x000000ff);
    y[1] = static_cast<tubyte>((x >> 8) & 0x000000ff);
    y[0] = static_cast<tubyte>(x & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write24BitsBigEndianFromSample(tfloat32 v,tchar *mem)
{
    write24BitsBigEndianFromSample(static_cast<tfloat64>(v),mem);
}

//-------------------------------------------------------------------------------------------

void write24BitsBigEndianFromSample(tfloat64 v,tchar *mem)
{
    tint x = sampleTo24BitsInteger(v);
    tubyte *y = reinterpret_cast<tubyte *>(mem);
    y[0] = static_cast<tubyte>((x >> 16) & 0x000000ff);
    y[1] = static_cast<tubyte>((x >> 8) & 0x000000ff);
    y[2] = static_cast<tubyte>(x & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsLittleEndianFromSample(tfloat32 v,tchar *mem)
{
    write32BitsLittleEndianFromSample(static_cast<tfloat64>(v),mem);
}

//-------------------------------------------------------------------------------------------

void write32BitsLittleEndianFromSample(tfloat64 v,tchar *mem)
{
    tint x = sampleTo32BitsInteger(v);
    tubyte *y = reinterpret_cast<tubyte *>(mem);
    y[3] = static_cast<tubyte>((x >> 24) & 0x000000ff);
    y[2] = static_cast<tubyte>((x >> 16) & 0x000000ff);
    y[1] = static_cast<tubyte>((x >> 8) & 0x000000ff);
    y[0] = static_cast<tubyte>(x & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void write32BitsBigEndianFromSample(tfloat32 v,tchar *mem)
{
    write32BitsBigEndianFromSample(static_cast<tfloat64>(v),mem);
}

//-------------------------------------------------------------------------------------------

void write32BitsBigEndianFromSample(tfloat64 v,tchar *mem)
{
    tint x = sampleTo32BitsInteger(v);
    tubyte *y = reinterpret_cast<tubyte *>(mem);
    y[0] = static_cast<tubyte>((x >> 24) & 0x000000ff);
    y[1] = static_cast<tubyte>((x >> 16) & 0x000000ff);
    y[2] = static_cast<tubyte>((x >> 8) & 0x000000ff);
    y[3] = static_cast<tubyte>(x & 0x000000ff);
}

//-------------------------------------------------------------------------------------------
/*
4-Bit case

Positive:
0 = [ 0/14 -  1/14)
1 = [ 1/14 -  3/14)
2 = [ 3/14 -  5/14)
3 = [ 5/14 -  7/14)
4 = [ 7/14 -  9/14)
5 = [ 9/14 - 11/14)
6 = [11/14 - 13/14)
7 = [13/14 - 14/14]

Negative
 0 = [ 0/16 -  1/16)
-1 = [ 1/16 -  3/16)
-2 = [ 3/16 -  5/16)
-3 = [ 5/16 -  7/16)
-4 = [ 7/16 -  9/16)
-5 = [ 9/16 - 11/16)
-6 = [11/16 - 13/16)
-7 = [13/16 - 15/16)
-8 = [15/16 - 16/16]
*/
//-------------------------------------------------------------------------------------------

tint sampleToInteger(tfloat32 v,tint noBits)
{
    return sampleToInteger(static_cast<tfloat64>(v),noBits);
}

//-------------------------------------------------------------------------------------------

tint sampleToInteger(tfloat64 v,tint noBits)
{
    tint x;
    tfloat64 n;
    tuint32 s = static_cast<tuint32>(1 << (noBits - 1));

    if(v >= 0.0)
    {
        tfloat64 d = static_cast<tfloat64>(s - 1);
        n = (v + (1.0 / (d * 2.0))) * d;
        x = static_cast<tint>(floor(n));
    }
    else
    {
        tfloat64 d = static_cast<tfloat64>(s);
        n = 0.0 - (((-v) + (1.0 / (d * 2.0))) * d);
        x = static_cast<tint>(ceil(n));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint sampleTo32BitsInteger(tfloat32 v)
{
    return sampleTo32BitsInteger(static_cast<tfloat64>(v));
}

//-------------------------------------------------------------------------------------------

tint sampleTo32BitsInteger(tfloat64 v)
{
    tint x;
    tfloat64 n;

    if(v >= 0.0f)
    {
        const tfloat64 d = 2147483647.0;
        n = (v + (1.0 / (d * 2.0))) * d;
        x = static_cast<tint>(floor(n));
    }
    else
    {
        const tfloat64 d = 2147483648.0;
        n = 0.0 - (((-v) + (1.0 / (d * 2.0))) * d);
        x = static_cast<tint>(ceil(n));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint sampleTo24BitsInteger(tfloat32 v)
{
    return sampleTo24BitsInteger(static_cast<tfloat64>(v));
}

//-------------------------------------------------------------------------------------------

tint sampleTo24BitsInteger(tfloat64 v)
{
    tint x;
    tfloat64 n;

    if(v >= 0.0f)
    {
        const tfloat64 d = 8388607.0;
        n = (v + (1.0 / (d * 2.0))) * d;
        x = static_cast<tint>(floor(n));
    }
    else
    {
        const tfloat64 d = 8388608.0;
        n = 0.0 - (((-v) + (1.0 / (d * 2.0))) * d);
        x = static_cast<tint>(ceil(n));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint sampleTo16BitsInteger(tfloat32 v)
{
    return sampleTo16BitsInteger(static_cast<tfloat64>(v));
}

//-------------------------------------------------------------------------------------------

tint sampleTo16BitsInteger(tfloat64 v)
{
    tint x;
    tfloat64 n;

    if(v >= 0.0f)
    {
        const tfloat64 d = 32767.0;
        n = (v + (1.0 / (d * 2.0))) * d;
        x = static_cast<tint>(floor(n));
    }
    else
    {
        const tfloat64 d = 32768.0;
        n = 0.0 - (((-v) + (1.0 / (d * 2.0))) * d);
        x = static_cast<tint>(ceil(n));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint sampleTo8BitsInteger(tfloat32 v)
{
    return sampleTo8BitsInteger(static_cast<tfloat64>(v));
}

//-------------------------------------------------------------------------------------------

tint sampleTo8BitsInteger(tfloat64 v)
{
    tint x;
    tfloat64 n;

    if(v >= 0.0f)
    {
        const tfloat64 d = 127.0;
        n = (v + (1.0 / (d * 2.0))) * d;
        x = static_cast<tint>(floor(n));
    }
    else
    {
        const tfloat64 d = 128.0;
        n = 0.0 - (((-v) + (1.0 / (d * 2.0))) * d);
        x = static_cast<tint>(ceil(n));
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tfloat32 sampleToFloat32(tfloat64 v)
{
    if(v < -1.0)
    {
        v = -1.0;
    }
    else if(v > 1.0)
    {
        v = 1.0;
    }
    return static_cast<tfloat32>(v);
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian16BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=2,dst+=2)
    {
        dst[0] = src[0];
        dst[1] = src[1];
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian16BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=2,dst+=2)
    {
        dst[0] = src[1];
        dst[1] = src[0];
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian16BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian16BitsAsLittleEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeBigEndian16BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian16BitsAsBigEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeNative16BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian16BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian16BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeNative16BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian16BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian16BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian24BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=3,dst+=3)
    {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian24BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=3,dst+=3)
    {
        dst[0] = src[2];
        dst[1] = src[1];
        dst[2] = src[0];
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian24BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian24BitsAsBigEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeBigEndian24BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian24BitsAsLittleEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeNative24BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian24BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian24BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeNative24BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian24BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian24BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian32BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=4,dst+=4)
    {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian32BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=4,dst+=4)
    {
        dst[0] = src[3];
        dst[1] = src[2];
        dst[2] = src[1];
        dst[3] = src[0];
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian32BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian32BitsAsBigEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeBigEndian32BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian32BitsAsLittleEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeNative32BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian32BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian32BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeNative32BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian32BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian32BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian64BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=8,dst+=8)
    {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian64BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    for(int i=0;i<noElements;i++,src+=8,dst+=8)
    {
        dst[0] = src[7];
        dst[1] = src[6];
        dst[2] = src[5];
        dst[3] = src[4];
        dst[4] = src[3];
        dst[5] = src[2];
        dst[6] = src[1];
        dst[7] = src[0];
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian64BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian64BitsAsBigEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeBigEndian64BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    writeLittleEndian64BitsAsLittleEndian(src,dst,noElements);
}

//-------------------------------------------------------------------------------------------

void writeNative64BitsAsLittleEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian64BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian64BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeNative64BitsAsBigEndian(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian64BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian64BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian16BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian16BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian16BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian16BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian16BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian16BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian24BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian24BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian24BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian24BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian24BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian24BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian32BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian32BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian32BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian32BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian32BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian32BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeLittleEndian64BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeLittleEndian64BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeLittleEndian64BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeBigEndian64BitsAsNative(const tbyte *src,tbyte *dst,tint noElements)
{
    if(isLittleEndian())
    {
        writeBigEndian64BitsAsLittleEndian(src,dst,noElements);
    }
    else
    {
        writeBigEndian64BitsAsBigEndian(src,dst,noElements);
    }
}

//-------------------------------------------------------------------------------------------

void writeFloat32ToSample(const tfloat32 *src,tfloat64 *dst,tint noElements)
{
    for(tint i=0;i<noElements;i++)
    {
        dst[i] = static_cast<tfloat64>(src[i]);
    }
}

//-------------------------------------------------------------------------------------------

SampleFromInteger::SampleFromInteger(tint noBits)
{
    tint32 minV, maxV;
    tuint32 s = static_cast<tuint32>(1 << (noBits - 1));
    m_divN = static_cast<sample_t>(s);
    m_divP = static_cast<sample_t>(s-1);
    m_noBits = noBits;
    maxV = 1 << (m_noBits - 1);
    minV = -maxV;
    maxV--;
    m_min = minV;
    m_max = maxV;
}

//-------------------------------------------------------------------------------------------

sample_t SampleFromInteger::convert(tint32 v)
{
    sample_t x;
    
    if(v >= 0)
    {
        x = static_cast<sample_t>(v) / m_divP;
    }
    else
    {
        x = static_cast<sample_t>(v) / m_divN;
    }
    
#if defined(SINGLE_FLOAT_SAMPLE)
    if(x < -1.0f)
    {
        x = -1.0f;
    }
    else if(x > 1.0f)
    {
        x = 1.0f;
    }
#else
    if(x < -1.0)
    {
        x = -1.0;
    }
    else if(x > 1.0)
    {
        x = 1.0;
    }
#endif

    return x;
}

//-------------------------------------------------------------------------------------------

tint16 SampleFromInteger::convertInt16(tint32 v)
{
    const tint32 c_mask[17] = {
        0x00000000, // 16 - 16 = 0
        0x00000001, // 17 - 16 = 1
        0x00000002, // 18 - 16 = 2
        0x00000004, // 19 - 16 = 3
        0x00000008, // 20 - 16 = 4
        0x00000010, // 21 - 16 = 5
        0x00000020, // 22 - 16 = 6
        0x00000040, // 23 - 16 = 7
        0x00000080, // 24 - 16 = 8
        0x00000100, // 25 - 16 = 9
        0x00000200, // 26 - 16 = 10
        0x00000400, // 27 - 16 = 11
        0x00000800, // 28 - 16 = 12
        0x00001000, // 29 - 16 = 13
        0x00002000, // 30 - 16 = 14
        0x00004000, // 31 - 16 = 15
        0x00008000  // 32 - 16 = 16
    };
    
    tint16 x;
    tint32 t;

    if(m_noBits < 32)
    {
        if(v < m_min)
        {
            v = m_min;
        }
        else if(v > m_max)
        {
            v = m_max;
        }    
    }
    t = v;

    if(m_noBits <= 16)
    {
        v <<= 16 - m_noBits;
        x = static_cast<tint16>(v);
    }
    else if(m_noBits > 16)
    {
        v >>= m_noBits - 16;
        x = static_cast<tint16>(v);
        if((t & c_mask[m_noBits - 16]) && x < 32767)
        {
            x++;
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 SampleFromInteger::convertInt24(tint32 v)
{
    const tint32 c_mask[17] = {
        0x00000000, // 24 - 24 = 0
        0x00000001, // 25 - 24 = 1
        0x00000002, // 26 - 24 = 2
        0x00000004, // 27 - 24 = 3
        0x00000008, // 28 - 24 = 4
        0x00000010, // 29 - 24 = 5
        0x00000020, // 30 - 24 = 6
        0x00000040, // 31 - 24 = 7
        0x00000080, // 32 - 24 = 8
    };

    tint32 t;

    if(m_noBits < 32)
    {
        if(v < m_min)
        {
            v = m_min;
        }
        else if(v > m_max)
        {
            v = m_max;
        }    
    }
    t = v;

    if(m_noBits < 24)
    {
        v <<= 24 - m_noBits;
    }
    else if(m_noBits > 24)
    {
        v >>= m_noBits - 24;
        if((t & c_mask[m_noBits - 24]) && v < 8388607)
        {
            v++;
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

tint32 SampleFromInteger::convertInt32(tint32 v)
{
    if(m_noBits < 32)
    {
        if(v < m_min)
        {
            v = m_min;
        }
        else if(v > m_max)
        {
            v = m_max;
        }
    
        v <<= 32 - m_noBits;
    }
    return v;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
