#include <QtGlobal>
#include "engine/whiteomega/inc/WSequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------
// WSequenceException
//-------------------------------------------------------------------------------------------

WSequenceException::WSequenceException() : common::Exception("WSequence")
{}

//-------------------------------------------------------------------------------------------
// WSequence::HNode
//-------------------------------------------------------------------------------------------

WSequence::HNode::HNode() : parent(0),
    left(0),
    right(0),
    w(0),
    x(0),
    y(0),
    z(0),
    leaf(false)
{}

//-------------------------------------------------------------------------------------------

WSequence::HNode::~HNode()
{
    if(left!=0)
    {
        delete left;
        left = 0;
    }
    if(right!=0)
    {
        delete right;
        right = 0;
    }
}

//-------------------------------------------------------------------------------------------
// WSequence
//-------------------------------------------------------------------------------------------

WSequence::WSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array) : engine::SequenceMemory(array),
    m_bitOffsetB(0),
    m_buffer32(reinterpret_cast<tuint32 *>(array->GetData()))
{}

//-------------------------------------------------------------------------------------------

WSequence::WSequence(QSharedPointer<common::Array<tubyte,tubyte> >& array,tint bitLength) : engine::SequenceMemory(array,bitLength),
    m_bitOffsetB(0),
    m_buffer32(reinterpret_cast<tuint32 *>(array->GetData()))
{}

//-------------------------------------------------------------------------------------------

WSequence::~WSequence()
{}

//-------------------------------------------------------------------------------------------

void WSequence::start()
{
    tint i;
    
    buildHTree(&(m_books[0]),reinterpret_cast<tint *>(m_scalefactorCodebookTable),121);
    buildHTree(&(m_books[1]),reinterpret_cast<tint *>(m_spectrumCodebook1Table),81);
    buildHTree(&(m_books[2]),reinterpret_cast<tint *>(m_spectrumCodebook2Table),81);
    buildHTree(&(m_books[3]),reinterpret_cast<tint *>(m_spectrumCodebook3Table),81);
    buildHTree(&(m_books[4]),reinterpret_cast<tint *>(m_spectrumCodebook4Table),81);
    buildHTree(&(m_books[5]),reinterpret_cast<tint *>(m_spectrumCodebook5Table),81);
    buildHTree(&(m_books[6]),reinterpret_cast<tint *>(m_spectrumCodebook6Table),81);
    buildHTree(&(m_books[7]),reinterpret_cast<tint *>(m_spectrumCodebook7Table),64);
    buildHTree(&(m_books[8]),reinterpret_cast<tint *>(m_spectrumCodebook8Table),64);
    buildHTree(&(m_books[9]),reinterpret_cast<tint *>(m_spectrumCodebook9Table),169);
    buildHTree(&(m_books[10]),reinterpret_cast<tint *>(m_spectrumCodebook10Table),169);
    buildHTree(&(m_books[11]),reinterpret_cast<tint *>(m_spectrumCodebook11Table),289);
    
    for(i=16;i<=31;++i)
    {
        m_books[i].maxBits = m_books[11].maxBits;
        m_books[i].root = m_books[11].root;
    }
    
    buildHTree(&(m_books[32]),reinterpret_cast<tint *>(m_rvlcCodebook1Table),15);
    buildHTree(&(m_books[33]),reinterpret_cast<tint *>(m_rvlcCodebook2Table),54);
}

//-------------------------------------------------------------------------------------------

void WSequence::stop()
{
    tint i;
    
    for(i=0;i<12;++i)
    {
        if(m_books[i].root!=0)
        {
            delete m_books[i].root;
            m_books[i].root = 0;
        }
    }
    for(i=32;i<34;++i)
    {
        if(m_books[i].root!=0)
        {
            delete m_books[i].root;
            m_books[i].root = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

void WSequence::buildHTree(Codebook *book,tint *cb,tint cbSize)
{
    static tuint32 mask[32] = {
        0x00000001, 0x00000002, 0x00000004, 0x00000008,  //  4
        0x00000010, 0x00000020, 0x00000040, 0x00000080,  //  8
        0x00000100, 0x00000200, 0x00000400, 0x00000800,  // 12
        0x00001000, 0x00002000, 0x00004000, 0x00008000,  // 16
        0x00010000, 0x00020000, 0x00040000, 0x00080000,  // 20
        0x00100000, 0x00200000, 0x00400000, 0x00800000,  // 24
        0x01000000, 0x02000000, 0x04000000, 0x08000000,  // 28
        0x10000000, 0x20000000, 0x40000000, 0x80000000   // 32
    };
    
    tint i,j,mod,off,idx;
    HNode *root = new HNode,*node;
    
    book->root = root;
    
    if(!(book->type==e_cbSigned1 || book->type==e_cbSigned2 || book->type==e_cbSigned4))
    {
        mod = book->lav + 1;
        off = 0;
    }
    else
    {
        mod = (2 * book->lav) + 1;
        off = book->lav;
    }
    
    for(i=0;i<cbSize;++i,cb+=3)
    {
        tuint32 codeword = static_cast<tuint32>(cb[2]);
        node = root;
        
        if(cb[1] > book->maxBits)
        {
            book->maxBits = cb[1];
        }
        
        for(j=cb[1]-1;j>=0;j--)
        {
            if(mask[j] & codeword)
            {
                if(node->right==0)
                {
                    node->right = new HNode;
                    node->right->parent = node;
                }
                node = node->right;
            }
            else
            {
                if(node->left==0)
                {
                    node->left = new HNode;
                    node->left->parent = node;
                }
                node = node->left;
            }
        }
        idx = cb[0];
        
        if(book->dimension==4)
        {
            node->w = static_cast<tint>(static_cast<tfloat32>(idx) / static_cast<tfloat32>(mod * mod * mod)) - off;
            idx -= (node->w + off) * (mod * mod * mod);
            node->x = static_cast<tint>(static_cast<tfloat32>(idx) / static_cast<tfloat32>(mod * mod)) - off;
            idx -= (node->x + off) * (mod * mod);
            node->y = static_cast<tint>(static_cast<tfloat32>(idx) / static_cast<tfloat32>(mod)) - off;
            idx -= (node->y + off) * (mod);
            node->z = idx - off;
        }
        else if(book->dimension==2)
        {
            node->y = static_cast<tint>(static_cast<tfloat32>(idx) / static_cast<tfloat32>(mod)) - off;
            idx -= (node->y + off) * mod;
            node->z = idx - off;
        }
        else
        {
            if(book->type==e_cbSigned1)
            {
                node->z = idx - 60;
            }
            else
            {
                node->z = idx;
            }
        }
        
        node->leaf = true;
    }
    
#if defined(OMEGA_DEBUG)
    if(book->type!=e_cbRVLC)
    {
        testHTree(root);
    }
#endif

    switch(book->type)
    {
        case e_cbUnsigned4:
            book->maxBits += 4;
            break;
            
        case e_cbUnsigned2:
            book->maxBits += 2;
            break;
            
        case e_cbEscape:
            book->maxBits += 24;
            break;
            
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_DEBUG)
//-------------------------------------------------------------------------------------------

void WSequence::testHTree(HNode *root)
{
    HNode *node = root;
    
    while(node->left!=0)
    {
        node = node->left;
    }
    
    while(node!=0)
    {
        if(node->leaf)
        {
            Q_ASSERT(node->left==0);
            Q_ASSERT(node->right==0);
        }
        else
        {
            Q_ASSERT(node->left!=0);
            Q_ASSERT(node->right!=0);
        }
        
        if(node->right!=0)
        {
            for(node=node->right;node->left!=0;node=node->left) ;
        }
        else
        {
            HNode *tNode;
            do
            {
                tNode = node;
                node = node->parent;
            } while(node!=0 && node->right==tNode);
        }
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

tint WSequence::rbA()
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
    
    if(m_bitOffset >= m_length)
    {
        return 0;
    }
    
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

tint WSequence::rbA(tint n)
{
    tint m;
    engine::Bitstream::BSMask *ms = &(engine::Bitstream::m_maskI[n][m_bitOffset & 0x0000001f]);
    
    if((m_bitOffset + n) > m_length)
    {
        return 0;
    }
    
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

tint WSequence::rbARev()
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

tint WSequence::decodeSf()
{
    Codebook *bk = &(m_books[0]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffset) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }        
    }
    return node->z;
}

//-------------------------------------------------------------------------------------------

tint WSequence::decodeRVLC()
{
    Codebook *bk = &(m_books[33]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffset) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                if(node->right!=0)
                {
                    node = node->right;
                }
                else
                {
                    throw WSequenceException();
                }
            }
            else
            {
                if(node->left!=0)
                {
                    node = node->left;
                }
                else
                {
                    throw WSequenceException();
                }
            }
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                if(node->right!=0)
                {
                    node = node->right;
                }
                else
                {
                    throw WSequenceException();
                }
            }
            else
            {
                if(node->left!=0)
                {
                    node = node->left;
                }
                else
                {
                    throw WSequenceException();
                }
            }
        }        
    }
    return node->z;
}

//-------------------------------------------------------------------------------------------

tint WSequence::decodeRVLCRev()
{
    Codebook *bk = &(m_books[33]);
    HNode *node = bk->root;
    
    if(m_bitOffset >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbARevFast())
            {
                if(node->right!=0)
                {
                    node = node->right;
                }
                else
                {
                    throw WSequenceException();
                }
            }
            else
            {
                if(node->left!=0)
                {
                    node = node->left;
                }
                else
                {
                    throw WSequenceException();
                }
            }
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbARev())
            {
                if(node->right!=0)
                {
                    node = node->right;
                }
                else
                {
                    throw WSequenceException();
                }
            }
            else
            {
                if(node->left!=0)
                {
                    node = node->left;
                }
                else
                {
                    throw WSequenceException();
                }
            }
        }        
    }
    return node->z;
}

//-------------------------------------------------------------------------------------------

tint WSequence::decodeRVLCEsc()
{
    Codebook *bk = &(m_books[34]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffsetB) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }        
    }
    return node->z;    
}

//-------------------------------------------------------------------------------------------

tint WSequence::decodeSpectral(tint cb,tint *q)
{
    switch(cb)
    {
        case 1:
        case 2:
            decodeSigned4(cb,q);
            return 4;
            
        case 3:
        case 4:
            decodeUnsigned4(cb,q);
            return 4;
            
        case 5:
        case 6:
            decodeSigned2(cb,q);
            return 2;
            
        case 7:
        case 8:
        case 9:
        case 10:
            decodeUnsigned2(cb,q);
            return 2;
            
        case 11:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
            decodeEsc(cb,q);
            return 2;
        
        case ZERO_HCB:
        case NOISE_HCB:
        case INTENSITY_HCB:
        case INTENSITY_HCB2:
        default:
            return 0;
    }
}

//-------------------------------------------------------------------------------------------

void WSequence::decodeUnsigned4(tint cb,tint *q)
{
    Codebook *bk = &(m_books[cb]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffset) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
        if(node->w && rbAFast())
        {
            q[0] = -(node->w);
        }
        else
        {
            q[0] = node->w;
        }
        if(node->x && rbAFast())
        {
            q[1] = -(node->x);
        }
        else
        {
            q[1] = node->x;
        }
        if(node->y && rbAFast())
        {
            q[2] = -(node->y);
        }
        else
        {
            q[2] = node->y;
        }
        if(node->z && rbAFast())
        {
            q[3] = -(node->z);
        }
        else
        {
            q[3] = node->z;
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
        if(node->w && rbA())
        {
            q[0] = -(node->w);
        }
        else
        {
            q[0] = node->w;
        }
        if(node->x && rbA())
        {
            q[1] = -(node->x);
        }
        else
        {
            q[1] = node->x;
        }
        if(node->y && rbA())
        {
            q[2] = -(node->y);
        }
        else
        {
            q[2] = node->y;
        }
        if(node->z && rbA())
        {
            q[3] = -(node->z);
        }
        else
        {
            q[3] = node->z;
        }        
    }
}

//-------------------------------------------------------------------------------------------

void WSequence::decodeUnsigned2(tint cb,tint *q)
{
    Codebook *bk = &(m_books[cb]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffset) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
        if(node->y && rbAFast())
        {
            q[0] = -(node->y);
        }
        else
        {
            q[0] = node->y;
        }
        if(node->z && rbAFast())
        {
            q[1] = -(node->z);
        }
        else
        {
            q[1] = node->z;
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
        if(node->y && rbA())
        {
            q[0] = -(node->y);
        }
        else
        {
            q[0] = node->y;
        }
        if(node->z && rbA())
        {
            q[1] = -(node->z);
        }
        else
        {
            q[1] = node->z;
        }        
    }
}

//-------------------------------------------------------------------------------------------

void WSequence::decodeSigned4(tint cb,tint *q)
{
    Codebook *bk = &(m_books[cb]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffset) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
    }
    q[0] = node->w;
    q[1] = node->x;
    q[2] = node->y;
    q[3] = node->z;
}

//-------------------------------------------------------------------------------------------

void WSequence::decodeSigned2(tint cb,tint *q)
{
    Codebook *bk = &(m_books[cb]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffset) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
    }
    q[0] = node->y;
    q[1] = node->z;
}

//-------------------------------------------------------------------------------------------

void WSequence::decodeEsc(tint cb,tint *q)
{
    Codebook *bk = &(m_books[cb]);
    HNode *node = bk->root;
    
    if((m_length - m_bitOffset) >= static_cast<tuint>(bk->maxBits))
    {
        while(!node->leaf)
        {
            if(rbAFast())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
        if(node->y && rbAFast())
        {
            q[0] = -(node->y);
        }
        else
        {
            q[0] = node->y;
        }
        if(node->z && rbAFast())
        {
            q[1] = -(node->z);
        }
        else
        {
            q[1] = node->z;
        }
        
        if(q[0]==16 || q[0]==-16)
        {
            tint w,i = 4;
        
            while(rbAFast())
            {
                i++;
            }
            w = (1 << i) + rbAFast(i);
            q[0] = (q[0]<0) ? -w : w;
        }
    
        if(q[1]==16 || q[1]==-16)
        {
            tint w,i = 4;
        
            while(rbAFast())
            {
                i++;
            }
            w = (1 << i) + rbAFast(i);
            q[1] = (q[1]<0) ? -w : w;
        }
    }
    else
    {
        while(!node->leaf)
        {
            if(rbA())
            {
                node = node->right;
            }
            else
            {
                node = node->left;
            }
        }
        if(node->y && rbA())
        {
            q[0] = -(node->y);
        }
        else
        {
            q[0] = node->y;
        }
        if(node->z && rbA())
        {
            q[1] = -(node->z);
        }
        else
        {
            q[1] = node->z;
        }
        
        if(q[0]==16 || q[0]==-16)
        {
            tint w,i = 4;
        
            while(rbA())
            {
                i++;
            }
            w = (1 << i) + rbA(i);
            q[0] = (q[0]<0) ? -w : w;
        }
    
        if(q[1]==16 || q[1]==-16)
        {
            tint w,i = 4;
        
            while(rbA())
            {
                i++;
            }
            w = (1 << i) + rbA(i);
            q[1] = (q[1]<0) ? -w : w;
        }
    }
}

//-------------------------------------------------------------------------------------------

void WSequence::readHCRSet(CWInfo *cwInfo,tint& cwIdx,tint cwLen,Segment *s,tint sLen,tint *spectralData,bool direction)
{
    tint i,j,k,N,count;
    CWState cwState[512];
    bool res;

    // 1. Map out initial set to segment lookup table for all codewords    
    N = cwLen - cwIdx;
    if(N > sLen)
    {
        N = sLen;
    }
    count = N;
    for(i=0;i<N;++i)
    {
        cwState[i].segment = i;
        cwState[i].state = 0;
        cwState[i].node = 0;
    }
    
    // 2. Read each segment in turn
    for(j=0;count>0 && j<N;++j)
    {
        for(k=cwIdx,i=0;i<(N - j);++i,++k)
        {
            if(cwState[i].state>0)
            {
                if(!direction)
                {
                    res = readHCRForward(cwInfo[k].codebook,&spectralData[cwInfo[k].position],&(cwState[i]),&(s[cwState[i].segment]));
                }
                else
                {
                    res = readHCRReverse(cwInfo[k].codebook,&spectralData[cwInfo[k].position],&(cwState[i]),&(s[cwState[i].segment]));
                }
                
                if(res)
                {
                    cwState[i].state = -1;
                    count--;
                }
                else
                {
                    cwState[i].segment += 1;
                }
            }
        }
    }
    
    cwIdx += N;
}

//-------------------------------------------------------------------------------------------

bool WSequence::readHCRForward(tint cb,tint *q,CWState *cw,Segment *s)
{
    switch(cb)
    {
        case 1:
        case 2:
            return decodeHCRFwdSigned4(cb,q,cw,s);
            
        case 3:
        case 4:
            return decodeHCRFwdUnsigned4(cb,q,cw,s);
            
        case 5:
        case 6:
            return decodeHCRFwdSigned2(cb,q,cw,s);
            
        case 7:
        case 8:
        case 9:
        case 10:
            return decodeHCRFwdUnsigned2(cb,q,cw,s);
            
        case 11:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
            return decodeHCRFwdEscape(cb,q,cw,s);
        
        case ZERO_HCB:
        case NOISE_HCB:
        case INTENSITY_HCB:
        case INTENSITY_HCB2:
        default:
            return true;
    }    
}

//-------------------------------------------------------------------------------------------

bool WSequence::readHCRReverse(tint cb,tint *q,CWState *cw,Segment *s)
{
    switch(cb)
    {
        case 1:
        case 2:
            return decodeHCRRevSigned4(cb,q,cw,s);
            
        case 3:
        case 4:
            return decodeHCRRevUnsigned4(cb,q,cw,s);
            
        case 5:
        case 6:
            return decodeHCRRevSigned2(cb,q,cw,s);
            
        case 7:
        case 8:
        case 9:
        case 10:
            return decodeHCRRevUnsigned2(cb,q,cw,s);
            
        case 11:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
            return decodeHCRRevEscape(cb,q,cw,s);
        
        case ZERO_HCB:
        case NOISE_HCB:
        case INTENSITY_HCB:
        case INTENSITY_HCB2:
        default:
            return true;
    }    
}

//-------------------------------------------------------------------------------------------

tint WSequence::readHCRFwdBit(Segment *s)
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
    
    if(s->start < s->end)
    {
        tint offset = s->start;
        
        if(static_cast<tuint>(offset) >= m_length)
        {
            return 2;
        }
        s->start += 1;
        
        if(m_buffer32[offset >> 5] & mask[offset & 0x0000001f])
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 2;
    }
}

//-------------------------------------------------------------------------------------------

tint WSequence::readHCRRevBit(Segment *s)
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
    
    if(s->start < s->end)
    {
        tint offset = s->end;
        
        if(static_cast<tuint>(offset) >= m_length)
        {
            return 2;
        }
        s->end -= 1;
        
        if(m_buffer32[offset >> 5] & mask[offset & 0x0000001f])
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 2;
    }
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRFwdUnsigned4(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    bool loop = true;
    
    do
    {
        switch(cw->state)
        {
            case 0:
                {
                    if(cw->node==0)
                    {
                        Codebook *bk = &(m_books[cb]);
                        node = bk->root;
                    }
                    else
                    {
                        node = cw->node;
                    }
                    
                    while(!node->leaf)
                    {
                        switch(readHCRFwdBit(s))
                        {
                            case 0:
                                node = node->left;
                                break;
                                
                            case 1:
                                node = node->right;
                                break;
                                
                            case 2:
                                cw->node = node;
                                return false;
                        }
                    }
                    cw->state = 1;
                    cw->value = 0;
                }
                break;
                
            case 1:
                {
                    node = cw->node;
                    switch(cw->value)
                    {
                        case 0: // w
                            {
                                if(node->w)
                                {
                                    switch(readHCRFwdBit(s))
                                    {
                                        case 0:
                                            q[0] = node->w;
                                            break;
                                            
                                        case 1:
                                            q[0] = -(node->w);
                                            break;
                                        
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[0] = 0;
                                }
                                cw->value = 1;
                            }
                            break;
                            
                        case 1: // x
                            {
                                if(node->x)
                                {
                                    switch(readHCRFwdBit(s))
                                    {
                                        case 0:
                                            q[1] = node->x;
                                            break;
                                            
                                        case 1:
                                            q[1] = -(node->x);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[1] = 0;
                                }
                                cw->value = 2;
                            }
                            break;
                            
                        case 2: // y
                            {
                                if(node->y)
                                {
                                    switch(readHCRFwdBit(s))
                                    {
                                        case 0:
                                            q[2] = node->y;
                                            break;
                                            
                                        case 1:
                                            q[2] = -(node->y);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[2] = 0;
                                }
                                cw->value = 3;
                            }
                            break;
                            
                        case 3: // z
                            {
                                if(node->z)
                                {
                                    switch(readHCRFwdBit(s))
                                    {
                                        case 0:
                                            q[3] = node->z;
                                            break;
                                            
                                        case 1:
                                            q[3] = -(node->z);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[3] = 0;
                                }
                                loop = false;
                            }
                            break;
                    }
                }
                break;
        }
    } while(loop);
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRRevUnsigned4(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    bool loop = true;
    
    do
    {
        switch(cw->state)
        {
            case 0:
                {
                    if(cw->node==0)
                    {
                        Codebook *bk = &(m_books[cb]);
                        node = bk->root;
                    }
                    else
                    {
                        node = cw->node;
                    }
                    
                    while(!node->leaf)
                    {
                        switch(readHCRRevBit(s))
                        {
                            case 0:
                                node = node->left;
                                break;
                                
                            case 1:
                                node = node->right;
                                break;
                                
                            case 2:
                                cw->node = node;
                                return false;
                        }
                    }
                    cw->state = 1;
                    cw->value = 0;
                }
                break;
                
            case 1:
                {
                    node = cw->node;
                    switch(cw->value)
                    {
                        case 0: // w
                            {
                                if(node->w)
                                {
                                    switch(readHCRRevBit(s))
                                    {
                                        case 0:
                                            q[0] = node->w;
                                            break;
                                            
                                        case 1:
                                            q[0] = -(node->w);
                                            break;
                                        
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[0] = 0;
                                }
                                cw->value = 1;
                            }
                            break;
                            
                        case 1: // x
                            {
                                if(node->x)
                                {
                                    switch(readHCRRevBit(s))
                                    {
                                        case 0:
                                            q[1] = node->x;
                                            break;
                                            
                                        case 1:
                                            q[1] = -(node->x);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[1] = 0;
                                }
                                cw->value = 2;
                            }
                            break;
                            
                        case 2: // y
                            {
                                if(node->y)
                                {
                                    switch(readHCRRevBit(s))
                                    {
                                        case 0:
                                            q[2] = node->y;
                                            break;
                                            
                                        case 1:
                                            q[2] = -(node->y);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[2] = 0;
                                }
                                cw->value = 3;
                            }
                            break;
                            
                        case 3: // z
                            {
                                if(node->z)
                                {
                                    switch(readHCRRevBit(s))
                                    {
                                        case 0:
                                            q[3] = node->z;
                                            break;
                                            
                                        case 1:
                                            q[3] = -(node->z);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[3] = 0;
                                }
                                loop = false;
                            }
                            break;
                    }
                }
                break;
        }
    } while(loop);
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRFwdUnsigned2(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    bool loop = true;
    
    do
    {
        switch(cw->state)
        {
            case 0:
                {
                    if(cw->node==0)
                    {
                        Codebook *bk = &(m_books[cb]);
                        node = bk->root;
                    }
                    else
                    {
                        node = cw->node;
                    }
                    
                    while(!node->leaf)
                    {
                        switch(readHCRFwdBit(s))
                        {
                            case 0:
                                node = node->left;
                                break;
                                
                            case 1:
                                node = node->right;
                                break;
                                
                            case 2:
                                cw->node = node;
                                return false;
                        }
                    }
                    cw->state = 1;
                    cw->value = 0;
                }
                break;
                
            case 1:
                {
                    node = cw->node;
                    switch(cw->value)
                    {
                        case 0: // y
                            {
                                if(node->y)
                                {
                                    switch(readHCRFwdBit(s))
                                    {
                                        case 0:
                                            q[0] = node->y;
                                            break;
                                            
                                        case 1:
                                            q[0] = -(node->y);
                                            break;
                                        
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[0] = 0;
                                }
                                cw->value = 1;
                            }
                            break;
                            
                        case 1: // z
                            {
                                if(node->z)
                                {
                                    switch(readHCRFwdBit(s))
                                    {
                                        case 0:
                                            q[1] = node->z;
                                            break;
                                            
                                        case 1:
                                            q[1] = -(node->z);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[1] = 0;
                                }
                                loop = false;
                            }
                            break;
                    }
                }
                break;
        }
    } while(loop);
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRRevUnsigned2(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    bool loop = true;
    
    do
    {
        switch(cw->state)
        {
            case 0:
                {
                    if(cw->node==0)
                    {
                        Codebook *bk = &(m_books[cb]);
                        node = bk->root;
                    }
                    else
                    {
                        node = cw->node;
                    }
                    
                    while(!node->leaf)
                    {
                        switch(readHCRRevBit(s))
                        {
                            case 0:
                                node = node->left;
                                break;
                                
                            case 1:
                                node = node->right;
                                break;
                                
                            case 2:
                                cw->node = node;
                                return false;
                        }
                    }
                    cw->state = 1;
                    cw->value = 0;
                }
                break;
                
            case 1:
                {
                    node = cw->node;
                    switch(cw->value)
                    {
                        case 0: // y
                            {
                                if(node->y)
                                {
                                    switch(readHCRRevBit(s))
                                    {
                                        case 0:
                                            q[0] = node->y;
                                            break;
                                            
                                        case 1:
                                            q[0] = -(node->y);
                                            break;
                                        
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[0] = 0;
                                }
                                cw->value = 1;
                            }
                            break;
                            
                        case 1: // z
                            {
                                if(node->z)
                                {
                                    switch(readHCRRevBit(s))
                                    {
                                        case 0:
                                            q[1] = node->z;
                                            break;
                                            
                                        case 1:
                                            q[1] = -(node->z);
                                            break;
                                            
                                        case 2:
                                            return false;
                                    }
                                }
                                else
                                {
                                    q[1] = 0;
                                }
                                loop = false;
                            }
                            break;
                    }
                }
                break;
        }
    } while(loop);
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRFwdSigned4(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    
    if(cw->node==0)
    {
        Codebook *bk = &(m_books[cb]);
        node = bk->root;
    }
    else
    {
        node = cw->node;
    }
    
    while(!node->leaf)
    {
        switch(readHCRFwdBit(s))
        {
            case 0:
                node = node->left;
                break;
                
            case 1:
                node = node->right;
                break;
                
            case 2:
                cw->node = node;
                return false;
        }
    }
    
    q[0] = node->w;
    q[1] = node->x;
    q[2] = node->y;
    q[3] = node->z;
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRRevSigned4(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    
    if(cw->node==0)
    {
        Codebook *bk = &(m_books[cb]);
        node = bk->root;
    }
    else
    {
        node = cw->node;
    }
    
    while(!node->leaf)
    {
        switch(readHCRRevBit(s))
        {
            case 0:
                node = node->left;
                break;
                
            case 1:
                node = node->right;
                break;
                
            case 2:
                cw->node = node;
                return false;
        }
    }
    
    q[0] = node->w;
    q[1] = node->x;
    q[2] = node->y;
    q[3] = node->z;
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRFwdSigned2(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    
    if(cw->node==0)
    {
        Codebook *bk = &(m_books[cb]);
        node = bk->root;
    }
    else
    {
        node = cw->node;
    }
    
    while(!node->leaf)
    {
        switch(readHCRFwdBit(s))
        {
            case 0:
                node = node->left;
                break;
                
            case 1:
                node = node->right;
                break;
                
            case 2:
                cw->node = node;
                return false;
        }
    }
    
    q[0] = node->y;
    q[1] = node->z;
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRRevSigned2(tint cb,tint *q,CWState *cw,Segment *s)
{
    HNode *node;
    
    if(cw->node==0)
    {
        Codebook *bk = &(m_books[cb]);
        node = bk->root;
    }
    else
    {
        node = cw->node;
    }
    
    while(!node->leaf)
    {
        switch(readHCRRevBit(s))
        {
            case 0:
                node = node->left;
                break;
                
            case 1:
                node = node->right;
                break;
                
            case 2:
                cw->node = node;
                return false;
        }
    }
    
    q[0] = node->y;
    q[1] = node->z;
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRFwdEscape(tint cb,tint *q,CWState *cw,Segment *s)
{
    static const tuint32 mask[32] = {
        0x00000000, 0x00000001, 0x00000002, 0x00000004, 
        0x00000008,    0x00000010, 0x00000020, 0x00000040, 
        0x00000080, 0x00000100, 0x00000200, 0x00000400,
        0x00000800, 0x00001000, 0x00002000, 0x00004000,
        0x00008000, 0x00010000, 0x00020000, 0x00040000,
        0x00080000, 0x00100000, 0x00200000, 0x00400000,
        0x00800000, 0x01000000, 0x02000000, 0x04000000,
        0x08000000, 0x10000000, 0x20000000, 0x40000000
    };
    
    HNode *node;
    bool loop = true;
    
    do
    {
        switch(cw->state)
        {
            case 0:
                {
                    if(cw->node==0)
                    {
                        Codebook *bk = &(m_books[cb]);
                        node = bk->root;
                    }
                    else
                    {
                        node = cw->node;
                    }
                
                    while(!node->leaf)
                    {
                        switch(readHCRFwdBit(s))
                        {
                            case 0:
                                node = node->left;
                                break;
                            
                            case 1:
                                node = node->right;
                                break;
                                
                            case 2:
                                cw->node = node;
                                return false;
                        }
                    }
                    cw->state = 1;
                    cw->value = 0;
                }
                break;
        
            case 1:
                {
                    node = cw->node;
                    if(!(cw->value))
                    {
                        if(node->y)
                        {
                            switch(readHCRFwdBit(s))
                            {
                                case 0:
                                    q[0] = node->y;
                                    break;
                                
                                case 1:
                                    q[0] = -(node->y);
                                    break;
                                    
                                case 2:
                                    return false;
                            }
                        }
                        else
                        {
                            q[0] = 0;
                        }
                        cw->value = 1;
                    }
                    else
                    {
                        if(node->z)
                        {
                            switch(readHCRFwdBit(s))
                            {
                                case 0:
                                    q[1] = node->z;
                                    break;
                                    
                                case 1:
                                    q[1] = -(node->z);
                                    break;
                                    
                                case 2:
                                    return false;
                            }
                        }
                        else
                        {
                            q[1] = 0;
                        }
                        if(q[0]==16 || q[0]==-16)
                        {
                            cw->state = 2;
                            cw->value = 0;
                        }
                        else if(q[1]==16 || q[1]==-16)
                        {
                            cw->state = 3;
                            cw->value = 0;
                        }
                        else
                        {
                            loop = false;
                        }
                    }
                }
                break;
            
            case 2:
            case 3:
                {
                    tuint32 va;
                    tint no,w,i = 4,j;
                    bool flag = true;
                    
                    no = static_cast<tint>((cw->value >> 27) & 0x0000001f);
                    va = cw->value & 0x07fffffff;
                    while(no && flag)
                    {
                        if(mask[no--] & va)
                        {
                            i++;
                        }
                        else
                        {
                            flag = false;
                        }
                    }
                    
                    if(!no && flag)
                    {
                        no = (cw->value >> 27) & 0x0000001f;
                        
                        while(flag)
                        {
                            no++;
                            va <<= 1;
                            
                            switch(readHCRFwdBit(s))
                            {
                                case 0:
                                    flag = false;
                                    break;
                                    
                                case 1:
                                    va |= 1;
                                    i++;
                                    break;
                                    
                                case 2:
                                    cw->value = ((static_cast<tuint>(no) << 27) & 0xf1000000) | (va & 0x07ffffff);
                                    return false;
                            }
                        }
                        no = 0;
                    }
                    
                    w = 0;
                    j = 0;
                    if(no)
                    {
                        while(j<i && no)
                        {
                            w <<= 1;
                            w |= (mask[no--] & va) ? 1 : 0;
                        }
                    }
                    
                    if(!no)
                    {
                        no = (cw->value >> 27) & 0x0000001f;
                        
                        while(j<i)
                        {
                            no++;
                            va <<= 1;
                            w  <<= 1;
                            
                            switch(readHCRFwdBit(s))
                            {
                                case 0:
                                    break;
                                    
                                case 1:
                                    va |= 1;
                                    w  |= 1;
                                    break;
                                    
                                case 2:
                                    cw->value = ((static_cast<tuint>(no) << 27)  & 0xf1000000) | (va & 0x07ffffff);
                                    return false;
                            }
                        }
                    }
                    
                    w = (1 << i) + w;
                    if(cw->state==2)
                    {
                        q[0] = (q[0]<0) ? -w : w;
                        
                        if(q[1]==16 || q[1]==-16)
                        {
                            cw->state = 3;
                            cw->value = 0;
                        }
                        else
                        {
                            loop = false;
                        }
                    }
                    else
                    {
                        q[1] = (q[1]<0) ? -w : w;
                        cw->state = 0;
                        loop = false;
                    }
                }
                break;
        }
    } while(loop);
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool WSequence::decodeHCRRevEscape(tint cb,tint *q,CWState *cw,Segment *s)
{
    static const tuint32 mask[32] = {
        0x00000000, 0x00000001, 0x00000002, 0x00000004, 
        0x00000008,    0x00000010, 0x00000020, 0x00000040, 
        0x00000080, 0x00000100, 0x00000200, 0x00000400,
        0x00000800, 0x00001000, 0x00002000, 0x00004000,
        0x00008000, 0x00010000, 0x00020000, 0x00040000,
        0x00080000, 0x00100000, 0x00200000, 0x00400000,
        0x00800000, 0x01000000, 0x02000000, 0x04000000,
        0x08000000, 0x10000000, 0x20000000, 0x40000000
    };
    
    HNode *node;
    bool loop = true;
    
    do
    {
        switch(cw->state)
        {
            case 0:
                {
                    if(cw->node==0)
                    {
                        Codebook *bk = &(m_books[cb]);
                        node = bk->root;
                    }
                    else
                    {
                        node = cw->node;
                    }
                
                    while(!node->leaf)
                    {
                        switch(readHCRRevBit(s))
                        {
                            case 0:
                                node = node->left;
                                break;
                            
                            case 1:
                                node = node->right;
                                break;
                                
                            case 2:
                                cw->node = node;
                                return false;
                        }
                    }
                    cw->state = 1;
                    cw->value = 0;
                }
                break;
        
            case 1:
                {
                    node = cw->node;
                    if(!(cw->value))
                    {
                        if(node->y)
                        {
                            switch(readHCRRevBit(s))
                            {
                                case 0:
                                    q[0] = node->y;
                                    break;
                                
                                case 1:
                                    q[0] = -(node->y);
                                    break;
                                    
                                case 2:
                                    return false;
                            }
                        }
                        else
                        {
                            q[0] = 0;
                        }
                        cw->value = 1;
                    }
                    else
                    {
                        if(node->z)
                        {
                            switch(readHCRRevBit(s))
                            {
                                case 0:
                                    q[1] = node->z;
                                    break;
                                    
                                case 1:
                                    q[1] = -(node->z);
                                    break;
                                    
                                case 2:
                                    return false;
                            }
                        }
                        else
                        {
                            q[1] = 0;
                        }
                        if(q[0]==16 || q[0]==-16)
                        {
                            cw->state = 2;
                            cw->value = 0;
                        }
                        else if(q[1]==16 || q[1]==-16)
                        {
                            cw->state = 3;
                            cw->value = 0;
                        }
                        else
                        {
                            loop = false;
                        }
                    }
                }
                break;
            
            case 2:
            case 3:
                {
                    tuint32 va;
                    tint no,w,i = 4,j;
                    bool flag = true;
                    
                    no = static_cast<tint>((cw->value >> 27) & 0x0000001f);
                    va = cw->value & 0x07fffffff;
                    while(no && flag)
                    {
                        if(mask[no--] & va)
                        {
                            i++;
                        }
                        else
                        {
                            flag = false;
                        }
                    }
                    
                    if(!no && flag)
                    {
                        no = (cw->value >> 27) & 0x0000001f;
                        
                        while(flag)
                        {
                            no++;
                            va <<= 1;
                            
                            switch(readHCRRevBit(s))
                            {
                                case 0:
                                    flag = false;
                                    break;
                                    
                                case 1:
                                    va |= 1;
                                    i++;
                                    break;
                                    
                                case 2:
                                    cw->value = ((static_cast<tuint>(no) << 27) & 0xf1000000) | (va & 0x07ffffff);
                                    return false;
                            }
                        }
                        no = 0;
                    }
                    
                    w = 0;
                    j = 0;
                    if(no)
                    {
                        while(j<i && no)
                        {
                            w <<= 1;
                            w |= (mask[no--] & va) ? 1 : 0;
                        }
                    }
                    
                    if(!no)
                    {
                        no = (cw->value >> 27) & 0x0000001f;
                        
                        while(j<i)
                        {
                            no++;
                            va <<= 1;
                            w  <<= 1;
                            
                            switch(readHCRRevBit(s))
                            {
                                case 0:
                                    break;
                                    
                                case 1:
                                    va |= 1;
                                    w  |= 1;
                                    break;
                                    
                                case 2:
                                    cw->value = ((static_cast<tuint>(no) << 27)  & 0xf1000000) | (va & 0x07ffffff);
                                    return false;
                            }
                        }
                    }
                    
                    w = (1 << i) + w;
                    if(cw->state==2)
                    {
                        q[0] = (q[0]<0) ? -w : w;
                        
                        if(q[1]==16 || q[1]==-16)
                        {
                            cw->state = 3;
                            cw->value = 0;
                        }
                        else
                        {
                            loop = false;
                        }
                    }
                    else
                    {
                        q[1] = (q[1]<0) ? -w : w;
                        cw->state = 0;
                        loop = false;
                    }
                }
                break;
        }
    } while(loop);
    
    return true;
}

//-------------------------------------------------------------------------------------------
// Table 4.131 & Table 4.132
//-------------------------------------------------------------------------------------------

WSequence::Codebook WSequence::m_books[34] = {
    { e_cbSigned1  , 1,  0, 0, 0}, //  0 - 4.A.1 - Scalefactor
    { e_cbSigned4  , 4,  1, 0, 0}, //  1 - 4.A.2
    { e_cbSigned4  , 4,  1, 0, 0}, //  2 - 4.A.3
    { e_cbUnsigned4, 4,  2, 0, 0}, //  3 - 4.A.4
    { e_cbUnsigned4, 4,  2, 0, 0}, //  4 - 4.A.5
    { e_cbSigned2  , 2,  4, 0, 0}, //  5 - 4.A.6
    { e_cbSigned2  , 2,  4, 0, 0}, //  6 - 4.A.7
    { e_cbUnsigned2, 2,  7, 0, 0}, //  7 - 4.A.8
    { e_cbUnsigned2, 2,  7, 0, 0}, //  8 - 4.A.9
    { e_cbUnsigned2, 2, 12, 0, 0}, //  9 - 4.A.10
    { e_cbUnsigned2, 2, 12, 0, 0}, // 10 - 4.A.11
    { e_cbUnsigned2, 2, 16, 0, 0}, // 11 - 4.A.12 - with ESC 8191
    { e_cbSigned1  , 0,  0, 0, 0}, // 12 - reserved
    { e_cbSigned1  , 0,  0, 0, 0}, // 13 - perceptual noise substitution
    { e_cbSigned1  , 0,  0, 0, 0}, // 14 - intensity out-of-phase
    { e_cbSigned1  , 0,  0, 0, 0}, // 15 - intensity in-phase
    { e_cbEscape   , 2, 16, 0, 0}, // 16 - 4.A.12 - with ESC 15
    { e_cbEscape   , 2, 16, 0, 0}, // 17 - 4.A.12 - with ESC 31
    { e_cbEscape   , 2, 16, 0, 0}, // 18 - 4.A.12 - with ESC 47
    { e_cbEscape   , 2, 16, 0, 0}, // 19 - 4.A.12 - with ESC 63
    { e_cbEscape   , 2, 16, 0, 0}, // 20 - 4.A.12 - with ESC 95
    { e_cbEscape   , 2, 16, 0, 0}, // 21 - 4.A.12 - with ESC 127
    { e_cbEscape   , 2, 16, 0, 0}, // 22 - 4.A.12 - with ESC 159
    { e_cbEscape   , 2, 16, 0, 0}, // 23 - 4.A.12 - with ESC 191
    { e_cbEscape   , 2, 16, 0, 0}, // 24 - 4.A.12 - with ESC 223
    { e_cbEscape   , 2, 16, 0, 0}, // 25 - 4.A.12 - with ESC 255
    { e_cbEscape   , 2, 16, 0, 0}, // 26 - 4.A.12 - with ESC 319
    { e_cbEscape   , 2, 16, 0, 0}, // 27 - 4.A.12 - with ESC 383
    { e_cbEscape   , 2, 16, 0, 0}, // 28 - 4.A.12 - with ESC 511
    { e_cbEscape   , 2, 16, 0, 0}, // 29 - 4.A.12 - with ESC 767
    { e_cbEscape   , 2, 16, 0, 0}, // 30 - 4.A.12 - with ESC 1023
    { e_cbEscape   , 2, 16, 0, 0}, // 31 - 4.A.12 - with ESC 2047
    { e_cbRVLC     , 1,  0, 0, 0}, // 33 - Table 4.147
    { e_cbRVLCHuff , 1,  0, 0, 0}  // 34 - Table 4.149
};

//-------------------------------------------------------------------------------------------
// Table 4.A.1 - Scalefactor Huffman Codebook
//-------------------------------------------------------------------------------------------

tint WSequence::m_scalefactorCodebookTable[121][3] = {
    {   0, 18, 0x0003ffe8}, {   1, 18, 0x0003ffe6}, {   2, 18, 0x0003ffe7}, {   3, 18, 0x0003ffe5},
    {   4, 19, 0x0007fff5}, {   5, 19, 0x0007fff1}, {   6, 19, 0x0007ffed}, {   7, 19, 0x0007fff6},
    {   8, 19, 0x0007ffee}, {   9, 19, 0x0007ffef}, {  10, 19, 0x0007fff0}, {  11, 19, 0x0007fffc},
    {  12, 19, 0x0007fffd}, {  13, 19, 0x0007ffff}, {  14, 19, 0x0007fffe}, {  15, 19, 0x0007fff7},
    {  16, 19, 0x0007fff8}, {  17, 19, 0x0007fffb}, {  18, 19, 0x0007fff9}, {  19, 18, 0x0003ffe4},
    {  20, 19, 0x0007fffa}, {  21, 18, 0x0003ffe3}, {  22, 17, 0x0001ffef}, {  23, 17, 0x0001fff0},
    {  24, 16, 0x0000fff5}, {  25, 17, 0x0001ffee}, {  26, 16, 0x0000fff2}, {  27, 16, 0x0000fff3},
    {  28, 16, 0x0000fff4}, {  29, 16, 0x0000fff1}, {  30, 15, 0x00007ff6}, {  31, 15, 0x00007ff7},
    {  32, 14, 0x00003ff9}, {  33, 14, 0x00003ff5}, {  34, 14, 0x00003ff7}, {  35, 14, 0x00003ff3},
    {  36, 14, 0x00003ff6}, {  37, 14, 0x00003ff2}, {  38, 13, 0x00001ff7}, {  39, 13, 0x00001ff5},
    {  40, 12, 0x00000ff9}, {  41, 12, 0x00000ff7}, {  42, 12, 0x00000ff6}, {  43, 11, 0x000007f9},
    {  44, 12, 0x00000ff4}, {  45, 11, 0x000007f8}, {  46, 10, 0x000003f9}, {  47, 10, 0x000003f7},
    {  48, 10, 0x000003f5}, {  49,  9, 0x000001f8}, {  50,  9, 0x000001f7}, {  51,  8, 0x000000fa},
    {  52,  8, 0x000000f8}, {  53,  8, 0x000000f6}, {  54,  7, 0x00000079}, {  55,  6, 0x0000003a},
    {  56,  6, 0x00000038}, {  57,  5, 0x0000001a}, {  58,  4, 0x0000000b}, {  59,  3, 0x00000004},
    {  60,  1, 0x00000000}, {  61,  4, 0x0000000a}, {  62,  4, 0x0000000c}, {  63,  5, 0x0000001b},
    {  64,  6, 0x00000039}, {  65,  6, 0x0000003b}, {  66,  7, 0x00000078}, {  67,  7, 0x0000007a},
    {  68,  8, 0x000000f7}, {  69,  8, 0x000000f9}, {  70,  9, 0x000001f6}, {  71,  9, 0x000001f9},
    {  72, 10, 0x000003f4}, {  73, 10, 0x000003f6}, {  74, 10, 0x000003f8}, {  75, 11, 0x000007f5},
    {  76, 11, 0x000007f4}, {  77, 11, 0x000007f6}, {  78, 11, 0x000007f7}, {  79, 12, 0x00000ff5},
    {  80, 12, 0x00000ff8}, {  81, 13, 0x00001ff4}, {  82, 13, 0x00001ff6}, {  83, 13, 0x00001ff8},
    {  84, 14, 0x00003ff8}, {  85, 14, 0x00003ff4}, {  86, 16, 0x0000fff0}, {  87, 15, 0x00007ff4},
    {  88, 16, 0x0000fff6}, {  89, 15, 0x00007ff5}, {  90, 18, 0x0003ffe2}, {  91, 19, 0x0007ffd9},
    {  92, 19, 0x0007ffda}, {  93, 19, 0x0007ffdb}, {  94, 19, 0x0007ffdc}, {  95, 19, 0x0007ffdd},
    {  96, 19, 0x0007ffde}, {  97, 19, 0x0007ffd8}, {  98, 19, 0x0007ffd2}, {  99, 19, 0x0007ffd3},
    { 100, 19, 0x0007ffd4}, { 101, 19, 0x0007ffd5}, { 102, 19, 0x0007ffd6}, { 103, 19, 0x0007fff2},
    { 104, 19, 0x0007ffdf}, { 105, 19, 0x0007ffe7}, { 106, 19, 0x0007ffe8}, { 107, 19, 0x0007ffe9},
    { 108, 19, 0x0007ffea}, { 109, 19, 0x0007ffeb}, { 110, 19, 0x0007ffe6}, { 111, 19, 0x0007ffe0},
    { 112, 19, 0x0007ffe1}, { 113, 19, 0x0007ffe2}, { 114, 19, 0x0007ffe3}, { 115, 19, 0x0007ffe4},
    { 116, 19, 0x0007ffe5}, { 117, 19, 0x0007ffd7}, { 118, 19, 0x0007ffec}, { 119, 19, 0x0007fff4},
    { 120, 19, 0x0007fff3}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.2 - Spectrum Huffman Codebook 1
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook1Table[81][3] = {
    {  0, 11, 0x000007f8}, {  1,  9, 0x000001f1}, {  2, 11, 0x000007fd}, {  3, 10, 0x000003f5},
    {  4,  7, 0x00000068}, {  5, 10, 0x000003f0}, {  6, 11, 0x000007f7}, {  7,  9, 0x000001ec},
    {  8, 11, 0x000007f5}, {  9, 10, 0x000003f1}, { 10,  7, 0x00000072}, { 11, 10, 0x000003f4},
    { 12,  7, 0x00000074}, { 13,  5, 0x00000011}, { 14,  7, 0x00000076}, { 15,  9, 0x000001eb},
    { 16,  7, 0x0000006c}, { 17, 10, 0x000003f6}, { 18, 11, 0x000007fc}, { 19,  9, 0x000001e1},
    { 20, 11, 0x000007f1}, { 21,  9, 0x000001f0}, { 22,  7, 0x00000061}, { 23,  9, 0x000001f6},
    { 24, 11, 0x000007f2}, { 25,  9, 0x000001ea}, { 26, 11, 0x000007fb}, { 27,  9, 0x000001f2},
    { 28,  7, 0x00000069}, { 29,  9, 0x000001ed}, { 30,  7, 0x00000077}, { 31,  5, 0x00000017},
    { 32,  7, 0x0000006f}, { 33,  9, 0x000001e6}, { 34,  7, 0x00000064}, { 35,  9, 0x000001e5},
    { 36,  7, 0x00000067}, { 37,  5, 0x00000015}, { 38,  7, 0x00000062}, { 39,  5, 0x00000012},
    { 40,  1, 0x00000000}, { 41,  5, 0x00000014}, { 42,  7, 0x00000065}, { 43,  5, 0x00000016},
    { 44,  7, 0x0000006d}, { 45,  9, 0x000001e9}, { 46,  7, 0x00000063}, { 47,  9, 0x000001e4},
    { 48,  7, 0x0000006b}, { 49,  5, 0x00000013}, { 50,  7, 0x00000071}, { 51,  9, 0x000001e3},
    { 52,  7, 0x00000070}, { 53,  9, 0x000001f3}, { 54, 11, 0x000007fe}, { 55,  9, 0x000001e7},
    { 56, 11, 0x000007f3}, { 57,  9, 0x000001ef}, { 58,  7, 0x00000060}, { 59,  9, 0x000001ee},
    { 60, 11, 0x000007f0}, { 61,  9, 0x000001e2}, { 62, 11, 0x000007fa}, { 63, 10, 0x000003f3},
    { 64,  7, 0x0000006a}, { 65,  9, 0x000001e8}, { 66,  7, 0x00000075}, { 67,  5, 0x00000010},
    { 68,  7, 0x00000073}, { 69,  9, 0x000001f4}, { 70,  7, 0x0000006e}, { 71, 10, 0x000003f7},
    { 72, 11, 0x000007f6}, { 73,  9, 0x000001e0}, { 74, 11, 0x000007f9}, { 75, 10, 0x000003f2},
    { 76,  7, 0x00000066}, { 77,  9, 0x000001f5}, { 78, 11, 0x000007ff}, { 79,  9, 0x000001f7},
    { 80, 11, 0x000007f4}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.3 - Spectrum Huffman Codebook 2
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook2Table[81][3] = {
    {  0,  9, 0x000001f3}, {  1,  7, 0x0000006f}, {  2,  9, 0x000001fd}, {  3,  8, 0x000000eb},
    {  4,  6, 0x00000023}, {  5,  8, 0x000000ea}, {  6,  9, 0x000001f7}, {  7,  8, 0x000000e8},
    {  8,  9, 0x000001fa}, {  9,  8, 0x000000f2}, { 10,  6, 0x0000002d}, { 11,  7, 0x00000070},
    { 12,  6, 0x00000020}, { 13,  5, 0x00000006}, { 14,  6, 0x0000002b}, { 15,  7, 0x0000006e},
    { 16,  6, 0x00000028}, { 17,  8, 0x000000e9}, { 18,  9, 0x000001f9}, { 19,  7, 0x00000066},
    { 20,  8, 0x000000f8}, { 21,  8, 0x000000e7}, { 22,  6, 0x0000001b}, { 23,  8, 0x000000f1},
    { 24,  9, 0x000001f4}, { 25,  7, 0x0000006b}, { 26,  9, 0x000001f5}, { 27,  8, 0x000000ec},
    { 28,  6, 0x0000002a}, { 29,  7, 0x0000006c}, { 30,  6, 0x0000002c}, { 31,  5, 0x0000000a},
    { 32,  6, 0x00000027}, { 33,  7, 0x00000067}, { 34,  6, 0x0000001a}, { 35,  8, 0x000000f5},
    { 36,  6, 0x00000024}, { 37,  5, 0x00000008}, { 38,  6, 0x0000001f}, { 39,  5, 0x00000009},
    { 40,  3, 0x00000000}, { 41,  5, 0x00000007}, { 42,  6, 0x0000001d}, { 43,  5, 0x0000000b},
    { 44,  6, 0x00000030}, { 45,  8, 0x000000ef}, { 46,  6, 0x0000001c}, { 47,  7, 0x00000064},
    { 48,  6, 0x0000001e}, { 49,  5, 0x0000000c}, { 50,  6, 0x00000029}, { 51,  8, 0x000000f3},
    { 52,  6, 0x0000002f}, { 53,  8, 0x000000f0}, { 54,  9, 0x000001fc}, { 55,  7, 0x00000071},
    { 56,  9, 0x000001f2}, { 57,  8, 0x000000f4}, { 58,  6, 0x00000021}, { 59,  8, 0x000000e6},
    { 60,  8, 0x000000f7}, { 61,  7, 0x00000068}, { 62,  9, 0x000001f8}, { 63,  8, 0x000000ee},
    { 64,  6, 0x00000022}, { 65,  7, 0x00000065}, { 66,  6, 0x00000031}, { 67,  4, 0x00000002},
    { 68,  6, 0x00000026}, { 69,  8, 0x000000ed}, { 70,  6, 0x00000025}, { 71,  7, 0x0000006a},
    { 72,  9, 0x000001fb}, { 73,  7, 0x00000072}, { 74,  9, 0x000001fe}, { 75,  7, 0x00000069},
    { 76,  6, 0x0000002e}, { 77,  8, 0x000000f6}, { 78,  9, 0x000001ff}, { 79,  7, 0x0000006d},
    { 80,  9, 0x000001f6}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.4 - Spectrum Huffman Codebook 3
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook3Table[81][3] = {
    {  0,  1, 0x00000000}, {  1,  4, 0x00000009}, {  2,  8, 0x000000ef}, {  3,  4, 0x0000000b},
    {  4,  5, 0x00000019}, {  5,  8, 0x000000f0}, {  6,  9, 0x000001eb}, {  7,  9, 0x000001e6},
    {  8, 10, 0x000003f2}, {  9,  4, 0x0000000a}, { 10,  6, 0x00000035}, { 11,  9, 0x000001ef},
    { 12,  6, 0x00000034}, { 13,  6, 0x00000037}, { 14,  9, 0x000001e9}, { 15,  9, 0x000001ed},
    { 16,  9, 0x000001e7}, { 17, 10, 0x000003f3}, { 18,  9, 0x000001ee}, { 19, 10, 0x000003ed},
    { 20, 13, 0x00001ffa}, { 21,  9, 0x000001ec}, { 22,  9, 0x000001f2}, { 23, 11, 0x000007f9},
    { 24, 11, 0x000007f8}, { 25, 10, 0x000003f8}, { 26, 12, 0x00000ff8}, { 27,  4, 0x00000008},
    { 28,  6, 0x00000038}, { 29, 10, 0x000003f6}, { 30,  6, 0x00000036}, { 31,  7, 0x00000075},
    { 32, 10, 0x000003f1}, { 33, 10, 0x000003eb}, { 34, 10, 0x000003ec}, { 35, 12, 0x00000ff4},
    { 36,  5, 0x00000018}, { 37,  7, 0x00000076}, { 38, 11, 0x000007f4}, { 39,  6, 0x00000039},
    { 40,  7, 0x00000074}, { 41, 10, 0x000003ef}, { 42,  9, 0x000001f3}, { 43,  9, 0x000001f4},
    { 44, 11, 0x000007f6}, { 45,  9, 0x000001e8}, { 46, 10, 0x000003ea}, { 47, 13, 0x00001ffc},
    { 48,  8, 0x000000f2}, { 49,  9, 0x000001f1}, { 50, 12, 0x00000ffb}, { 51, 10, 0x000003f5},
    { 52, 11, 0x000007f3}, { 53, 12, 0x00000ffc}, { 54,  8, 0x000000ee}, { 55, 10, 0x000003f7},
    { 56, 15, 0x00007ffe}, { 57,  9, 0x000001f0}, { 58, 11, 0x000007f5}, { 59, 15, 0x00007ffd},
    { 60, 13, 0x00001ffb}, { 61, 14, 0x00003ffa}, { 62, 16, 0x0000ffff}, { 63,  8, 0x000000f1},
    { 64, 10, 0x000003f0}, { 65, 14, 0x00003ffc}, { 66,  9, 0x000001ea}, { 67, 10, 0x000003ee},
    { 68, 14, 0x00003ffb}, { 69, 12, 0x00000ff6}, { 70, 12, 0x00000ffa}, { 71, 15, 0x00007ffc},
    { 72, 11, 0x000007f2}, { 73, 12, 0x00000ff5}, { 74, 16, 0x0000fffe}, { 75, 10, 0x000003f4},
    { 76, 11, 0x000007f7}, { 77, 15, 0x00007ffb}, { 78, 12, 0x00000ff7}, { 79, 12, 0x00000ff9},
    { 80, 15, 0x00007ffa}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.5 - Spectrum Huffman Codebook 4
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook4Table[81][3] = {
    {  0,  4, 0x00000007}, {  1,  5, 0x00000016}, {  2,  8, 0x000000f6}, {  3,  5, 0x00000018},
    {  4,  4, 0x00000008}, {  5,  8, 0x000000ef}, {  6,  9, 0x000001ef}, {  7,  8, 0x000000f3},
    {  8, 11, 0x000007f8}, {  9,  5, 0x00000019}, { 10,  5, 0x00000017}, { 11,  8, 0x000000ed},
    { 12,  5, 0x00000015}, { 13,  4, 0x00000001}, { 14,  8, 0x000000e2}, { 15,  8, 0x000000f0},
    { 16,  7, 0x00000070}, { 17, 10, 0x000003f0}, { 18,  9, 0x000001ee}, { 19,  8, 0x000000f1},
    { 20, 11, 0x000007fa}, { 21,  8, 0x000000ee}, { 22,  8, 0x000000e4}, { 23, 10, 0x000003f2},
    { 24, 11, 0x000007f6}, { 25, 10, 0x000003ef}, { 26, 11, 0x000007fd}, { 27,  4, 0x00000005},
    { 28,  5, 0x00000014}, { 29,  8, 0x000000f2}, { 30,  4, 0x00000009}, { 31,  4, 0x00000004},
    { 32,  8, 0x000000e5}, { 33,  8, 0x000000f4}, { 34,  8, 0x000000e8}, { 35, 10, 0x000003f4},
    { 36,  4, 0x00000006}, { 37,  4, 0x00000002}, { 38,  8, 0x000000e7}, { 39,  4, 0x00000003},
    { 40,  4, 0x00000000}, { 41,  7, 0x0000006b}, { 42,  8, 0x000000e3}, { 43,  7, 0x00000069},
    { 44,  9, 0x000001f3}, { 45,  8, 0x000000eb}, { 46,  8, 0x000000e6}, { 47, 10, 0x000003f6},
    { 48,  7, 0x0000006e}, { 49,  7, 0x0000006a}, { 50,  9, 0x000001f4}, { 51, 10, 0x000003ec},
    { 52,  9, 0x000001f0}, { 53, 10, 0x000003f9}, { 54,  8, 0x000000f5}, { 55,  8, 0x000000ec},
    { 56, 11, 0x000007fb}, { 57,  8, 0x000000ea}, { 58,  7, 0x0000006f}, { 59, 10, 0x000003f7},
    { 60, 11, 0x000007f9}, { 61, 10, 0x000003f3}, { 62, 12, 0x00000fff}, { 63,  8, 0x000000e9},
    { 64,  7, 0x0000006d}, { 65, 10, 0x000003f8}, { 66,  7, 0x0000006c}, { 67,  7, 0x00000068},
    { 68,  9, 0x000001f5}, { 69, 10, 0x000003ee}, { 70,  9, 0x000001f2}, { 71, 11, 0x000007f4},
    { 72, 11, 0x000007f7}, { 73, 10, 0x000003f1}, { 74, 12, 0x00000ffe}, { 75, 10, 0x000003ed},
    { 76,  9, 0x000001f1}, { 77, 11, 0x000007f5}, { 78, 11, 0x000007fe}, { 79, 10, 0x000003f5},
    { 80, 11, 0x000007fc}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.6 - Spectrum Huffman Codebook 5
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook5Table[81][3] = {
    {  0, 13, 0x00001fff}, {  1, 12, 0x00000ff7}, {  2, 11, 0x000007f4}, {  3, 11, 0x000007e8},
    {  4, 10, 0x000003f1}, {  5, 11, 0x000007ee}, {  6, 11, 0x000007f9}, {  7, 12, 0x00000ff8}, 
    {  8, 13, 0x00001ffd}, {  9, 12, 0x00000ffd}, { 10, 11, 0x000007f1}, { 11, 10, 0x000003e8}, 
    { 12,  9, 0x000001e8}, { 13,  8, 0x000000f0}, { 14,  9, 0x000001ec}, { 15, 10, 0x000003ee}, 
    { 16, 11, 0x000007f2}, { 17, 12, 0x00000ffa}, { 18, 12, 0x00000ff4}, { 19, 10, 0x000003ef}, 
    { 20,  9, 0x000001f2}, { 21,  8, 0x000000e8}, { 22,  7, 0x00000070}, { 23,  8, 0x000000ec}, 
    { 24,  9, 0x000001f0}, { 25, 10, 0x000003ea}, { 26, 11, 0x000007f3}, { 27, 11, 0x000007eb}, 
    { 28,  9, 0x000001eb}, { 29,  8, 0x000000ea}, { 30,  5, 0x0000001a}, { 31,  4, 0x00000008}, 
    { 32,  5, 0x00000019}, { 33,  8, 0x000000ee}, { 34,  9, 0x000001ef}, { 35, 11, 0x000007ed}, 
    { 36, 10, 0x000003f0}, { 37,  8, 0x000000f2}, { 38,  7, 0x00000073}, { 39,  4, 0x0000000b}, 
    { 40,  1, 0x00000000}, { 41,  4, 0x0000000a}, { 42,  7, 0x00000071}, { 43,  8, 0x000000f3}, 
    { 44, 11, 0x000007e9}, { 45, 11, 0x000007ef}, { 46,  9, 0x000001ee}, { 47,  8, 0x000000ef}, 
    { 48,  5, 0x00000018}, { 49,  4, 0x00000009}, { 50,  5, 0x0000001b}, { 51,  8, 0x000000eb}, 
    { 52,  9, 0x000001e9}, { 53, 11, 0x000007ec}, { 54, 11, 0x000007f6}, { 55, 10, 0x000003eb}, 
    { 56,  9, 0x000001f3}, { 57,  8, 0x000000ed}, { 58,  7, 0x00000072}, { 59,  8, 0x000000e9}, 
    { 60,  9, 0x000001f1}, { 61, 10, 0x000003ed}, { 62, 11, 0x000007f7}, { 63, 12, 0x00000ff6}, 
    { 64, 11, 0x000007f0}, { 65, 10, 0x000003e9}, { 66,  9, 0x000001ed}, { 67,  8, 0x000000f1}, 
    { 68,  9, 0x000001ea}, { 69, 10, 0x000003ec}, { 70, 11, 0x000007f8}, { 71, 12, 0x00000ff9}, 
    { 72, 13, 0x00001ffc}, { 73, 12, 0x00000ffc}, { 74, 12, 0x00000ff5}, { 75, 11, 0x000007ea}, 
    { 76, 10, 0x000003f3}, { 77, 10, 0x000003f2}, { 78, 11, 0x000007f5}, { 79, 12, 0x00000ffb}, 
    { 80, 13, 0x00001ffe}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.7 - Spectrum Huffman Codebook 6
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook6Table[81][3] = {
    {  0, 11, 0x000007fe}, {  1, 10, 0x000003fd}, {  2,  9, 0x000001f1}, {  3,  9, 0x000001eb},
    {  4,  9, 0x000001f4}, {  5,  9, 0x000001ea}, {  6,  9, 0x000001f0}, {  7, 10, 0x000003fc},
    {  8, 11, 0x000007fd}, {  9, 10, 0x000003f6}, { 10,  9, 0x000001e5}, { 11,  8, 0x000000ea},
    { 12,  7, 0x0000006c}, { 13,  7, 0x00000071}, { 14,  7, 0x00000068}, { 15,  8, 0x000000f0},
    { 16,  9, 0x000001e6}, { 17, 10, 0x000003f7}, { 18,  9, 0x000001f3}, { 19,  8, 0x000000ef},
    { 20,  6, 0x00000032}, { 21,  6, 0x00000027}, { 22,  6, 0x00000028}, { 23,  6, 0x00000026},
    { 24,  6, 0x00000031}, { 25,  8, 0x000000eb}, { 26,  9, 0x000001f7}, { 27,  9, 0x000001e8},
    { 28,  7, 0x0000006f}, { 29,  6, 0x0000002e}, { 30,  4, 0x00000008}, { 31,  4, 0x00000004},
    { 32,  4, 0x00000006}, { 33,  6, 0x00000029}, { 34,  7, 0x0000006b}, { 35,  9, 0x000001ee},
    { 36,  9, 0x000001ef}, { 37,  7, 0x00000072}, { 38,  6, 0x0000002d}, { 39,  4, 0x00000002},
    { 40,  4, 0x00000000}, { 41,  4, 0x00000003}, { 42,  6, 0x0000002f}, { 43,  7, 0x00000073},
    { 44,  9, 0x000001fa}, { 45,  9, 0x000001e7}, { 46,  7, 0x0000006e}, { 47,  6, 0x0000002b},
    { 48,  4, 0x00000007}, { 49,  4, 0x00000001}, { 50,  4, 0x00000005}, { 51,  6, 0x0000002c},
    { 52,  7, 0x0000006d}, { 53,  9, 0x000001ec}, { 54,  9, 0x000001f9}, { 55,  8, 0x000000ee},
    { 56,  6, 0x00000030}, { 57,  6, 0x00000024}, { 58,  6, 0x0000002a}, { 59,  6, 0x00000025},
    { 60,  6, 0x00000033}, { 61,  8, 0x000000ec}, { 62,  9, 0x000001f2}, { 63, 10, 0x000003f8},
    { 64,  9, 0x000001e4}, { 65,  8, 0x000000ed}, { 66,  7, 0x0000006a}, { 67,  7, 0x00000070},
    { 68,  7, 0x00000069}, { 69,  7, 0x00000074}, { 70,  8, 0x000000f1}, { 71, 10, 0x000003fa},
    { 72, 11, 0x000007ff}, { 73, 10, 0x000003f9}, { 74,  9, 0x000001f6}, { 75,  9, 0x000001ed},
    { 76,  9, 0x000001f8}, { 77,  9, 0x000001e9}, { 78,  9, 0x000001f5}, { 79, 10, 0x000003fb},
    { 80, 11, 0x000007fc}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.8 - Spectrum Huffman Codebook 7
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook7Table[64][3] = {
    {  0,  1, 0x00000000}, {  1,  3, 0x00000005}, {  2,  6, 0x00000037}, {  3,  7, 0x00000074},
    {  4,  8, 0x000000f2}, {  5,  9, 0x000001eb}, {  6, 10, 0x000003ed}, {  7, 11, 0x000007f7},
    {  8,  3, 0x00000004}, {  9,  4, 0x0000000c}, { 10,  6, 0x00000035}, { 11,  7, 0x00000071},
    { 12,  8, 0x000000ec}, { 13,  8, 0x000000ee}, { 14,  9, 0x000001ee}, { 15,  9, 0x000001f5},
    { 16,  6, 0x00000036}, { 17,  6, 0x00000034}, { 18,  7, 0x00000072}, { 19,  8, 0x000000ea},
    { 20,  8, 0x000000f1}, { 21,  9, 0x000001e9}, { 22,  9, 0x000001f3}, { 23, 10, 0x000003f5},
    { 24,  7, 0x00000073}, { 25,  7, 0x00000070}, { 26,  8, 0x000000eb}, { 27,  8, 0x000000f0},
    { 28,  9, 0x000001f1}, { 29,  9, 0x000001f0}, { 30, 10, 0x000003ec}, { 31, 10, 0x000003fa},
    { 32,  8, 0x000000f3}, { 33,  8, 0x000000ed}, { 34,  9, 0x000001e8}, { 35,  9, 0x000001ef},
    { 36, 10, 0x000003ef}, { 37, 10, 0x000003f1}, { 38, 10, 0x000003f9}, { 39, 11, 0x000007fb},
    { 40,  9, 0x000001ed}, { 41,  8, 0x000000ef}, { 42,  9, 0x000001ea}, { 43,  9, 0x000001f2},
    { 44, 10, 0x000003f3}, { 45, 10, 0x000003f8}, { 46, 11, 0x000007f9}, { 47, 11, 0x000007fc},
    { 48, 10, 0x000003ee}, { 49,  9, 0x000001ec}, { 50,  9, 0x000001f4}, { 51, 10, 0x000003f4},
    { 52, 10, 0x000003f7}, { 53, 11, 0x000007f8}, { 54, 12, 0x00000ffd}, { 55, 12, 0x00000ffe},
    { 56, 11, 0x000007f6}, { 57, 10, 0x000003f0}, { 58, 10, 0x000003f2}, { 59, 10, 0x000003f6},
    { 60, 11, 0x000007fa}, { 61, 11, 0x000007fd}, { 62, 12, 0x00000ffc}, { 63, 12, 0x00000fff}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.9 - Spectrum Huffman Codebook 8
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook8Table[64][3] = {
    {  0,  5, 0x0000000e}, {  1,  4, 0x00000005}, {  2,  5, 0x00000010}, {  3,  6, 0x00000030},
    {  4,  7, 0x0000006f}, {  5,  8, 0x000000f1}, {  6,  9, 0x000001fa}, {  7, 10, 0x000003fe},
    {  8,  4, 0x00000003}, {  9,  3, 0x00000000}, { 10,  4, 0x00000004}, { 11,  5, 0x00000012},
    { 12,  6, 0x0000002c}, { 13,  7, 0x0000006a}, { 14,  7, 0x00000075}, { 15,  8, 0x000000f8},
    { 16,  5, 0x0000000f}, { 17,  4, 0x00000002}, { 18,  4, 0x00000006}, { 19,  5, 0x00000014},
    { 20,  6, 0x0000002e}, { 21,  7, 0x00000069}, { 22,  7, 0x00000072}, { 23,  8, 0x000000f5},
    { 24,  6, 0x0000002f}, { 25,  5, 0x00000011}, { 26,  5, 0x00000013}, { 27,  6, 0x0000002a},
    { 28,  6, 0x00000032}, { 29,  7, 0x0000006c}, { 30,  8, 0x000000ec}, { 31,  8, 0x000000fa},
    { 32,  7, 0x00000071}, { 33,  6, 0x0000002b}, { 34,  6, 0x0000002d}, { 35,  6, 0x00000031},
    { 36,  7, 0x0000006d}, { 37,  7, 0x00000070}, { 38,  8, 0x000000f2}, { 39,  9, 0x000001f9},
    { 40,  8, 0x000000ef}, { 41,  7, 0x00000068}, { 42,  6, 0x00000033}, { 43,  7, 0x0000006b},
    { 44,  7, 0x0000006e}, { 45,  8, 0x000000ee}, { 46,  8, 0x000000f9}, { 47, 10, 0x000003fc},
    { 48,  9, 0x000001f8}, { 49,  7, 0x00000074}, { 50,  7, 0x00000073}, { 51,  8, 0x000000ed},
    { 52,  8, 0x000000f0}, { 53,  8, 0x000000f6}, { 54,  9, 0x000001f6}, { 55,  9, 0x000001fd},
    { 56, 10, 0x000003fd}, { 57,  8, 0x000000f3}, { 58,  8, 0x000000f4}, { 59,  8, 0x000000f7},
    { 60,  9, 0x000001f7}, { 61,  9, 0x000001fb}, { 62,  9, 0x000001fc}, { 63, 10, 0x000003ff}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.10 - Spectrum Huffman Codebook 9
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook9Table[169][3] = {
    {   0,  1, 0x00000000}, {   1,  3, 0x00000005}, {   2,  6, 0x00000037}, {   3,  8, 0x000000e7},
    {   4,  9, 0x000001de}, {   5, 10, 0x000003ce}, {   6, 10, 0x000003d9}, {   7, 11, 0x000007c8},
    {   8, 11, 0x000007cd}, {   9, 12, 0x00000fc8}, {  10, 12, 0x00000fdd}, {  11, 13, 0x00001fe4},
    {  12, 13, 0x00001fec}, {  13,  3, 0x00000004}, {  14,  4, 0x0000000c}, {  15,  6, 0x00000035},
    {  16,  7, 0x00000072}, {  17,  8, 0x000000ea}, {  18,  8, 0x000000ed}, {  19,  9, 0x000001e2},
    {  20, 10, 0x000003d1}, {  21, 10, 0x000003d3}, {  22, 10, 0x000003e0}, {  23, 11, 0x000007d8},
    {  24, 12, 0x00000fcf}, {  25, 12, 0x00000fd5}, {  26,  6, 0x00000036}, {  27,  6, 0x00000034},
    {  28,  7, 0x00000071}, {  29,  8, 0x000000e8}, {  30,  8, 0x000000ec}, {  31,  9, 0x000001e1},
    {  32, 10, 0x000003cf}, {  33, 10, 0x000003dd}, {  34, 10, 0x000003db}, {  35, 11, 0x000007d0},
    {  36, 12, 0x00000fc7}, {  37, 12, 0x00000fd4}, {  38, 12, 0x00000fe4}, {  39,  8, 0x000000e6},
    {  40,  7, 0x00000070}, {  41,  8, 0x000000e9}, {  42,  9, 0x000001dd}, {  43,  9, 0x000001e3},
    {  44, 10, 0x000003d2}, {  45, 10, 0x000003dc}, {  46, 11, 0x000007cc}, {  47, 11, 0x000007ca},
    {  48, 11, 0x000007de}, {  49, 12, 0x00000fd8}, {  50, 12, 0x00000fea}, {  51, 13, 0x00001fdb},
    {  52,  9, 0x000001df}, {  53,  8, 0x000000eb}, {  54,  9, 0x000001dc}, {  55,  9, 0x000001e6},
    {  56, 10, 0x000003d5}, {  57, 10, 0x000003de}, {  58, 11, 0x000007cb}, {  59, 11, 0x000007dd},
    {  60, 11, 0x000007dc}, {  61, 12, 0x00000fcd}, {  62, 12, 0x00000fe2}, {  63, 12, 0x00000fe7},
    {  64, 13, 0x00001fe1}, {  65, 10, 0x000003d0}, {  66,  9, 0x000001e0}, {  67,  9, 0x000001e4},
    {  68, 10, 0x000003d6}, {  69, 11, 0x000007c5}, {  70, 11, 0x000007d1}, {  71, 11, 0x000007db},
    {  72, 12, 0x00000fd2}, {  73, 11, 0x000007e0}, {  74, 12, 0x00000fd9}, {  75, 12, 0x00000feb},
    {  76, 13, 0x00001fe3}, {  77, 13, 0x00001fe9}, {  78, 11, 0x000007c4}, {  79,  9, 0x000001e5},
    {  80, 10, 0x000003d7}, {  81, 11, 0x000007c6}, {  82, 11, 0x000007cf}, {  83, 11, 0x000007da},
    {  84, 12, 0x00000fcb}, {  85, 12, 0x00000fda}, {  86, 12, 0x00000fe3}, {  87, 12, 0x00000fe9},
    {  88, 13, 0x00001fe6}, {  89, 13, 0x00001ff3}, {  90, 13, 0x00001ff7}, {  91, 11, 0x000007d3},
    {  92, 10, 0x000003d8}, {  93, 10, 0x000003e1}, {  94, 11, 0x000007d4}, {  95, 11, 0x000007d9},
    {  96, 12, 0x00000fd3}, {  97, 12, 0x00000fde}, {  98, 13, 0x00001fdd}, {  99, 13, 0x00001fd9},
    { 100, 13, 0x00001fe2}, { 101, 13, 0x00001fea}, { 102, 13, 0x00001ff1}, { 103, 13, 0x00001ff6},
    { 104, 11, 0x000007d2}, { 105, 10, 0x000003d4}, { 106, 10, 0x000003da}, { 107, 11, 0x000007c7},
    { 108, 11, 0x000007d7}, { 109, 11, 0x000007e2}, { 110, 12, 0x00000fce}, { 111, 12, 0x00000fdb},
    { 112, 13, 0x00001fd8}, { 113, 13, 0x00001fee}, { 114, 14, 0x00003ff0}, { 115, 13, 0x00001ff4},
    { 116, 14, 0x00003ff2}, { 117, 11, 0x000007e1}, { 118, 10, 0x000003df}, { 119, 11, 0x000007c9},
    { 120, 11, 0x000007d6}, { 121, 12, 0x00000fca}, { 122, 12, 0x00000fd0}, { 123, 12, 0x00000fe5},
    { 124, 12, 0x00000fe6}, { 125, 13, 0x00001feb}, { 126, 13, 0x00001fef}, { 127, 14, 0x00003ff3},
    { 128, 14, 0x00003ff4}, { 129, 14, 0x00003ff5}, { 130, 12, 0x00000fe0}, { 131, 11, 0x000007ce},
    { 132, 11, 0x000007d5}, { 133, 12, 0x00000fc6}, { 134, 12, 0x00000fd1}, { 135, 12, 0x00000fe1},
    { 136, 13, 0x00001fe0}, { 137, 13, 0x00001fe8}, { 138, 13, 0x00001ff0}, { 139, 14, 0x00003ff1},
    { 140, 14, 0x00003ff8}, { 141, 14, 0x00003ff6}, { 142, 15, 0x00007ffc}, { 143, 12, 0x00000fe8},
    { 144, 11, 0x000007df}, { 145, 12, 0x00000fc9}, { 146, 12, 0x00000fd7}, { 147, 12, 0x00000fdc},
    { 148, 13, 0x00001fdc}, { 149, 13, 0x00001fdf}, { 150, 13, 0x00001fed}, { 151, 13, 0x00001ff5},
    { 152, 14, 0x00003ff9}, { 153, 14, 0x00003ffb}, { 154, 15, 0x00007ffd}, { 155, 15, 0x00007ffe},
    { 156, 13, 0x00001fe7}, { 157, 12, 0x00000fcc}, { 158, 12, 0x00000fd6}, { 159, 12, 0x00000fdf},
    { 160, 13, 0x00001fde}, { 161, 13, 0x00001fda}, { 162, 13, 0x00001fe5}, { 163, 13, 0x00001ff2},
    { 164, 14, 0x00003ffa}, { 165, 14, 0x00003ff7}, { 166, 14, 0x00003ffc}, { 167, 14, 0x00003ffd},
    { 168, 15, 0x00007fff}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.11 - Spectrum Huffman Codebook 10
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook10Table[169][3] = {
    {   0,  6, 0x00000022}, {   1,  5, 0x00000008}, {   2,  6, 0x0000001d}, {   3,  6, 0x00000026},
    {   4,  7, 0x0000005f}, {   5,  8, 0x000000d3}, {   6,  9, 0x000001cf}, {   7, 10, 0x000003d0},
    {   8, 10, 0x000003d7}, {   9, 10, 0x000003ed}, {  10, 11, 0x000007f0}, {  11, 11, 0x000007f6},
    {  12, 12, 0x00000ffd}, {  13,  5, 0x00000007}, {  14,  4, 0x00000000}, {  15,  4, 0x00000001},
    {  16,  5, 0x00000009}, {  17,  6, 0x00000020}, {  18,  7, 0x00000054}, {  19,  7, 0x00000060},
    {  20,  8, 0x000000d5}, {  21,  8, 0x000000dc}, {  22,  9, 0x000001d4}, {  23, 10, 0x000003cd},
    {  24, 10, 0x000003de}, {  25, 11, 0x000007e7}, {  26,  6, 0x0000001c}, {  27,  4, 0x00000002},
    {  28,  5, 0x00000006}, {  29,  5, 0x0000000c}, {  30,  6, 0x0000001e}, {  31,  6, 0x00000028},
    {  32,  7, 0x0000005b}, {  33,  8, 0x000000cd}, {  34,  8, 0x000000d9}, {  35,  9, 0x000001ce},
    {  36,  9, 0x000001dc}, {  37, 10, 0x000003d9}, {  38, 10, 0x000003f1}, {  39,  6, 0x00000025},
    {  40,  5, 0x0000000b}, {  41,  5, 0x0000000a}, {  42,  5, 0x0000000d}, {  43,  6, 0x00000024},
    {  44,  7, 0x00000057}, {  45,  7, 0x00000061}, {  46,  8, 0x000000cc}, {  47,  8, 0x000000dd},
    {  48,  9, 0x000001cc}, {  49,  9, 0x000001de}, {  50, 10, 0x000003d3}, {  51, 10, 0x000003e7},
    {  52,  7, 0x0000005d}, {  53,  6, 0x00000021}, {  54,  6, 0x0000001f}, {  55,  6, 0x00000023},
    {  56,  6, 0x00000027}, {  57,  7, 0x00000059}, {  58,  7, 0x00000064}, {  59,  8, 0x000000d8},
    {  60,  8, 0x000000df}, {  61,  9, 0x000001d2}, {  62,  9, 0x000001e2}, {  63, 10, 0x000003dd},
    {  64, 10, 0x000003ee}, {  65,  8, 0x000000d1}, {  66,  7, 0x00000055}, {  67,  6, 0x00000029},
    {  68,  7, 0x00000056}, {  69,  7, 0x00000058}, {  70,  7, 0x00000062}, {  71,  8, 0x000000ce},
    {  72,  8, 0x000000e0}, {  73,  8, 0x000000e2}, {  74,  9, 0x000001da}, {  75, 10, 0x000003d4},
    {  76, 10, 0x000003e3}, {  77, 11, 0x000007eb}, {  78,  9, 0x000001c9}, {  79,  7, 0x0000005e},
    {  80,  7, 0x0000005a}, {  81,  7, 0x0000005c}, {  82,  7, 0x00000063}, {  83,  8, 0x000000ca},
    {  84,  8, 0x000000da}, {  85,  9, 0x000001c7}, {  86,  9, 0x000001ca}, {  87,  9, 0x000001e0},
    {  88, 10, 0x000003db}, {  89, 10, 0x000003e8}, {  90, 11, 0x000007ec}, {  91,  9, 0x000001e3},
    {  92,  8, 0x000000d2}, {  93,  8, 0x000000cb}, {  94,  8, 0x000000d0}, {  95,  8, 0x000000d7},
    {  96,  8, 0x000000db}, {  97,  9, 0x000001c6}, {  98,  9, 0x000001d5}, {  99,  9, 0x000001d8},
    { 100, 10, 0x000003ca}, { 101, 10, 0x000003da}, { 102, 11, 0x000007ea}, { 103, 11, 0x000007f1},
    { 104,  9, 0x000001e1}, { 105,  8, 0x000000d4}, { 106,  8, 0x000000cf}, { 107,  8, 0x000000d6},
    { 108,  8, 0x000000de}, { 109,  8, 0x000000e1}, { 110,  9, 0x000001d0}, { 111,  9, 0x000001d6},
    { 112, 10, 0x000003d1}, { 113, 10, 0x000003d5}, { 114, 10, 0x000003f2}, { 115, 11, 0x000007ee},
    { 116, 11, 0x000007fb}, { 117, 10, 0x000003e9}, { 118,  9, 0x000001cd}, { 119,  9, 0x000001c8},
    { 120,  9, 0x000001cb}, { 121,  9, 0x000001d1}, { 122,  9, 0x000001d7}, { 123,  9, 0x000001df},
    { 124, 10, 0x000003cf}, { 125, 10, 0x000003e0}, { 126, 10, 0x000003ef}, { 127, 11, 0x000007e6},
    { 128, 11, 0x000007f8}, { 129, 12, 0x00000ffa}, { 130, 10, 0x000003eb}, { 131,  9, 0x000001dd},
    { 132,  9, 0x000001d3}, { 133,  9, 0x000001d9}, { 134,  9, 0x000001db}, { 135, 10, 0x000003d2},
    { 136, 10, 0x000003cc}, { 137, 10, 0x000003dc}, { 138, 10, 0x000003ea}, { 139, 11, 0x000007ed},
    { 140, 11, 0x000007f3}, { 141, 11, 0x000007f9}, { 142, 12, 0x00000ff9}, { 143, 11, 0x000007f2},
    { 144, 10, 0x000003ce}, { 145,  9, 0x000001e4}, { 146, 10, 0x000003cb}, { 147, 10, 0x000003d8},
    { 148, 10, 0x000003d6}, { 149, 10, 0x000003e2}, { 150, 10, 0x000003e5}, { 151, 11, 0x000007e8},
    { 152, 11, 0x000007f4}, { 153, 11, 0x000007f5}, { 154, 11, 0x000007f7}, { 155, 12, 0x00000ffb},
    { 156, 11, 0x000007fa}, { 157, 10, 0x000003ec}, { 158, 10, 0x000003df}, { 159, 10, 0x000003e1},
    { 160, 10, 0x000003e4}, { 161, 10, 0x000003e6}, { 162, 10, 0x000003f0}, { 163, 11, 0x000007e9},
    { 164, 11, 0x000007ef}, { 165, 12, 0x00000ff8}, { 166, 12, 0x00000ffe}, { 167, 12, 0x00000ffc},
    { 168, 12, 0x00000fff}
};

//-------------------------------------------------------------------------------------------
// Table 4.A.12 - Spectrum Huffman Codebook 11
//-------------------------------------------------------------------------------------------

tint WSequence::m_spectrumCodebook11Table[289][3] = {
    {   0,  4, 0x00000000}, {   1,  5, 0x00000006}, {   2,  6, 0x00000019}, {   3,  7, 0x0000003d},
    {   4,  8, 0x0000009c}, {   5,  8, 0x000000c6}, {   6,  9, 0x000001a7}, {   7, 10, 0x00000390},
    {   8, 10, 0x000003c2}, {   9, 10, 0x000003df}, {  10, 11, 0x000007e6}, {  11, 11, 0x000007f3},
    {  12, 12, 0x00000ffb}, {  13, 11, 0x000007ec}, {  14, 12, 0x00000ffa}, {  15, 12, 0x00000ffe},
    {  16, 10, 0x0000038e}, {  17,  5, 0x00000005}, {  18,  4, 0x00000001}, {  19,  5, 0x00000008},
    {  20,  6, 0x00000014}, {  21,  7, 0x00000037}, {  22,  7, 0x00000042}, {  23,  8, 0x00000092},
    {  24,  8, 0x000000af}, {  25,  9, 0x00000191}, {  26,  9, 0x000001a5}, {  27,  9, 0x000001b5},
    {  28, 10, 0x0000039e}, {  29, 10, 0x000003c0}, {  30, 10, 0x000003a2}, {  31, 10, 0x000003cd},
    {  32, 11, 0x000007d6}, {  33,  8, 0x000000ae}, {  34,  6, 0x00000017}, {  35,  5, 0x00000007},
    {  36,  5, 0x00000009}, {  37,  6, 0x00000018}, {  38,  7, 0x00000039}, {  39,  7, 0x00000040},
    {  40,  8, 0x0000008e}, {  41,  8, 0x000000a3}, {  42,  8, 0x000000b8}, {  43,  9, 0x00000199},
    {  44,  9, 0x000001ac}, {  45,  9, 0x000001c1}, {  46, 10, 0x000003b1}, {  47, 10, 0x00000396},
    {  48, 10, 0x000003be}, {  49, 10, 0x000003ca}, {  50,  8, 0x0000009d}, {  51,  7, 0x0000003c},
    {  52,  6, 0x00000015}, {  53,  6, 0x00000016}, {  54,  6, 0x0000001a}, {  55,  7, 0x0000003b},
    {  56,  7, 0x00000044}, {  57,  8, 0x00000091}, {  58,  8, 0x000000a5}, {  59,  8, 0x000000be},
    {  60,  9, 0x00000196}, {  61,  9, 0x000001ae}, {  62,  9, 0x000001b9}, {  63, 10, 0x000003a1},
    {  64, 10, 0x00000391}, {  65, 10, 0x000003a5}, {  66, 10, 0x000003d5}, {  67,  8, 0x00000094},
    {  68,  8, 0x0000009a}, {  69,  7, 0x00000036}, {  70,  7, 0x00000038}, {  71,  7, 0x0000003a},
    {  72,  7, 0x00000041}, {  73,  8, 0x0000008c}, {  74,  8, 0x0000009b}, {  75,  8, 0x000000b0},
    {  76,  8, 0x000000c3}, {  77,  9, 0x0000019e}, {  78,  9, 0x000001ab}, {  79,  9, 0x000001bc},
    {  80, 10, 0x0000039f}, {  81, 10, 0x0000038f}, {  82, 10, 0x000003a9}, {  83, 10, 0x000003cf},
    {  84,  8, 0x00000093}, {  85,  8, 0x000000bf}, {  86,  7, 0x0000003e}, {  87,  7, 0x0000003f},
    {  88,  7, 0x00000043}, {  89,  7, 0x00000045}, {  90,  8, 0x0000009e}, {  91,  8, 0x000000a7},
    {  92,  8, 0x000000b9}, {  93,  9, 0x00000194}, {  94,  9, 0x000001a2}, {  95,  9, 0x000001ba},
    {  96,  9, 0x000001c3}, {  97, 10, 0x000003a6}, {  98, 10, 0x000003a7}, {  99, 10, 0x000003bb},
    { 100, 10, 0x000003d4}, { 101,  8, 0x0000009f}, { 102,  9, 0x000001a0}, { 103,  8, 0x0000008f},
    { 104,  8, 0x0000008d}, { 105,  8, 0x00000090}, { 106,  8, 0x00000098}, { 107,  8, 0x000000a6},
    { 108,  8, 0x000000b6}, { 109,  8, 0x000000c4}, { 110,  9, 0x0000019f}, { 111,  9, 0x000001af},
    { 112,  9, 0x000001bf}, { 113, 10, 0x00000399}, { 114, 10, 0x000003bf}, { 115, 10, 0x000003b4},
    { 116, 10, 0x000003c9}, { 117, 10, 0x000003e7}, { 118,  8, 0x000000a8}, { 119,  9, 0x000001b6},
    { 120,  8, 0x000000ab}, { 121,  8, 0x000000a4}, { 122,  8, 0x000000aa}, { 123,  8, 0x000000b2},
    { 124,  8, 0x000000c2}, { 125,  8, 0x000000c5}, { 126,  9, 0x00000198}, { 127,  9, 0x000001a4},
    { 128,  9, 0x000001b8}, { 129, 10, 0x0000038c}, { 130, 10, 0x000003a4}, { 131, 10, 0x000003c4},
    { 132, 10, 0x000003c6}, { 133, 10, 0x000003dd}, { 134, 10, 0x000003e8}, { 135,  8, 0x000000ad},
    { 136, 10, 0x000003af}, { 137,  9, 0x00000192}, { 138,  8, 0x000000bd}, { 139,  8, 0x000000bc},
    { 140,  9, 0x0000018e}, { 141,  9, 0x00000197}, { 142,  9, 0x0000019a}, { 143,  9, 0x000001a3},
    { 144,  9, 0x000001b1}, { 145, 10, 0x0000038d}, { 146, 10, 0x00000398}, { 147, 10, 0x000003b7},
    { 148, 10, 0x000003d3}, { 149, 10, 0x000003d1}, { 150, 10, 0x000003db}, { 151, 11, 0x000007dd},
    { 152,  8, 0x000000b4}, { 153, 10, 0x000003de}, { 154,  9, 0x000001a9}, { 155,  9, 0x0000019b},
    { 156,  9, 0x0000019c}, { 157,  9, 0x000001a1}, { 158,  9, 0x000001aa}, { 159,  9, 0x000001ad},
    { 160,  9, 0x000001b3}, { 161, 10, 0x0000038b}, { 162, 10, 0x000003b2}, { 163, 10, 0x000003b8},
    { 164, 10, 0x000003ce}, { 165, 10, 0x000003e1}, { 166, 10, 0x000003e0}, { 167, 11, 0x000007d2},
    { 168, 11, 0x000007e5}, { 169,  8, 0x000000b7}, { 170, 11, 0x000007e3}, { 171,  9, 0x000001bb},
    { 172,  9, 0x000001a8}, { 173,  9, 0x000001a6}, { 174,  9, 0x000001b0}, { 175,  9, 0x000001b2},
    { 176,  9, 0x000001b7}, { 177, 10, 0x0000039b}, { 178, 10, 0x0000039a}, { 179, 10, 0x000003ba},
    { 180, 10, 0x000003b5}, { 181, 10, 0x000003d6}, { 182, 11, 0x000007d7}, { 183, 10, 0x000003e4},
    { 184, 11, 0x000007d8}, { 185, 11, 0x000007ea}, { 186,  8, 0x000000ba}, { 187, 11, 0x000007e8},
    { 188, 10, 0x000003a0}, { 189,  9, 0x000001bd}, { 190,  9, 0x000001b4}, { 191, 10, 0x0000038a},
    { 192,  9, 0x000001c4}, { 193, 10, 0x00000392}, { 194, 10, 0x000003aa}, { 195, 10, 0x000003b0},
    { 196, 10, 0x000003bc}, { 197, 10, 0x000003d7}, { 198, 11, 0x000007d4}, { 199, 11, 0x000007dc},
    { 200, 11, 0x000007db}, { 201, 11, 0x000007d5}, { 202, 11, 0x000007f0}, { 203,  8, 0x000000c1},
    { 204, 11, 0x000007fb}, { 205, 10, 0x000003c8}, { 206, 10, 0x000003a3}, { 207, 10, 0x00000395},
    { 208, 10, 0x0000039d}, { 209, 10, 0x000003ac}, { 210, 10, 0x000003ae}, { 211, 10, 0x000003c5},
    { 212, 10, 0x000003d8}, { 213, 10, 0x000003e2}, { 214, 10, 0x000003e6}, { 215, 11, 0x000007e4},
    { 216, 11, 0x000007e7}, { 217, 11, 0x000007e0}, { 218, 11, 0x000007e9}, { 219, 11, 0x000007f7},
    { 220,  9, 0x00000190}, { 221, 11, 0x000007f2}, { 222, 10, 0x00000393}, { 223,  9, 0x000001be},
    { 224,  9, 0x000001c0}, { 225, 10, 0x00000394}, { 226, 10, 0x00000397}, { 227, 10, 0x000003ad},
    { 228, 10, 0x000003c3}, { 229, 10, 0x000003c1}, { 230, 10, 0x000003d2}, { 231, 11, 0x000007da},
    { 232, 11, 0x000007d9}, { 233, 11, 0x000007df}, { 234, 11, 0x000007eb}, { 235, 11, 0x000007f4},
    { 236, 11, 0x000007fa}, { 237,  9, 0x00000195}, { 238, 11, 0x000007f8}, { 239, 10, 0x000003bd},
    { 240, 10, 0x0000039c}, { 241, 10, 0x000003ab}, { 242, 10, 0x000003a8}, { 243, 10, 0x000003b3},
    { 244, 10, 0x000003b9}, { 245, 10, 0x000003d0}, { 246, 10, 0x000003e3}, { 247, 10, 0x000003e5},
    { 248, 11, 0x000007e2}, { 249, 11, 0x000007de}, { 250, 11, 0x000007ed}, { 251, 11, 0x000007f1},
    { 252, 11, 0x000007f9}, { 253, 11, 0x000007fc}, { 254,  9, 0x00000193}, { 255, 12, 0x00000ffd},
    { 256, 10, 0x000003dc}, { 257, 10, 0x000003b6}, { 258, 10, 0x000003c7}, { 259, 10, 0x000003cc},
    { 260, 10, 0x000003cb}, { 261, 10, 0x000003d9}, { 262, 10, 0x000003da}, { 263, 11, 0x000007d3},
    { 264, 11, 0x000007e1}, { 265, 11, 0x000007ee}, { 266, 11, 0x000007ef}, { 267, 11, 0x000007f5},
    { 268, 11, 0x000007f6}, { 269, 12, 0x00000ffc}, { 270, 12, 0x00000fff}, { 271,  9, 0x0000019d},
    { 272,  9, 0x000001c2}, { 273,  8, 0x000000b5}, { 274,  8, 0x000000a1}, { 275,  8, 0x00000096},
    { 276,  8, 0x00000097}, { 277,  8, 0x00000095}, { 278,  8, 0x00000099}, { 279,  8, 0x000000a0},
    { 280,  8, 0x000000a2}, { 281,  8, 0x000000ac}, { 282,  8, 0x000000a9}, { 283,  8, 0x000000b1},
    { 284,  8, 0x000000b3}, { 285,  8, 0x000000bb}, { 286,  8, 0x000000c0}, { 287,  9, 0x0000018f},
    { 288,  5, 0x00000004}
};

//-------------------------------------------------------------------------------------------

tint WSequence::m_rvlcCodebook1Table[15][3] = {
    { -7, 7,  65}, { -6, 9, 257}, { -5, 8, 129}, { -4, 6,  33},
    { -3, 5,  17}, { -2, 4,   9}, { -1, 3,   5}, {  0, 1,   0},
    {  1, 3,   7}, {  2, 5,  27}, {  3, 6,  51}, {  4, 7, 107},
    {  5, 8, 195}, {  6, 9, 427}, {  7, 7,  99}
};

//-------------------------------------------------------------------------------------------

tint WSequence::m_rvlcCodebook2Table[54][3] = {
    {  0,  2,      2}, {  1,  2,      0}, {  2,  3,      6}, {  3,  3,      2},
    {  4,  4,     14}, {  5,  5,     31}, {  6,  5,     15}, {  7,  5,     13},
    {  8,  6,     61}, {  9,  6,     29}, { 10,  6,     25}, { 11,  6,     24},
    { 12,  7,    120}, { 13,  7,     56}, { 14,  8,    242}, { 15,  8,    114},
    { 16,  9,    486}, { 17,  9,    230}, { 18, 10,    974}, { 19, 10,    463},
    { 20, 11,   1950}, { 21, 11,   1951}, { 22, 11,    925}, { 23, 12,   1848},
    { 24, 14,   7399}, { 25, 13,   3698}, { 26, 15,  14797}, { 27, 20, 473482},
    { 28, 20, 473483}, { 29, 20, 473484}, { 30, 20, 473485}, { 31, 20, 473486},
    { 32, 20, 473487}, { 33, 20, 473488}, { 34, 20, 473489}, { 35, 20, 473490},
    { 36, 20, 473491}, { 37, 20, 473492}, { 38, 20, 473493}, { 39, 20, 473494},
    { 40, 20, 473495}, { 41, 20, 473496}, { 42, 20, 473497}, { 43, 20, 473498},
    { 44, 20, 473499}, { 45, 20, 473500}, { 46, 20, 473501}, { 47, 20, 473502},
    { 48, 20, 473503}, { 49, 19, 236736}, { 50, 19, 236737}, { 51, 19, 236738},
    { 52, 19, 236739}, { 53, 19, 236740}
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
