#include "engine/silveromega/inc/SilverCodec.h"
#include "engine/inc/RData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------
// SilverCodecInitialize
//-------------------------------------------------------------------------------------------

SilverCodecInitialize *SilverCodecInitialize::m_instance = 0;

//-------------------------------------------------------------------------------------------

SilverCodecInitialize::SilverCodecInitialize()
{
    VSilverWindow::start();
}

//-------------------------------------------------------------------------------------------

SilverCodecInitialize::~SilverCodecInitialize()
{
    VSilverWindow::stop();
}

//-------------------------------------------------------------------------------------------

void SilverCodecInitialize::start()
{
    if(m_instance==0)
    {
        m_instance = new SilverCodecInitialize();
    }
}

//-------------------------------------------------------------------------------------------

void SilverCodecInitialize::end()
{
    if(m_instance!=0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------
// SilverCodec
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            SilverCodecFactory,SilverCodec, \
                            "ogg",false)

//-------------------------------------------------------------------------------------------

void SilverCodec::frameSynthesis(VSilverContainer *data,engine::Sequence *seq,VSilverWindow *window)
{
    engine::silveromega::frameSynthesis(data,seq,window);
}

//-------------------------------------------------------------------------------------------

SilverCodec::SilverCodec(QObject *parent) : Codec(Codec::e_codecVorbis,parent),
    m_name(),
    m_seekService(false),
    m_remoteService(false),
    m_cachedFile(0),
    m_file(0),
    m_stream(0),
    m_oggStream(0),
    m_container(0),
    m_window(0),
    m_state(-1),
    m_time(),
    m_seeker(0)
{}

//-------------------------------------------------------------------------------------------

SilverCodec::~SilverCodec()
{
    try
    {
        SilverCodec::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void SilverCodec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "SilverCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::open(const QString& name)
{
    bool res = false;
    
    close();
    if(getFileExtension(name).toLower()=="ogg")
    {
        m_name = name;

        m_cachedFile = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
        m_file = new engine::IOFile(m_cachedFile);

        if(m_file->open(name))
        {
            m_stream = new engine::Bitstream;
            if(m_stream->open(m_file))
            {
                m_oggStream = new VSilverOgg(m_stream);
                m_container = new VSilverContainer;
                
                if(m_container->read(m_oggStream))
                {
                    if(m_container->setup())
                    {
                        m_window = new VSilverWindow(m_container);
                        
                        m_seeker = new VSilverSeeker(m_container);
                        if(m_seeker->init(name))
                        {
                            m_seekService = true;
                        }
                        else
                        {
                            m_seekService = false;
                            delete m_seeker;
                            m_seeker = 0;
                        }
                    
                        m_state = 0;
                        m_time = 0;
                        res = true;
                    }
                    else
                    {
                        printError("open","Failed to setup ogg stream header contents");
                    }
                }
                else
                {
                    printError("open","Error decoding ogg stream contents");
                }
            }
            else
            {
                printError("open","Error opening bitstream from given file");
            }
        }
        else
        {
            printError("open","Failed to open given stream file");
        }
    }
    else
    {
        printError("open","Given file type not supported by codec");
    }
    
    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SilverCodec::close()
{
    if(m_stream!=0)
    {
        if(m_seeker!=0)
        {
            delete m_seeker;
            m_seeker = 0;
        }
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
        if(m_oggStream!=0)
        {
            delete m_oggStream;
            m_oggStream = 0;
        }
        m_stream->close();
        delete m_stream;
        m_stream = 0;
    }
    if(m_file!=0)
    {
        m_file->close();
        delete m_file;
        m_file = 0;
    }
    if(m_cachedFile!=0)
    {
        m_cachedFile->close();
        delete m_cachedFile;
        m_cachedFile = 0;
    }
    m_state = -1;
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::init()
{
    if(m_container!=0)
    {
        m_initFlag = true;
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::next(AData& data)
{
    tint i,len,total;
    engine::Sequence *seq;
    sample_t *buffer;
    bool res = true;
    engine::RData& rData = dynamic_cast<engine::RData &>(data);
    
    if(!rData.noParts())
    {    
        data.start() = m_time;
    }
        
    if(m_state>=0)
    {
        engine::RData::Part *part = &(rData.nextPart());
        
        buffer = rData.partData(rData.noParts() - 1);
        part->start() = m_time;
        
        i = 0;
        len = rData.rLength();
        
        while(i<len && res)
        {
            switch(m_state)
            {
                case 0:
                    {
                        seq = m_oggStream->next();
                        if(seq!=0)
                        {
                            if(m_window->setup(seq))
                            {
                                engine::silveromega::frameSynthesis(m_container,seq,m_window);
                                m_window->synthesis();
                                m_state = 1;
                            }
                        }
                        else
                        {
                            part->end() = m_time;
                            part->done() = true;
                            m_state = -1;
                            res = false;
                        }
                    }
                    break;
                
                case 1:
                    {
                        tint amount;
                        
                        amount = len - i;
                        total = m_window->getPCM(&buffer[i * noChannels()],amount);
                        m_time += (static_cast<tfloat64>(total) / static_cast<tfloat64>(m_container->m_information->m_audioSampleRate));
                        i += total;
                        if(i < len)
                        {
                            m_state = 0;
                        }
                    }
                    break;    
            }
        }
        
        part->length() = i;
        part->end() = m_time;
        part->done() = true;
        data.end() = m_time;
    }
    else
    {
        res = false;
    }
    
    return res;
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::isSeek() const
{
    return m_seekService;
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::seek(const common::TimeStamp& v)
{
    tuint offset = 0;
    common::TimeStamp seekT(v),actual;
    bool res = false;
    
    if(m_seekService && m_seeker!=0)
    {
        if(m_seeker->seek(seekT,actual,offset))
        {
            if(m_oggStream->seek(static_cast<tint>(offset)))
            {
                m_state = 0;
                m_time = actual;
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::isComplete() const
{
    return (m_state>=0) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::isRemote() const
{
    return m_remoteService;
}

//-------------------------------------------------------------------------------------------

bool SilverCodec::isBuffered(tfloat32& percent)
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint SilverCodec::frequency() const
{
    if(m_container!=0 && m_state>=0)
    {
        return m_container->m_information->m_audioSampleRate;
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

tint SilverCodec::noChannels() const
{
    return 2;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp SilverCodec::length() const
{
    common::TimeStamp len;
    
    if(m_seeker!=0)
    {
        len = m_seeker->totalTime();
    }
    return len;
}

//-------------------------------------------------------------------------------------------

tint SilverCodec::bitrate() const
{
    tint rate = 0;
    
    if(m_seeker!=0)
    {
        rate = m_seeker->bitrate();
    }
    return rate;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
