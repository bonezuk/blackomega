//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_WSEQUENCE_H
#define __OMEGA_WHITEOMEGA_WSEQUENCE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Exception.h"
#include "engine/inc/SequenceMemory.h"
#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/WhiteOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WSequenceException : public common::Exception
{
    public:
        WSequenceException();
};

//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT WSequence : public engine::SequenceMemory
{
    public:
    
        class HNode
        {
            public:
                HNode();
                ~HNode();
            
                HNode *parent;
                HNode *left;
                HNode *right;
                tint w;
                tint x;
                tint y;
                tint z;
                bool leaf;
        };

        typedef enum
        {
            e_cbUnsigned4 = 0,
            e_cbUnsigned2,
            e_cbSigned4,
            e_cbSigned2,
            e_cbSigned1,
            e_cbEscape,
            e_cbRVLC,
            e_cbRVLCHuff
        } CodebookType;

        typedef struct
        {
            CodebookType type;
            tint dimension;
            tint lav;
            tint maxBits;
            HNode *root;
        } Codebook;
        
        typedef struct
        {
            tint start;
            tint end;
        } Segment;
        
        typedef struct
        {
            tint codebook;
            tint position;
        } CWInfo;
        
        typedef struct
        {
            tint segment;
            tuint state;
            HNode *node;
            tuint value;
        } CWState;
    
    public:
        WSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array);
        WSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array,tint bitLength);
        virtual ~WSequence();
        
        static void start();
        static void stop();
        
        tint offset() const;
        void setOffset(tint amount);
                
        tint decodeSf();
        
        tint decodeRVLC();
        tint decodeRVLCRev();
        tint decodeRVLCEsc();
        
        tint decodeSpectral(tint cb,tint *q);
        void decodeUnsigned4(tint cb,tint *q);
        void decodeUnsigned2(tint cb,tint *q); 
        void decodeSigned4(tint cb,tint *q);
        void decodeSigned2(tint cb,tint *q);
        void decodeEsc(tint cb,tint *q);
        
        void readHCRSet(CWInfo *cwInfo,tint& cwIdx,tint cwLen,Segment *s,tint sLen,tint *spectralData,bool direction);
                    
    protected:
    
        static Codebook m_books[34];
        
        static tint m_scalefactorCodebookTable[121][3];
        static tint m_spectrumCodebook1Table[81][3];
        static tint m_spectrumCodebook2Table[81][3];
        static tint m_spectrumCodebook3Table[81][3];
        static tint m_spectrumCodebook4Table[81][3];
        static tint m_spectrumCodebook5Table[81][3];
        static tint m_spectrumCodebook6Table[81][3];
        static tint m_spectrumCodebook7Table[64][3];
        static tint m_spectrumCodebook8Table[64][3];
        static tint m_spectrumCodebook9Table[169][3];
        static tint m_spectrumCodebook10Table[169][3];
        static tint m_spectrumCodebook11Table[289][3];
        static tint m_rvlcCodebook1Table[15][3];
        static tint m_rvlcCodebook2Table[54][3];
        
        tint m_bitOffsetB;
        tuint32 *m_buffer32;
        
        static void buildHTree(Codebook *book,tint *cb,tint cbSize);
#if defined(OMEGA_DEBUG)
        static void testHTree(HNode *root);
#endif

        tint rbA();
        tint rbA(tint n);
        tint rbAFast();
        tint rbAFast(tint n);
        tint rbARev();
        tint rbARevFast();
        
        bool readHCRForward(tint cb,tint *q,CWState *cw,Segment *s);
        bool readHCRReverse(tint cb,tint *q,CWState *cw,Segment *s);
        
        tint readHCRFwdBit(Segment *s);
        tint readHCRRevBit(Segment *s);

        bool decodeHCRFwdUnsigned4(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRRevUnsigned4(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRFwdUnsigned2(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRRevUnsigned2(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRFwdSigned4(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRRevSigned4(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRFwdSigned2(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRRevSigned2(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRFwdEscape(tint cb,tint *q,CWState *cw,Segment *s);
        bool decodeHCRRevEscape(tint cb,tint *q,CWState *cw,Segment *s);
};

//-------------------------------------------------------------------------------------------

inline tint WSequence::offset() const
{
    return m_bitOffset;
}

//-------------------------------------------------------------------------------------------

inline void WSequence::setOffset(tint amount)
{
    m_bitOffset = amount;
}

//-------------------------------------------------------------------------------------------

inline tint WSequence::rbAFast()
{
    static const tuint32 mask[32] = {
        0x80000000, 0x40000000, 0x20000000, 0x10000000,
        0x08000000, 0x04000000, 0x02000000, 0x01000000,
        0x00800000, 0x00400000, 0x00200000, 0x00100000,
        0x00080000, 0x00040000, 0x00020000, 0x00010000,
        0x00008000, 0x00004000, 0x00002000, 0x00001000,
        0x00000800, 0x00000400, 0x00000200, 0x00000100,
        0x00000080, 0x00000040, 0x00000020, 0x00000010,
        0x00000008, 0x00000004, 0x00000002, 0x00000001
    };
    
    if(m_buffer32[m_bitOffset >> 5] & mask[m_bitOffset & 0x0000001f])
    {
        m_bitOffset++;
        return 1;
    }
    else
    {
        m_bitOffset++;
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

inline tint WSequence::rbAFast(tint n)
{
    tint m;
    engine::Bitstream::BSMask *ms = &(engine::Bitstream::m_maskI[n][m_bitOffset & 0x0000001f]);
    
    if(ms->width==2)
    {
        tuint32 *buffer = &(m_buffer32[m_bitOffset >> 5]);
        m  = static_cast<tint>((buffer[0] << ms->rShiftA) & ms->rMaskA);
        m |= static_cast<tint>((buffer[1] >> ms->rShiftB) & ms->rMaskB);
    }
    else
    {
        m  = static_cast<tint>((m_buffer32[m_bitOffset >> 5] >> ms->rShiftA) & ms->rMaskA);
    }
    m_bitOffset += n;
    return m;
}

//-------------------------------------------------------------------------------------------

inline tint WSequence::rbARevFast()
{
    static const tuint32 mask[32] = {
        0x80000000, 0x40000000, 0x20000000, 0x10000000,
        0x08000000, 0x04000000, 0x02000000, 0x01000000,
        0x00800000, 0x00400000, 0x00200000, 0x00100000,
        0x00080000, 0x00040000, 0x00020000, 0x00010000,
        0x00008000, 0x00004000, 0x00002000, 0x00001000,
        0x00000800, 0x00000400, 0x00000200, 0x00000100,
        0x00000080, 0x00000040, 0x00000020, 0x00000010,
        0x00000008, 0x00000004, 0x00000002, 0x00000001
    };
    
    m_bitOffset--;
    if(m_buffer32[m_bitOffset >> 5] & mask[m_bitOffset & 0x0000001f])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

