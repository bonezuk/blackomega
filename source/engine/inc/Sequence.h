//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SEQUENCE_H
#define __OMEGA_ENGINE_SEQUENCE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BString.h"
#include "common/inc/Array.h"
#include "engine/inc/BitStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Sequence
{
    public:
        friend class Bitstream;

    public:
        Sequence();
        virtual ~Sequence();

        Bitstream *bitstream();

        virtual bool seek(tint offset);

        virtual bool move(tint bkMark);

        tint readBit();

        void byteAlignment();

        tint readBits(tint n);
        tint browseBits(tint n);

        tint readBitI();
        tint readBitFastI();
        tint readBitsI(tint n);

        tint readBitsIL(tint n);
        tint readBitsSignI(tint n);

        tuint64 read64BitsI();

        virtual void reset();

        virtual bool isEnd() const;
        virtual bool isEndStream();

        virtual tint bookmark();

        virtual tint copy(tbyte *mem,tint len);

        virtual tint readMemory(tbyte *mem,tint len);
        virtual tint readMemory(tubyte *mem,tuint len);

        virtual tint remain() const;

        virtual bool readBookmarkMemory(tint bkStart,tint bkEnd,common::Array<tubyte,tubyte>& arr);

    protected:

        Bitstream *m_bitstream;
        Associate *m_item;

        tuint m_bitOffset;
        tuint m_length;
        tubyte *m_buffer;
        bool m_end;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual bool previous();
        virtual bool next();
        virtual bool checkNext();
};

//-------------------------------------------------------------------------------------------

inline Bitstream *Sequence::bitstream()
{
    return m_bitstream;
}

//-------------------------------------------------------------------------------------------

inline tint Sequence::readBit()
{
    static const tubyte mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

    tint x;

    // Ensure that the end of the bitstream has not already been done.
    if(m_end || m_bitOffset>=m_length)
    {
        if(!next())
        {
            return 0;
        }
    }

    x = (m_buffer[m_bitOffset >> 3] & mask[m_bitOffset & 0x00000007]) ? 1 : 0;
    m_bitOffset++;
    return x;
}

//-------------------------------------------------------------------------------------------
// Used if it guarenteed that m_bitOffset < m_length
//-------------------------------------------------------------------------------------------

inline tint Sequence::readBitFastI()
{
    tint a,n;
    tuint32 *buffer = &(reinterpret_cast<tuint32 *>(m_buffer)[m_bitOffset >> 5]);

    a = 31 - (m_bitOffset & 0x0000001f);
    if(a)
    {
        n = static_cast<tint>(((*buffer) >> a) & 0x00000001);
    }
    else
    {
        n = static_cast<tint>((*buffer) & 0x00000001);
    }
    m_bitOffset++;
    return n;
}

//-------------------------------------------------------------------------------------------

inline void Sequence::byteAlignment()
{
    tint offset = m_bitOffset & 0x00000007;

    if(offset)
    {
        offset = 8 - offset;
        m_bitOffset += offset;
    }
}

//-------------------------------------------------------------------------------------------

inline tint Sequence::remain() const
{
    return m_length - m_bitOffset;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

