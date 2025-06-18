//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_QBIOSTREAMDEVICE_H
#define __OMEGA_COMMON_QBIOSTREAMDEVICE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include <QIODevice>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT QBIOStreamDevice : public QIODevice
{
    public:
        QBIOStreamDevice(BIOStream *ioStream);
        virtual ~QBIOStreamDevice();
        
        virtual bool open(OpenMode mode);
        virtual void close();
        
        virtual bool isSequential() const;
        virtual bool atEnd() const;
        
        virtual qint64 size() const;
        virtual qint64 pos() const;
        
        virtual bool seek(qint64 pos);
        
    protected:
    
        BIOStream *m_ioStream;
    
        virtual qint64 readData(char *data,qint64 maxSize);
        virtual qint64 writeData(const char *data,qint64 maxSize);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
