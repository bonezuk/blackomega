//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_MD5DIGEST_H
#define __OMEGA_COMMON_MD5DIGEST_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT MD5Digest
{
    public:
        MD5Digest();
        virtual ~MD5Digest();
        
        void init();
        void write(tubyte *buf,tint len);
        void complete();
        void final(tubyte *digest);
        
    protected:
    
        tuint32 m_state[4];
        tuint32 m_count[2];
        tubyte m_buffer[64];
        tubyte m_padding[64];
        tubyte m_digest[33];
        
        void digestMemCpy(tubyte *output,const tubyte *input,tint len);

        void digestInit();
        void digestUpdate(tubyte *input,tuint len);
        void digestFinal();
        void digestTransform(tubyte *block);
        
        void encode(tubyte *output,tuint32 *input,tint len);
        void decode(tuint32 *output,tubyte *input,tint len);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

