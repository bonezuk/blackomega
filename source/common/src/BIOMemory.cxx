#include "common/inc/BIOMemory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

BIOMemory::BIOMemory(const QByteArray& mem) : BIOStream(e_BIOStream_FileRead),
    m_array(mem)
{}

//-------------------------------------------------------------------------------------------

BIOMemory::~BIOMemory()
{}

//-------------------------------------------------------------------------------------------

bool BIOMemory::open(const tchar *name)
{
    QString n = QString::fromUtf8(name);
    return open(n);
}

//-------------------------------------------------------------------------------------------

bool BIOMemory::open(const BString& name)
{
    QString n = QString::fromUtf8(name.getString());
    return open(n);
}

//-------------------------------------------------------------------------------------------

bool BIOMemory::open(const QString& name)
{
    bool res;

    if(!m_array.isNull())
    {
        m_Name = name;
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOMemory::close()
{
    m_Name = "";
    return BIOStream::close();
}

//-------------------------------------------------------------------------------------------

tint BIOMemory::read(tbyte *mem,tint len)
{
    tint amount;

    if(!m_array.isNull() && mem!=0)
    {
        if(len > 0)
        {
            tint remain;

            if(m_Position < 0)
            {
                m_Position = 0;
            }
            else if(m_Position > size64())
            {
                m_Position = size64();
            }

            remain = static_cast<tint>(size64() - m_Position);
            amount = (remain < len) ? remain : len;
            if(amount > 0)
            {
                const tbyte *source = reinterpret_cast<const tbyte *>(m_array.constData());
                ::memcpy(mem,&source[m_Position],amount * sizeof(tbyte));
            }
            m_Position += amount;
        }
        else
        {
            amount = 0;
        }
    }
    else
    {
        amount = -1;
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tint BIOMemory::read(tubyte *mem,tint len)
{
    return read(reinterpret_cast<tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint BIOMemory::write(const tbyte *,tint)
{
    return -1;
}

//-------------------------------------------------------------------------------------------

tint BIOMemory::write(const tubyte *mem,tint len)
{
    return write(reinterpret_cast<const tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

bool BIOMemory::seek64(tint64 pos,BIOStreamPosition flag)
{
    tint64 newPos;
    bool res;

    switch(flag)
    {
        case e_Seek_Start:
            newPos = pos;
            break;

        case e_Seek_End:
            newPos = size64() + pos;
            break;

        case e_Seek_Current:
        default:
            newPos = m_Position + pos;
            break;
    }
    if(newPos>=0 && newPos<=size64())
    {
        m_Position = newPos;
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint64 BIOMemory::size64()
{
    return static_cast<tint64>(m_array.size());
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
