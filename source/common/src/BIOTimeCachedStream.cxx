#include "common/inc/BIOTimeCachedStream.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// BIOTimeCachedStreamSettings
//-------------------------------------------------------------------------------------------

QSharedPointer<BIOTimeCachedStreamSettings> BIOTimeCachedStreamSettings::m_instance;

//-------------------------------------------------------------------------------------------

BIOTimeCachedStreamSettings::BIOTimeCachedStreamSettings()
{}

//-------------------------------------------------------------------------------------------

BIOTimeCachedStreamSettings::~BIOTimeCachedStreamSettings()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<BIOTimeCachedStreamSettings> BIOTimeCachedStreamSettings::instance()
{
    if(m_instance.isNull())
    {
        QSharedPointer<BIOTimeCachedStreamSettings> nInstance(new BIOTimeCachedStreamSettings);
        m_instance = nInstance;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamSettings::release()
{
    m_instance.clear();
}

//-------------------------------------------------------------------------------------------

common::TimeStamp BIOTimeCachedStreamSettings::getCacheTimeLength()
{
    common::TimeStamp len;
    QSettings settings;

    settings.beginGroup("buffer");
    if(settings.contains("lengthCache"))
    {
        len = settings.value("lengthCache",QVariant(5.0)).toDouble();
    }
    else
    {
        len = 5.0;
    }
    settings.endGroup();
    return len;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamSettings::setCacheTimeLength(const common::TimeStamp& t)
{
    QSettings settings;

    settings.beginGroup("buffer");
    settings.setValue("lengthCache",QVariant(static_cast<tfloat64>(t)));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

common::TimeStamp BIOTimeCachedStreamSettings::getBufferTimeLength()
{
    common::TimeStamp len;
    QSettings settings;

    settings.beginGroup("buffer");
    if(settings.contains("lengthBuffer"))
    {
        len = settings.value("lengthBuffer",QVariant(5.0)).toDouble();
    }
    else
    {
        len = 5.0;
    }
    settings.endGroup();
    return len;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamSettings::setBufferTimeLength(const common::TimeStamp& t)
{
    QSettings settings;

    settings.beginGroup("buffer");
    settings.setValue("lengthBuffer",QVariant(static_cast<tfloat64>(t)));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------
// BIOTimeCachedStream
//-------------------------------------------------------------------------------------------

BIOTimeCachedStream::BIOTimeCachedStream() : BIOStream(e_BIOStream_FileRead),
    m_fileCached(0),
    m_bitrate(0),
    m_cachedFrom(0),
    m_lastCleanPosition(-1),
    m_cacheL2(0),
    m_cacheL2Length(0),
    m_cacheL2Size(CachedFileStream::c_blockSize)
{
    m_bufferTimeLength = BIOTimeCachedStreamSettings::instance()->getBufferTimeLength();
    m_cachedTimeLength = BIOTimeCachedStreamSettings::instance()->getCacheTimeLength();
}

//-------------------------------------------------------------------------------------------

BIOTimeCachedStream::~BIOTimeCachedStream()
{
    BIOTimeCachedStream::close();
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStream::PrintError(const tchar *strR,const tchar *strE) const
{
    Log::g_Log << "BIOTimeCachedStream::" << strR << " - " << strE << c_endl;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStream::PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2) const
{
    BIOStream::PrintError(strR,strE1,strE2);
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStream::PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code) const
{
    BIOStream::PrintError(strR,strE1,strE2,code);
}

//-------------------------------------------------------------------------------------------

const QString& BIOTimeCachedStream::name() const
{
    if(m_fileCached!=0)
    {
        return m_fileCached->file()->name();
    }
    else
    {
        return BIOStream::name();
    }
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::open(const tchar *name)
{
    QString n = QString::fromUtf8(name);
    return open(n);
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::open(const BString& name)
{
    QString n = QString::fromUtf8(name.GetString());
    return open(n);
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::open(const QString& name)
{
    bool res = false;

    close();

    m_fileCached = new CachedFileStream;

#if defined(OMEGA_MAC_STORE)
    m_fileCached->setCheckOutFlag(m_checkOutFlag);
#endif

    if(m_fileCached->open(name))
    {
        if(m_fileCached->cache(0,initialCacheSize()) >= 0)
        {
            allocateL2Cache();
            res = true;
        }
        else
        {
            QString err = "Error creating initial cache request on file '" + name + "'";
            PrintError("open",err.toUtf8().constData());
        }
    }
    else
    {
        QString err = "Failed to open cached file '" + name + "'";
        PrintError("open",err.toUtf8().constData());
    }
    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::close()
{
    freeL2Cache();

    if(m_fileCached != 0)
    {
        m_fileCached->close();
        delete m_fileCached;
        m_fileCached = 0;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStream::allocateL2Cache()
{
    freeL2Cache();
    m_cacheL2Size = calculateL2CacheSize();
    m_cacheL2 = new tchar [m_cacheL2Size];
    m_cacheL2Length = 0;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStream::freeL2Cache()
{
    if(m_cacheL2!=0)
    {
        delete [] m_cacheL2;
        m_cacheL2 = 0;
        m_cacheL2Length = 0;
    }
}

//-------------------------------------------------------------------------------------------
// The L2 cache size should be smaller than the current cache size of the large cache held
// and managed in the CachedFileStream. However it should not be so small as to continually
// make requests to the CachedFileStream instance when processing data in the local region
// of a file.
// The L2 cache size is the nearest of multiple of CachedFileStream::c_blockSize:
// - That holds 0.2s audio data, given bitrate is set.
// - 10% of initial cache size, given bitrate is not set.
//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStream::calculateL2CacheSize() const
{
    tfloat64 sizeL2;

    if(m_bitrate > 0)
    {
        tfloat64 t = (static_cast<tfloat64>(m_bitrate) * 0.025) / (static_cast<tfloat64>(CachedFileStream::c_blockSize));
        sizeL2 = static_cast<tint>(round(t));
    }
    else
    {
        tfloat64 t = static_cast<tfloat64>(initialCacheSize()) * 0.1 / (static_cast<tfloat64>(CachedFileStream::c_blockSize));
        sizeL2 = static_cast<tint>(round(t));
    }
    if(sizeL2 <= 0)
    {
        sizeL2 = 1;
    }
    sizeL2 *= CachedFileStream::c_blockSize;
    return sizeL2;
}

//-------------------------------------------------------------------------------------------

CachedFileStream *BIOTimeCachedStream::getCachedFile()
{
    return m_fileCached;
}

//-------------------------------------------------------------------------------------------

const CachedFileStream *BIOTimeCachedStream::getCachedFileConst() const
{
    return m_fileCached;
}

//-------------------------------------------------------------------------------------------
// Initially cache 5% of the file size or 256kB, whichever is smaller.
//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStream::initialCacheSize() const
{
    const tint64 c_initialSizeThreshold = 1024 * 256; // 256kB
    tint64 fSize = getCachedFileConst()->size();
    tint64 fivePercentSize = (fSize * 5) / 100;
    return (fivePercentSize < c_initialSizeThreshold) ? fivePercentSize : c_initialSizeThreshold;
}

//-------------------------------------------------------------------------------------------
// Use the bitrate, that has been set, to calculate the size in bytes for the given time length
//-------------------------------------------------------------------------------------------

tint64 BIOTimeCachedStream::lengthFromTime(const common::TimeStamp& tLen) const
{
    tint64 len;

    if(m_bitrate > 0)
    {
        len = static_cast<tint64>(round(static_cast<tfloat64>(tLen) * static_cast<tfloat64>(m_bitrate) / 8.0));
    }
    else
    {
        len = -1;
    }
    return len;
}

//-------------------------------------------------------------------------------------------

tint64& BIOTimeCachedStream::readPosition()
{
    return m_Position;
}

//-------------------------------------------------------------------------------------------

const tint64& BIOTimeCachedStream::readPositionConst() const
{
    return m_Position;
}

//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStream::read(tbyte *mem,tint len)
{
    tint amount;

    if(eof())
    {
        return 0;
    }

    if(getCachedFile()!=0)
    {
        amount = 0;
        while(amount < len)
        {
            bool readFromCache = false;
            tint offset = offsetL2Cache();

            if(offset < m_cacheL2Length)
            {
                tint l2Index = indexL2Cache();
                tint noBytesStillToRead = len - amount;
                tint noBytesLeftInL2Cache = m_cacheL2Length - offset;
                tint noBytes = (noBytesStillToRead < noBytesLeftInL2Cache) ? noBytesStillToRead : noBytesLeftInL2Cache;
                memcpy(&mem[amount],&m_cacheL2[offset],static_cast<size_t>(noBytes));
                readPosition() += noBytes;
                amount += noBytes;
                if(l2Index != indexL2Cache())
                {
                    readFromCache = true;
                }
            }
            else
            {
                readFromCache = true;
            }

            if(readFromCache)
            {
                if(!eof())
                {
                    tint64 fromPosition = static_cast<tint64>(indexL2Cache()) * static_cast<tint64>(m_cacheL2Size);
                    m_cacheL2Length = getCachedFile()->read(m_cacheL2,fromPosition,m_cacheL2Size);
                    if(m_cacheL2Length >= 0)
                    {
                        tint cacheLen = static_cast<tint>(lengthFromTime(m_cachedTimeLength));
                        if(cacheLen > 0)
                        {
                            if((fromPosition + cacheLen) > getCachedFile()->size())
                            {
                                cacheLen = static_cast<tint>(getCachedFile()->size() - fromPosition);
                            }
                            if(cacheLen > 0)
                            {
                                getCachedFile()->cache(fromPosition,cacheLen);
                            }
                        }
                    }
                    else
                    {
                        QString err = "Error reading from cached file '" + getCachedFile()->file()->name() + "'";
                        PrintError("read",err.toUtf8().constData());
                        return -1;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        PrintError("read","No file open to read from");
        amount = -1;
    }
    return amount;
}


//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStream::read(tubyte *mem,tint len)
{
    return read(reinterpret_cast<tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStream::write(const tbyte *,tint)
{
    return -1;
}

//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStream::write(const tubyte *mem,tint len)
{
    return write(reinterpret_cast<const tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::seek(tint pos,BIOStreamPosition flag)
{
    return seek64(static_cast<tint64>(pos),flag);
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::seek64(tint64 pos,BIOStreamPosition flag)
{
    bool res;

    if(getCachedFile()!=0)
    {
        res = isValidSeek(pos,readPosition(),flag);

        if(res)
        {
            tint oldL2Index = indexL2Cache();

            QPair<tint64,tint64> range = getBufferRange(pos);
            if(isRangeValid(range))
            {
                TimeStamp resetThreshold(1.0);
                tint64 thresholdLen = lengthFromTime(resetThreshold);
                if(!(pos>range.first && (pos+thresholdLen)<range.second))
                {
                    getCachedFile()->clearWithRetention(range.first,range.second - range.first);
                }
            }
            readPosition() = pos;
            m_lastCleanPosition = -1;

            if(oldL2Index != indexL2Cache())
            {
                m_cacheL2Length = 0;
            }
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::sof()
{
    return (readPosition() <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::eof()
{
    return (readPosition() >= size64()) ? true : false;
}

//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStream::size()
{
    return static_cast<tint>(size64());
}

//-------------------------------------------------------------------------------------------

tint64 BIOTimeCachedStream::size64()
{
    return (getCachedFile() != 0) ? getCachedFile()->size() : -1;
}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStream::isRangeValid(const QPair<tint64,tint64>& range) const
{
    return (range.first>=0 && range.second>=0);
}

//-------------------------------------------------------------------------------------------

QPair<tint64,tint64> BIOTimeCachedStream::getBufferRange(tint64 pos) const
{
    common::TimeStamp errorMargin(1.0);
    return getBufferRange(pos,errorMargin);
}

//-------------------------------------------------------------------------------------------

QPair<tint64,tint64> BIOTimeCachedStream::getBufferRange(tint64 pos,const common::TimeStamp& errorMargin) const
{
    tint64 cacheStartPos = -1;
    tint64 cacheEndPos = -1;

    if(m_bitrate>0 && getCachedFileConst()!=0)
    {
        if(pos>=0 && pos<=getCachedFileConst()->size())
        {
            TimeStamp startBufferTime = m_bufferTimeLength + errorMargin;
            cacheStartPos = pos - lengthFromTime(startBufferTime);
            cacheEndPos = pos + lengthFromTime(m_cachedTimeLength);

            if(cacheStartPos < 0)
            {
                cacheStartPos = 0;
            }
            if(cacheEndPos > getCachedFileConst()->size())
            {
                cacheEndPos = getCachedFileConst()->size();
            }
        }
    }
    return QPair<tint64,tint64>(cacheStartPos,cacheEndPos);
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStream::setBitrate(tint rateInBitsPerSecond)
{
    m_bitrate = rateInBitsPerSecond;
    if(getCachedFile()!=0)
    {
        QPair<tint64,tint64> range = getBufferRange(readPosition());
        if(isRangeValid(range))
        {
            getCachedFile()->cache(range.first,range.second - range.first);
        }
        allocateL2Cache();
    }
}

//-------------------------------------------------------------------------------------------
// This removes any old cached data behind the buffer + error margin of cached data.
//-------------------------------------------------------------------------------------------

void BIOTimeCachedStream::springCleanTheCache()
{
    if(getCachedFile()!=0)
    {
        QPair<tint64,tint64> range = getBufferRange(readPosition());
        if(isRangeValid(range))
        {
            if(range.first > 0)
            {
                tint64 cleanFrom;

                if(m_lastCleanPosition > 0)
                {
                    common::TimeStamp tMargin(2.0);
                    QPair<tint64,tint64> fromRange = getBufferRange(m_lastCleanPosition,tMargin);
                    if(isRangeValid(fromRange))
                    {
                        cleanFrom = fromRange.first;
                    }
                    else
                    {
                        cleanFrom = 0;
                    }
                }
                else
                {
                    cleanFrom = 0;
                }

                if(cleanFrom > range.first)
                {
                    cleanFrom = 0;
                }
                getCachedFile()->drop(cleanFrom,range.first - cleanFrom);
            }
            m_lastCleanPosition = readPosition();
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
