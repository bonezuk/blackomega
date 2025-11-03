#include "remote/inc/WinLIRCClient.h"
#include "remote/inc/WinLIRCSettings.h"
#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------
// WinLIRCClient::LIRCCommand
//-------------------------------------------------------------------------------------------

WinLIRCClient::LIRCCommand::LIRCCommand() : m_command(WinLIRCClient::LIRCCommand::e_UnknownCommand),
    m_host(),
    m_port(8765)
{}

//-------------------------------------------------------------------------------------------

WinLIRCClient::LIRCCommand::LIRCCommand(const LIRCCommand& rhs) : m_command(WinLIRCClient::LIRCCommand::e_UnknownCommand),
    m_host(),
    m_port(8765)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

WinLIRCClient::LIRCCommand::~LIRCCommand()
{}

//-------------------------------------------------------------------------------------------

const WinLIRCClient::LIRCCommand& WinLIRCClient::LIRCCommand::operator = (const LIRCCommand& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------


void WinLIRCClient::LIRCCommand::copy(const LIRCCommand& rhs)
{
    command() = rhs.command();
    host() = rhs.host();
    port() = rhs.port();
}

//-------------------------------------------------------------------------------------------

WinLIRCClient::LIRCCommand::Command& WinLIRCClient::LIRCCommand::command()
{
    return m_command;
}

//-------------------------------------------------------------------------------------------

const WinLIRCClient::LIRCCommand::Command& WinLIRCClient::LIRCCommand::command() const
{
    return m_command;
}

//-------------------------------------------------------------------------------------------

QString& WinLIRCClient::LIRCCommand::host()
{
    return m_host;
}

//-------------------------------------------------------------------------------------------

const QString& WinLIRCClient::LIRCCommand::host() const
{
    return m_host;
}

//-------------------------------------------------------------------------------------------

int& WinLIRCClient::LIRCCommand::port()
{
    return m_port;
}

//-------------------------------------------------------------------------------------------

const int& WinLIRCClient::LIRCCommand::port() const
{
    return m_port;
}

//-------------------------------------------------------------------------------------------
// WinLIRCClient
//-------------------------------------------------------------------------------------------

WinLIRCClient::WinLIRCClient(network::Service *svr,QObject *parent) : network::TCPConnClientSocket(svr,parent),
    m_processState(0),
    m_processQueueMutex(),
    m_processQueue()
{
    setConnectReferenceTime();
}

//-------------------------------------------------------------------------------------------

WinLIRCClient::~WinLIRCClient()
{}

//-------------------------------------------------------------------------------------------

bool WinLIRCClient::process()
{
    bool res = true;
    LIRCCommand cmd = getNextCommand();

    if(cmd.command()!=LIRCCommand::e_ShutdownClient)
    {
        if(getProcessState()==0)
        {
            processNotConnected(cmd);
        }
        else
        {
            processConnected();
        }
    }
    else
    {
        emitRemoteStatus("LIRC Service is shutting down",0);
        if(getSocket()!=network::c_invalidSocket)
        {
            close();
        }
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::processNotConnected(const LIRCCommand& cmd)
{
    if(getSocket()!=network::c_invalidSocket)
    {
        emitRemoteStatus("Closing connection to LIRC server",1);
        close();
    }

    if(cmd.command()==LIRCCommand::e_OpenConnection)
    {
        QString connectionText = "Trying to connect to " + cmd.host().toLower().trimmed() + " on port " + QString::number(cmd.port());
        emitRemoteStatus(connectionText,1);

        if(open(cmd.host(),cmd.port()))
        {
            emitRemoteStatus("Connected to LIRC server",2);
            setProcessState(1);
        }
        else
        {
            close();
            connectionText = "Failed to connect to " + cmd.host().toLower().trimmed() + " on port " + QString::number(cmd.port());
            emitRemoteStatus(connectionText,0);
            setConnectReferenceTime();
        }
    }
    else
    {
        common::TimeStamp retryT = getConnectReferenceTime() + 60.0;

        if(retryT < getCurrentReferenceTime())
        {
            if(open(WinLIRCSettings::instance().host(),WinLIRCSettings::instance().port()))
            {
                emitRemoteStatus("Connected to LIRC server",2);
                setProcessState(1);
            }
            else
            {
                emitRemoteStatus("No connection to LIRC server",0);
                setConnectReferenceTime();
            }
        }
        else
        {
            emitRemoteStatus("No connection to LIRC server",0);
        }
    }
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::processConnected()
{
    if(connectSocketProcess())
    {
        while(canGetNextLine())
        {
            QString cmd;

            if(getNextLine(cmd))
            {
                processLine(cmd);
            }
        }
    }
    else
    {
        close();
        setProcessState(0);
        emitRemoteStatus("Connection to LIRC server has been closed",0);
        setConnectReferenceTime();
    }
}

//-------------------------------------------------------------------------------------------

bool WinLIRCClient::connectSocketProcess()
{
    return network::TCPConnectionSocket::process();
}

//-------------------------------------------------------------------------------------------

common::TimeStamp WinLIRCClient::getCurrentReferenceTime()
{
    return common::TimeStamp::reference();
}

//-------------------------------------------------------------------------------------------

common::TimeStamp WinLIRCClient::getConnectReferenceTime()
{
    return m_connectReferenceTime;
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::setConnectReferenceTime()
{
    m_connectReferenceTime = getCurrentReferenceTime();
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::lockProcessQueue()
{
    m_processQueueMutex.lock();
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::unlockProcessQueue()
{
    m_processQueueMutex.unlock();
}

//-------------------------------------------------------------------------------------------

QList<WinLIRCClient::LIRCCommand>& WinLIRCClient::getProcessQueue()
{
    return m_processQueue;
}

//-------------------------------------------------------------------------------------------

int WinLIRCClient::getProcessState() const
{
    return m_processState;
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::setProcessState(int nState)
{
    m_processState = nState;
}

//-------------------------------------------------------------------------------------------

WinLIRCClient::LIRCCommand WinLIRCClient::getNextCommand()
{
    LIRCCommand cmd;

    lockProcessQueue();
    if(!getProcessQueue().isEmpty())
    {
        cmd = getProcessQueue().takeFirst();
    }
    else
    {
        cmd.command() = LIRCCommand::e_UnknownCommand;
    }
    unlockProcessQueue();
    return cmd;
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::queueProcessCommand(const LIRCCommand& cmd)
{
    lockProcessQueue();
    getProcessQueue().append(cmd);
    unlockProcessQueue();
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::emitRemoteCommand(QString cmd,int repeat)
{
    emit remoteCommand(cmd,repeat);
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::emitRemoteStatus(QString text,int statusCode)
{
    emit remoteStatus(text,statusCode);
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::processLine(const QString& line)
{
    int code,repeat;
    QString cmd,remote;

    if(parseIRLine(line,code,repeat,cmd,remote))
    {
        emit remoteCommand(cmd,repeat);
    }
}

//-------------------------------------------------------------------------------------------

bool WinLIRCClient::parseIRLine(const QString& line,int& code,int& repeat,QString& cmd,QString& remote) const
{
    QStringList parts;
    bool res = false;

    parts = getStringParts(line);
    if(parts.size()==4)
    {
        if(isValidCode(parts.at(0)) && isValidCode(parts.at(1)))
        {
            common::BString codeS(parts.at(0).toUtf8().constData());
            common::BString repeatS(parts.at(1).toUtf8().constData());

            code = codeS.Htoi();
            repeat = repeatS.Htoi();
            cmd = parts.at(2).toLower().trimmed();
            remote = parts.at(3).toLower().trimmed();
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool WinLIRCClient::isValidCode(const QString& code) const
{
    bool res = true;

    if(!code.isEmpty())
    {
        for(QString::const_iterator ppI=code.begin();ppI!=code.end() && res;ppI++)
        {
            const QChar& c = *ppI;
            if(!((c>=QChar('0') && c<=QChar('9')) || (c>=QChar('a') && c<=QChar('f')) || (c>=QChar('A') && c<=QChar('F'))))
            {
                res = false;
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

QStringList WinLIRCClient::getStringParts(const QString& line) const
{
    int i,state,start;
    QStringList parts;
    QString part;

    state = 0;
    start = 0;
    for(i=0;i<line.length();i++)
    {
        QChar c = line.at(i);

        switch(state)
        {
            case 0:
                if(!(c==QChar(' ') || c==QChar('\t')))
                {
                    start = i;
                    state = 1;
                }
                break;

            case 1:
                if(c==QChar(' ') || c==QChar('\t'))
                {
                    if(start < i)
                    {
                        part = line.mid(start,i - start).trimmed();
                        if(!part.isEmpty())
                        {
                            parts.append(part);
                        }
                    }
                    state = 0;
                }
                break;
        }
    }
    if(state==1 && start < line.length())
    {
        part = line.mid(start);
        if(!part.isEmpty())
        {
            parts.append(part);
        }
    }
    return parts;
}

//-------------------------------------------------------------------------------------------

bool WinLIRCClient::isConnected()
{
    return (getProcessState()) ? true : false;
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::connect(const QString& host,int port)
{
    LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_OpenConnection;
    cmd.host() = host;
    cmd.port() = port;
    queueProcessCommand(cmd);
}

//-------------------------------------------------------------------------------------------

void WinLIRCClient::shutdown()
{
    LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_ShutdownClient;
    queueProcessCommand(cmd);
}

//-------------------------------------------------------------------------------------------

network::socket_type WinLIRCClient::getSocket()
{
    return m_socket;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
