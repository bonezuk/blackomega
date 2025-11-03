//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BIOMEMORY_H
#define __OMEGA_COMMON_BIOMEMORY_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BIOMemory : public BIOStream
{
    public:
        BIOMemory(const QByteArray& mem);
        virtual ~BIOMemory();

        virtual bool open(const tchar *name);
        virtual bool open(const BString& name);
        virtual bool open(const QString& name);

        virtual bool close();

        virtual tint read(tbyte *mem,tint len);
        virtual tint read(tubyte *mem,tint len);

        virtual tint write(const tbyte *mem,tint len);
        virtual tint write(const tubyte *mem,tint len);

        virtual bool seek64(tint64 pos,BIOStreamPosition flag);

        virtual tint64 size64();

    protected:

        QByteArray m_array;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
