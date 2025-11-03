#include "track/info/inc/EmbeddedID3Info.h"
#include "track/info/inc/ID3Info1.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

EmbeddedID3Info::EmbeddedID3Info() : CueInfo(),
    m_id3Tag(0)
{}

//-------------------------------------------------------------------------------------------

EmbeddedID3Info::~EmbeddedID3Info()
{
    if(m_id3Tag!=0)
    {
        delete m_id3Tag;
        m_id3Tag = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool EmbeddedID3Info::isImage() const
{
    bool res;
    
    if(m_id3Tag!=0)
    {
        res = m_id3Tag->isImage();
    }
    else
    {
        res = Info::isImage();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *EmbeddedID3Info::getImageData(ImageFormat& format) const
{
    ImageInfoArray *pArray;
    
    if(m_id3Tag!=0)
    {
        pArray = m_id3Tag->getImageData(format);
    }
    else
    {
        pArray = Info::getImageData(format);
    }
    return pArray;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *EmbeddedID3Info::getImageData(IDTagImageType type,ImageFormat& format) const
{
    ImageInfoArray *pArray;
    
    if(m_id3Tag!=0)
    {
        pArray = m_id3Tag->getImageData(type,format);
    }
    else
    {
        pArray = Info::getImageData(type,format);
    }
    return pArray;
}

//-------------------------------------------------------------------------------------------

void EmbeddedID3Info::readID3(common::BIOStream *file)
{
    if(file!=0 && file->seek(0,common::e_Seek_Start))
    {
        ID3Info2 *id2Info = new ID3Info2;
        bool id2Flag = false;
        
        if(id2Info->read(file))
        {
            copyInfo(id2Info);
            id2Flag = true;
        }
        else if(file->seek(0,common::e_Seek_Start))
        {
            ID3Info1 id1Info;
            if(id1Info.read(file))
            {
                copyInfo(&id1Info);
            }
        }
        
        if(id2Flag)
        {
            if(m_id3Tag!=0)
            {
                delete m_id3Tag;
            }
            m_id3Tag = id2Info;
        }
        else
        {
            delete id2Info;
        }
    }
}

//-------------------------------------------------------------------------------------------

void EmbeddedID3Info::copyInfo(Info *pInfo)
{
    artist() = pInfo->artist();
    title() = pInfo->title();
    album() = pInfo->album();
    year() = pInfo->year();
    comment() = pInfo->comment();
    genre() = pInfo->genre();
    track() = pInfo->track();
    disc() = pInfo->disc();
    composer() = pInfo->composer();
    originalArtist() = pInfo->originalArtist();
    copyright() = pInfo->copyright();
    encoder() = pInfo->encoder();
    year() = pInfo->year();
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
