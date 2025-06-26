#include "engine/silveromega/inc/VSilverContainer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverContainer::VSilverContainer() : m_information(0),
    m_comments(0),
    m_data(0)
{}

//-------------------------------------------------------------------------------------------

VSilverContainer::~VSilverContainer()
{
    try
    {
        if(m_information!=0)
        {
            delete m_information;
            m_information = 0;
        }
        if(m_comments!=0)
        {
            delete m_comments;
            m_comments = 0;
        }
        if(m_data!=0)
        {
            delete m_data;
            m_data = 0;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverContainer::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverContainer::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverContainer::read(VSilverOgg *stream)
{
    tint type,i;
    tchar tmp[6];
    engine::Sequence *seq;
    VSilverInfoObject *info = 0;

    if(stream==0)
    {
        printError("read","No ogg packet streamer given");
        return false;
    }

    while(m_information==0 || m_data==0)
    {
        seq = stream->next();
        if(seq==0)
        {
            printError("read","Failed to get next packet sequence");
            return false;
        }
        type = seq->readBits(8);
        for(i=0;i<6;++i)
        {
            tmp[i] = static_cast<tchar>(seq->readBits(8));
        }
        if(!seq->seek(-56))
        {
            printError("read","Failed to rewind sequence to start of packet");
            return false;
        }
        if(::memcmp("vorbis",tmp,6)==0)
        {
            switch(type)
            {
                case 1:
                    {
                        m_information = new VSilverCodecInformation;
                        info = reinterpret_cast<VSilverInfoObject *>(m_information);
                    }
                    break;

                case 3:
                    {
                        m_comments = new VSilverCodecComments;
                        info = reinterpret_cast<VSilverInfoObject *>(m_comments);
                    }
                    break;

                case 5:
                    {
                        if(m_information==0)
                        {
                            printError("read","Codec information required before codebook data can be read");
                            return false;
                        }
                        m_data = new VSilverCodecData(m_information);
                        info = reinterpret_cast<VSilverInfoObject *>(m_data);
                    }
                    break;

                default:
                    break;
            }

            if(info!=0)
            {
                if(!info->read(seq))
                {
                    printError("read","Failed to read in packet information");
                    return false;
                }
                if(!info->isValid())
                {
                    printError("read","Failed to validate packet information");
                    return false;
                }
                info = 0;
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverContainer::setup()
{
    if(m_data==0)
    {
        return false;
    }
    if(!m_data->setup())
    {
        printError("setup","Failed to setup codec data");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverContainer::readInformation(Sequence *seq)
{
    if(m_information!=0)
    {
        delete m_information;
    }
    m_information = new VSilverCodecInformation;
    if(!m_information->read(seq))
    {
        printError("readInformation","Failed to read in vorbis information packet");
        return false;
    }
    if(!m_information->isValid())
    {
        printError("readInfomation","Failed to validate vorbis information packet");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverContainer::readComments(Sequence *seq)
{
    if(m_comments!=0)
    {
        delete m_comments;
    }
    m_comments = new VSilverCodecComments;
    if(!m_comments->read(seq))
    {
        printError("readComments","Failed to read in vorbis comments packet");
        return false;
    }
    if(!m_comments->isValid())
    {
        printError("readComments","Failed to validate vorbis comments packet");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverContainer::readData(Sequence *seq)
{
    if(m_data!=0)
    {
        delete m_data;
    }
    m_data = new VSilverCodecData(m_information);
    if(!m_data->read(seq))
    {
        printError("readData","Failed to read in vorbis data packet");
        return false;
    }
    if(!m_data->isValid())
    {
        printError("readData","Failed to validate vorbis data packet");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

