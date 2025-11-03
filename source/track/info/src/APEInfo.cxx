#include "engine/inc/Codec.h"
#include "track/info/inc/APEInfo.h"
#include "track/info/inc/MPCChapterTag.h"
#include "track/info/inc/FileInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

APEInfo::APEInfo() : CueInfo(),
    m_imageArray(0)
{
    m_type = e_InfoAPE;
}

//-------------------------------------------------------------------------------------------

APEInfo::~APEInfo()
{
    if(m_imageArray!=0)
    {
        delete m_imageArray;
        m_imageArray = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool APEInfo::read(common::BIOStream *input)
{
    bool res = false;

    if(input!=0)
    {
        APETag tag;
        
        if(tag.read(input))
        {
            for(QVector<APETagItem>::const_iterator ppI=tag.items().begin();ppI!=tag.items().end();ppI++)
            {
                const APETagItem& item = *ppI;
                
                if(item.key().toLower().trimmed()=="cuesheet")
                {
                    readCueSheet(item.data());
                }
                else if(item.isString())
                {
                    if(item.key().toLower().trimmed()=="album")
                    {
                        m_Album = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="artist")
                    {
                        m_Artist = item.text();
                        if(m_OriginalArtist.isEmpty())
                        {
                            m_OriginalArtist = m_Artist;
                        }
                        if(m_Composer.isEmpty())
                        {
                            m_Composer = m_Artist;
                        }
                    }
                    else if(item.key().toLower().trimmed()=="comment")
                    {
                        m_Comment = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="genre")
                    {
                        m_Genre = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="title")
                    {
                        m_Title = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="track")
                    {
                        m_Track = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="year")
                    {
                        m_Year = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="albumartist")
                    {
                        m_OriginalArtist = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="composer")
                    {
                        m_Composer = item.text();
                    }
                    else if(item.key().toLower().trimmed()=="discnumber")
                    {
                        m_Disc = item.text();
                    }
                }
                else if(m_imageArray==0 && item.isData())
                {
                    APETagImage imageItem(item);
                    if(imageItem.isImage())
                    {
                        m_imageArray = imageItem.imageData();
                    }
                }
            }
            
            res = !m_Title.isEmpty();
        }

        if(isMusepack(input->name()) && m_chapters.isEmpty())
        {
            MPCChapterTag chapterTag;
            if(chapterTag.read(input))
            {
                for(tint i = 0; i < chapterTag.items().size(); i++)
                {
                    const MPCChapterTagItem &item = chapterTag.items().at(i);
                    ChildInfo cInfo;

                    cInfo.name() = item.title();
                    cInfo.startTime() = item.time();
                    if((i + 1) < chapterTag.items().size())
                    {
                        cInfo.length() = chapterTag.items().at(i + 1).time();
                    }
                    else
                    {
                        cInfo.length() = chapterTag.length();
                    }
                    cInfo.length() -= cInfo.startTime();
                    m_chapters.append(cInfo);

                    if(m_Artist.isEmpty())
                    {
                        m_Artist = item.performer();
                    }
                    if(m_OriginalArtist.isEmpty())
                    {
                        m_OriginalArtist = item.songWriter();
                    }
                    if(m_Composer.isEmpty())
                    {
                        m_Composer = item.composer();
                    }
                }

                if(m_Title.isEmpty() || m_Album.isEmpty())
                {
                    if(m_Title.isEmpty() && m_Album.isEmpty())
                    {
                        FileInfo fileInfo;
                        if(fileInfo.read(input))
                        {
                            m_Title = fileInfo.title();
                            m_Album = fileInfo.album();
                        }
                    }
                    else if(m_Title.isEmpty())
                    {
                        m_Title = m_Album;
                    }
                    else
                    {
                        m_Album = m_Title;
                    }
                }
                                
                res = true;
            }
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
    return res;
}

//-------------------------------------------------------------------------------------------

bool APEInfo::isImage() const
{
    return (m_imageArray!=0);
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *APEInfo::getImageData(ImageFormat& format) const
{
    format = e_imageUnknown;
    return m_imageArray;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *APEInfo::getImageData(IDTagImageType type,ImageFormat& format) const
{
    return getImageData(format);
}

//-------------------------------------------------------------------------------------------

bool APEInfo::isMusepack(const QString& name) const
{
    QString ext = engine::Codec::getFileExtension(name);
    return (ext=="mpc" || ext=="mp+" || ext=="mpp");
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
