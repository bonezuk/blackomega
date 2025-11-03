#include "common/inc/BIOMemoryStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

BIOMemoryStream::BIOMemoryStream() : BIOStream(e_BIOStream_FileRead),
    m_memMutex(),
    m_memBuffer(0),
    m_readArray(0),
    m_fileLength(0),
    m_cPosition(0),
    m_thread(0),
    m_loaderCmd(),
    m_errorFlag(false),
    m_runningFlag(false),
    m_fileStream(0)
{}

//-------------------------------------------------------------------------------------------

BIOMemoryStream::~BIOMemoryStream()
{
    BIOMemoryStream::close();
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::open(const tchar *name)
{
    return open(QString::fromUtf8(name));
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::open(const BString& name)
{
    return open(QString::fromUtf8(static_cast<const tchar *>(name)));
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::open(const QString& name)
{
    bool res = false;

    if(BIOStream::open(name))
    {
        m_fileLength = BIOStream::size64();
        if(m_fileLength>0)
        {
            m_memBuffer = reinterpret_cast<tuchar *>(::malloc(m_fileLength * sizeof(tuchar)));
            if(m_memBuffer!=0)
            {
                tint64 aSize = (m_fileLength >> 19) + 1;
                m_readArray = reinterpret_cast<tuchar *>(::malloc(aSize * sizeof(tuchar)));
                if(m_readArray!=0)
                {
                    ::memset(m_readArray,0,aSize * sizeof(tuchar));
                    m_runningFlag = true;

#if defined(OMEGA_WIN32)
                    DWORD threadID;
                    m_thread = ::CreateThread(0,0,BIOMemoryStream::main,reinterpret_cast<LPVOID>(this),0,&threadID);
#else
                    if(::pthread_create(&m_thread,0,BIOMemoryStream::main,reinterpret_cast<void *>(this))!=0)
                    {
                        m_thread = 0;
                    }
#endif
                    if(m_thread!=0)
                    {
                        res = true;
                    }
                    else
                    {
                        m_runningFlag = false;
                    }
                }
            }
            else
            {
                BIOStream::close();

                const tint c_fixedBufferSize = 1000000;
                tbyte *mBuffer = reinterpret_cast<tbyte *>(malloc(c_fixedBufferSize * sizeof(tbyte)));
                if(mBuffer!=0)
                {
                    m_fileStream = new BIOBufferedStream(e_BIOStream_FileRead,mBuffer,c_fixedBufferSize);
                    res = m_fileStream->open(name);
                }
            }
        }
    }
    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::close()
{
    if(m_fileStream!=0)
    {
        m_fileStream->close();
        delete m_fileStream;
        m_fileStream = 0;
    }
    if(m_thread!=0)
    {
        postCommand(-1,-1);
#if defined(OMEGA_WIN32)
        ::WaitForSingleObject(m_thread,INFINITE);
#else
        ::pthread_join(m_thread,0);
#endif
        m_thread = 0;
    }
    if(m_readArray!=0)
    {
        ::free(m_readArray);
        m_readArray = 0;
    }
    if(m_memBuffer!=0)
    {
        ::free(m_memBuffer);
        m_memBuffer = 0;
    }
    return BIOStream::close();
}

//-------------------------------------------------------------------------------------------

void BIOMemoryStream::postCommand(tint64 s,tint64 e)
{
    QPair<tint64,tint64> nCmd(s,e);
    m_memMutex.lock();
    m_loaderCmd.prepend(nCmd);
    m_memMutex.unlock();
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

DWORD WINAPI BIOMemoryStream::main(LPVOID arg)
{
    if(arg!=0)
    {
        BIOMemoryStream *io = reinterpret_cast<BIOMemoryStream *>(arg);
        io->loader();
    }
    return 0;
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

void *BIOMemoryStream::main(void *arg)
{
    if(arg!=0)
    {
        BIOMemoryStream *io = reinterpret_cast<BIOMemoryStream *>(arg);
        io->loader();
    }
    return 0;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void BIOMemoryStream::loader()
{
    static const unsigned char a[8] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
    static const unsigned char m[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

    tint64 cmdSPos=-1,cmdEPos=-1;
    tint64 cPos = 0;

    while(cPos<m_fileLength && !m_errorFlag)
    {
        tint amount = 1 << 16;
        QPair<tint64,tint64> nR;

        if(cmdSPos>=0)
        {
            nR = QPair<tint64,tint64>(cmdSPos,cmdEPos);
        }
        else
        {
            m_memMutex.lock();
            if(!m_loaderCmd.isEmpty())
            {
                nR = m_loaderCmd.takeLast();
                cmdSPos = nR.first;
                cmdEPos = nR.second;
                if(cmdSPos==-1 && cmdEPos==-1)
                {
                    m_memMutex.unlock();
                    break;
                }
                cmdSPos &= 0xffffffffffff0000ULL;
                if(cmdSPos>=m_fileLength)
                {
                    cmdSPos = -1;
                }
                if(cmdEPos>=m_fileLength)
                {
                    cmdEPos = m_fileLength;
                }
                nR = QPair<tint64,tint64>(cmdSPos,cmdEPos);
            }
            if(cmdSPos==-1)
            {
                nR = QPair<tint64,tint64>(cPos,cPos + (1<<16));
            }
            m_memMutex.unlock();
        }

        if(!isLoaded(nR.first,1))
        {
            tint64 sP = nR.first;

            if(BIOStream::seek64(sP,e_Seek_Start))
            {
                if((sP + amount)>m_fileLength)
                {
                    amount = static_cast<tint>(m_fileLength - sP);
                }
                if(BIOStream::read(reinterpret_cast<tbyte *>(&m_memBuffer[sP]),amount)==amount)
                {
                    tint64 idxA = sP >> 16;
                    tint64 idxB = idxA & 0x0000000000000007ULL;
                    idxA >>= 3;
                    m_readArray[idxA] = (m_readArray[idxA] & a[idxB]) | m[idxB];
                }
                else
                {
                    m_errorFlag = true;
                }
            }
            else
            {
                m_errorFlag = true;
            }
        }
        if(cmdSPos>=0)
        {
            cmdSPos += amount;
            if(cmdSPos>=cmdEPos)
            {
                cmdSPos = -1;
                cmdEPos = -1;
            }
        }
        else
        {
            cPos += amount;
        }
    }
    m_thread = 0;
    m_runningFlag = false;
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::isLoaded(tint64 pos,tint64 len) const
{
    static const tuchar m[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    bool res;

    if(pos>=0 && pos<m_fileLength)
    {
        if((pos+len)>m_fileLength)
        {
            len = m_fileLength - pos;
        }

        tint64 i;
        tint64 sBit = pos >> 16;
        tint64 eBit = (pos + len) >> 16;
        res = true;

        for(i=sBit;i<=eBit && res;i++)
        {
            tint64 idxA = i >> 3;
            tint64 idxB = i & 0x00000007;
            if(!(m_readArray[idxA] & m[idxB]))
            {
                res = false;
            }
        }
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint BIOMemoryStream::read(tbyte *mem,tint len)
{
    return read(reinterpret_cast<tubyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint BIOMemoryStream::read(tubyte *mem,tint len)
{
    tint amount = -1;

    if(m_fileStream!=0)
    {
        return m_fileStream->read(mem,len);
    }

    if(mem!=0)
    {
        if((m_cPosition+len)>m_fileLength)
        {
            len = m_fileLength - m_cPosition;
        }

        if(len>0)
        {
            bool flag = false;

            if(!isLoaded(m_cPosition,m_cPosition+len))
            {
                postCommand(m_cPosition,m_cPosition+len);
                while(m_runningFlag && !flag)
                {
                    if(!isLoaded(m_cPosition,m_cPosition+len))
                    {
#if defined(OMEGA_WIN32)
                        ::Sleep(50);
#else
                        ::usleep(50000);
#endif
                    }
                    else
                    {
                        flag = true;
                    }
                }
                if(!flag && isLoaded(m_cPosition,m_cPosition+len))
                {
                    flag = true;
                }
            }
            else
            {
                flag = true;
            }

            if(flag)
            {
                memcpy(mem,&m_memBuffer[m_cPosition],static_cast<size_t>(len));
                m_cPosition += len;
                amount = len;
            }
        }
        else
        {
            amount = 0;
        }
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tint BIOMemoryStream::write(const tbyte *mem,tint len)
{
    return -1;
}

//-------------------------------------------------------------------------------------------

tint BIOMemoryStream::write(const tubyte *mem,tint len)
{
    return -1;
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::seek64(tint64 pos,BIOStreamPosition flag)
{
    bool res = true;

    if(m_fileStream!=0)
    {
        return m_fileStream->seek64(pos,flag);
    }

    switch(flag)
    {
        case e_Seek_End:
            pos = m_fileLength - pos;

        case e_Seek_Start:
            if(pos>=0 && pos<=static_cast<tint64>(m_fileLength))
            {
                m_cPosition = static_cast<tint>(pos);
            }
            else
            {
                res = false;
            }
            break;

        case e_Seek_Current:
            {
                tint64 nPos = m_cPosition + pos;
                if(nPos>=0 && nPos<=m_fileLength)
                {
                    m_cPosition = nPos;
                }
                else
                {
                    res = false;
                }
            }
            break;

        default:
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::sof()
{
    if(m_fileStream!=0)
    {
        return m_fileStream->sof();
    }
    else
    {
        return (m_cPosition==0);
    }
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::eof()
{
    if(m_fileStream!=0)
    {
        return m_fileStream->eof();
    }
    else
    {
        return (m_cPosition==m_fileLength);
    }
}

//-------------------------------------------------------------------------------------------

tint64 BIOMemoryStream::size64()
{
    if(m_fileStream!=0)
    {
        return m_fileStream->size64();
    }
    else
    {
        return m_fileLength;
    }
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::readable() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool BIOMemoryStream::writeable() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint64 BIOMemoryStream::offset64() const
{
    if(m_fileStream!=0)
    {
        return m_fileStream->offset64();
    }
    else
    {
        return m_cPosition;
    }
}

//-------------------------------------------------------------------------------------------

tint BIOMemoryStream::bookmark(tint offset)
{
    if(m_fileStream==0)
    {
        tint nextID = m_nextBookmarkID;
        tint64 pos = m_cPosition + offset;
        m_Bookmarks.insert(nextID,pos);
        m_nextBookmarkID++;
        return nextID;
    }
    else
    {
        return m_fileStream->bookmark(offset);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
