//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BIOMEMORYSTREAM_H
#define __OMEGA_COMMON_BIOMEMORYSTREAM_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "common/inc/BIOBufferedStream.h"
#include <QMutex>
#include <QList>
#include <QPair>
#include <QThread>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BIOMemoryStream : public BIOStream
{
    public:
        BIOMemoryStream();
        virtual ~BIOMemoryStream();
        
        virtual bool open(const tchar *name);
        virtual bool open(const BString& name);
        virtual bool open(const QString& name);
        
        virtual bool close();
        
        virtual tint read(tbyte *mem,tint len);
        virtual tint read(tubyte *mem,tint len);

        virtual tint write(const tbyte *mem,tint len);
        virtual tint write(const tubyte *mem,tint len);

        virtual bool seek64(tint64 pos,BIOStreamPosition flag);

        virtual bool sof();
        virtual bool eof();

        virtual tint64 size64();
        
        virtual bool readable() const;
        virtual bool writeable() const;
        
        virtual tint64 offset64() const;
        
        virtual tint bookmark(tint offset);
        
    protected:
    
        QMutex m_memMutex;
    
        tuchar *m_memBuffer;
        tuchar *m_readArray;
        
        tint64 m_fileLength;
        tint64 m_cPosition;        
        
        t_thread m_thread;
        QList<QPair<tint64,tint64> > m_loaderCmd;
        volatile bool m_errorFlag;
        volatile bool m_runningFlag;
        
        BIOBufferedStream *m_fileStream;
        
#if defined(OMEGA_WIN32)
        static DWORD WINAPI main(LPVOID arg);
#else
        static void *main(void *arg);
#endif
        void loader();
        bool isLoaded(tint64 pos,tint64 len) const;
        void postCommand(tint64 s,tint64 e);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
