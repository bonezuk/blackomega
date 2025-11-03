#include "common/inc/SHA1Digest.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

SHA1Digest::SHA1Digest() : m_lengthLow(0),
    m_lengthHigh(0),
    m_index(0),
    m_computed(false)
{
    reset();
}

//-------------------------------------------------------------------------------------------

SHA1Digest::~SHA1Digest()
{
    try
    {
        reset();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void SHA1Digest::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "SHA1Digest::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void SHA1Digest::reset()
{
    m_lengthLow = 0;
    m_lengthHigh = 0;
    m_index = 0;
    m_computed = false;

    m_hash[0] = 0x67452301;
    m_hash[1] = 0xEFCDAB89;
    m_hash[2] = 0x98BADCFE;
    m_hash[3] = 0x10325476;
    m_hash[4] = 0xC3D2E1F0;

    ::memset(m_block,0,64 * sizeof(tubyte));
}

//-------------------------------------------------------------------------------------------

bool SHA1Digest::getDigest(tbyte *mem,tint len)
{
    tint i;

    if(mem==NULL || len!=c_SHA1HashSize)
    {
        printError("GetDigest","SHA1 digest buffer required to be exactly 20 bytes");
        return false;
    }

    for(i=0;i<c_SHA1HashSize;i++)
    {
        mem[i] = static_cast<tbyte>(m_hash[i>>2] >> (8 * (3 - (i & 0x03))));
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool SHA1Digest::getDigestFinal(tbyte *mem,tint len)
{
    tint i;

    if(mem==NULL || len!=c_SHA1HashSize)
    {
        printError("GetDigest","SHA1 digest buffer required to be exactly 20 bytes");
        return false;
    }

    if(!m_computed)
    {
        padMessage();
        for(i=0;i<64;i++)
        {
            m_block[i] = 0;
        }
        m_lengthLow    = 0;
        m_lengthHigh = 0;
        m_computed = true;
    }
    else
    {
        printError("GetDigest","Final SHA1 digest has been computed");
        return false;
    }

    for(i=0;i<c_SHA1HashSize;i++)
    {
        mem[i] = static_cast<tbyte>(m_hash[i>>2] >> (8 * (3 - (i & 0x03))));
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool SHA1Digest::input(const tbyte *mem,tint len)
{
    tint i=0;
    bool res = false;

    if(len==0)
    {
        return true;
    }

    if(mem!=NULL && len>0)
    {
        while(i<len)
        {
            if(m_index < 64)
            {
                m_block[m_index++] = static_cast<tubyte>(mem[i++]);
            }
            m_lengthLow += 8;
            if(m_lengthLow==0)
            {
                m_lengthHigh++;
            }
            if(m_index==64)
            {
                processMessageBlock();
                m_index = 0;
            }
        }
        res = true;
    }
    else
    {
        printError("Input","No input buffer given");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SHA1Digest::padMessage()
{
    if(m_index > 55)
    {
        m_block[m_index++] = 0x80;
        while(m_index < 64)
        {
            m_block[m_index++] = 0;
        }
        processMessageBlock();
        while(m_index < 56)
        {
            m_block[m_index++] = 0;
        }
    }
    else
    {
        m_block[m_index++] = 0x80;
        while(m_index < 56)
        {
            m_block[m_index++] = 0;
        }
    }

    m_block[56] = static_cast<tubyte>((m_lengthHigh >> 24) & 0x000000ff);
    m_block[57] = static_cast<tubyte>((m_lengthHigh >> 16) & 0x000000ff);
    m_block[58] = static_cast<tubyte>((m_lengthHigh >> 8) & 0x000000ff);
    m_block[59] = static_cast<tubyte>(m_lengthHigh & 0x000000ff);

    m_block[60] = static_cast<tubyte>((m_lengthLow >> 24) & 0x000000ff);
    m_block[61] = static_cast<tubyte>((m_lengthLow >> 16) & 0x000000ff);
    m_block[62] = static_cast<tubyte>((m_lengthLow >> 8) & 0x000000ff);
    m_block[63] = static_cast<tubyte>(m_lengthLow & 0x000000ff);

    processMessageBlock();
}

//-------------------------------------------------------------------------------------------

#define SHA1CircularShift(bits,word) (((word) << (bits)) | ((word) >> (32-(bits))))

void SHA1Digest::processMessageBlock()
{
    const tuint32 K[4] = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };

    tint i,t;
    tuint32 temp;
    tuint32 W[80];
    tuint32 A,B,C,D,E;

    for(t=0,i=0;t<16;t++,i+=4)
    {
        W[t]  = static_cast<tuint32>(m_block[i]) << 24 & 0xff000000;
        W[t] |= static_cast<tuint32>(m_block[i + 1]) << 16 & 0x00ff0000;
        W[t] |= static_cast<tuint32>(m_block[i + 2]) << 8  & 0x0000ff00;
        W[t] |= static_cast<tuint32>(m_block[i + 3]) & 0x000000ff;
    }

    for(t=16;t<80;t++)
    {
        W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = m_hash[0];
    B = m_hash[1];
    C = m_hash[2];
    D = m_hash[3];
    E = m_hash[4];

    for(t=0;t<20;t++)
    {
        temp = SHA1CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t=20;t<40;t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t=40;t<60;t++)
    {
        temp = SHA1CircularShift(5,A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t=60;t<80;t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    m_hash[0] += A;
    m_hash[1] += B;
    m_hash[2] += C;
    m_hash[3] += D;
    m_hash[4] += E;

    m_index = 0;

    temp = 0;
    A = B = C = D = E = 0;
    ::memset(W,0,80 * sizeof(tuint32));
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
