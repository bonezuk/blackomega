#include "common/inc/DiskOps.h"
#include "track/info/inc/FLACInfo.h"
#include "engine/greenomega/inc/FLACMetaPicture.h"
#include "engine/greenomega/inc/FLACMetaCuesheet.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

FLACInfo::FLACInfo() : CueInfo()
{
    m_type = Info::e_InfoFLAC;
}

//-------------------------------------------------------------------------------------------

FLACInfo::~FLACInfo()
{
    QMap<IDTagImageType,QPair<ImageFormat,ImageInfoArray *> >::iterator ppI;

    for(ppI=m_imageMap.begin();ppI!=m_imageMap.end();ppI++)
    {
        ImageInfoArray *pArr = ppI.value().second;
        delete pArr;
    }
    m_imageMap.clear();
}

//-------------------------------------------------------------------------------------------

void FLACInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FLACInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool FLACInfo::read(common::BIOStream *input)
{
    bool res = false;

    if(input->seek(0,common::e_Seek_Start))
    {
        engine::IOFile *file = new engine::IOFile(input);
        engine::greenomega::FLACFramework *frame = new engine::greenomega::FLACFramework;

        if(frame->init(file))
        {
            const engine::greenomega::FLACMetaComment *comments = frame->comments();

            m_length = frame->streamInfo()->length();

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

                readCuesheet(frame);
                readImages(frame);

                res = true;
            }

            if(m_chapters.isEmpty() && isSeparateCuesheet(input->name()))
            {
                readCueSheet(separateCueFilename(input->name()));
                if(!m_chapters.isEmpty())
                {
                    res = true;
                }
            }
        }
        else
        {
            printError("read","Could not read FLAC information");
        }
        delete frame;
        delete file;
    }
    else
    {
        printError("read","Error position file to start of file");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void FLACInfo::readCuesheet(engine::greenomega::FLACFramework *frameWork)
{
    tfloat64 frequency;
    QVector<engine::greenomega::FLACMetaBase *>& metaData = frameWork->metaData();
    QVector<engine::greenomega::FLACMetaBase *>::iterator ppI;

    if(frameWork->streamInfo()==0)
    {
        return;
    }
    frequency = static_cast<tfloat64>(frameWork->streamInfo()->m_frequency);

    for(ppI=metaData.begin();ppI!=metaData.end();ppI++)
    {
        engine::greenomega::FLACMetaBase *base = *ppI;

        if(base->type()==engine::greenomega::FLACMetaBase::e_Cuesheet)
        {
            engine::greenomega::FLACMetaCuesheet *cueSheet = dynamic_cast<engine::greenomega::FLACMetaCuesheet *>(base);

            if(cueSheet!=0)
            {
                QMap<tint,engine::greenomega::FLACMetaCuesheet::Track> trackMap;
                QMap<tint,engine::greenomega::FLACMetaCuesheet::Track>::iterator ppK;
                const QVector<engine::greenomega::FLACMetaCuesheet::Track>& tracks = cueSheet->tracks();
                QVector<engine::greenomega::FLACMetaCuesheet::Track>::const_iterator ppJ;

                for(ppJ=tracks.begin();ppJ!=tracks.end();ppJ++)
                {
                    const engine::greenomega::FLACMetaCuesheet::Track& t = *ppJ;

                    if(t.isAudio())
                    {
                        trackMap.insert(t.trackNumber(),t);
                    }
                }

                tint i;
                for(i=0,ppK=trackMap.begin();ppK!=trackMap.end();i++)
                {
                    ChildInfo c;
                    engine::greenomega::FLACMetaCuesheet::Track& t = ppK.value();

                    c.name() = "Track " + QString::number(i+1);
                    c.startTime() = static_cast<tfloat64>(t.trackOffset()) / frequency;
                    if(t.index1()>0 && !Info::playPreGap())
                    {
                        c.startTime() += static_cast<tfloat64>(t.index1()) / frequency;
                    }
                    else if(t.index0()>0)
                    {
                        c.startTime() += static_cast<tfloat64>(t.index0()) / frequency;
                    }

                    ppK++;
                    if((i+1)<trackMap.size())
                    {
                        engine::greenomega::FLACMetaCuesheet::Track& nT = ppK.value();
                        c.length() = static_cast<tfloat64>(nT.trackOffset()) / frequency;
                        if(nT.index0()>0)
                        {
                            c.length() += static_cast<tfloat64>(nT.index0()) / frequency;
                        }
                        else
                        {
                            c.length() += static_cast<tfloat64>(nT.index1()) / frequency;
                        }
                        c.length() -= c.startTime();
                        m_chapters.append(c);
                    }
                }
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void FLACInfo::readImages(engine::greenomega::FLACFramework *frameWork)
{
    QVector<engine::greenomega::FLACMetaBase *>& metaData = frameWork->metaData();
    QVector<engine::greenomega::FLACMetaBase *>::iterator ppI;

    for(ppI=metaData.begin();ppI!=metaData.end();ppI++)
    {
        engine::greenomega::FLACMetaBase *base = *ppI;

        if(base->type()==engine::greenomega::FLACMetaBase::e_Picture)
        {
            engine::greenomega::FLACMetaPicture *picture = dynamic_cast<engine::greenomega::FLACMetaPicture *>(base);

            if(picture!=0)
            {
                IDTagImageType type;
                ImageFormat format;

                switch(picture->imageFormat())
                {
                    case engine::greenomega::FLACMetaPicture::e_imageJPEG:
                        format = e_imageJPEG;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_imageGIF:
                        format = e_imageGIF;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_imagePNG:
                        format = e_imagePNG;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_imageBMP:
                        format = e_imageBMP;
                        break;
                    default:
                        format = e_imageUnknown;
                        break;
                }

                switch(picture->imageType())
                {
                    case engine::greenomega::FLACMetaPicture::e_Other:
                        type = e_TagImage_Other;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Icon:
                        type = e_TagImage_Icon32x32;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_IconFile:
                        type = e_TagImage_OtherIcon;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_CoverFront:
                        type = e_TagImage_CoverFront;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_CoverBack:
                        type = e_TagImage_CoverBack;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_LeafletPage:
                        type = e_TagImage_Leaflet;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Media:
                        type = e_TagImage_Media;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_LeadArtist:
                        type = e_TagImage_LeadArtist;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Artist:
                        type = e_TagImage_Artist;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Conductor:
                        type = e_TagImage_Conductor;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Band:
                        type = e_TagImage_Band;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Composer:
                        type = e_TagImage_Composer;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Lyricist:
                        type = e_TagImage_Lyricist;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_RecordingLocation:
                        type = e_TagImage_RecLocation;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_DuringRecording:
                        type = e_TagImage_Recording;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_DuringPerformance:
                        type = e_TagImage_Performance;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Movie:
                        type = e_TagImage_VidCaptute;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Fish:
                        type = e_TagImage_Fish;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_Illustration:
                        type = e_TagImage_Illustration;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_BandLogo:
                        type = e_TagImage_BandLogo;
                        break;
                    case engine::greenomega::FLACMetaPicture::e_PublisherLogo:
                        type = e_TagImage_StudioLogo;
                        break;
                    default:
                        type = e_TagImage_Other;
                        break;
                }

                if(format!=e_imageUnknown && m_imageMap.find(type)==m_imageMap.end())
                {
                    ImageInfoArray *arr = new ImageInfoArray();
                    arr->Append(*(picture->image()));
                    m_imageMap.insert(type,QPair<ImageFormat,ImageInfoArray *>(format,arr));
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool FLACInfo::isImage() const
{
    if(m_imageMap.size()>0)
    {
        return true;
    }
    return Info::isImage();
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *FLACInfo::getImageData(ImageFormat& format) const
{
    static IDTagImageType typeOrder[13] = {
        e_TagImage_CoverFront,
        e_TagImage_LeadArtist,
        e_TagImage_Artist,
        e_TagImage_Conductor,
        e_TagImage_Band,
        e_TagImage_Lyricist,
        e_TagImage_Illustration,
        e_TagImage_BandLogo,
        e_TagImage_StudioLogo,
        e_TagImage_CoverBack,
        e_TagImage_Composer,
        e_TagImage_Performance,
        e_TagImage_Other};

    tint i;
    ImageInfoArray *pArr = 0;

    for(i=0;i<13 && pArr==0;i++)
    {
        pArr = getImageData(typeOrder[i],format);
    }
    if(pArr==0)
    {
        pArr = Info::getImageData(format);
    }
    return pArr;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *FLACInfo::getImageData(IDTagImageType type,ImageFormat& format) const
{
    ImageInfoArray *pArr = 0;
    QMap<IDTagImageType,QPair<ImageFormat,ImageInfoArray *> >::const_iterator ppI;

    ppI = m_imageMap.find(type);
    if(ppI!=m_imageMap.end())
    {
        const QPair<ImageFormat,ImageInfoArray *>& p = ppI.value();
        format = p.first;
        pArr = p.second;
    }
    return pArr;
}

//-------------------------------------------------------------------------------------------

bool FLACInfo::canGetTrackLength() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp FLACInfo::getTrackLength()
{
    return m_length;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
