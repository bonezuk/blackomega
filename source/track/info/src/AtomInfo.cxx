#include "track/info/inc/AtomInfo.h"
#include "engine/whiteomega/inc/Atom.h"
#include "engine/inc/Codec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------
// AtomInfo
//-------------------------------------------------------------------------------------------

AtomInfo::AtomInfo() : Info(),
    m_chapters(),
    m_coverFormat(e_imageUnknown),
    m_coverArray(0)
{
    m_type = Info::e_InfoMP4Atom;
}

//-------------------------------------------------------------------------------------------

AtomInfo::~AtomInfo()
{
    if(m_coverArray!=0)
    {
        delete m_coverArray;
        m_coverArray = 0;
    }
}

//-------------------------------------------------------------------------------------------

void AtomInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "AtomInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool AtomInfo::read(common::BIOStream *input)
{
    bool res = false;
    
    if(input->seek(0,common::e_Seek_Start))
    {
        engine::IOFile *file = new engine::IOFile(input);
        engine::whiteomega::Atom *atomic = new engine::whiteomega::Atom;
        
        if(atomic->readMP4File(file))
        {
            m_Artist = getAtomData("artist",atomic);
            m_Title = getAtomData("title",atomic);
            m_Album = getAtomData("album",atomic);
            m_Comment = getAtomData("comment",atomic);
            m_Genre = getAtomData("genre",atomic);
            m_Track = getAtomData("track",atomic);
            m_Disc = getAtomData("disc",atomic);
            m_Composer = getAtomData("writer",atomic);
            m_OriginalArtist = getAtomData("artist",atomic);
            m_Encoder = getAtomData("tool",atomic);

            {
                tint i;
                QString aDate = getAtomData("date",atomic).trimmed();

                for(i=0;i<aDate.length();i++)
                {
                    if(!(aDate.at(i)>=QChar('0') && aDate.at(i)<=QChar('9')))
                    {
                        m_Year = aDate.mid(0,i);
                        break;
                    }
                }
                if(i==aDate.length())
                {
                    m_Year = aDate;
                }
            }
            
            if(atomic->m_coverArtArray!=0)
            {
                m_coverFormat = e_imageJPEG;
                m_coverArray = atomic->m_coverArtArray;
                atomic->m_coverArtArray = 0;
            }

            // Get chapter bookmarks for audio book.
            if(atomic->m_tracks.size()>=2)
            {
                tint i,j;
                
                for(i=0;i<atomic->m_tracks.size();++i)
                {
                    engine::whiteomega::Atom::Track *track = atomic->m_tracks.at(i);
                    
                    if(track->m_type==engine::whiteomega::Atom::Track::e_track_text && track->m_sttsEntryCount>=0)
                    {
                        tuint32 cStart = 0,cEnd = 0;
                        ChildInfo ch;
                        
                        m_chapters.clear();
                        for(j=0;j<track->m_sttsEntryCount;++j)
                        {
                            cEnd += track->m_sttsSampleDelta[j];
                            
                            ch.startTime() = static_cast<tfloat64>(cStart) / static_cast<tfloat64>(track->m_timeScale);
                            ch.length() = static_cast<tfloat64>(cEnd - cStart) / static_cast<tfloat64>(track->m_timeScale);
                            ch.name() = "Chapter " + QString::number(j + 1);
                            m_chapters.append(ch);
                            cStart = cEnd;
                        }                        
                        break;
                    }
                }
            }

            res = true;
        }
        else
        {
            printError("read","Failed to reading information from MP4 file");
        }
        
        delete atomic;
        delete file;
    }
    else
    {
        printError("read","Error position file to start of file");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString AtomInfo::getAtomData(const tchar *name,engine::whiteomega::Atom *a)
{
    return getAtomData(QString::fromUtf8(name),a);
}

//-------------------------------------------------------------------------------------------

QString AtomInfo::getAtomData(const QString& name,engine::whiteomega::Atom *a)
{
    QString d;
    
    if(a!=0)
    {
        QMap<QString,QString>::const_iterator ppI;
        
        ppI = a->m_metadata.find(name);
        if(ppI!=a->m_metadata.end())
        {
            d = ppI.value();
        }
    }
    return d;
}

//-------------------------------------------------------------------------------------------

bool AtomInfo::isImage() const
{
    if(m_coverArray!=0)
    {
        return true;
    }
    else
    {
        return Info::isImage();
    }
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *AtomInfo::getImageData(ImageFormat& format) const
{
    if(m_coverArray!=0)
    {
        format = m_coverFormat;
        return m_coverArray;
    }
    else
    {
        return Info::getImageData(format);
    }
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *AtomInfo::getImageData(IDTagImageType type,ImageFormat& format) const
{
    if(type==e_TagImage_CoverFront)
    {
        return getImageData(format);
    }
    else
    {
        return Info::getImageData(format);
    }
}

//-------------------------------------------------------------------------------------------

bool AtomInfo::isChildren() const
{
    return (m_chapters.size()>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

tint AtomInfo::noChildren() const
{
    return m_chapters.size();
}

//-------------------------------------------------------------------------------------------

const Info::ChildInfo& AtomInfo::child(tint idx) const
{
    return m_chapters.at(idx);
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
