#include "track/info/inc/ID3Info1.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

const tchar *ID3Info1::m_ID3Genres[147] = {
    "Blues","Classic Rock","Country","Dance"
    "Disco","Funk","Grunge","Hip-Hop",
    "Jazz","Metal","New Age","Oldies",
    "Other","Pop","R&B","Rap","Reggae",
    "Rock","Techno","Industrial","Alternative",
    "Ska","Death Metal","Pranks","Soundtrack",
    "Euro-Techno","Ambient","Trip-Hop","Vocal",
    "Jazz+Funk","Fusion","Trance","Classical",
    "Instrumental","Acid","House","Game",
    "Sound Clip","Gospel","Noise","Alt",
    "Bass","Soul","Punk","Space",
    "Meditative","Instrumental Pop",
    "Instrumental Rock","Ethnic","Gothic",
    "Darkwave","Techno-Industrial","Electronic",
    "Pop-Folk","Eurodance","Dream",
    "Southern Rock","Comedy","Cult",
    "Gangsta Rap","Top 40","Christian Rap",
    "Pop/Funk","Jungle","Native American",
    "Cabaret","New Wave","Psychedelic","Rave",
    "Showtunes","Trailer","Lo-Fi","Tribal",
    "Acid Punk","Acid Jazz","Polka","Retro",
    "Musical","Rock & Roll","Hard Rock","Folk",
    "Folk/Rock","National Folk","Swing",
    "Fast-Fusion","Bebob","Latin","Revival",
    "Celtic","Bluegrass","Avantgarde",
    "Gothic Rock","Progressive Rock",
    "Psychedelic Rock","Symphonic Rock","Slow Rock",
    "Big Band","Chorus","Easy Listening",
    "Acoustic","Humour","Speech","Chanson",
    "Opera","Chamber Music","Sonata","Symphony",
    "Booty Bass","Primus","Porn Groove",
    "Satire","Slow Jam","Club","Tango",
    "Samba","Folklore","Ballad","Power Ballad",
    "Rhythmic Soul","Freestyle","Duet",
    "Punk Rock","Drum Solo","A Cappella",
    "Euro-House","Dance Hall","Goa",
    "Drum & Bass","Club-House","Hardcore",
    "Terror","Indie","BritPop","Negerpunk",
    "Polsk Punk","Beat","Christian Gangsta Rap",
    "Heavy Metal","Black Metal","Crossover",
    "Contemporary Christian","Christian Rock",
    "Merengue","Salsa","Thrash Metal",
    "Anime","JPop","Synthpop"
};

//-------------------------------------------------------------------------------------------

ID3Info1::ID3Info1() : Info()
{
    m_type = Info::e_InfoID3Type1;
}

//-------------------------------------------------------------------------------------------

ID3Info1::~ID3Info1()
{}

//-------------------------------------------------------------------------------------------

void ID3Info1::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ID3Info1::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void ID3Info1::extractData(QString& str,tbyte *tag,tbyte *mem,tint start,tint end)
{
    tint i = end - start;

    ::memcpy(mem,&tag[start],i);
    mem[i] = '\0';
    while(i>=0)
    {
        if(mem[i]==' ' || mem[i]=='\n')
        {
            mem[i] = '\0';
        }
        else
        {
            break;
        }
        i--;
    }
    str = QString::fromLatin1(mem).trimmed();
}

//-------------------------------------------------------------------------------------------

void ID3Info1::toString(QString& str,int n)
{
    str = QString::number(n);
}

//-------------------------------------------------------------------------------------------

bool ID3Info1::read(common::BIOStream *input)
{
    tint t;
    tbyte tag[128],mem[128];

    if(input==0)
    {
        printError("read","No input to read from given");
        return false;
    }
    if(input->read(tag,128)!=128)
    {
        printError("read","Failed to read in expected tag");
        return false;
    }

    extractData(m_Title,tag,mem,3,33);
    m_Title = m_Title.trimmed();
    extractData(m_Artist,tag,mem,33,63);
    m_Artist = m_Artist.trimmed();
    extractData(m_Album,tag,mem,63,93);
    m_Album = m_Album.trimmed();
    extractData(m_Year,tag,mem,93,97);
    m_Year = m_Year.trimmed();
    if(tag[126]=='\0' && tag[127]!='\0')
    {
        extractData(m_Comment,tag,mem,97,126);
        m_Comment = m_Comment.trimmed();
        t = static_cast<tint>(tag[126]);
        toString(m_Track,t);
    }
    else
    {
        extractData(m_Comment,tag,mem,97,127);
        m_Comment = m_Comment.trimmed();
    }
    m_OriginalArtist = m_Artist;
    m_Composer = m_Artist;
    t = static_cast<tint>(static_cast<tubyte>(tag[127]));
    if(t>0 && t<=147)
    {
        m_Genre = QString::fromLatin1(m_ID3Genres[t - 1]);
    }
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
