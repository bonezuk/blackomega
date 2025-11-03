#include "track/info/inc/VorbisInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

VorbisInfo::VorbisInfo() : Info()
{
    m_type = Info::e_InfoVorbis;
}

//-------------------------------------------------------------------------------------------

VorbisInfo::~VorbisInfo()
{}

//-------------------------------------------------------------------------------------------

void VorbisInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VorbisInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VorbisInfo::read(common::BIOStream *input)
{
    bool res = false;

    if(input->seek(0,common::e_Seek_Start))
    {
        engine::IOFile *file = new engine::IOFile(input);
        engine::Bitstream *stream = new engine::Bitstream;

        if(stream->open(file))
        {
            tint i,type;
            tchar tmp[6];
            engine::silveromega::VSilverOgg *oggStream = new engine::silveromega::VSilverOgg(stream);
            engine::silveromega::VSilverCodecInformation *info = 0;
            engine::silveromega::VSilverCodecComments *comments = 0;
            engine::Sequence *seq;
            bool loop = true;

            while(loop && comments==0)
            {
                seq = oggStream->next();
                if(seq==0)
                {
                    break;
                }

                type = seq->readBits(8);
                for(i=0;i<6;++i)
                {
                    tmp[i] = static_cast<tchar>(seq->readBits(8));
                }
                if(!seq->seek(-56))
                {
                    break;
                }

                if(::memcmp("vorbis",tmp,6)==0)
                {
                    switch(type)
                    {
                        case 1:
                            info = new engine::silveromega::VSilverCodecInformation;
                            if(!info->read(seq))
                            {
                                loop = false;
                            }
                            break;

                        case 3:
                            comments = new engine::silveromega::VSilverCodecComments;
                            if(comments->read(seq))
                            {
                                loop = false;
                            }
                            break;

                        default:
                            loop = false;
                            break;
                    }
                }
            }

            if(comments!=0)
            {
                if(comments->performer().isEmpty())
                {
                    m_Artist = comments->artist();
                }
                else
                {
                    m_Artist = comments->performer();
                }
                m_Title = comments->title();
                m_Album = comments->album();
                m_Year = comments->date();
                m_Comment = comments->description();
                m_Genre = comments->genre();
                m_Track = comments->trackNumber();
                m_Composer = comments->artist();
                m_OriginalArtist = comments->artist();
                m_Copyright = comments->copyright();
                m_Encoder = comments->version();
                m_Disc = comments->diskNumber();

                delete comments;

                res = true;
            }
            if(info!=0)
            {
                delete info;
            }
            delete oggStream;
        }
        else
        {
            printError("read","Error opening bitstream from input file");
        }
        delete stream;
        delete file;
    }
    else
    {
        printError("read","Error position file to start of file");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
