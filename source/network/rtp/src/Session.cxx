#include "network/inc/Resource.h"
#include "common/inc/SHA1Digest.h"
#include "network/rtp/inc/Session.h"

//#define OMEGA_SESSION_DEBUG_PRINT 1
//#define OMEGA_SESSION_DEBUG_PRINT_REPORT 1

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(SessionFactory,Session)

//-------------------------------------------------------------------------------------------

Session::Session(QObject *parent) : QObject(parent),
    m_scheduler(0),
    m_input(0),
    m_output(0),
    m_rHost(),
    m_rPort(),
    m_source(false),
    m_time(),
    m_startClockTime(),
    m_randomTimeOffset(),
    m_lastRecieveTime(),
    m_sequenceNo(0),
    m_currentPacket(),
    m_completeFlag(true),
    m_byeFlag(true),
    m_senderPacketCount(0),
    m_senderDataCount(0),
    m_sourceID(0),
    m_sessionBW(256000.0),
    m_tp(),
    m_tn(),
    m_rtcpBW(0.0),
    m_avgRTCPSize(0.0),
    m_initialRTCPFlag(false),
    m_memberSet(),
    m_senderSet(),
    m_timerData(0),
    m_timerReport(0),
    m_sourceSessionMap()
{
    m_timerData = new QTimer(this);
    QObject::connect(m_timerData,SIGNAL(timeout()),this,SLOT(onSendData()));
    m_timerReport = new QTimer(this);
    QObject::connect(m_timerReport,SIGNAL(timeout()),this,SLOT(onSendReport()));
}

//-------------------------------------------------------------------------------------------

Session::~Session()
{
    try
    {
        QObject::disconnect(m_timerData,SIGNAL(timeout()),this,SLOT(onSendData()));
        delete m_timerData;
        m_timerData = 0;
        QObject::disconnect(m_timerReport,SIGNAL(timeout()),this,SLOT(onSendReport()));
        delete m_timerReport;
        m_timerReport = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void Session::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Session::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp Session::clockTime()
{
    common::TimeStamp t;
    QDateTime c(QDateTime::currentDateTime());

    t.year(c.date().year());
    t.month(c.date().month());
    t.day(c.date().day());
    t.hour(c.time().hour());
    t.minute(c.time().minute());
    t.second(c.time().second());
    t.millisecond(c.time().msec());
    return t;
}

//-------------------------------------------------------------------------------------------

bool Session::isSupportedPayload(tint t) const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool Session::isSource() const
{
    return m_source;
}

//-------------------------------------------------------------------------------------------

bool Session::marker() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint Session::payloadType() const
{
    return 0;
}

//-------------------------------------------------------------------------------------------

tuint32 Session::sequenceNo() const
{
    return m_sequenceNo;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp Session::time() const
{
    if(isSource())
    {
        return m_time;
    }
    else
    {
        common::TimeStamp x(clockTime());
        x -= startClockTime();
        return x;
    }
}

//-------------------------------------------------------------------------------------------

common::TimeStamp Session::time(tuint32 id) const
{
    common::TimeStamp x(clockTime());
    x -= startClockTime(id);
    return x;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp Session::rtime() const
{
    common::TimeStamp x(m_randomTimeOffset);
    x += m_time;
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 Session::sourceID() const
{
    return m_sourceID;
}

//-------------------------------------------------------------------------------------------

tuint32 Session::contributingID(tint idx) const
{
    return 0;
}

//-------------------------------------------------------------------------------------------

tint Session::contributingNumber() const
{
    return 0;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& Session::startClockTime() const
{
    return m_startClockTime;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& Session::startClockTime(tuint32 srcID) const
{
    QMap<tuint32,SourceStateSPtr>::const_iterator ppI = m_sourceSessionMap.find(srcID);

    if(ppI!=m_sourceSessionMap.end())
    {
        return ppI.value()->startClock();
    }
    else
    {
        return m_startClockTime;
    }
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& Session::randomClockTime(tuint32 srcID) const
{
    QMap<tuint32,SourceStateSPtr>::const_iterator ppI = m_sourceSessionMap.find(srcID);

    if(ppI!=m_sourceSessionMap.end())
    {
        return ppI.value()->randomClock();
    }
    else
    {
        return m_randomTimeOffset;
    }
}

//-------------------------------------------------------------------------------------------

tint Session::members() const
{
    return m_memberSet.size() + 1;
}

//-------------------------------------------------------------------------------------------

tint Session::senders() const
{
    return m_senderSet.size();
}

//-------------------------------------------------------------------------------------------

bool Session::isMember(tuint32 id) const
{
    return (m_memberSet.find(id)!=m_memberSet.end()) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool Session::isSender(tuint32 id) const
{
    return (m_senderSet.find(id)!=m_senderSet.end()) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool Session::start(Scheduler *scheduler,const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort)
{
    QString err;
    bool res = true;

    m_rHost = rHost;
    m_rPort = rPort;

    m_scheduler = scheduler;
    m_source = true;

    if(openResource(resource))
    {
        if(openNetwork(host,port))
        {
            startSession();
            res = true;
        }
        else
        {
            err = "Failed to open UDP network transport layer to host '" + host + "' on port " + QString::number(port);
            printError("start",err.toLatin1().constData());
        }
    }
    else
    {
        err = "Failed to open resource '" + resource + "'";
        printError("start",err.toLatin1().constData());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Session::start(Scheduler *scheduler,const QString& host,tint port,const QString& rHost,tint rPort)
{
    QString err;
    bool res = true;

    m_rHost = rHost;
    m_rPort = rPort;

    m_scheduler = scheduler;
    m_source = false;

    if(openNetwork(host,port))
    {
        startSession();
        res = true;
    }
    else
    {
        err = "Failed to open UDP network transport layer to host '" + host + "' on port " + QString::number(port);
        printError("start",err.toLatin1().constData());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void Session::stop()
{
    stopSession();
    closeNetwork();
    if(m_source)
    {
        closeResource();
    }
}

//-------------------------------------------------------------------------------------------

bool Session::openNetwork(const QString& host,tint port)
{
    bool res = false;

    closeNetwork();

    m_input = new UDPRead(m_scheduler,this);
    if(m_input->open(host,port))
    {
        m_output = new UDPWrite(m_scheduler,this);
        if(m_output->open(m_rHost,m_rPort))
        {
            QObject::connect(m_input,SIGNAL(onRecieve(const QString,tint,network::NetArraySPtr)),this,SLOT(onRecieve(const QString&,tint,network::NetArraySPtr)));
            res = true;
        }
        else
        {
            printError("open","Failed to open UDP output connection for RTP stream");
        }
    }
    else
    {
        printError("open","Failed to open UDP input connection for RTP stream");
    }

    if(!res)
    {
        closeNetwork();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void Session::closeNetwork()
{
    if(m_input!=0)
    {
        m_input->close();
        QObject::disconnect(m_input,SIGNAL(onRecieve(const QString,tint,network::NetArraySPtr)),this,SLOT(onRecieve(const QString&,tint,network::NetArraySPtr)));
        delete m_input;
        m_input = 0;
    }

    if(m_output!=0)
    {
        m_output->close();
        delete m_output;
        m_output = 0;
    }
}

//-------------------------------------------------------------------------------------------

void Session::startSession()
{
    RTPPacketSPtr emptyP;

    stopSession();

    m_time = 0;
    m_startClockTime = clockTime();
    m_lastRecieveTime = m_startClockTime;
    m_randomTimeOffset = Resource::instance().random(0);
    m_sequenceNo = Resource::instance().random(1) & 0x0000ffff;
    m_currentPacket = emptyP;
    m_completeFlag = false;
    m_byeFlag = false;
    m_senderPacketCount = 0;
    m_senderDataCount = 0;
    m_sourceID = Resource::instance().random(2);

    // RTCP control variables
    m_tp = 0;
    m_tn = 0;
    m_rtcpBW = m_sessionBW * 0.05;
    m_initialRTCPFlag = true;
    m_avgRTCPSize = 64.0; // single RTCP session report + description.

    m_memberSet.clear();
    m_senderSet.clear();
    m_sourceSessionMap.clear();

    onSendDescription();

    if(isSource())
    {
        if(compileNextPacket())
        {
            if(!send())
            {
                stopSession();
            }
        }
        else
        {
            printError("startSession","Failed to generate first packet in sequence");
        }
    }
    scheduleReport();
}

//-------------------------------------------------------------------------------------------

void Session::stopSession()
{
    if(m_timerData->isActive())
    {
        m_timerData->stop();
    }
    if(m_timerReport->isActive())
    {
        m_timerReport->stop();
    }

    m_completeFlag = true;
    if(!m_byeFlag)
    {
        onSendBye();
    }
}

//-------------------------------------------------------------------------------------------

void Session::onSendData()
{
    if(!send())
    {
        onSendBye();
    }
}

//-------------------------------------------------------------------------------------------

bool Session::send()
{
    bool res = false;

    if(!m_completeFlag && isSource())
    {
        if(sendPacket())
        {
            if(compileNextPacket())
            {
                common::TimeStamp cT(clockTime());

                cT -= m_startClockTime;
                if(m_time > cT)
                {
                    tint tDelayActual;
                    tfloat64 tDelay = static_cast<tfloat64>(m_time - cT);
                    tDelay *= 1000.0;
                    tDelayActual = static_cast<tint>(tDelay);

#if defined(OMEGA_SESSION_DEBUG_PRINT)
                    common::Log::g_Log.print("SPTimer - %d\n",tDelayActual);
#endif
                    setTimer(m_timerData,tDelayActual);
                    res = true;
                }
                else
                {
#if defined(OMEGA_SESSION_DEBUG_PRINT)
                    common::Log::g_Log.print("SPTimer - Recursive\n");
#endif
                    res = send();
                }
            }
            else
            {
                m_completeFlag = true;
                res = false;
            }
        }
        else
        {
            printError("send","Error sending next packet in sequence");
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Session::sendPacket()
{
    bool res = true;

    if(m_currentPacket.data()!=0 && isSource())
    {
        RTPPacketSPtr emptyP;
        NetArraySPtr mem(new NetArray);

#if defined(OMEGA_SESSION_DEBUG_PRINT)
        common::Log::g_Log.print("SP = %d\t%d:%d.%d\n",m_sequenceNo,m_time.minute(),m_time.second(),m_time.millisecond());
#endif

        if(m_currentPacket->packet(mem))
        {
            if(m_output->send(m_rHost,m_rPort,mem))
            {
                m_senderPacketCount++;
                m_senderDataCount += m_currentPacket->body()->GetSize();
                m_scheduler->doWrite();
            }
            else
            {
                printError("sendPacket","Error posting RTP to output queue");
                res = false;
            }
        }
        else
        {
            printError("sendPacket","Error compiliing RTP packet");
            res = false;
        }
        m_currentPacket = emptyP;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Session::compileNextPacket()
{
    RTPPacketSPtr p(new RTPPacket(*this));

    if(!isSource())
    {
        return false;
    }
    if(!generatePacketData(m_time,p))
    {
        return false;
    }
    m_currentPacket = p;
    p->buildHeader();
    m_sequenceNo++;
    return true;
}

//-------------------------------------------------------------------------------------------

bool Session::processPacket(RTPPacketSPtr& p,QList<DataPacket>& pList)
{
    SourceStateSPtr source;
    QMap<tuint32,SourceStateSPtr>::iterator ppI;
    QMap<tuint32,SDESBlock>::iterator ppK;
    common::TimeStamp n(clockTime());

    ppK = m_memberSet.find(p->sourceID());
    if(ppK==m_memberSet.end())
    {
        SDESBlock desc;
        desc.sessionID(p->sourceID());
        desc.lastReceived(n);
        m_memberSet.insert(p->sourceID(),desc);
    }
    else
    {
        ppK.value().lastReceived(n);
    }

    ppI = m_sourceSessionMap.find(p->sourceID());
    if(ppI!=m_sourceSessionMap.end())
    {
        source = ppI.value();
    }
    else
    {
        QSet<tuint32>::iterator ppJ;
        SourceStateSPtr nPtr(new SourceState(*this,p->sourceID()));
        nPtr->init(p);

        ppJ = m_senderSet.find(p->sourceID());
        if(ppJ==m_senderSet.end())
        {
            m_senderSet.insert(p->sourceID());
        }

        m_sourceSessionMap.insert(p->sourceID(),nPtr);
        source = nPtr;
    }
    return source->processPacket(p,pList);
}

//-------------------------------------------------------------------------------------------

void Session::setTimer(QTimer *timer,tint msec)
{
    if(timer->isActive())
    {
        timer->stop();
    }
    timer->setSingleShot(true);
    timer->setInterval(msec);
    timer->start();
}

//-------------------------------------------------------------------------------------------

void Session::scheduleReport()
{
    tint tDelayActual;
    tfloat64 tDelay;
    common::TimeStamp cT,curT(clockTime()),interval;

    interval = rtcpInterval(false);
    m_tn = m_tp + interval;
    if(m_tn < curT)
    {
#if defined(OMEGA_SESSION_DEBUG_PRINT_REPORT)
        common::Log::g_Log.print("ReportTimer - Recursive\n");
#endif
        onSendReport();
    }
    else
    {
        cT = m_tn - curT;
        tDelay = static_cast<tfloat64>(cT);
        tDelay *= 1000.0;
        tDelayActual = static_cast<tint>(tDelay);

#if defined(OMEGA_SESSION_DEBUG_PRINT_REPORT)
        common::Log::g_Log.print("SPTimer - %d %.3f %.3f %.3f\n",tDelayActual,
            static_cast<tfloat64>(m_tn - m_startClockTime),
            static_cast<tfloat64>(m_tp - m_startClockTime),
            static_cast<tfloat64>(interval));
#endif

        setTimer(m_timerReport,tDelayActual);
    }
}

//-------------------------------------------------------------------------------------------

void Session::onSendReport()
{
    m_tp = clockTime();

#if defined(OMEGA_SESSION_DEBUG_PRINT_REPORT)
        common::Log::g_Log.print("SR = %d:%d.%d\n",m_tp.minute(),m_tp.second(),m_tp.millisecond());
#endif

    if(isSource())
    {
        onSendSenderReport();
    }
    else
    {
        onSendReceiverReport();
    }
    scheduleReport();
}

//-------------------------------------------------------------------------------------------

void Session::onSendSenderReport()
{
    RTCPPacketSR srReport(*this);
    common::TimeStamp tC(clockTime());
    QMap<tuint32,SourceStateSPtr>::iterator ppI;
    QList<RTCPReportBlock>& bList = srReport.blockList();
    NetArraySPtr mem(new NetArray);

    srReport.sessionID(m_sourceID);
    srReport.time(tC);
    srReport.rtpTime(tC + m_randomTimeOffset);
    srReport.senderPacketCount(m_senderPacketCount);
    srReport.senderOctetCount(m_senderDataCount);

    for(ppI=m_sourceSessionMap.begin();ppI!=m_sourceSessionMap.end();++ppI)
    {
        SourceStateSPtr sState(ppI.value());
        RTCPReportBlock block;

        sState->report(block);
        bList.append(block);
    }

    if(srReport.packet(mem))
    {
        addCompoundDescription(mem);
        if(m_output->send(m_rHost,m_rPort,mem))
        {
            m_avgRTCPSize = ((1.0/16.0) * static_cast<tfloat64>(mem->GetSize())) + ((15.0/16.0) * m_avgRTCPSize);
            m_scheduler->doWrite();
        }
        else
        {
            printError("onSendSenderReport","Error sending sender report to network");
        }
    }
    else
    {
        printError("onSendSenderReport","Error building RTCP sender report");
    }
}

//-------------------------------------------------------------------------------------------

void Session::onSendReceiverReport()
{
    RTCPPacketRR rrReport(*this);
    QMap<tuint32,SourceStateSPtr>::iterator ppI;
    QList<RTCPReportBlock>& bList = rrReport.blockList();
    NetArraySPtr mem(new NetArray);

    rrReport.sessionID(m_sourceID);

    for(ppI=m_sourceSessionMap.begin();ppI!=m_sourceSessionMap.end();++ppI)
    {
        SourceStateSPtr sState(ppI.value());
        RTCPReportBlock block;

        sState->report(block);
        bList.append(block);
    }

    if(rrReport.packet(mem))
    {
        addCompoundDescription(mem);

        if(m_output->send(m_rHost,m_rPort,mem))
        {
            m_avgRTCPSize = ((1.0/16.0) * static_cast<tfloat64>(mem->GetSize())) + ((15.0/16.0) * m_avgRTCPSize);
            m_scheduler->doWrite();
        }
        else
        {
            printError("sendRecieverReport","Error sending receiver report to network");
        }
    }
    else
    {
        printError("sendRecieverReport","Error building RTCP receiver report");
    }
}

//-------------------------------------------------------------------------------------------

void Session::onSendDescription()
{
    SDESBlock rInfo(Scheduler::idInfo());
    RTCPPacketSDES sdesPacket(*this);
    QString cname;
    NetArraySPtr mem(new NetArray);

    cname = Resource::instance().username() + "@" + Resource::instance().hostname();
    rInfo.canonical(cname);
    rInfo.sessionID(m_sourceID);
    sdesPacket.blockList().append(rInfo);

    if(sdesPacket.packet(mem))
    {
        if(m_output->send(m_rHost,m_rPort,mem))
        {
            m_avgRTCPSize = ((1.0/16.0) * static_cast<tfloat64>(mem->GetSize())) + ((15.0/16.0) * m_avgRTCPSize);
            m_scheduler->doWrite();
        }
        else
        {
            printError("onSendDescription","Error sending RTCP session description");
        }
    }
    else
    {
        printError("onSendDescription","Error building RTCP session description");
    }
}

//-------------------------------------------------------------------------------------------

void Session::onSendBye()
{
    if(!m_byeFlag)
    {
        RTCPPacketBYE byePacket(*this);
        NetArraySPtr mem(new NetArray);

        byePacket.sessionList().append(m_sourceID);
        if(byePacket.packet(mem))
        {
            addCompoundDescription(mem);

            if(m_output->send(m_rHost,m_rPort,mem))
            {
                m_avgRTCPSize = ((1.0/16.0) * static_cast<tfloat64>(mem->GetSize())) + ((15.0/16.0) * m_avgRTCPSize);
                m_scheduler->doWrite();
            }
            else
            {
                printError("onSendByE","Error sending bye packet to network");
            }
        }
        else
        {
            printError("onSendByE","Error building bye packet");
        }

        m_byeFlag = true;
        stopSession();

        emit onComplete(this);
    }
}

//-------------------------------------------------------------------------------------------

void Session::addCompoundDescription(NetArraySPtr mem)
{
    SDESBlock rInfo(Scheduler::idInfo());
    RTCPPacketSDES sdesPacket(*this);
    QString cname;

    cname = Resource::instance().username() + "@" + Resource::instance().hostname();
    rInfo.canonical(cname);
    rInfo.sessionID(m_sourceID);
    sdesPacket.blockList().append(rInfo);

    if(!sdesPacket.packet(mem))
    {
        printError("addCompoundDescription","Error building RTCP session description");
    }
}

//-------------------------------------------------------------------------------------------
// rtcpInterval : computes the deterministic calculated interval for RTCP transmission.
//-------------------------------------------------------------------------------------------

common::TimeStamp Session::rtcpInterval(bool rFlag)
{
    // Minimum average time between RTCP packets from this site (in
    // seconds). This time prevents the reports from 'clumping' when
    // sessions are small and the law of large numbers isn't helping
    // to smooth out the traffic. It also keeps the report interval
    // from becoming ridiculously small during transient outrages like
    // a network partition.
    const tfloat64 c_RTCP_MIN_TIME = 5.0;

    // Fraction of the RTCP bandwidth to be shared among active
    // senders. (This fraction was chosen so that in a typical
    // session with one or two senders, the computed report
    // time would be roughly equal to the minimum report time so that
    // we don't unnecessarily slow down reciever report.) The
    // receiver fraction must be 1 - the sender fraction.
    const tfloat64 c_RTCP_SENDER_BW_FRACTION = 0.25;
    const tfloat64 c_RTCP_RCVR_BW_FRACTION = 1.0 - c_RTCP_SENDER_BW_FRACTION;

    // To compensate for "timer reconsideration" converging to a
    // value below the intended average.
    const tfloat64 c_COMPENSATION = 2.71828 - 1.5;

    // interval
    tfloat64 t,rtcpBW;
    tfloat64 rtcpMinTime = c_RTCP_MIN_TIME;
    // no. of member for computation.
    tint n;
    common::TimeStamp x;

    // Very first call at application start-up uses half the min
    // delay for quicker notification while still allowing some time
    // before reporting for randomization and to learn about other
    // sources so the report interval will converge to the correct
    // interval more quickly.
    if(m_initialRTCPFlag)
    {
        rtcpMinTime /= 2.0;
    }

    // Dedicate a fraction of the RTCP bandwidth to senders unless
    // the number of senders is large enough that their share is
    // more than that fraction.
    n = members();
    if(static_cast<tfloat64>(senders()) <= (static_cast<tfloat64>(members()) * c_RTCP_SENDER_BW_FRACTION))
    {
        if(isSource() && rFlag)
        {
            rtcpBW = m_rtcpBW * c_RTCP_SENDER_BW_FRACTION;
            n = members();
        }
        else
        {
            rtcpBW = m_rtcpBW * c_RTCP_RCVR_BW_FRACTION;
            n -= senders();
        }
    }
    else
    {
        rtcpBW = m_rtcpBW ;
    }

    // The effective number of sites times the average packet size is
    // the total number of octets sent when each site sends a report.
    // Dividing this by the effective bandwidth gives the time
    // interval over which those packets must be sent in order to
    // meet the bandwidth target, with a minimum enforced. In that
    // time interval we send one report so this time is also our
    // average time between reports.
    t = m_avgRTCPSize * static_cast<tfloat64>(n) / rtcpBW;
    if(t < rtcpMinTime)
    {
        t = rtcpMinTime;
    }

    // To avoid traffic bursts from unintended sychronization with
    // other sites, we then pick our actual next report interval as a
    // random number uniformly distributed between 0.5*t and 1.5*t.
    if(rFlag)
    {
        t = t * ((static_cast<tfloat64>(Resource::instance().random()) / 4294967296.0) + 0.5);
    }
    t = t / c_COMPENSATION;

    x = t;
    return x;
}

//-------------------------------------------------------------------------------------------

void Session::onRecieve(const QString& host,tint port,network::NetArraySPtr mem)
{
    if(mem.data()!=0 && mem->GetSize()>2)
    {
        tint offset = 0;
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());

        m_lastRecieveTime = clockTime();

        while(offset<(mem->GetSize() - 2) && offset!=-1)
        {
            tuint type = static_cast<tuint>(x[1 + offset]);

            if(type>=200 && type<=204)
            {
                tint nOffset;
                QVector<RTCPPacketSPtr> cVector;

                nOffset = RTCPPacket::generate(*this,cVector,mem,offset);
                if(nOffset > offset)
                {
                    QVector<RTCPPacketSPtr>::iterator ppI;

                    m_avgRTCPSize = ((1.0/16.0) * static_cast<tfloat64>(nOffset - offset)) + ((15.0/16.0) * m_avgRTCPSize);

                    for(ppI=cVector.begin();ppI!=cVector.end();++ppI)
                    {
                        RTCPPacketSPtr rPacket(*ppI);

                        switch(rPacket->type())
                        {
                            case RTCPPacket::e_SenderReport:
                                {
                                    RTCPPacketSR& pRTCP = dynamic_cast<RTCPPacketSR&>(*(rPacket.data()));
                                    onRecieveSR(pRTCP);
                                }
                                break;

                            case RTCPPacket::e_RecieverReport:
                                {
                                    RTCPPacketRR& pRTCP = dynamic_cast<RTCPPacketRR&>(*(rPacket.data()));
                                    onRecieveRR(pRTCP);
                                }
                                break;

                            case RTCPPacket::e_SourceDescription:
                                {
                                    RTCPPacketSDES& pRTCP = dynamic_cast<RTCPPacketSDES&>(*(rPacket.data()));
                                    onRecieveSDES(pRTCP);
                                }
                                break;

                            case RTCPPacket::e_Goodbye:
                                {
                                    RTCPPacketBYE& pRTCP = dynamic_cast<RTCPPacketBYE&>(*(rPacket.data()));
                                    onRecieveBYE(pRTCP);
                                }
                                break;

                            case RTCPPacket::e_Application:
                                {
                                    RTCPPacketAPP& pRTCP = dynamic_cast<RTCPPacketAPP&>(*(rPacket.data()));
                                    onRecieveAPP(pRTCP);
                                }
                                break;

                            default:
                                break;
                        }
                    }
                    offset = nOffset;
                }
                else
                {
                    offset = -1;
                }
            }
            else
            {
                RTPPacketSPtr pRTP(new RTPPacket(*this));

                offset = pRTP->parse(mem,offset);
                if(offset>0)
                {
                    QList<DataPacket> pList;

                    if(processPacket(pRTP,pList))
                    {
                        DataPacketList *packetList = new DataPacketList(pList);
                        emit onData(m_sourceID,reinterpret_cast<void *>(packetList));
                        delete packetList;
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Session::onRecieveSR(RTCPPacketSR& p)
{
    if(p.sessionID()!=m_sourceID)
    {
        QMap<tuint32,SourceStateSPtr>::iterator ppI = m_sourceSessionMap.find(p.sessionID());
        QMap<tuint32,SDESBlock>::iterator ppJ;
        QSet<tuint32>::iterator ppK;

        if(m_byeFlag)
        {
            return;
        }

        if(ppI!=m_sourceSessionMap.end())
        {
            SourceStateSPtr sState(ppI.value());
            sState->onSR(p);
        }

        ppJ = m_memberSet.find(p.sessionID());
        if(ppJ==m_memberSet.end())
        {
            SDESBlock desc;
            desc.sessionID(p.sessionID());
            desc.lastReceived(clockTime());
            m_memberSet.insert(p.sessionID(),desc);
        }
        else
        {
            ppJ.value().lastReceived(clockTime());
        }

        ppK = m_senderSet.find(p.sessionID());
        if(ppK==m_senderSet.end())
        {
            m_senderSet.insert(p.sessionID());
        }
    }
}

//-------------------------------------------------------------------------------------------

void Session::onRecieveRR(RTCPPacketRR& p)
{
    if(p.sessionID()!=m_sourceID)
    {
        QMap<tuint32,SDESBlock>::iterator ppI = m_memberSet.find(p.sessionID());

        if(m_byeFlag)
        {
            return;
        }

        if(ppI==m_memberSet.end())
        {
            SDESBlock desc;
            desc.sessionID(p.sessionID());
            desc.lastReceived(clockTime());
            m_memberSet.insert(p.sessionID(),desc);
        }
        else
        {
            ppI.value().lastReceived(clockTime());
        }
    }
}

//-------------------------------------------------------------------------------------------

void Session::onRecieveAPP(RTCPPacketAPP& p)
{}

//-------------------------------------------------------------------------------------------

void Session::onRecieveBYE(RTCPPacketBYE& p)
{
    if(p.sessionID()!=m_sourceID)
    {
        QMap<tuint32,SDESBlock>::iterator ppI = m_memberSet.find(p.sessionID());
        QSet<tuint32>::iterator ppJ = m_senderSet.find(p.sessionID());

        if(ppI!=m_memberSet.end())
        {
            m_memberSet.erase(ppI);
        }
        if(ppJ!=m_senderSet.end())
        {
            m_senderSet.erase(ppJ);
        }

        if(isSource())
        {
            if(m_memberSet.empty())
            {
                stopSession();
            }
        }
        else
        {
            if(m_senderSet.empty())
            {
                stopSession();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Session::onRecieveSDES(RTCPPacketSDES& p)
{
    if(p.sessionID()!=m_sourceID)
    {
        QList<SDESBlock>& dList = p.blockList();
        QList<SDESBlock>::iterator ppI;
        QMap<tuint32,SDESBlock>::iterator ppJ;

        if(m_byeFlag)
        {
            return;
        }

        ppJ = m_memberSet.find(p.sessionID());
        if(ppJ!=m_memberSet.end())
        {
            ppJ.value().lastReceived(clockTime());
        }
        else
        {
            SDESBlock desc;
            desc.sessionID(p.sessionID());
            desc.lastReceived(clockTime());
            m_memberSet.insert(p.sessionID(),desc);
        }

        for(ppI=dList.begin();ppI!=dList.end();++ppI)
        {
            SDESBlock& desc = *ppI;

            ppJ = m_memberSet.find(desc.sessionID());
            if(ppJ!=m_memberSet.end())
            {
                ppJ.value() = desc;
                ppJ.value().lastReceived(clockTime());
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Session::onTimeout()
{
    if(!m_completeFlag)
    {
        common::TimeStamp Td,rTd,sTd,sTimeout,rTimeout;

        if(m_memberSet.size() > 0)
        {
            QMap<tuint32,SDESBlock>::iterator ppI;

            Td  = rtcpInterval(false);
            sTd = static_cast<tfloat64>(Td) * 3.0;
            rTd = static_cast<tfloat64>(Td) * 5.0;
            sTimeout = clockTime();
            rTimeout = sTimeout;
            sTimeout -= sTd;
            rTimeout -= rTd;

            for(ppI=m_memberSet.begin();ppI!=m_memberSet.end();)
            {
                SDESBlock& b = ppI.value();

                if(m_senderSet.find(b.sessionID())!=m_senderSet.end())
                {
                    if(b.lastReceived() < sTimeout)
                    {
                        m_senderSet.remove(ppI.value().sessionID());
                        ppI = m_memberSet.erase(ppI);
                    }
                    else
                    {
                        ppI++;
                    }
                }
                else
                {
                    if(b.lastReceived() < rTimeout)
                    {
                        ppI = m_memberSet.erase(ppI);
                    }
                    else
                    {
                        ppI++;
                    }
                }
            }

            if(isSource())
            {
                if(m_memberSet.empty())
                {
                    stopSession();
                }
            }
            else
            {
                if(m_senderSet.empty())
                {
                    stopSession();
                }
            }
        }
        else
        {
            rTd = 5.0;
            rTimeout = clockTime() - rTd;

            if(m_lastRecieveTime < rTimeout)
            {
                stopSession();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Session::resync()
{
    QMap<tuint32,SourceStateSPtr>::iterator ppI;

    for(ppI=m_sourceSessionMap.begin();ppI!=m_sourceSessionMap.end();++ppI)
    {
        SourceStateSPtr source = ppI.value();
        source->resync();
    }
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
