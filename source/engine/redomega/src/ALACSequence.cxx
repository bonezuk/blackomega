#include "engine/redomega/inc/ALACSequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACSequence::ALACSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array) : engine::SequenceMemory(array)
{}

//-------------------------------------------------------------------------------------------

ALACSequence::ALACSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array,tint bitLength) : engine::SequenceMemory(array)
{}

//-------------------------------------------------------------------------------------------

ALACSequence::~ALACSequence()
{}

//-------------------------------------------------------------------------------------------

tuint32 ALACSequence::readBERInteger()
{
    tint count = 0;
    tuint32 a = 0,b;

    byteAlignment();

    do
    {
        tuint32 *buffer = &(reinterpret_cast<tuint32 *>(m_buffer)[m_bitOffset >> 5]);
        engine::Bitstream::BSMask *ms = &(Bitstream::m_maskI[8][m_bitOffset & 0x0000001f]);
        b = ((*buffer) >> ms->rShiftA)  & ms->rMaskA;
        a = (a << 7) | (b & 0x0000007f);
        count++;
        if(count>5)
        {
            return 0;
        }
        m_bitOffset += 8;
    } while((b & 0x00000080) && m_bitOffset<m_length);

    return a;
}

//-------------------------------------------------------------------------------------------

tfloat32 ALACSequence::readFloat()
{
    union
    {
        tfloat32 f;
        tint32 i;
    } x;
    x.i = readBitsI(32);
    return x.f;
}

//-------------------------------------------------------------------------------------------

tfloat64 ALACSequence::readDouble()
{
    union
    {
        tfloat64 f;
        tint64 i;
    } x;
    x.i = ((static_cast<tint64>(readBits(32)) << 32) & 0xffffffff00000000ULL) | (static_cast<tint64>(readBits(32)) & 0x00000000ffffffffULL);
    return x.f;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
