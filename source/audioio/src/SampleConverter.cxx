#include "audioio/inc/SampleConverter.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

SampleConverter::SampleConverter() : m_type(FormatDescription::e_DataSignedInteger),
    m_noInChannels(1),
    m_noOutChannels(1),
    m_noBits(16),
    m_bytesPerSample(2),
    m_littleEndian(true),
    m_alignHigh(false),
    m_volume(c_plusOneSample),
    m_vIntBuffer(NULL),
    m_vIntBufSize(0)
{}

//-------------------------------------------------------------------------------------------

SampleConverter::SampleConverter(tint noBits,tint bytesPerSample,bool littleEndian,bool alignHigh,bool isSigned) : m_type(FormatDescription::e_DataSignedInteger),
    m_noInChannels(1),
    m_noOutChannels(1),
    m_noBits(noBits),
    m_bytesPerSample(bytesPerSample),
    m_littleEndian(littleEndian),
    m_alignHigh(alignHigh),
    m_volume(c_plusOneSample),
    m_vIntBuffer(NULL),
    m_vIntBufSize(0)
{
    m_type = (isSigned) ? FormatDescription::e_DataSignedInteger : FormatDescription::e_DataUnsignedInteger;
}

//-------------------------------------------------------------------------------------------

SampleConverter::SampleConverter(bool isSinglePrecision,bool littleEndian) : m_type(FormatDescription::e_DataSignedInteger),
    m_noInChannels(1),
    m_noOutChannels(1),
    m_noBits(16),
    m_bytesPerSample(2),
    m_littleEndian(true),
    m_alignHigh(false),
    m_volume(c_plusOneSample),
    m_vIntBuffer(NULL),
    m_vIntBufSize(0)
{
    if(isSinglePrecision)
    {
        m_type = FormatDescription::e_DataFloatSingle;
        m_noBits = 32;
        m_bytesPerSample = 4;
    }
    else
    {
        m_type = FormatDescription::e_DataFloatDouble;
        m_noBits = 64;
        m_bytesPerSample = 8;
    }
    m_littleEndian = littleEndian;
}

//-------------------------------------------------------------------------------------------

SampleConverter::SampleConverter(const SampleConverter& rhs) : m_type(FormatDescription::e_DataSignedInteger),
    m_noInChannels(1),
    m_noOutChannels(1),
    m_noBits(16),
    m_bytesPerSample(2),
    m_littleEndian(true),
    m_alignHigh(false),
    m_volume(c_plusOneSample),
    m_vIntBuffer(NULL),
    m_vIntBufSize(0)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

SampleConverter::~SampleConverter()
{
    try
    {
        if(m_vIntBuffer != NULL)
        {
            delete [] m_vIntBuffer;
            m_vIntBuffer = NULL;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

const SampleConverter& SampleConverter::operator = (const SampleConverter& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void SampleConverter::copy(const SampleConverter& rhs)
{
    m_type = rhs.m_type;
    m_noInChannels = rhs.m_noInChannels;
    m_noOutChannels = rhs.m_noOutChannels;
    m_noBits = rhs.m_noBits;
    m_bytesPerSample = rhs.m_bytesPerSample;
    m_littleEndian = rhs.m_littleEndian;
    m_alignHigh = rhs.m_alignHigh;
    m_volume = rhs.m_volume;
}

//-------------------------------------------------------------------------------------------

bool SampleConverter::isSupported() const
{
    bool res = false;
    
    switch(m_bytesPerSample)
    {
        case 1:
            if(m_noBits>=1 && m_noBits<=8)
            {
                res = true;
            }
            break;
            
        case 2:
            if(m_noBits>=1 && m_noBits<=16)
            {
                res = true;
            }
            break;
        
        case 3:
            if(m_noBits>=1 && m_noBits<=24)
            {
                res = true;
            }
            break;

        case 4:
            if(m_noBits>=1 && m_noBits<=32)
            {
                res = true;
            }
            break;
            
        case 8:
            if(m_noBits==64)
            {
                res = true;
            }
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SampleConverter::isFloat() const
{
    return (m_type == FormatDescription::e_DataFloatSingle || m_type == FormatDescription::e_DataFloatDouble);
}

//-------------------------------------------------------------------------------------------

FormatDescription::DataType SampleConverter::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

bool SampleConverter::isLittleEndian() const
{
    return m_littleEndian;
}

//-------------------------------------------------------------------------------------------

bool SampleConverter::isAlignedHigh() const
{
    return m_alignHigh;
}

//-------------------------------------------------------------------------------------------

tint SampleConverter::numberOfInputChannels() const
{
    return m_noInChannels;
}

//-------------------------------------------------------------------------------------------

void SampleConverter::setNumberOfInputChannels(tint no)
{
    m_noInChannels = no;
}

//-------------------------------------------------------------------------------------------

tint SampleConverter::numberOfOutputChannels() const
{
    return m_noOutChannels;
}

//-------------------------------------------------------------------------------------------

void SampleConverter::setNumberOfOutputChannels(tint no)
{
    m_noOutChannels = no;
}

//-------------------------------------------------------------------------------------------

tint SampleConverter::bits() const
{
    return m_noBits;
}

//-------------------------------------------------------------------------------------------

tint SampleConverter::bytesPerSample() const
{
    return m_bytesPerSample;
}

//-------------------------------------------------------------------------------------------

void SampleConverter::setVolume(sample_t v)
{
    m_volume = v;
}

//-------------------------------------------------------------------------------------------

tint SampleConverter::calculateShift() const
{
    return (m_bytesPerSample << 3) - m_noBits;
}

//-------------------------------------------------------------------------------------------

tuint16 SampleConverter::unsignedMask16() const
{
    return static_cast<tuint16>(unsignedMask());
}

//-------------------------------------------------------------------------------------------

tuint32 SampleConverter::unsignedMask() const
{
    static const tuint32 c_mask[] = {
        0x00000000, // 0 
        0x00000001, // 1
        0x00000003, // 2
        0x00000007, // 3
        0x0000000f, // 4
        0x0000001f, // 5
        0x0000003f, // 6
        0x0000007f, // 7
        0x000000ff, // 8
        0x000001ff, // 9
        0x000003ff, // 10
        0x000007ff, // 11
        0x00000fff, // 12
        0x00001fff, // 13
        0x00003fff, // 14
        0x00007fff, // 15
        0x0000ffff, // 16
        0x0001ffff, // 17
        0x0003ffff, // 18
        0x0007ffff, // 19
        0x000fffff, // 20
        0x001fffff, // 21
        0x003fffff, // 22
        0x007fffff, // 23
        0x00ffffff, // 24
        0x01ffffff, // 25
        0x03ffffff, // 26
        0x07ffffff, // 27
        0x0fffffff, // 28
        0x1fffffff, // 29
        0x3fffffff, // 30
        0x7fffffff, // 31
        0xffffffff, // 32
    };
    
    return c_mask[m_noBits];
}

//-------------------------------------------------------------------------------------------

// 0000 = 0 1000 = 8
// 0001 = 1 1001 = 9
// 0010 = 2 1010 = A
// 0011 = 3 1011 = B
// 0100 = 4 1100 = C
// 0101 = 5 1101 = D
// 0110 = 6 1110 = E
// 0111 = 7 1111 = F
//
// 0123 4567
// 1111 1110 - 1 = 0xfe
// 1111 1100 - 2 = 0xfc
// 1111 1000 - 3 = 0xf8
// 1111 0000 - 4 = 0xf0

tuint16 SampleConverter::signedMaskInt16() const
{
    static const tuint16 c_mask[] = {
        0xffff, // 0 
        0xfffe, // 1
        0xfffc, // 2
        0xfff8, // 3
        0xfff0, // 4
        0xffe0, // 5
        0xffc0, // 6
        0xff80, // 7
        0xff00, // 8
        0xfe00, // 9
        0xfc00, // 10
        0xf800, // 11
        0xf000, // 12
        0xe000, // 13
        0xc000, // 14
        0x8000  // 15
    };
    return (m_noBits < 16) ? c_mask[m_noBits & 0xf] : 0x0000;
};

//-------------------------------------------------------------------------------------------

tuint32 SampleConverter::signedMaskInt24() const
{
    static const tuint32 c_mask[] = {
        0xffffffff, // 0 
        0xfffffffe, // 1
        0xfffffffc, // 2
        0xfffffff8, // 3
        0xfffffff0, // 4
        0xffffffe0, // 5
        0xffffffc0, // 6
        0xffffff80, // 7
        0xffffff00, // 8
        0xfffffe00, // 9
        0xfffffc00, // 10
        0xfffff800, // 11
        0xfffff000, // 12
        0xffffe000, // 13
        0xffffc000, // 14
        0xffff8000, // 15
        0xffff0000, // 16
        0xfffe0000, // 17
        0xfffc0000, // 18
        0xfff80000, // 19
        0xfff00000, // 20
        0xffe00000, // 21
        0xffc00000, // 22
        0xff800000  // 23
    };
    return (m_noBits < 24) ? c_mask[m_noBits % 24] : 0xff000000;
};

//-------------------------------------------------------------------------------------------

tuint32 SampleConverter::signedMaskInt32() const
{
    static const tuint32 c_mask[] = {
        0xffffffff, // 0 
        0xfffffffe, // 1
        0xfffffffc, // 2
        0xfffffff8, // 3
        0xfffffff0, // 4
        0xffffffe0, // 5
        0xffffffc0, // 6
        0xffffff80, // 7
        0xffffff00, // 8
        0xfffffe00, // 9
        0xfffffc00, // 10
        0xfffff800, // 11
        0xfffff000, // 12
        0xffffe000, // 13
        0xffffc000, // 14
        0xffff8000, // 15
        0xffff0000, // 16
        0xfffe0000, // 17
        0xfffc0000, // 18
        0xfff80000, // 19
        0xfff00000, // 20
        0xffe00000, // 21
        0xffc00000, // 22
        0xff800000, // 23
        0xff000000, // 24
        0xfe000000, // 25
        0xfc000000, // 26
        0xf8000000, // 27
        0xf0000000, // 28
        0xe0000000, // 29
        0xc0000000, // 30
        0x80000000  // 31
    };
    return (m_noBits < 32) ? c_mask[m_noBits & 0x1f] : 0x80000000;
};

//-------------------------------------------------------------------------------------------

tuint16 SampleConverter::minIntegerValue16() const
{
    static const tuint32 c_min[] = {
        0xffffffff, // 1
        0xfffffffe, // 2
        0xfffffffc, // 3
        0xfffffff8, // 4
        0xfffffff0, // 5
        0xffffffe0, // 6
        0xffffffc0, // 7
        0xffffff80, // 8
        0xffffff00, // 9
        0xfffffe00, // 10
        0xfffffc00, // 11
        0xfffff800, // 12
        0xfffff000, // 13
        0xffffe000, // 14
        0xffffc000, // 15
        0xffff8000, // 16
    };
    return (m_noBits > 16) ? 0x8000 : c_min[(m_noBits - 1) & 0x1f];
}

//-------------------------------------------------------------------------------------------

tuint32 SampleConverter::minIntegerValue32() const
{
    static const tuint32 c_min[] = {
        0xffffffff, // 1 
        0xfffffffe, // 2
        0xfffffffc, // 3
        0xfffffff8, // 4
        0xfffffff0, // 5
        0xffffffe0, // 6
        0xffffffc0, // 7
        0xffffff80, // 8
        0xffffff00, // 9
        0xfffffe00, // 10
        0xfffffc00, // 11
        0xfffff800, // 12
        0xfffff000, // 13
        0xffffe000, // 14
        0xffffc000, // 15
        0xffff8000, // 16
        0xffff0000, // 17
        0xfffe0000, // 18
        0xfffc0000, // 19
        0xfff80000, // 20
        0xfff00000, // 21
        0xffe00000, // 22
        0xffc00000, // 23
        0xff800000, // 24
        0xff000000, // 25
        0xfe000000, // 26
        0xfc000000, // 27
        0xf8000000, // 28
        0xf0000000, // 29
        0xe0000000, // 30
        0xc0000000, // 31
        0x80000000  // 32
    };
    return c_min[(m_noBits - 1) & 0x1f];
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitLSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian8BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian8BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian8BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian8BitLSBFloat(in,out,noSamples);
    }
}

void SampleConverter::convertLittleEndian8BitLSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        *out = static_cast<tbyte>(x & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitLSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        *out = static_cast<tbyte>(x & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitLSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;

    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        *out = static_cast<tbyte>(x & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitLSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        *out = static_cast<tbyte>(x & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitLSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned8BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned8BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned8BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned8BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitLSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);    
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        *out = static_cast<tubyte>(v & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitLSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint shiftF, max;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    min = minIntegerValue16();
    umask = unsignedMask16();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        v = toUnsigned16(x, min, umask);
        *out = static_cast<tubyte>(v & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitLSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask);
        *out = static_cast<tbyte>(v & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitLSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask);
        *out = static_cast<tbyte>(v & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitMSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian8BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian8BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian8BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian8BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitMSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB) << shift;
        *out = static_cast<tbyte>(x & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitMSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        *out = static_cast<tbyte>((x << shiftMSB) & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitMSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        *out = static_cast<tbyte>((x << shiftMSB) & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian8BitMSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        *out = static_cast<tbyte>((x << shiftMSB) & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitMSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned8BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned8BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned8BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned8BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitMSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        v <<= shift;
        *out = static_cast<tbyte>(v & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitMSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint shiftF, max, shiftMSB;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    min = minIntegerValue16();
    umask = unsignedMask16();
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        v = toUnsigned16(x, min, umask) << shiftMSB;
        *out = static_cast<tubyte>(v & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitMSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max, shiftMSB;
    
    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        *out = static_cast<tbyte>(v & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned8BitMSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        *out = static_cast<tbyte>(v & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitLSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian16BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian16BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian16BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian16BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitLSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitLSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitLSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitLSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitLSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned16BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned16BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned16BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned16BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitLSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitLSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint shiftF, max;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    min = minIntegerValue16();
    umask = unsignedMask16();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        v = toUnsigned16(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitLSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitLSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitMSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian16BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian16BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian16BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian16BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitMSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB) << shift;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitMSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        x <<= shiftMSB;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitMSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        x <<= shiftMSB;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian16BitMSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        x <<= shiftMSB;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitMSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned16BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned16BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned16BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned16BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitMSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();
    tint shiftMSB = calculateShift();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask) << shiftMSB;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitMSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    min = minIntegerValue16();
    umask = unsignedMask16();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        v = toUnsigned16(x, min, umask) << shiftMSB;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitMSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned16BitMSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitLSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndian16BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndian16BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndian16BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndian16BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitLSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitLSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitLSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitLSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitLSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndianUnsigned16BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndianUnsigned16BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndianUnsigned16BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndianUnsigned16BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitLSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitLSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint shiftF, max;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    min = minIntegerValue16();
    umask = unsignedMask16();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        v = toUnsigned16(x, min, umask);
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitLSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask);
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitLSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask);
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitMSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndian16BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndian16BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndian16BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndian16BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitMSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB) << shift;
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitMSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        x <<= shiftMSB;
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitMSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        x <<= shiftMSB;
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian16BitMSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        x <<= shiftMSB;
        out[1] = static_cast<tbyte>(x & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitMSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndianUnsigned16BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndianUnsigned16BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndianUnsigned16BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndianUnsigned16BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitMSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tint16) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();
    tint shiftMSB = calculateShift();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask) << shiftMSB;
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitMSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();
    
    shiftF = 16 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fff >> shiftF;
    min = minIntegerValue16();
    umask = unsignedMask16();
    for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt16;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x8000)
        {
            x |= signedMaskInt16();
        }
        v = toUnsigned16(x, min, umask) << shiftMSB;
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitMSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 24 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned16BitMSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 2;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[1] = static_cast<tubyte>(v & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitLSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian24BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian24BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian24BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian24BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitLSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitLSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x);
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitLSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitLSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitLSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned24BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned24BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned24BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned24BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitLSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitLSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v);
            out[0] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitLSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitLSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitMSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian24BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian24BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian24BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian24BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitMSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB) << shift;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitMSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x) << shiftMSB;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitMSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        x <<= shiftMSB;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian24BitMSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        x <<= shiftMSB;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitMSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned24BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned24BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned24BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned24BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitMSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();
    tint shift = calculateShift();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask) << shift;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitMSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v) << shiftMSB;
            out[0] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitMSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned24BitMSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitLSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndian24BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndian24BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndian24BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndian24BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitLSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        out[2] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitLSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[2] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x);
            out[2] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitLSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        out[2] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitLSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        out[2] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitLSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndianUnsigned24BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndianUnsigned24BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndianUnsigned24BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndianUnsigned24BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitLSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        out[2] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitLSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[2] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v);
            out[2] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitLSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask);
        out[2] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitLSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask);
        out[2] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitMSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndian24BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndian24BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndian24BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndian24BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitMSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB) << shift;
        out[2] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitMSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[2] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x) << shiftMSB;
            out[2] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian24BitMSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        x <<= shiftMSB;
        out[2] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------v

void SampleConverter::convertBigEndian24BitMSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        x <<= shiftMSB;
        out[2] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitMSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndianUnsigned24BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndianUnsigned24BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndianUnsigned24BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndianUnsigned24BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitMSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 3 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();
    tint shift = calculateShift();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask) << shift;
        out[2] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitMSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[2] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v) << shiftMSB;
            out[2] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitMSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 24 - m_noBits;
    maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
    max = 0x007fffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt24;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x00800000)
        {
            x |= signedMaskInt24();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[2] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned24BitMSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 3;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 32 - m_noBits;
    maskM = 1 << (shiftF - 1);
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[2] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitLSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian32BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian32BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian32BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian32BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitLSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[3] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitLSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x);
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitLSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM, xFinal;
    tint shiftF, max;
    
    if(m_noBits > 24)
    {
        shiftF = m_noBits - 24;
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            out[0] = static_cast<tbyte>(x & 0x000000ff);
            out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((x >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitLSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[3] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitLSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned32BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned32BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned32BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned32BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitLSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 4 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitLSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v);
            out[0] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitLSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM, min, xFinal;
    tuint32 v, umask;
    tint shiftF, max;

    if(m_noBits > 24)
    {
        shiftF = m_noBits - 24;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            x <<= shiftF;
            v = toUnsigned32(x, min, umask);
            xFinal = static_cast<tint32>(v);
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            v = toUnsigned32(x, min, umask);
            out[0] = static_cast<tubyte>(v & 0x000000ff);
            out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
            out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitLSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask);
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitMSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndian32BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndian32BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndian32BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndian32BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitMSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB) << shift;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[3] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitMSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x) << shiftMSB;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitMSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 24)
    {
        shiftF = (m_noBits - 24) + calculateShift();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            x <<= shiftMSB;
            out[0] = static_cast<tbyte>(x & 0x000000ff);
            out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
            out[3] = static_cast<tbyte>((x >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndian32BitMSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        x <<= shiftMSB;
        out[0] = static_cast<tbyte>(x & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[3] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitMSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertLittleEndianUnsigned32BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertLittleEndianUnsigned32BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertLittleEndianUnsigned32BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertLittleEndianUnsigned32BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitMSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 4 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();
    tint shift = calculateShift();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask) << shift;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitMSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max;
    tint shiftMSB = calculateShift();
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v) << shiftMSB;
            out[0] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitMSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min, xFinal;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    if(m_noBits > 24)
    {
        shiftF = m_noBits - 24;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            x <<= shiftF;
            v = toUnsigned32(x, min, umask) << shiftMSB;
            xFinal = static_cast<tint32>(v);
            out[0] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            v = toUnsigned32(x, min, umask) << shiftMSB;
            out[0] = static_cast<tubyte>(v & 0x000000ff);
            out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
            out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
            out[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianUnsigned32BitMSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[0] = static_cast<tubyte>(v & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitLSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndian32BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndian32BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndian32BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndian32BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitLSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        out[3] = static_cast<tbyte>(x & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitLSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x);
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitLSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM, xFinal;
    tint shiftF, max;
    
    if(m_noBits > 24)
    {
        shiftF = m_noBits - 24;
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            out[3] = static_cast<tbyte>(x & 0x000000ff);
            out[2] = static_cast<tbyte>((x >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((x >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((x >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitLSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max;
    
    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        out[3] = static_cast<tbyte>(x & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitLSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndianUnsigned32BitLSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndianUnsigned32BitLSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndianUnsigned32BitLSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndianUnsigned32BitLSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitLSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 4 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask);
        out[3] = static_cast<tubyte>(v & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitLSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max;
    
    if(m_noBits > 16)
    {
        shiftF = m_noBits - 16;
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v);
            out[3] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitLSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);
    tint32 x, xOrg, maskM, min, xFinal;
    tuint32 v, umask;
    tint shiftF, max;

    if(m_noBits > 24)
    {
        shiftF = m_noBits - 24;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            x <<= shiftF;
            v = toUnsigned32(x, min, umask);
            xFinal = static_cast<tint32>(v);
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            v = toUnsigned32(x, min, umask);
            out[3] = static_cast<tubyte>(v & 0x000000ff);
            out[2] = static_cast<tubyte>((v >> 8) & 0x000000ff);
            out[1] = static_cast<tubyte>((v >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((v >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitLSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;

    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask);
        out[3] = static_cast<tubyte>(v & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitMSB(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndian32BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndian32BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndian32BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndian32BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitMSBFloat(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    tint shift = calculateShift();
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB) << shift;
        out[3] = static_cast<tbyte>(x & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitMSBInt16(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);    
    tint16 x, xOrg, maskM;
    tint32 xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            xFinal = static_cast<tint32>(x) << shiftMSB;
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitMSBInt24(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, xFinal;
    tint shiftF, max, shiftMSB;
    
    if(m_noBits > 24)
    {
        shiftF = (m_noBits - 24) + calculateShift();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            xFinal = static_cast<tint32>(x) << shiftF;
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        shiftMSB = calculateShift();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            x <<= shiftMSB;
            out[3] = static_cast<tbyte>(x & 0x000000ff);
            out[2] = static_cast<tbyte>((x >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((x >> 16) & 0x000000ff);
            out[0] = static_cast<tbyte>((x >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndian32BitMSBInt32(const sample_t *in,tbyte *out,tint noSamples) const
{
    tint inc = sizeof(tint32) * m_noOutChannels;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM;
    tint shiftF, max, shiftMSB;
    
    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    shiftMSB = calculateShift();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        x <<= shiftMSB;
        out[3] = static_cast<tbyte>(x & 0x000000ff);
        out[2] = static_cast<tbyte>((x >> 8) & 0x000000ff);
        out[1] = static_cast<tbyte>((x >> 16) & 0x000000ff);
        out[0] = static_cast<tbyte>((x >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitMSB(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    if(type & engine::e_SampleInt16)
    {
        convertBigEndianUnsigned32BitMSBInt16(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt24)
    {
        convertBigEndianUnsigned32BitMSBInt24(in,out,noSamples);
    }
    else if(type & engine::e_SampleInt32)
    {
        convertBigEndianUnsigned32BitMSBInt32(in,out,noSamples);
    }
    else
    {
        convertBigEndianUnsigned32BitMSBFloat(in,out,noSamples);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitMSBFloat(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = 4 * m_noOutChannels;
    tuint32 s = static_cast<tuint32>(1 << (m_noBits - 1));
    sample_t dA = static_cast<sample_t>(s - 1);
    sample_t dB = static_cast<sample_t>(s);
    tuint32 min = static_cast<tuint32>(sampleToInteger(-1,dA,dB));
    tuint32 umask = unsignedMask();
    tint shift = calculateShift();

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        tint x = sampleToInteger(*in,dA,dB);
        tuint32 v = toUnsigned32(x, min, umask) << shift;
        out[3] = static_cast<tubyte>(v & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitMSBInt16(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
    tint16 x, xOrg, maskM, min;
    tuint16 v, umask;
    tint32 xFinal;
    tint shiftF, max;
    tint shiftMSB = calculateShift();
    
    if(m_noBits > 16)
    {
        shiftF = (m_noBits - 16) + calculateShift();
        min = minIntegerValue16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = *inInt16;
            v = toUnsigned16(x, min, 0xffff);
            xFinal = static_cast<tint32>(v) << shiftF;
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 16 - m_noBits;
        maskM = 1 << (shiftF - 1);
        max = 0x7fff >> shiftF;
        min = minIntegerValue16();
        umask = unsignedMask16();
        for(tint i=0;i<noSamples;i++,inInt16+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt16;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x8000)
            {
                x |= signedMaskInt16();
            }
            v = toUnsigned16(x, min, umask);
            xFinal = static_cast<tint32>(v) << shiftMSB;
            out[3] = static_cast<tubyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tubyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tubyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitMSBInt24(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt24 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask, xFinal;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    if(m_noBits > 24)
    {
        shiftF = m_noBits - 24;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = *inInt24;
            x <<= shiftF;
            v = toUnsigned32(x, min, umask) << shiftMSB;
            xFinal = static_cast<tint32>(v);
            out[3] = static_cast<tbyte>(xFinal & 0x000000ff);
            out[2] = static_cast<tbyte>((xFinal >> 8) & 0x000000ff);
            out[1] = static_cast<tbyte>((xFinal >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((xFinal >> 24) & 0x000000ff);
        }
    }
    else
    {
        shiftF = 24 - m_noBits;
        maskM = (m_noBits < 24) ? 1 << (shiftF - 1) : 0;
        max = 0x007fffff >> shiftF;
        min = minIntegerValue32();
        umask = unsignedMask();
        for(tint i=0;i<noSamples;i++,inInt24+=m_noInChannels,out+=inc)
        {
            x = xOrg = *inInt24;
            x >>= shiftF;
            if((xOrg & maskM) && x < max)
            {
                x++;
            }
            if(xOrg & 0x00800000)
            {
                x |= signedMaskInt24();
            }
            v = toUnsigned32(x, min, umask) << shiftMSB;
            out[3] = static_cast<tubyte>(v & 0x000000ff);
            out[2] = static_cast<tubyte>((v >> 8) & 0x000000ff);
            out[1] = static_cast<tubyte>((v >> 16) & 0x000000ff);
            out[0] = static_cast<tubyte>((v >> 24) & 0x000000ff);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianUnsigned32BitMSBInt32(const sample_t *in,tubyte *out,tint noSamples) const
{
    tint inc = sizeof(tbyte) * m_noOutChannels * 4;
    const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);    
    tint32 x, xOrg, maskM, min;
    tuint32 v, umask;
    tint shiftF, max;
    tint shiftMSB = calculateShift();

    shiftF = 32 - m_noBits;
    maskM = (m_noBits < 32) ? 1 << (shiftF - 1) : 0;
    max = 0x7fffffff >> shiftF;
    min = minIntegerValue32();
    umask = unsignedMask();
    for(tint i=0;i<noSamples;i++,inInt32+=m_noInChannels,out+=inc)
    {
        x = xOrg = *inInt32;
        x >>= shiftF;
        if((xOrg & maskM) && x < max)
        {
            x++;
        }
        if(xOrg & 0x80000000)
        {
            x |= signedMaskInt32();
        }
        v = toUnsigned32(x, min, umask) << shiftMSB;
        out[3] = static_cast<tubyte>(v & 0x000000ff);
        out[2] = static_cast<tubyte>((v >> 8) & 0x000000ff);
        out[1] = static_cast<tubyte>((v >> 16) & 0x000000ff);
        out[0] = static_cast<tubyte>((v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianSinglePrecision(const sample_t *in,tbyte *out,tint noSamples) const
{
    union P 
    {
        tfloat32 x;
        tuint32 v;
    };

    tint inc = sizeof(tfloat32) * m_noOutChannels;

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        P x;
        x.x = static_cast<tfloat32>(*in);
        out[0] = static_cast<tbyte>(x.v & 0x000000ff);
        out[1] = static_cast<tbyte>((x.v >> 8) & 0x000000ff);
        out[2] = static_cast<tbyte>((x.v >> 16) & 0x000000ff);
        out[3] = static_cast<tbyte>((x.v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianSinglePrecision(const sample_t *in,tbyte *out,tint noSamples) const
{
    union P 
    {
        tfloat32 x;
        tuint32 v;
    };

    tint inc = sizeof(tfloat32) * m_noOutChannels;

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        P x;
        x.x = static_cast<tfloat32>(*in);
        out[3] = static_cast<tbyte>(x.v & 0x000000ff);
        out[2] = static_cast<tbyte>((x.v >> 8) & 0x000000ff);
        out[1] = static_cast<tbyte>((x.v >> 16) & 0x000000ff);
        out[0] = static_cast<tbyte>((x.v >> 24) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertLittleEndianDoublePrecision(const sample_t *in,tbyte *out,tint noSamples) const
{
    union P 
    {
        tfloat64 x;
        tuint64 v;
    };

    tint inc = sizeof(tfloat64) * m_noOutChannels;

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        P x;
        x.x = static_cast<tfloat64>(*in);
        out[0] = static_cast<tbyte>(x.v & 0x00000000000000ffULL);
        out[1] = static_cast<tbyte>((x.v >> 8) & 0x00000000000000ffULL);
        out[2] = static_cast<tbyte>((x.v >> 16) & 0x00000000000000ffULL);
        out[3] = static_cast<tbyte>((x.v >> 24) & 0x00000000000000ffULL);
        out[4] = static_cast<tbyte>((x.v >> 32) & 0x00000000000000ffULL);
        out[5] = static_cast<tbyte>((x.v >> 40) & 0x00000000000000ffULL);
        out[6] = static_cast<tbyte>((x.v >> 48) & 0x00000000000000ffULL);
        out[7] = static_cast<tbyte>((x.v >> 56) & 0x00000000000000ffULL);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertBigEndianDoublePrecision(const sample_t *in,tbyte *out,tint noSamples) const
{
    union P 
    {
        tfloat64 x;
        tuint64 v;
    };

    tint inc = sizeof(tfloat64) * m_noOutChannels;

    for(tint i=0;i<noSamples;i++,in+=m_noInChannels,out+=inc)
    {
        P x;
        x.x = static_cast<tfloat64>(*in);
        out[7] = static_cast<tbyte>(x.v & 0x00000000000000ffULL);
        out[6] = static_cast<tbyte>((x.v >> 8) & 0x00000000000000ffULL);
        out[5] = static_cast<tbyte>((x.v >> 16) & 0x00000000000000ffULL);
        out[4] = static_cast<tbyte>((x.v >> 24) & 0x00000000000000ffULL);
        out[3] = static_cast<tbyte>((x.v >> 32) & 0x00000000000000ffULL);
        out[2] = static_cast<tbyte>((x.v >> 40) & 0x00000000000000ffULL);
        out[1] = static_cast<tbyte>((x.v >> 48) & 0x00000000000000ffULL);
        out[0] = static_cast<tbyte>((x.v >> 56) & 0x00000000000000ffULL);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::volumeInt16Upscale(const tint16 *in, tint32 *out, tint noSamples) const
{
    tint64 vS = static_cast<tint64>(4294967296.0 * m_volume);

    for(tint i=0; i<noSamples; i++, in+=m_noInChannels, out+=m_noInChannels)
    {
        tint64 x = static_cast<tint64>(*in) * vS;
        tint32 y = static_cast<tint32>(x >> 16);
        if(x & 0x0000000000008000ULL)
        {
            y++;
        }
        *out = y;
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::volumeInt24Upscale(const tint32 *in, tint32 *out, tint noSamples) const
{
    tint64 vS = static_cast<tint64>(4294967296.0 * m_volume);

    for(tint i=0; i<noSamples; i++, in+=m_noInChannels, out+=m_noInChannels)
    {
        tint64 x = static_cast<tint64>(*in) * vS;
        tint32 y = static_cast<tint32>(x >> 24);
        if(x & 0x0000000000800000ULL)
        {
            y++;
        }
        *out = y;
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::volumeInt32Upscale(const tint32 *in, tint32 *out, tint noSamples) const
{
    tint64 vS = static_cast<tint64>(4294967296.0 * m_volume);

    for(tint i=0; i<noSamples; i++, in+=m_noInChannels, out+=m_noInChannels)
    {
        tint64 x = static_cast<tint64>(*in) * vS;
        tint32 y = static_cast<tint32>(x >> 32);
        if(x & 0x0000000080000000ULL)
        {
            y++;
        }
        *out = y;
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::volumeIntUpscale(const sample_t *in, tint32 *out, tint noSamples, engine::CodecDataType type) const
{
    if(type == engine::e_SampleInt24)
    {
        const tint32 *input = reinterpret_cast<const tint32 *>(in);
        volumeInt24Upscale(input, out, noSamples);
    }
    else if(type == engine::e_SampleInt32)
    {
        const tint32 *input = reinterpret_cast<const tint32 *>(in);
        volumeInt32Upscale(input, out, noSamples);
    }
    else
    {
        const tint16 *input = reinterpret_cast<const tint16 *>(in);
        volumeInt16Upscale(input, out, noSamples);
    }
}

//-------------------------------------------------------------------------------------------

tint32 *SampleConverter::volumeIntBuffer(tint noSamples) const
{
    tint reqSize = noSamples * m_noInChannels;
    
    if(m_vIntBuffer == NULL || reqSize > m_vIntBufSize)
    {
        if(m_vIntBuffer != NULL)
        {
            delete [] m_vIntBuffer;
        }
        m_vIntBuffer = new tint32 [reqSize];
        m_vIntBufSize = reqSize;
    }
    return m_vIntBuffer;
}

//-------------------------------------------------------------------------------------------

bool SampleConverter::isIntegerType(engine::CodecDataType type) const
{
    return (type == engine::e_SampleInt16 || type == engine::e_SampleInt24 || type == engine::e_SampleInt32);
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertInteger(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    switch(m_bytesPerSample)
    {
        case 1:
            if(!m_alignHigh)
            {
                convertLittleEndian8BitLSB(in,out,noSamples,type);
            }
            else
            {
                convertLittleEndian8BitMSB(in,out,noSamples,type);
            }
            break;

        case 2:
            if(m_littleEndian)
            {
                if(!m_alignHigh)
                {
                    convertLittleEndian16BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertLittleEndian16BitMSB(in,out,noSamples,type);
                }
            }
            else
            {
                if(!m_alignHigh)
                {
                    convertBigEndian16BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertBigEndian16BitMSB(in,out,noSamples,type);
                }
            }
            break;

        case 3:
            if(m_littleEndian)
            {
                if(!m_alignHigh)
                {
                    convertLittleEndian24BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertLittleEndian24BitMSB(in,out,noSamples,type);
                }
            }
            else
            {
                if(!m_alignHigh)
                {
                    convertBigEndian24BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertBigEndian24BitMSB(in,out,noSamples,type);
                }
            }
            break;

        case 4:
        default:
            if(m_littleEndian)
            {
                if(!m_alignHigh)
                {
                    convertLittleEndian32BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertLittleEndian32BitMSB(in,out,noSamples,type);
                }
            }
            else
            {
                if(!m_alignHigh)
                {
                    convertBigEndian32BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertBigEndian32BitMSB(in,out,noSamples,type);
                }
            }
            break;
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertUnsignedInteger(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    switch(m_bytesPerSample)
    {
        case 1:
            if(!m_alignHigh)
            {
                convertLittleEndianUnsigned8BitLSB(in,out,noSamples,type);
            }
            else
            {
                convertLittleEndianUnsigned8BitMSB(in,out,noSamples,type);
            }
            break;

        case 2:
            if(m_littleEndian)
            {
                if(!m_alignHigh)
                {
                    convertLittleEndianUnsigned16BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertLittleEndianUnsigned16BitMSB(in,out,noSamples,type);
                }
            }
            else
            {
                if(!m_alignHigh)
                {
                    convertBigEndianUnsigned16BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertBigEndianUnsigned16BitMSB(in,out,noSamples,type);
                }
            }
            break;

        case 3:
            if(m_littleEndian)
            {
                if(!m_alignHigh)
                {
                    convertLittleEndianUnsigned24BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertLittleEndianUnsigned24BitMSB(in,out,noSamples,type);
                }
            }
            else
            {
                if(!m_alignHigh)
                {
                    convertBigEndianUnsigned24BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertBigEndianUnsigned24BitMSB(in,out,noSamples,type);
                }
            }
            break;

        case 4:
        default:
            if(m_littleEndian)
            {
                if(!m_alignHigh)
                {
                    convertLittleEndianUnsigned32BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertLittleEndianUnsigned32BitMSB(in,out,noSamples,type);
                }
            }
            else
            {
                if(!m_alignHigh)
                {
                    convertBigEndianUnsigned32BitLSB(in,out,noSamples,type);
                }
                else
                {
                    convertBigEndianUnsigned32BitMSB(in,out,noSamples,type);
                }
            }
            break;
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convert(const sample_t *in,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    switch(m_type)
    {
        case FormatDescription::e_DataFloatSingle:
            if(m_littleEndian)
            {
                convertLittleEndianSinglePrecision(in,out,noSamples);
            }
            else
            {
                convertBigEndianSinglePrecision(in,out,noSamples);
            }
            break;
            
        case FormatDescription::e_DataFloatDouble:
            if(m_littleEndian)
            {
                convertLittleEndianDoublePrecision(in,out,noSamples);
            }
            else
            {
                convertBigEndianDoublePrecision(in,out,noSamples);
            }
            break;
        
        default:
            if(isIntegerType(type) && !isEqual(m_volume, 1.0))
            {
                tint32 *uI = volumeIntBuffer(noSamples);
                volumeIntUpscale(in, uI, noSamples, type);
                if(m_type == FormatDescription::e_DataUnsignedInteger)
                {
                    convertUnsignedInteger(reinterpret_cast<const sample_t *>(uI),reinterpret_cast<tubyte *>(out),noSamples,engine::e_SampleInt32);
                }
                else
                {
                    convertInteger(reinterpret_cast<const sample_t *>(uI),out,noSamples,engine::e_SampleInt32);
                }
            }
            else
            {
                if (m_type == FormatDescription::e_DataUnsignedInteger)
                {
                    convertUnsignedInteger(in, reinterpret_cast<tubyte*>(out), noSamples, type);
                }
                else
                {
                    convertInteger(in, out, noSamples, type);
                }
            }
            break;
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convert(const sample_t *in,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    convert(in,reinterpret_cast<tbyte *>(out),noSamples,type);
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertAtIndex(const sample_t *in,tint idx,tbyte *out,tint noSamples,engine::CodecDataType type) const
{
    const sample_t *inFinal;

    if(type & engine::e_SampleInt16)
    {
        const tint16 *inInt16 = reinterpret_cast<const tint16 *>(in);
        inFinal = reinterpret_cast<const sample_t *>(&inInt16[idx]);
        convert(inFinal,out,noSamples,type);
    }
    else if(type & engine::e_SampleInt24 || type & engine::e_SampleInt32)
    {
        const tint32 *inInt32 = reinterpret_cast<const tint32 *>(in);
        inFinal = reinterpret_cast<const sample_t *>(&inInt32[idx]);
        convert(inFinal,out,noSamples,type);
    }
    else
    {
        convert(&in[idx],out,noSamples,type);
    }
}

//-------------------------------------------------------------------------------------------

void SampleConverter::convertAtIndex(const sample_t *in,tint idx,tubyte *out,tint noSamples,engine::CodecDataType type) const
{
    convertAtIndex(in,idx,reinterpret_cast<tbyte *>(out),noSamples,type);
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
