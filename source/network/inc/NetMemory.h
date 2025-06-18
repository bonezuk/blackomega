//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_MEMORY_H
#define __OMEGA_NETWORK_MEMORY_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include <QtGlobal>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT NetMemory
{
    public:
        NetMemory();
        ~NetMemory();
        
        static tuint16 toShort(NetArraySPtr mem,tint offset);
        static tuint16 toShort(const NetArray& mem,tint offset);
        static tuint16 toShort(const tbyte *x,tint offset = 0);
        static tuint16 toShort(const tubyte *x,tint offset = 0);
        static void fromShort(NetArraySPtr mem,tint offset,tuint16 d);
        static void fromShort(NetArray& mem,tint offset,tuint16 d);
        static void fromShort(tbyte *x,tint offset,tuint16 d);
        static void fromShort(tubyte *x,tint offset,tuint16 d);
        
        static tuint32 toInt(NetArraySPtr mem,tint offset);
        static tuint32 toInt(const NetArray& mem,tint offset);
        static tuint32 toInt(const tbyte *x,tint offset = 0);
        static tuint32 toInt(const tubyte *x,tint offset = 0);
        static void fromInt(NetArraySPtr mem,tint offset,tuint32 d);
        static void fromInt(NetArray& mem,tint offset,tuint32 d);
        static void fromInt(tbyte *x,tint offset,tuint32 d);
        static void fromInt(tubyte *x,tint offset,tuint32 d);
        
        static tuint64 toLong(NetArraySPtr mem,tint offset);
        static tuint64 toLong(const NetArray& mem,tint offset);
        static tuint64 toLong(const tbyte *x,tint offset = 0);
        static tuint64 toLong(const tubyte *x,tint offset = 0);
        static void fromLong(NetArraySPtr mem,tint offset,tuint64 d);
        static void fromLong(NetArray& mem,tint offset,tuint64 d);
        static void fromLong(tbyte *x,tint offset,tuint64 d);
        static void fromLong(tubyte *x,tint offset,tuint64 d);
        
        static tuint32 toInt24(NetArraySPtr mem,tint offset);
        static tuint32 toInt24(const NetArray& mem,tint offset);
        static tuint32 toInt24(const tbyte *x,tint offset = 0);
        static tuint32 toInt24(const tubyte *x,tint offset = 0);
        static void fromInt24(NetArraySPtr mem,tint offset,tuint32 d);
        static void fromInt24(NetArray& mem,tint offset,tuint32 d);
        static void fromInt24(tbyte *x,tint offset,tuint32 d);
        static void fromInt24(tubyte *x,tint offset,tuint32 d);
        
        static tint32 toSInt24(NetArraySPtr mem,tint offset);
        static tint32 toSInt24(const NetArray& mem,tint offset);
        static tint32 toSInt24(const tbyte *x,tint offset = 0);
        static tint32 toSInt24(const tubyte *x,tint offset = 0);
        static void fromSInt24(NetArraySPtr mem,tint offset,tint32 d);
        static void fromSInt24(NetArray& mem,tint offset,tint32 d);
        static void fromSInt24(tbyte *x,tint offset,tint32 d);
        static void fromSInt24(tubyte *x,tint offset,tint32 d);
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

