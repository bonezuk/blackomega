#include "common/inc/QBIOStreamDevice.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QBIOStreamDevice::QBIOStreamDevice(BIOStream *ioStream) : QIODevice(),
    m_ioStream(ioStream)
{}

//-------------------------------------------------------------------------------------------

QBIOStreamDevice::~QBIOStreamDevice()
{}

//-------------------------------------------------------------------------------------------

qint64 QBIOStreamDevice::readData(char *data,qint64 maxSize)
{
    return static_cast<qint64>(m_ioStream->read(reinterpret_cast<tint8 *>(data),static_cast<tint>(maxSize)));
}

//-------------------------------------------------------------------------------------------

qint64 QBIOStreamDevice::writeData(const char *data,qint64 maxSize)
{
    return static_cast<qint64>(m_ioStream->write(reinterpret_cast<const tint8 *>(data),static_cast<tint>(maxSize)));
}

//-------------------------------------------------------------------------------------------

qint64 QBIOStreamDevice::size() const
{
    return static_cast<qint64>(m_ioStream->size());
}

//-------------------------------------------------------------------------------------------

bool QBIOStreamDevice::isSequential() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool QBIOStreamDevice::atEnd() const
{
    return m_ioStream->eof();
}

//-------------------------------------------------------------------------------------------

bool QBIOStreamDevice::open(OpenMode mode)
{
    setOpenMode(mode);
    return true;
}

//-------------------------------------------------------------------------------------------

void QBIOStreamDevice::close()
{
    m_ioStream->close();
    setOpenMode(QIODevice::NotOpen);
}

//-------------------------------------------------------------------------------------------

qint64 QBIOStreamDevice::pos() const
{
    return static_cast<qint64>(m_ioStream->offset());
}

//-------------------------------------------------------------------------------------------

bool QBIOStreamDevice::seek(qint64 pos)
{
    return m_ioStream->seek(static_cast<tint>(pos),common::e_Seek_Start);
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
