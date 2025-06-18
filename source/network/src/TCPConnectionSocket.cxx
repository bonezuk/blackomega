#include "network/inc/TCPConnectionSocket.h"
#include "network/inc/TCPService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

#ifdef _DEBUG
const tint c_ConnectionTimeout = 1000;
#else
const tint c_ConnectionTimeout = 3 * 60; // 3 minute timeout period
#endif

//-------------------------------------------------------------------------------------------

QList<MemoryPacket *> TCPConnectionSocket::m_freePackets;

//-------------------------------------------------------------------------------------------

TCPConnectionSocket::TCPConnectionSocket(Service *svr,QObject *parent) : Socket(svr,parent),
    m_host(),
    m_port(0),
    m_sQueue(),
    m_rQueue(),
    m_timerFlag(false),
    m_time(0),
    m_errorFlag(false),
    m_lock(false)
{
    if(m_service!=0)
    {
        m_service->addConnection(this);
    }
}

//-------------------------------------------------------------------------------------------

TCPConnectionSocket::~TCPConnectionSocket()
{
    try
    {
        if(m_service!=0)
        {
            m_service->delConnection(this);
        }
        TCPConnectionSocket::close();
        while(m_sQueue.size() > 0)
        {
            TCPConnectionSocket::freeIOPacket(m_sQueue,0);
        }
        while(m_rQueue.size() > 0)
        {
            TCPConnectionSocket::freeIOPacket(m_rQueue,0);
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------


void TCPConnectionSocket::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("TCPConnectionSocket",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("TCPConnectionSocket",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::lock()
{
    m_lock = true;
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::unlock()
{
    m_lock = false;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::isLocked()
{
    return m_lock;
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::close()
{
    if(m_socket!=c_invalidSocket)
    {
        Q_EMIT onStop();
    }

    while(m_sQueue.size()>0)
    {
        freeIOPacket(m_sQueue,0);
    }
    while(m_rQueue.size()>0)
    {
        freeIOPacket(m_rQueue,0);
    }

    if(m_socket!=c_invalidSocket)
    {
        closeSocket(m_socket);
        m_socket = c_invalidSocket;
    }
    m_state = 0;
}

//-------------------------------------------------------------------------------------------

MemoryPacket *TCPConnectionSocket::newIOPacket()
{
    MemoryPacket *item;

    if(m_freePackets.size() > 0)
    {
        item = m_freePackets.takeFirst();
    }
    else
    {
        item = new MemoryPacket;
    }
    return item;
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::freeIOPacket(QList<MemoryPacket *>& queue,tint i)
{
    MemoryPacket *item;

    if(i < queue.size())
    {
        item = queue.takeAt(i);
        item->reset();
        m_freePackets.append(item);
    }
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::canRead() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::canWrite() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::doRead()
{
    tint len = 0,amount = 0;
    MemoryPacket *item;
    tbyte *mem;
    bool res = true;

    if(!(m_state & c_socketStateRead))
    {
        return (m_state & c_socketStateClose) ? false : true;
    }

    if(m_rQueue.size() > 0)
    {
        item = m_rQueue.last();
    }
    else
    {
        item = newIOPacket();
        m_rQueue.append(item);
    }

    while(res)
    {
        if(item->writeRemaining() <= 0)
        {
            item = newIOPacket();
            m_rQueue.append(item);
        }

        mem = item->memoryToWrite(len);
        if(mem!=0)
        {
#if defined(OMEGA_WIN32)
            amount = ::recv(m_socket,mem,len,0);
            if(amount==SOCKET_ERROR)
            {
                tint err = ::WSAGetLastError();
                if(err==WSAEWOULDBLOCK)
                {
                    m_state ^= c_socketStateRead;
                    return true;
                }
                else
                {
                    printError("doRead","Error in recieving data from socket",err);
                    m_errorFlag = true;
                    res = false;
                }
            }
#elif defined(OMEGA_POSIX)
            amount = static_cast<tint>(::read(m_socket,mem,static_cast<size_t>(len)));
            if(amount==-1)
            {
                if(errno==EAGAIN)
                {
                    m_state ^= c_socketStateRead;
                    return true;
                }
                else if(errno!=EINTR)
                {
                    printError("doRead","Error in recieving data from socket",errno);
                    m_errorFlag = true;
                    res = false;
                }
            }
#endif
            else if(amount==0)
            {
                m_state ^= c_socketStateRead;
                m_state |= c_socketStateClose;
                return true;
            }
            else
            {
                item->writeIncrement(amount);
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::doWrite()
{
    tint amount,len = 0;
    MemoryPacket *item;
    tbyte *mem;

    if(!(m_state & c_socketStateWrite))
    {
        return true;
    }
    if(m_sQueue.size() <= 0)
    {
        return true;
    }
    if(m_state & c_socketStateClose)
    {
        return false;
    }

    while(m_sQueue.size()>0)
    {
        item = m_sQueue.first();
        if(item->readRemaining()>0)
        {
            mem = item->memoryToRead(len);
            if(mem!=0)
            {
#if defined(OMEGA_WIN32)
                amount = ::send(m_socket,mem,len,0);
                if(amount==SOCKET_ERROR)
                {
                    tint err = ::WSAGetLastError();
                    if(err==WSAEWOULDBLOCK)
                    {
                        m_state ^= c_socketStateWrite;
                        return true;
                    }
                    else
                    {
                        printError("doWrite","Error while sending data to socket",err);
                        m_errorFlag = true;
                        return false;
                    }
                }
#elif defined(OMEGA_POSIX)
                amount = static_cast<tint>(::write(m_socket,mem,static_cast<size_t>(len)));
                if(amount==-1)
                {
                    if(errno==EAGAIN)
                    {
                        m_state ^= c_socketStateWrite;
                        return true;
                    }
                    else if(errno!=EINTR)
                    {
                        printError("doWrite","Error while sending data to socket",errno);
                        m_errorFlag = true;
                        return false;
                    }
                }
#endif
                else
                {
                    item->readIncrement(amount);
                }
            }
        }
        else
        {
            freeIOPacket(m_sQueue,0);
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::write(const tbyte *mem,tint len)
{
    MemoryPacket *item;
    tint amount,total = 0;

    if(mem==0 || len <=0)
    {
        printError("send","No buffer given to send");
        return false;
    }

    if(m_sQueue.size()>0)
    {
        item = m_sQueue.last();
    }
    else
    {
        item = newIOPacket();
        m_sQueue.append(item);
    }

    while(total < len)
    {
        if(item->writeRemaining()<=0)
        {
            item = newIOPacket();
            m_sQueue.append(item);
        }
        amount = item->write(&mem[total],len - total);
        if(amount==-1)
        {
            printError("send","Failed to write send buffer to output queue");
            return false;
        }
        total += amount;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::write(const tubyte *mem,tint len)
{
    return write(reinterpret_cast<const tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint TCPConnectionSocket::read(tbyte *mem,tint len)
{
    tint amount,total=0;
    MemoryPacket *item;

    if(mem==0 || len<=0)
    {
        printError("read","No memory buffer given");
        return -1;
    }

    while(total<len && m_rQueue.size()>0)
    {
        item = m_rQueue.first();
        if(item->readRemaining()>0)
        {
            amount = item->read(&mem[total],len - total);
            if(amount==-1)
            {
                printError("read","Error in reading data from packet");
            }
            else
            {
                total += amount;
            }
        }
        else
        {
            freeIOPacket(m_rQueue,0);
        }
    }
    return total;
}

//-------------------------------------------------------------------------------------------

tint TCPConnectionSocket::read(tubyte *mem,tint len)
{
    return read(reinterpret_cast<tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint TCPConnectionSocket::available()
{
    tint i,len = 0;

    for(i=0;i<m_rQueue.size();++i)
    {
        len += m_rQueue.at(i)->readRemaining();
    }
    return len;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::canRead(tint len)
{
    tint i,j;
    MemoryPacket *item;
    bool res = false;

    if(m_socket!=c_invalidSocket && !(m_state & c_socketStateClose))
    {
        for(i=0,j=0;j<len && i<m_rQueue.size();++i)
        {
            item = m_rQueue.at(i);
            if(item->readRemaining() > 0)
            {
                j += item->readRemaining();
            }
            else
            {
                freeIOPacket(m_rQueue,i);
                i--;
            }
        }
        res = (j>=len) ? true : false;
    }
    else
    {
        for(i=0;!res && i<m_rQueue.size();++i)
        {
            item = m_rQueue.at(i);
            if(item->readRemaining() > 0)
            {
                res = true;
            }
            else
            {
                freeIOPacket(m_rQueue,i);
                i--;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::canGetNextLine()
{
    tint i=-1,j,amount;
    bool res = false;
    MemoryPacket *item = 0;
    tbyte mem[128];

    while(!res && i<m_rQueue.size())
    {
        if(item==0)
        {
            ++i;
            if(i>=m_rQueue.size())
            {
                break;
            }
            item = m_rQueue.at(i);
            item->transaction();
        }
        amount = item->read(mem,128);
        for(j=0;!res && j<amount;++j)
        {
            if(mem[j]=='\n')
            {
                res = true;
            }
        }
        if(item->readRemaining()<=0)
        {
            item = 0;
        }
    }
    while(i>=0)
    {
        if(i < m_rQueue.size())
        {
            item = m_rQueue.at(i);
            if(item!=0)
            {
                item->rollback();
            }
        }
        i--;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::getNextLine(QString& line)
{
    common::BString s;
    bool res = getNextLine(s);

    if(res)
    {
        line = QString::fromUtf8(static_cast<const tchar *>(s));
    }
    else
    {
        line.clear();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::getNextLine(common::BString& line)
{
    tint i,j,amount;
    MemoryPacket *item = 0;
    tbyte mem[33];
    bool res = false;

    line = "";

    while(!res && m_rQueue.size()>0)
    {
        if(item==0)
        {
            item = m_rQueue.first();
        }
        if(item->readRemaining() > 0)
        {
            item->transaction();
            amount = item->read(mem,32);
            for(i=0,j=0;i<amount && !res;++i)
            {
                if(mem[i]=='\r')
                {
                    mem[i] = '\0';
                    line += &mem[j];
                    j = i + 1;
                }
                if(mem[i]=='\n')
                {
                    mem[i] = '\0';
                    res = true;
                }
            }
            mem[i] = '\0';

            line += &mem[j];

            if(res)
            {
                item->commit(i);
            }
            else
            {
                item->commit();
            }
        }
        else
        {
            freeIOPacket(m_rQueue,0);
            item = 0;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::setForRead()
{
    if(m_timerFlag)
    {
        ::time(&m_time);
    }
    m_state |= c_socketStateRead;
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::setForWrite()
{
    if(m_timerFlag)
    {
        ::time(&m_time);
    }
    m_state |= c_socketStateWrite;
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::timerOn()
{
    ::time(&m_time);
    m_timerFlag = true;
}

//-------------------------------------------------------------------------------------------

void TCPConnectionSocket::timerOff()
{
    m_timerFlag = false;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::timeout()
{
    time_t t;

    if(!m_timerFlag)
    {
        return false;
    }
    ::time(&t);
    return timeout(t);
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::timeout(time_t t)
{
    if(!m_timerFlag)
    {
        return false;
    }
    return (t < (m_time + c_ConnectionTimeout)) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::isTimer() const
{
    return m_timerFlag;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::process()
{
    if((m_state & c_socketStateClose) || m_errorFlag)
    {
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

tint TCPConnectionSocket::writeQueueSize()
{
    tint amount = 0;
    QList<MemoryPacket *>::iterator ppI;

    for(ppI=m_sQueue.begin();ppI!=m_sQueue.end();ppI++)
    {
        MemoryPacket *item = *ppI;
        amount += item->readRemaining();
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

bool TCPConnectionSocket::isError() const
{
    return m_errorFlag;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
