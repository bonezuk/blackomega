//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_SHA1DIGEST_H
#define __OMEGA_COMMON_SHA1DIGEST_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

const tint c_SHA1HashSize = 20;

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT SHA1Digest
{
    public:
        SHA1Digest();
        ~SHA1Digest();

        bool getDigest(tbyte *mem,tint len);
        bool getDigestFinal(tbyte *mem,tint len);

        void reset();

        bool input(const tbyte *mem,tint len);

    private:

        tuint32 m_hash[c_SHA1HashSize / 4];
        tuint32 m_lengthLow;
        tuint32 m_lengthHigh;

        tint m_index;

        tubyte m_block[64];

        bool m_computed;

        void printError(const tchar *strR,const tchar *strE) const;

        void padMessage();
        void processMessageBlock();
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
