#include "network/inc/NetMemory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

NetMemory::NetMemory() {}

//-------------------------------------------------------------------------------------------

NetMemory::~NetMemory() {}

//-------------------------------------------------------------------------------------------

tuint16 NetMemory::toShort(NetArraySPtr mem,tint offset)
{
    Q_ASSERT(mem.data()!=0);
    return toShort(*(mem.data()),offset);
}

//-------------------------------------------------------------------------------------------

tuint16 NetMemory::toShort(const NetArray& mem,tint offset)
{
    const tubyte *x = reinterpret_cast<const tubyte *>(mem.GetData());
    tuint16 a;

    Q_ASSERT((offset + 2) <= mem.GetSize());
    x = &x[offset];
    a  =  static_cast<tuint16>(x[0]) & 0x00ff;
    a |= (static_cast<tuint16>(x[1]) << 8) & 0xff00;
    return a;
}

//-------------------------------------------------------------------------------------------

tuint16 NetMemory::toShort(const tbyte *x,tint offset)
{
    return toShort(reinterpret_cast<const tubyte *>(x),offset);
}

//-------------------------------------------------------------------------------------------

tuint16 NetMemory::toShort(const tubyte *x,tint offset)
{
    tuint16 a;

    x = &x[offset];
    a  =  static_cast<tuint16>(x[0]) & 0x00ff;
    a |= (static_cast<tuint16>(x[1]) << 8) & 0xff00;
    return a;
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromShort(NetArraySPtr mem,tint offset,tuint16 d)
{
    Q_ASSERT(mem.data()!=0);
    fromShort(*(mem.data()),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromShort(NetArray& mem,tint offset,tuint16 d)
{
    if(mem.GetSize() < (offset + 2))
    {
        mem.SetSize(offset + 2);
    }

    tubyte *x = reinterpret_cast<tubyte *>(mem.GetData());
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x00ff);
    x[1] = static_cast<tubyte>((d >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromShort(tbyte *x,tint offset,tuint16 d)
{
    fromShort(reinterpret_cast<tubyte *>(x),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromShort(tubyte *x,tint offset,tuint16 d)
{
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x00ff);
    x[1] = static_cast<tubyte>((d >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

tuint32 NetMemory::toInt(NetArraySPtr mem,tint offset)
{
    Q_ASSERT(mem.data()!=0);
    return toInt(*(mem.data()),offset);
}

//-------------------------------------------------------------------------------------------

tuint32 NetMemory::toInt(const NetArray& mem,tint offset)
{
    const tubyte *x = reinterpret_cast<const tubyte *>(mem.GetData());
    tuint32 a;

    Q_ASSERT((offset + 4) <= mem.GetSize());
    x = &x[offset];
    a  =  static_cast<tuint32>(x[0]) & 0x000000ff;
    a |= (static_cast<tuint32>(x[1]) <<  8) & 0x0000ff00;
    a |= (static_cast<tuint32>(x[2]) << 16) & 0x00ff0000;
    a |= (static_cast<tuint32>(x[3]) << 24) & 0xff000000;
    return a;
}

//-------------------------------------------------------------------------------------------

tuint32 NetMemory::toInt(const tbyte *x,tint offset)
{
    return toInt(reinterpret_cast<const tubyte *>(x),offset);
}

//-------------------------------------------------------------------------------------------

tuint32 NetMemory::toInt(const tubyte *x,tint offset)
{
    tuint32 a;

    x = &x[offset];
    a  =  static_cast<tuint32>(x[0]) & 0x000000ff;
    a |= (static_cast<tuint32>(x[1]) <<  8) & 0x0000ff00;
    a |= (static_cast<tuint32>(x[2]) << 16) & 0x00ff0000;
    a |= (static_cast<tuint32>(x[3]) << 24) & 0xff000000;
    return a;
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt(NetArraySPtr mem,tint offset,tuint32 d)
{
    Q_ASSERT(mem.data()!=0);
    fromInt(*(mem.data()),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt(NetArray& mem,tint offset,tuint32 d)
{
    if(mem.GetSize() < (offset + 4))
    {
        mem.SetSize(offset + 4);
    }

    tubyte *x = reinterpret_cast<tubyte *>(mem.GetData());
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x000000ff);
    x[1] = static_cast<tubyte>((d >>  8) & 0x000000ff);
    x[2] = static_cast<tubyte>((d >> 16) & 0x000000ff);
    x[3] = static_cast<tubyte>((d >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt(tbyte *x,tint offset,tuint32 d)
{
    fromInt(reinterpret_cast<tubyte *>(x),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt(tubyte *x,tint offset,tuint32 d)
{
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x000000ff);
    x[1] = static_cast<tubyte>((d >>  8) & 0x000000ff);
    x[2] = static_cast<tubyte>((d >> 16) & 0x000000ff);
    x[3] = static_cast<tubyte>((d >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

tuint64 NetMemory::toLong(NetArraySPtr mem,tint offset)
{
    Q_ASSERT(mem.data()!=0);
    return toLong(*(mem.data()),offset);
}

//-------------------------------------------------------------------------------------------

tuint64 NetMemory::toLong(const NetArray& mem,tint offset)
{
    const tubyte *x = reinterpret_cast<const tubyte *>(mem.GetData());
    tuint64 a;

    Q_ASSERT((offset + 8) <= mem.GetSize());
    x = &x[offset];
    a  =  static_cast<tuint64>(x[0]) & 0x00000000000000ffULL;
    a |= (static_cast<tuint64>(x[1]) <<  8ULL) & 0x000000000000ff00ULL;
    a |= (static_cast<tuint64>(x[2]) << 16ULL) & 0x0000000000ff0000ULL;
    a |= (static_cast<tuint64>(x[3]) << 24ULL) & 0x00000000ff000000ULL;
    a |= (static_cast<tuint64>(x[4]) << 32ULL) & 0x000000ff00000000ULL;
    a |= (static_cast<tuint64>(x[5]) << 40ULL) & 0x0000ff0000000000ULL;
    a |= (static_cast<tuint64>(x[6]) << 48ULL) & 0x00ff000000000000ULL;
    a |= (static_cast<tuint64>(x[7]) << 56ULL) & 0xff00000000000000ULL;
    return a;
}

//-------------------------------------------------------------------------------------------

tuint64 NetMemory::toLong(const tbyte *x,tint offset)
{
    return toLong(reinterpret_cast<const tubyte *>(x),offset);
}

//-------------------------------------------------------------------------------------------

tuint64 NetMemory::toLong(const tubyte *x,tint offset)
{
    tuint64 a;

    x = &x[offset];
    a  =  static_cast<tuint64>(x[0]) & 0x00000000000000ffULL;
    a |= (static_cast<tuint64>(x[1]) <<  8) & 0x000000000000ff00ULL;
    a |= (static_cast<tuint64>(x[2]) << 16) & 0x0000000000ff0000ULL;
    a |= (static_cast<tuint64>(x[3]) << 24) & 0x00000000ff000000ULL;
    a |= (static_cast<tuint64>(x[4]) << 32) & 0x000000ff00000000ULL;
    a |= (static_cast<tuint64>(x[5]) << 40) & 0x0000ff0000000000ULL;
    a |= (static_cast<tuint64>(x[6]) << 48) & 0x00ff000000000000ULL;
    a |= (static_cast<tuint64>(x[7]) << 56) & 0xff00000000000000ULL;
    return a;
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromLong(NetArraySPtr mem,tint offset,tuint64 d)
{
    Q_ASSERT(mem.data()!=0);
    fromLong(*(mem.data()),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromLong(NetArray& mem,tint offset,tuint64 d)
{
    if(mem.GetSize() < (offset + 8))
    {
        mem.SetSize(offset + 8);
    }

    tubyte *x = reinterpret_cast<tubyte *>(mem.GetData());
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x00000000000000ffULL);
    x[1] = static_cast<tubyte>((d >>  8) & 0x00000000000000ffULL);
    x[2] = static_cast<tubyte>((d >> 16) & 0x00000000000000ffULL);
    x[3] = static_cast<tubyte>((d >> 24) & 0x00000000000000ffULL);
    x[4] = static_cast<tubyte>((d >> 32) & 0x00000000000000ffULL);
    x[5] = static_cast<tubyte>((d >> 40) & 0x00000000000000ffULL);
    x[6] = static_cast<tubyte>((d >> 48) & 0x00000000000000ffULL);
    x[7] = static_cast<tubyte>((d >> 56) & 0x00000000000000ffULL);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromLong(tbyte *x,tint offset,tuint64 d)
{
    fromLong(reinterpret_cast<tubyte *>(x),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromLong(tubyte *x,tint offset,tuint64 d)
{
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x00000000000000ffULL);
    x[1] = static_cast<tubyte>((d >>  8) & 0x00000000000000ffULL);
    x[2] = static_cast<tubyte>((d >> 16) & 0x00000000000000ffULL);
    x[3] = static_cast<tubyte>((d >> 24) & 0x00000000000000ffULL);
    x[4] = static_cast<tubyte>((d >> 32) & 0x00000000000000ffULL);
    x[5] = static_cast<tubyte>((d >> 40) & 0x00000000000000ffULL);
    x[6] = static_cast<tubyte>((d >> 48) & 0x00000000000000ffULL);
    x[7] = static_cast<tubyte>((d >> 56) & 0x00000000000000ffULL);
}

//-------------------------------------------------------------------------------------------


tuint32 NetMemory::toInt24(NetArraySPtr mem,tint offset)
{
    Q_ASSERT(mem.data()!=0);
    return toInt24(*(mem.data()),offset);
}

//-------------------------------------------------------------------------------------------

tuint32 NetMemory::toInt24(const NetArray& mem,tint offset)
{
    const tubyte *x = reinterpret_cast<const tubyte *>(mem.GetData());
    tuint32 a;

    Q_ASSERT((offset + 3) <= mem.GetSize());
    x = &x[offset];
    a  =  static_cast<tuint32>(x[0]) & 0x000000ff;
    a |= (static_cast<tuint32>(x[1]) <<  8) & 0x0000ff00;
    a |= (static_cast<tuint32>(x[2]) << 16) & 0x00ff0000;
    return a;
}

//-------------------------------------------------------------------------------------------

tuint32 NetMemory::toInt24(const tbyte *x,tint offset)
{
    return toInt24(reinterpret_cast<const tubyte *>(x),offset);
}

//-------------------------------------------------------------------------------------------

tuint32 NetMemory::toInt24(const tubyte *x,tint offset)
{
    tuint32 a;

    x = &x[offset];
    a  =  static_cast<tuint32>(x[0]) & 0x000000ff;
    a |= (static_cast<tuint32>(x[1]) <<  8) & 0x0000ff00;
    a |= (static_cast<tuint32>(x[2]) << 16) & 0x00ff0000;
    return a;
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt24(NetArraySPtr mem,tint offset,tuint32 d)
{
    Q_ASSERT(mem.data()!=0);
    fromInt24(*(mem.data()),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt24(NetArray& mem,tint offset,tuint32 d)
{
    if(mem.GetSize() < (offset + 3))
    {
        mem.SetSize(offset + 3);
    }

    tubyte *x = reinterpret_cast<tubyte *>(mem.GetData());
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x000000ff);
    x[1] = static_cast<tubyte>((d >>  8) & 0x000000ff);
    x[2] = static_cast<tubyte>((d >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt24(tbyte *x,tint offset,tuint32 d)
{
    fromInt24(reinterpret_cast<tubyte *>(x),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromInt24(tubyte *x,tint offset,tuint32 d)
{
    x = &x[offset];
    x[0] = static_cast<tubyte>(d & 0x000000ff);
    x[1] = static_cast<tubyte>((d >>  8) & 0x000000ff);
    x[2] = static_cast<tubyte>((d >> 16) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

tint32 NetMemory::toSInt24(NetArraySPtr mem,tint offset)
{
    Q_ASSERT(mem.data()!=0);
    return toSInt24(*(mem.data()),offset);
}

//-------------------------------------------------------------------------------------------

tint32 NetMemory::toSInt24(const NetArray& mem,tint offset)
{
    tuint32 a = toInt24(mem,offset);
    tint32 b;

    if(a & 0x008000000)
    {
        a &= 0x007fffff;
        b = static_cast<tint32>(a);
        b = -b;
    }
    else
    {
        b = static_cast<tint32>(a);
    }
    return b;
}

//-------------------------------------------------------------------------------------------

tint32 NetMemory::toSInt24(const tbyte *x,tint offset)
{
    return toSInt24(reinterpret_cast<const tubyte *>(x),offset);
}

//-------------------------------------------------------------------------------------------

tint32 NetMemory::toSInt24(const tubyte *x,tint offset)
{
    tuint32 a = toInt24(x,offset);
    tint32 b;

    if(a & 0x008000000)
    {
        a &= 0x007fffff;
        b = static_cast<tint32>(a);
        b = -b;
    }
    else
    {
        b = static_cast<tint32>(a);
    }
    return b;
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromSInt24(NetArraySPtr mem,tint offset,tint32 d)
{
    Q_ASSERT(mem.data()!=0);
    return fromSInt24(*(mem.data()),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromSInt24(NetArray& mem,tint offset,tint32 d)
{
    tuint32 a;

    if(d>0)
    {
        a = static_cast<tuint32>(d);
    }
    else
    {
        a = (static_cast<tuint32>(d) & 0x007fffff) | 0x00800000;
    }
    fromInt24(mem,offset,a);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromSInt24(tbyte *x,tint offset,tint32 d)
{
    fromSInt24(reinterpret_cast<tubyte *>(x),offset,d);
}

//-------------------------------------------------------------------------------------------

void NetMemory::fromSInt24(tubyte *x,tint offset,tint32 d)
{
    tuint32 a;

    if(d>0)
    {
        a = static_cast<tuint32>(d);
    }
    else
    {
        a = (static_cast<tuint32>(d) & 0x007fffff) | 0x00800000;
    }
    fromInt24(x,offset,a);
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
