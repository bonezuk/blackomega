//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BIOBUFFEREDSTREAM_H
#define __OMEGA_COMMON_BIOBUFFEREDSTREAM_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BIOBufferedStream : public BIOStream
{
    public:
        BIOBufferedStream(tint flags);
        BIOBufferedStream(tint flags,tint bufferSize);
        BIOBufferedStream(tint flags,tbyte *nBuffer,tint bufferSize);
        virtual ~BIOBufferedStream();

        virtual bool close();

        virtual tint read(tbyte *mem,tint len);
        virtual tint read(tubyte *mem,tint len);

        virtual tint write(const tbyte *mem,tint len);
        virtual tint write(const tubyte *mem,tint len);

        virtual bool seek64(tint64 pos,BIOStreamPosition flag);

        virtual tint64 size64();

        virtual bool sof();
        virtual bool eof();

        virtual tint bookmark(tint offset=0);

        virtual tint64 offset64() const;

    protected:

        tubyte *m_buffer;
        tint64 m_positionB;
        tint64 m_length;
        tint m_bufferSize;

        bool m_readBufferFlag;
        bool m_writeBufferFlag;

        bool m_isOwnBuffer;

        virtual void PrintError(const tchar *strR,const tchar *strE) const;
        virtual void PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2) const;
        virtual void PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code) const;

        virtual void flush();
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
