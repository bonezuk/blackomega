#include "network/rtp/silveromega/inc/SilverRTPCodec.h"
#include "common/inc/BOParse.h"

#include <QDir>

//#define OMEGA_NETWORK_SILVERRTPCODEC_DB 1
#if defined(OMEGA_NETWORK_SILVERRTPCODEC_DB)
#define DB(x) x
#else
#define DB(x)
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------
// SilverRTPCodecDebugItem
//-------------------------------------------------------------------------------------------

SilverRTPCodecDebugItem::SilverRTPCodecDebugItem(Type t) : common::DebugOutputItem("SilverRTPCodec",5),
    m_type(t),
    m_time()
{}

//-------------------------------------------------------------------------------------------

SilverRTPCodecDebugItem::SilverRTPCodecDebugItem(const common::DebugOutputItem& item) : common::DebugOutputItem("SilverRTPCodec",5),
    m_type(e_onData),
    m_time()
{
    copy(item);
}

//-------------------------------------------------------------------------------------------

SilverRTPCodecDebugItem::Type& SilverRTPCodecDebugItem::type()
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& SilverRTPCodecDebugItem::time()
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

void SilverRTPCodecDebugItem::copy(const common::DebugOutputItem& item)
{
    try
    {
        const SilverRTPCodecDebugItem& aItem = dynamic_cast<const SilverRTPCodecDebugItem&>(item);
        m_type = aItem.m_type;
        m_time = aItem.m_time;
    }
    catch(...) {}
    common::DebugOutputItem::copy(item);
}

//-------------------------------------------------------------------------------------------

QString SilverRTPCodecDebugItem::print() const
{
    QString x;

    x = printStamp();
    switch(m_type)
    {
        case e_onData:
            x += "onData";
            break;

        case e_onNext:
            x += "onNext";
            break;
    }
    x += " , ";
    x += printTime(m_time);
    return x;
}

//-------------------------------------------------------------------------------------------
// SilverRTPCodec
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(engine::CodecFactory,engine::Codec, \
                            SilverRTPCodecFactory,SilverRTPCodec, \
                            "ogg:rtp",false)

//-------------------------------------------------------------------------------------------

SilverRTPCodec::SilverRTPCodec(QObject *parent) : engine::silveromega::SilverCodec(parent),
    m_packets(0),
    m_scheduler(0),
    m_session(0),
    m_pList(),
    m_completeFlag(false),
    m_bufferFlag(false),
    m_doneBufferFlag(false),
    m_tCalc(),
    m_timeBuffer(),
    m_offset(0)
{}

//-------------------------------------------------------------------------------------------

SilverRTPCodec::~SilverRTPCodec()
{
    try
    {
        SilverRTPCodec::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void SilverRTPCodec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "SilverRTPCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::getName(const QString& name,QString& sHost,tint& sPort,QString& rHost,tint& rPort,QString& fName) const
{
    tint state[4],s = 0,start = 0;
    common::BOParse parser;
    common::BO_Parse_Unit *item;
    bool res = false;
    QByteArray mem(name.toUtf8());

    state[0] = parser.String("rtp://");
    state[1] = parser.String("/");
    state[2] = parser.String("?");
    state[3] = parser.String(":");

    item = parser.Lexical(mem.constData());
    while(s<5 && item!=0)
    {
        switch(s)
        {
            case 0:
                if(state[0]==item->state)
                {
                    start = item->start + item->length;
                    s = 1;
                }
                break;

            case 1:
                if(state[3]==item->state)
                {
                    sHost = name.mid(start,item->start - start);
                    start = item->start + item->length;
                    s = 2;
                }
                break;

            case 2:
                if(state[2]==item->state)
                {
                    sPort = name.mid(start,item->start - start).toInt();
                    start = item->start + item->length;
                    s = 3;
                }
                break;

            case 3:
                if(state[3]==item->state)
                {
                    rHost = name.mid(start,item->start - start);
                    start = item->start + item->length;
                    s = 4;
                }
                break;

            case 4:
                if(state[1]==item->state)
                {
                    rPort = name.mid(start,item->start - start).toInt();
                    start = item->start + item->length;
                    s = 5;
                }
                break;
        }
        item = item->next;
    }

    if(s>=5)
    {
        fName = QDir::toNativeSeparators(name.mid(start));
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::open(const QString& name)
{
    QString sHost,rHost;
    tint sPort,rPort;
    bool res = false;

    close();
    if(getName(name,sHost,sPort,rHost,rPort,m_name))
    {
        m_packets = new VOggPackets;
        if(m_packets->open(m_name))
        {
            if(readConfiguration())
            {
                m_pList.clear();

                if(openSession(sHost,sPort,rHost,rPort))
                {
                    m_window = new engine::silveromega::VSilverWindow(m_container);
                    m_seekService = false;
                    m_completeFlag = false;
                    m_bufferFlag = false;
                    m_doneBufferFlag = false;
                    m_state = 0;
                    m_time = 0;
                    m_timeBuffer = 0;
                    m_offset = 0;
                    res = true;
                }
                else
                {
                    printError("open","Error opening RTP receiver session");
                }
            }
            else
            {
                printError("open","Failed to decode vorbis configuration data");
            }
        }
        else
        {
            printError("open","Failed to open vorbis file to stream packets from");
        }
    }
    else
    {
        printError("open","Given file type not supported");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SilverRTPCodec::close()
{
    closeSession();

    if(m_window!=0)
    {
        delete m_window;
        m_window = 0;
    }
    if(m_container!=0)
    {
        delete m_container;
        m_container = 0;
    }
    if(m_packets!=0)
    {
        m_packets->close();
        delete m_packets;
        m_packets = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::readConfiguration()
{
    bool res = true;
    NetArray memInfo,memComm,memData;

    if(m_packets==0)
    {
        printError("readConfiguration","No packet parser instance");
        return false;
    }
    if(m_container!=0)
    {
        delete m_container;
    }
    m_container = new engine::silveromega::VSilverContainer;

    if(m_packets->configuration(VOggPackets::e_configInformation,memInfo))
    {
        engine::SequenceMemory seq(memInfo);

        if(!m_container->readInformation(&seq))
        {
            printError("readConfiguration","Failed to decode vorbis information packet");
            res = false;
        }
    }
    else
    {
        printError("readConfiguration","Failed to get vorbis information packet");
        res = false;
    }

    if(res)
    {
        if(m_packets->configuration(VOggPackets::e_configComments,memComm))
        {
            engine::SequenceMemory seq(memComm);

            if(!m_container->readComments(&seq))
            {
                printError("readConfiguration","Failed to decode vorbis comments packet");
                res = false;
            }
        }
        else
        {
            printError("readConfiguration","Failed to get vorbis comments packet");
            res = false;
        }
    }

    if(res)
    {
        if(m_packets->configuration(VOggPackets::e_configCodecData,memData))
        {
            engine::SequenceMemory seq(memData);

            if(!m_container->readData(&seq))
            {
                printError("readConfiguration","Failed to decode vorbis data packet");
                res = false;
            }
        }
        else
        {
            printError("readConfiguration","Failed to get vorbis data packet");
            res = false;
        }
    }

    if(!m_container->setup())
    {
        printError("readConfiguration","Failed to setup vorbis configuration");
        res = false;
    }

    if(res)
    {
        if(!m_tCalc.setConfiguration(memInfo,memComm,memData))
        {
            printError("readConfiguration","Error setting up vorbis time calculator");
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SilverRTPCodec::onData(unsigned int srcID,void *pData)
{
    DataPacketList *pList = reinterpret_cast<DataPacketList *>(pData);
    QList<DataPacket>::iterator ppI;

#if defined(OMEGA_DEBUG_LOG)
    bool debugFlag = (common::DebugOutput::instance().level() >= 5) ? true : false;
    if(debugFlag)
    {
        common::DebugOutput& dOutput = common::DebugOutput::instance();

        for(ppI=pList->list().begin();ppI!=pList->list().end();++ppI)
        {
            DataPacket& p = *ppI;
            SilverRTPCodecDebugItem item(SilverRTPCodecDebugItem::e_onData);
            item.time() = p.time();
            dOutput.append(item);
        }
    }
#endif

    if(!m_bufferFlag)
    {
        for(ppI=pList->list().begin();ppI!=pList->list().end();++ppI)
        {
            DataPacket& p = *ppI;
            m_timeBuffer += p.length();
        }
        m_pList += pList->list();

        if(m_doneBufferFlag || m_timeBuffer >= m_bufferTimeLength)
        {
            m_doneBufferFlag = true;
            m_bufferFlag = true;
            if(!m_initFlag)
            {
                Q_EMIT onInit((void *)this);
            }
        }
    }
    else
    {
        m_pList += pList->list();
    }
}

//-------------------------------------------------------------------------------------------

void SilverRTPCodec::onComplete(network::rtp::Session *s)
{
    m_completeFlag = true;
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::next(engine::AData& data)
{
    bool res = true;

    try
    {
        engine::RData& rData = dynamic_cast<engine::RData&>(data);
        engine::RData::Part *part = 0;
        tint len,total,amount = 0,pNo = -1;
        sample_t *buffer = rData.data();

        if(m_completeFlag)
        {
            closeSession();
        }

        if(m_state>=0)
        {
            if(rData.noParts() > 0)
            {
                pNo = rData.noParts() - 1;
                part = &(rData.part(pNo));
                part->start() = m_time;
            }
            else if(m_state > 0)
            {
                pNo = 0;
                part = &(rData.nextPart());
                part->start() = m_time;
            }

            while((rData.rLength() - amount)>0 && res)
            {
                switch(m_state)
                {
                    case 0:
                        if(!m_pList.empty())
                        {
                            DataPacket p(m_pList.takeFirst());
                            common::Array<tubyte,tubyte> mem;
                            NetArraySPtr sMem(p.data());
                            mem.AppendRaw(reinterpret_cast<const tchar *>(sMem->GetData()),sMem->GetSize());
                            engine::SequenceMemory seq(mem);

                            if(part!=0)
                            {
                                part->end() = m_time;
                            }

                            // Comparison of packet time done as NTP timestamp is 32-bit resolution.
                            tuint64 cT,pT;
                            cT = static_cast<tuint64>(m_time)   & 0xffffffffffff0000ULL;
                            pT = static_cast<tuint64>(p.time()) & 0xffffffffffff0000ULL;
                            if(cT!=pT)
                            {
                                m_time = p.time();
                            }

                            pNo++;
                            part = &(rData.nextPart());
                            part->start() = m_time;

#if defined(OMEGA_DEBUG_LOG)
                            bool debugFlag = (common::DebugOutput::instance().level() >= 5) ? true : false;
                            if(debugFlag)
                            {
                                common::DebugOutput& dOutput = common::DebugOutput::instance();
                                SilverRTPCodecDebugItem item(SilverRTPCodecDebugItem::e_onNext);
                                item.time() = m_time;
                                dOutput.append(item);
                            }
#endif

                            if(m_window->setup(&seq))
                            {
                                frameSynthesis(m_container,&seq,m_window);
                                m_window->synthesis();
                                m_state = 1;
                            }
                        }
                        else
                        {
                            if(m_completeFlag)
                            {
                                m_state = -1;
                            }
                            else
                            {
                                m_state = 2;
                            }
                            m_bufferFlag = false;
                            m_timeBuffer = 0;
                            res = false;
                        }
                        break;

                    case 1:
                        {
                            len = rData.rLength() - amount;
                            total = m_window->getPCM(&buffer[(part->offset() + amount) * noChannels()],len);
                            m_time += static_cast<tfloat64>(total) / static_cast<tfloat64>(m_container->m_information->m_audioSampleRate);
                            amount += total;
                            if(total < len)
                            {
                                part->length() = amount;
                                part->end() = m_time;
                                part->done() = true;
                                amount = 0;
                                m_state = 0;
                            }
                        }
                        break;

                    case 2:
                        if(m_bufferFlag)
                        {
                            m_state = 0;
                        }
                        else
                        {
                            res = false;
                        }
                        break;
                }
            }

            if(part!=0)
            {
                part->length() = amount;
                part->end() = m_time;
                part->done() = true;
            }
        }
        else
        {
            res = false;
        }
    }
    catch(...)
    {
        return false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::openSession(const tchar *host,tint port,const tchar *rHost,tint rPort)
{
    return openSession(QString::fromLatin1(host),port,QString::fromLatin1(rHost),rPort);
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::openSession(const QString& host,tint port,const QString& rHost,tint rPort)
{
    bool res = false;

    closeSession();

    if(m_scheduler==0)
    {
        m_scheduler = reinterpret_cast<Scheduler *>(Controller::instance()->getService("rtp"));
    }
    if(m_scheduler!=0)
    {
        m_session = m_scheduler->newReceiver(m_name,host,port,rHost,rPort);
        if(m_session!=0)
        {
            SilverSession *sSession = dynamic_cast<SilverSession *>(m_session);
            if(sSession!=0)
            {
                sSession->setTCalc(&m_tCalc);
            }

            QObject::connect(m_session,SIGNAL(onData(unsigned int,void *)),this,SLOT(onData(unsigned int,void *)),Qt::BlockingQueuedConnection);
            QObject::connect(m_session,SIGNAL(onComplete(network::rtp::Session *)),this,SLOT(onComplete(network::rtp::Session *)),Qt::BlockingQueuedConnection);
            res = true;
        }
        else
        {
            printError("openSession","Error opening RTP receiver session");
        }
    }
    else
    {
        printError("openSession","Error getting RTP service");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SilverRTPCodec::closeSession()
{
    if(m_session!=0)
    {
        if(m_scheduler==0)
        {
            m_scheduler = reinterpret_cast<Scheduler *>(Controller::instance()->getService("rtp"));
        }
        if(m_scheduler!=0)
        {
            QObject::disconnect(m_session,SIGNAL(onData(unsigned int,void *)),this,SLOT(onData(unsigned int,void *)));
            QObject::disconnect(m_session,SIGNAL(onComplete(network::rtp::Session *)),this,SLOT(onComplete(network::rtp::Session *)));
            m_scheduler->complete(m_session);
            m_session = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

common::TimeStamp SilverRTPCodec::length() const
{
    common::TimeStamp len;

    if(m_packets!=0)
    {
        len = m_packets->length();
    }
    return len;
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::isRemote() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::isBuffered(tfloat32& percent)
{
    if(!m_bufferFlag)
    {
        if(m_pList.size()>0)
        {
            m_bufferFlag = true;
            percent = 100.0f;
        }
        else
        {
            percent = static_cast<tfloat32>((static_cast<tfloat64>(m_timeBuffer) / m_bufferTimeLength) * 100.0);
        }
    }
    else
    {
        percent = 100.0f;
    }
    return m_bufferFlag;
}

//-------------------------------------------------------------------------------------------

bool SilverRTPCodec::isComplete() const
{
    return m_completeFlag;
}

//-------------------------------------------------------------------------------------------

void SilverRTPCodec::resync()
{
    if(m_session!=0 && m_scheduler!=0)
    {
        m_scheduler->resync(m_session);
    }
    m_state = 0;
    m_offset = 0;
    m_pList.clear();
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
