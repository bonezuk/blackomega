#include "engine/whiteomega/inc/Atom.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

const char *Atom::m_ID3Genres[] = {
    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
    "Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies",
    "Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
    "Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks",
    "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
    "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House",
    "Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass",
    "Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
    "Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
    "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
    "Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret",
    "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
    "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical",
    "Rock & Roll", "Hard Rock", "Folk", "Folk/Rock", "National Folk", "Swing",
    "Fast-Fusion", "Bebob", "Latin", "Revival", "Celtic", "Bluegrass", "Avantgarde",
    "Gothic Rock", "Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band",
    "Chorus", "Easy Listening", "Acoustic", "Humour", "Speech", "Chanson",
    "Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus",
    "Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
    "Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet",
    "Punk Rock", "Drum Solo", "A capella", "Euro-House", "Dance Hall",
    "Goa", "Drum & Bass", "Club House", "Hardcore", "Terror",
    "Indie", "BritPop", "NegerPunk", "Polsk Punk", "Beat",
    "Christian Gangsta", "Heavy Metal", "Black Metal", "Crossover", "Contemporary C",
    "Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime", "JPop",
    "SynthPop",
};

//-------------------------------------------------------------------------------------------
// AtomException
//-------------------------------------------------------------------------------------------

AtomException::AtomException(const tchar *sFunc,const tchar *msg) : common::Exception("Atom",sFunc,msg)
{}

//-------------------------------------------------------------------------------------------
//AtomFile
//-------------------------------------------------------------------------------------------

AtomFile::AtomFile(File *in) : m_file(in)
{
    if(in==0)
    {
        throw AtomException("AtomFile","No file given to read from");
    }
}

//-------------------------------------------------------------------------------------------

AtomFile::~AtomFile()
{}

//-------------------------------------------------------------------------------------------

void AtomFile::read(tbyte *mem,tint len)
{
    if(m_file->read(mem,len)!=len)
    {
        throw AtomException("read","Failed to read expected data");
    }
}

//-------------------------------------------------------------------------------------------

void AtomFile::read(tubyte *mem,tint len)
{
    read(reinterpret_cast<tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

bool AtomFile::seek(tint offset,File::Position position)
{
    if(!m_file->seek(offset,position))
    {
        throw AtomException("seek","Error finding expected file position");
    }
    else
    {
        return true;
    }
}

//-------------------------------------------------------------------------------------------

tint AtomFile::length() const
{
    return m_file->length();
}

//-------------------------------------------------------------------------------------------

tint AtomFile::offset() const
{
    return m_file->offset();
}

//-------------------------------------------------------------------------------------------
// Atom::Track
//-------------------------------------------------------------------------------------------

Atom::Track::Track() : m_type(e_track_unknown),
    m_channelCount(0),
    m_sampleSize(0),
    m_sampleRate(0),
    m_audioType(0),
    m_maxBitrate(0),
    m_avgBitrate(0),
    m_timeScale(0),
    m_duration(0),
    m_stsdEntryCount(0),
    m_stszSampleSize(0),
    m_stszSampleCount(0),
    m_stszTable(0),
    m_sttsEntryCount(0),
    m_sttsSampleCount(0),
    m_sttsSampleDelta(0),
    m_stscEntryCount(0),
    m_stscFirstChunk(0),
    m_stscSamplesPerChunk(0),
    m_stscSampleDescIndex(0),
    m_stcoEntryCount(0),
    m_stcoChunkOffset(0),
    m_cttsEntryCount(0),
    m_cttsSampleCount(0),
    m_cttsSampleOffset(0),
    m_decoderConfigLen(0),
    m_decoderConfig(0),
    m_alacFrameLength(0),
    m_alacCompatibleVersion(0),
    m_alacBitDepth(0),
    m_alacPb(0),
    m_alacMb(0),
    m_alacKb(0),
    m_alacNumChannels(0),
    m_alacMaxRun(0),
    m_alacMaxFrameBytes(0),
    m_alacAvgBitRate(0),
    m_alacSampleRate(0),
    m_sampleLookup(0)
{}

//-------------------------------------------------------------------------------------------

Atom::Track::~Track()
{
    try
    {
        if(m_stszTable!=0)
        {
            delete [] m_stszTable;
            m_stszTable = 0;
        }
        if(m_sttsSampleCount!=0)
        {
            delete [] m_sttsSampleCount;
            m_sttsSampleCount = 0;
        }
        if(m_sttsSampleDelta!=0)
        {
            delete [] m_sttsSampleDelta;
            m_sttsSampleDelta = 0;
        }
        if(m_stscFirstChunk!=0)
        {
            delete [] m_stscFirstChunk;
            m_stscFirstChunk = 0;
        }
        if(m_stscSamplesPerChunk!=0)
        {
            delete [] m_stscSamplesPerChunk;
            m_stscSamplesPerChunk = 0;
        }
        if(m_stscSampleDescIndex!=0)
        {
            delete [] m_stscSampleDescIndex;
            m_stscSampleDescIndex = 0;
        }
        if(m_stcoChunkOffset!=0)
        {
            delete [] m_stcoChunkOffset;
            m_stcoChunkOffset = 0;
        }
        if(m_cttsSampleCount!=0)
        {
            delete [] m_cttsSampleCount;
            m_cttsSampleCount = 0;
        }
        if(m_cttsSampleOffset!=0)
        {
            delete [] m_cttsSampleOffset;
            m_cttsSampleOffset = 0;
        }
        if(m_decoderConfig!=0)
        {
            delete [] m_decoderConfig;
            m_decoderConfig = 0;
        }
        if(m_sampleLookup!=0)
        {
            delete [] m_sampleLookup;
            m_sampleLookup = 0;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------
// Atom
//-------------------------------------------------------------------------------------------

Atom::Atom() : m_moovRead(false),
    m_moovOffset(0),
    m_moovSize(0),
    m_timeScale(0),
    m_duration(0),
    m_tracks(),
    m_metadata(),
    m_coverArtArray(0)
{}

//-------------------------------------------------------------------------------------------

Atom::~Atom()
{
    try
    {
        clear();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void Atom::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Atom::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void Atom::clear()
{
    while(m_tracks.size() > 0)
    {
        Track *t = m_tracks.takeFirst();
        delete t;
    }
    m_metadata.clear();
    m_moovRead = false;
    m_moovOffset = 0;
    m_moovSize = 0;
    if(m_coverArtArray!=0)
    {
        delete m_coverArtArray;
        m_coverArtArray = 0;
    }
}

//-------------------------------------------------------------------------------------------

tbyte Atom::readByte(AtomFile *in)
{
    return static_cast<tbyte>(readUByte(in));
}

//-------------------------------------------------------------------------------------------

tubyte Atom::readUByte(AtomFile *in)
{
    tubyte x;
    
    in->read(&x,1);
    return x;
}

//-------------------------------------------------------------------------------------------

tint16 Atom::readInt16(AtomFile *in)
{
    return static_cast<tint16>(readUInt16(in));
}

//-------------------------------------------------------------------------------------------

tuint16 Atom::readUInt16(AtomFile *in)
{
    tuint16 x;
    tubyte y[2];
    
    in->read(y,2);
    x = ((static_cast<tuint16>(y[0]) << 8) & 0xff00) |
        ((static_cast<tuint16>(y[1])     ) & 0x00ff);
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 Atom::readInt24(AtomFile *in)
{
    return static_cast<tint32>(readUInt24(in));
}

//-------------------------------------------------------------------------------------------

tuint32 Atom::readUInt24(AtomFile *in)
{
    tuint32 x;
    tubyte y[3];
    
    in->read(y,3);
    x = ((static_cast<tuint32>(y[0]) << 16) & 0x00ff0000) |
        ((static_cast<tuint32>(y[1]) <<  8) & 0x0000ff00) |
        ((static_cast<tuint32>(y[2])      ) & 0x000000ff);
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 Atom::readInt32(AtomFile *in)
{
    return static_cast<tint32>(readUInt32(in));
}

//-------------------------------------------------------------------------------------------

tuint32 Atom::readUInt32(AtomFile *in)
{
    tuint32 x;
    tubyte y[4];
    
    in->read(y,4);
    x = ((static_cast<tuint32>(y[0]) << 24) & 0xff000000) |
        ((static_cast<tuint32>(y[1]) << 16) & 0x00ff0000) |
        ((static_cast<tuint32>(y[2]) <<  8) & 0x0000ff00) |
        ((static_cast<tuint32>(y[3])      ) & 0x000000ff);
    return x;
}

//-------------------------------------------------------------------------------------------

tint64 Atom::readInt64(AtomFile *in)
{
    return static_cast<tint64>(readUInt64(in));
}

//-------------------------------------------------------------------------------------------

tuint64 Atom::readUInt64(AtomFile *in)
{
    tuint64 x;
    tubyte y[8];
    
    in->read(y,8);
    x = ((static_cast<tuint64>(y[0]) << 56) & 0xff00000000000000ULL) |
        ((static_cast<tuint64>(y[1]) << 48) & 0x00ff000000000000ULL) |
        ((static_cast<tuint64>(y[2]) << 40) & 0x0000ff0000000000ULL) |
        ((static_cast<tuint64>(y[3]) << 32) & 0x000000ff00000000ULL) |
        ((static_cast<tuint64>(y[4]) << 24) & 0x00000000ff000000ULL) |
        ((static_cast<tuint64>(y[5]) << 16) & 0x0000000000ff0000ULL) |
        ((static_cast<tuint64>(y[6]) <<  8) & 0x000000000000ff00ULL) |
        ((static_cast<tuint64>(y[7])      ) & 0x00000000000000ffULL);
    return x;
}

//-------------------------------------------------------------------------------------------

QString Atom::readString(AtomFile *in,tint len)
{
    QString s;
    tchar *tmp = new tchar [len + 1];
    in->read(tmp,len);
    tmp[len] = '\0';
    s = QString::fromUtf8(tmp);
    delete [] tmp;
    return s;
}

//-------------------------------------------------------------------------------------------

tint Atom::readHeader(AtomFile *in,Type& t)
{
    tint hdrSize = 0;
    return readHeader(in,t,hdrSize);
}

//-------------------------------------------------------------------------------------------

tint Atom::readHeader(AtomFile *in,Type& t,tint& hdrSize)
{
    tuint32 ID;
    tint size = -1;

    size = readInt32(in);
    ID = readUInt32(in);
    if(size==1)
    {
        size = static_cast<tint>(readInt64(in));
        hdrSize = 16;
    }
    else
    {
        hdrSize = 8;
    }
    t = getAtomType(ID);

    return size;
}

//-------------------------------------------------------------------------------------------

void Atom::readAtom(AtomFile *in,Type t,tint size)
{
    tint destPosition = in->offset() + size - 8;
    
    switch(t)
    {
        case e_atom_stsz:
            // sample size box
            readSTSZ(in);
            break;
            
        case e_atom_ctts:
            // time to sample box
            readCTTS(in);
            break;
            
        case e_atom_stsc:
            // sample to chunk box
            readSTSC(in);
            break;
            
        case e_atom_stco:
            // chunk offset box
            readSTCO(in);
            break;
            
        case e_atom_stsd:
            // sample description box
            readSTSD(in);
            break;
            
        case e_atom_mvhd:
            // movie header box
            readMVHD(in);
            break;
            
        case e_atom_mdhd:
            // track header
            readMDHD(in);
            break;

        case e_atom_stts:
            readSTTS(in);
            break;
        
        case e_atom_meta:
            // iTunes metadata box
            readMETA(in,size);
            break;
            
        default:
            break;
    }
    in->seek(destPosition,File::e_startPosition);
}

//-------------------------------------------------------------------------------------------

void Atom::readSTSZ(AtomFile *in)
{
    Track *t = m_tracks.last();
    
    // version
    readByte(in);
    // flags
    readInt24(in);
    t->m_stszSampleSize = readInt32(in);
    t->m_stszSampleCount = readInt32(in);
    if(t->m_stszSampleSize==0)
    {
        tint i;
        
        if(t->m_stszTable!=0)
        {
            delete [] t->m_stszTable;
        }
        t->m_stszTable = new tint32 [t->m_stszSampleCount];
        for(i=0;i<t->m_stszSampleCount;++i)
        {
            t->m_stszTable[i] = readInt32(in);
        }
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readSTTS(AtomFile *in)
{
    Track *t = m_tracks.last();
    
    if(t->m_sttsEntryCount==0)
    {
        // version
        readByte(in);
        // flags
        readInt24(in);
        
        t->m_sttsEntryCount = readInt32(in);
        if(t->m_sttsEntryCount)
        {
            tint i;
            
            if(t->m_sttsSampleCount!=0)
            {
                delete [] t->m_sttsSampleCount;
            }
            t->m_sttsSampleCount = new tint32 [t->m_sttsEntryCount];
            if(t->m_sttsSampleDelta!=0)
            {
                delete [] t->m_sttsSampleDelta;
            }
            t->m_sttsSampleDelta = new tint32 [t->m_sttsEntryCount];
            for(i=0;i<t->m_sttsEntryCount;++i)
            {
                t->m_sttsSampleCount[i] = readInt32(in);
                t->m_sttsSampleDelta[i] = readInt32(in);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readCTTS(AtomFile *in)
{
    Track *t = m_tracks.last();
    
    if(t->m_cttsEntryCount==0)
    {
        //version
        readByte(in);
        // flags
        readInt24(in);
        
        t->m_cttsEntryCount = readInt32(in);
        if(t->m_cttsEntryCount)
        {
            tint i;
            
            if(t->m_cttsSampleCount!=0)
            {
                delete [] t->m_cttsSampleCount;
            }
            t->m_cttsSampleCount = new tint32 [t->m_cttsEntryCount];
            if(t->m_cttsSampleOffset!=0)
            {
                delete [] t->m_cttsSampleCount;
            }
            t->m_cttsSampleOffset = new tint32 [t->m_cttsEntryCount];
            for(i=0;i<t->m_cttsEntryCount;++i)
            {
                t->m_cttsSampleCount[i] = readInt32(in);
                t->m_cttsSampleOffset[i] = readInt32(in);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readSTSC(AtomFile *in)
{
    tint i;
    Track *t = m_tracks.last();
    
    // version
    readByte(in);
    // flags
    readInt24(in);
    
    t->m_stscEntryCount = readInt32(in);
    
    if(t->m_stscFirstChunk!=0)
    {
        delete [] t->m_stscFirstChunk;
    }
    t->m_stscFirstChunk = new tint32 [t->m_stscEntryCount];
    if(t->m_stscSamplesPerChunk!=0)
    {
        delete [] t->m_stscSamplesPerChunk;
    }
    t->m_stscSamplesPerChunk = new tint32 [t->m_stscEntryCount];
    if(t->m_stscSampleDescIndex!=0)
    {
        delete [] t->m_stscSampleDescIndex;
    }
    t->m_stscSampleDescIndex = new tint32 [t->m_stscEntryCount];
    
    for(i=0;i<t->m_stscEntryCount;++i)
    {
        t->m_stscFirstChunk[i] = readInt32(in);
        t->m_stscSamplesPerChunk[i] = readInt32(in);
        t->m_stscSampleDescIndex[i] = readInt32(in);
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readSTCO(AtomFile *in)
{
    tint i;
    Track *t = m_tracks.last();
    
    // version
    readByte(in);
    // flags
    readInt24(in);
    
    t->m_stcoEntryCount = readInt32(in);
    if(t->m_stcoChunkOffset!=0)
    {
        delete [] t->m_stcoChunkOffset;
    }
    t->m_stcoChunkOffset = new tint32 [t->m_stcoEntryCount];
    
    for(i=0;i<t->m_stcoEntryCount;++i)
    {
        t->m_stcoChunkOffset[i] = readInt32(in);
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readSTSD(AtomFile *in)
{
    tint i;
    Track *t = m_tracks.last();
    
    // version
    readByte(in);
    // flags
    readInt24(in);
    
    t->m_stsdEntryCount = readInt32(in);
    for(i=0;i<t->m_stsdEntryCount;++i)
    {
        tint pos,size,hdrSize;
        Type type;
        
        pos = in->offset();
        size = readHeader(in,type,hdrSize);
        switch(type)
        {
            case e_atom_mp4a:
            case e_atom_drms:
                t->m_type = Track::e_track_audio;
                readMP4A(in);
                break;
                
            case e_atom_alac:
                t->m_type = Track::e_track_alac;
                readMP4A(in);
                break;
                
            case e_atom_mp4v:
                t->m_type = Track::e_track_video;
                break;
                
            case e_atom_mp4s:
                t->m_type = Track::e_track_system;
                break;
                
            case e_atom_text:
                t->m_type = Track::e_track_text;
                break;
                
            default:
                t->m_type = Track::e_track_unknown;                
                break;
        }
        in->seek(pos + size,File::e_startPosition);
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readMVHD(AtomFile *in)
{
    tint i;
    
    // version
    readByte(in);
    // flags
    readInt24(in);
    // creation time
    readInt32(in);
    // modification time
    readInt32(in);
    // time scale
    m_timeScale = readUInt32(in);
    // duration
    m_duration = readUInt32(in);
    // preferrred rate
    readInt32(in);
    // preferred volume
    readInt16(in);
    
    for(i=0;i<10;++i)
    {
        // reserved
        readByte(in);
    }
    for(i=0;i<9;++i)
    {
        // matrix
        readInt32(in);
    }
    
    // preview time
    readInt32(in);
    // preview duration
    readInt32(in);
    // poster time
    readInt32(in);
    // selection time
    readInt32(in);
    // selection duration
    readInt32(in);
    // current time
    readInt32(in);
    // next track id
    readInt32(in);
}

//-------------------------------------------------------------------------------------------

void Atom::readMDHD(AtomFile *in)
{
    tint32 version;
    Track *t = m_tracks.last();
    
    version = readInt32(in);
    if(version==1)
    {
        // creation time
        readInt64(in);
        // modification time
        readInt64(in);
        // time scale
        t->m_timeScale = readUInt32(in);
        // duration
        t->m_duration = readUInt64(in);
    }
    else
    {
        tuint32 temp;
        
        // creation time
        readInt32(in);
        // modification time
        readInt32(in);
        // time scale
        t->m_timeScale = readUInt32(in);
        // duration
        temp = readUInt32(in);
        t->m_duration = (temp==(tuint32)(-1)) ? (tuint64)(-1) : (tuint64)(temp);
    }
    readInt16(in);
    readInt16(in);
}

//-------------------------------------------------------------------------------------------

void Atom::readMETA(AtomFile *in,tint size)
{
    tint subsize, sumsize = 0, hdrSize = 0;
    Type type;
    
    // version
    readByte(in);
    // flags
    readInt24(in);
    
    while(sumsize < (size-(hdrSize+4)))
    {
        subsize = readHeader(in,type,hdrSize);
        if(subsize <= (hdrSize+4))
        {
            break;
        }
        if(type==e_atom_ilst)
        {
            parseMeta(in,subsize - (hdrSize + 4));
        }
        else
        {
            in->seek(in->offset() + subsize - hdrSize,File::e_startPosition);
        }
        sumsize += subsize;
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readMP4A(AtomFile *in)
{
    tint i,hdrSize = 0,size;
    Type type;
    Track *t = m_tracks.last();
    
    for(i=0;i<6;++i)
    {
        // reserved
        readByte(in);
    }
    
    // data reference index
    readInt16(in);
    // reserved
    readInt32(in);
    // reserved
    readInt32(in);
    
    t->m_channelCount = static_cast<tint32>(readInt16(in));
    t->m_sampleSize = static_cast<tint32>(readInt16(in));
    
    readInt16(in);
    readInt16(in);
    
    t->m_sampleRate = static_cast<tuint32>(readUInt16(in));

    readInt16(in);

    size = readHeader(in,type,hdrSize);
    if(type==e_atom_esds)
    {
        readESDS(in);
    }
    else if(type==e_atom_alac)
    {
        readALAC(in);
    }
}

//-------------------------------------------------------------------------------------------

void Atom::readALAC(AtomFile *in)
{
    Track *t = m_tracks.last();
    
    // reserved
    readInt32(in);
    t->m_alacFrameLength = readInt32(in);
    t->m_alacCompatibleVersion = static_cast<tint>(readUByte(in));
    t->m_alacBitDepth = static_cast<tint>(readUByte(in));
    t->m_alacPb = static_cast<tint>(readUByte(in));
    t->m_alacMb = static_cast<tint>(readUByte(in));
    t->m_alacKb = static_cast<tint>(readUByte(in));
    t->m_alacNumChannels = static_cast<tint>(readUByte(in));
    t->m_alacMaxRun = static_cast<tint>(readUInt16(in));
    t->m_alacMaxFrameBytes = readInt32(in);
    t->m_alacAvgBitRate = readInt32(in);
    t->m_alacSampleRate = readInt32(in);
}

//-------------------------------------------------------------------------------------------

void Atom::readESDS(AtomFile *in)
{
    tubyte tag;
    tint temp;
    Track *t = m_tracks.last();
    
    // version
    readByte(in);
    // flags
    readInt24(in);
    
    tag = readUByte(in);
    if(tag==0x03)
    {
        if(readMP4DescLength(in) < (5 + 15))
        {
            return;
        }
        readInt24(in);
    }
    else
    {
        readInt16(in);
    }
    
    if(readUByte(in)!=0x04)
    {
        return;
    }
    
    temp = readMP4DescLength(in);
    if(temp < 13)
    {
        return;
    }
    
    t->m_audioType = static_cast<tint32>(readUByte(in));
    readInt32(in);
    t->m_maxBitrate = readUInt32(in);
    t->m_avgBitrate = readUInt32(in);
    
    // get and verify DecSpecificInfoTag
    if(readUByte(in)!=0x05)
    {
        return;
    }
    
    // read length
    t->m_decoderConfigLen = readMP4DescLength(in);
    if(t->m_decoderConfig!=0)
    {
        delete [] t->m_decoderConfig;
        t->m_decoderConfig = 0;
    }
    if(t->m_decoderConfigLen>0)
    {
        t->m_decoderConfig = new tubyte [t->m_decoderConfigLen];
        in->read(t->m_decoderConfig,t->m_decoderConfigLen);
    }
}

//-------------------------------------------------------------------------------------------

tint Atom::readMP4DescLength(AtomFile *in)
{
    tubyte b;
    tint numBytes = 0;
    tuint32 length = 0;
    
    do
    {
        b = readUByte(in);
        numBytes++;
        length = (length << 7) | static_cast<tuint32>(b & 0x7F);
    } while((b & 0x80) && numBytes<4);
    
    return static_cast<tint>(length);
}

//-------------------------------------------------------------------------------------------

QString Atom::getMetaTagName(Type type)
{
    static const tchar *tagNames[] = {
        "unknown",        // 0
        "title",          // 1
        "artist",         // 2
        "writer",         // 3
        "album",          // 4
        "date",           // 5
        "tool",           // 6
        "comment",        // 7
        "genre",          // 8
        "track",          // 9
        "disc",           // 10
        "compilation",    // 11
        "genre",          // 12
        "tempo",          // 13
        "cover"           // 14
    };
    
    tint idx;
    QString name;
    
    switch(type)
    {
        case e_atom_title:
            idx = 1;
            break;
            
        case e_atom_artist:
            idx = 2;
            break;

        case e_atom_writer:
            idx = 3;
            break;

        case e_atom_album:
            idx = 4;
            break;

        case e_atom_date:
            idx = 5;
            break;

        case e_atom_tool:
            idx = 6;
            break;

        case e_atom_comment:
            idx = 7;
            break;

        case e_atom_genre1:
            idx = 8;
            break;

        case e_atom_track:
            idx = 9;
            break;

        case e_atom_disc:
            idx = 10;
            break;

        case e_atom_compilation:
            idx = 11;
            break;

        case e_atom_genre2:
            idx = 12;
            break;

        case e_atom_tempo:
            idx = 13;
            break;

        case e_atom_cover:
            idx = 14;
            break;

        default:
            idx = 0;
            break;
    }
    name = QString::fromLatin1(tagNames[idx]);
    return name;
}

//-------------------------------------------------------------------------------------------

void Atom::parseMeta(AtomFile *in,tint size)
{
    tint subsize, sumsize = 0,hdrSize = 0;
    Type type;
    
    while(sumsize < size)
    {
        subsize = readHeader(in,type,hdrSize);
        if(subsize==0)
        {
            break;
        }
        parseMetaTag(in,type,subsize - hdrSize);
        sumsize += subsize;
    }
}

//-------------------------------------------------------------------------------------------

void Atom::parseMetaTag(AtomFile *in,Type parentType,tint size)
{
    QString name,data;
    tint offset,subsize,sumsize = 0,hdrSize = 0;
    bool done = false;
    Type type;
    
    while(sumsize < size)
    {
        subsize = readHeader(in,type,hdrSize);
        offset = in->offset();
        if(!done)
        {
            if(type==e_atom_data)
            {
                // version
                readByte(in);
                // flags
                readInt24(in);
                // reserved
                readInt32(in);

                if(parentType==e_atom_cover)
                {
                    tint cSize = subsize - (hdrSize + 8);
                    
                    if(cSize>0 && m_coverArtArray==0)
                    {
                        m_coverArtArray = new common::Array<tubyte,tubyte>();
                        m_coverArtArray->SetSize(cSize);
                        in->read(reinterpret_cast<tchar *>(m_coverArtArray->GetData()),cSize);
                    }
                }
                else
                {                
                    if(parentType==e_atom_genre2 || parentType==e_atom_tempo)
                    {
                        if((subsize - hdrSize) >= (8 + 2))
                        {
                            tuint16 val = readUInt16(in);
                            
                            if(parentType==e_atom_tempo)
                            {
                                data.asprintf("%.5u BPM",val);
                                m_metadata.insert(QString::fromLatin1("tempo"),data);
                            }
                            else
                            {
                                if(val>0 && val<=(sizeof(Atom::m_ID3Genres)/sizeof(Atom::m_ID3Genres[0])))
                                {
                                    data = QString::fromLatin1(m_ID3Genres[val - 1]);
                                }
                                else
                                {
                                    data = "";
                                }
                                m_metadata.insert(QString::fromLatin1("genre"),data);
                            }
                            done = true;
                        }
                    }
                    else if(parentType==e_atom_track || parentType==e_atom_disc)
                    {
                        if(!done && (subsize - hdrSize)>=(8 + 6))
                        {
                            tuint16 index,total;
                            
                            readUInt16(in);
                            index = readUInt16(in);
                            total = readUInt16(in);
                            
                            m_metadata.insert(QString::fromLatin1((parentType==e_atom_track) ? "track" : "disc"),QString::number(index));
                            if(total>0)
                            {
                                m_metadata.insert(QString::fromLatin1((parentType==e_atom_track) ? "totaltracks" : "totaldiscs"),QString::number(total));
                            }
                        }    
                    }
                    else
                    {
                        data = readString(in,subsize - (hdrSize + 8));
                    }
                }
            }
            else if(type==e_atom_name)
            {
                if(!done)
                {
                    // version
                    readByte(in);
                    // flags
                    readInt24(in);
                    name = readString(in,subsize - (hdrSize + 4));
                }
            }
        }
        in->seek(offset + (subsize - hdrSize),File::e_startPosition);
        sumsize += subsize;
    }
    
    if(!data.isEmpty() && !done)
    {
        if(name.isEmpty())
        {
            name = getMetaTagName(parentType);
        }
        if(!name.isEmpty())
        {
            m_metadata.insert(name,data);
        }
    }
}

//-------------------------------------------------------------------------------------------

Atom::Type Atom::getAtomType(tuint32 ID)
{
    switch(ID)
    {
        case ATOM_MOOV_ID:
            return e_atom_moov;
        
        case ATOM_MINF_ID:
            return e_atom_minf;
            
        case ATOM_MDIA_ID:
            return e_atom_mdia;
            
        case ATOM_MDAT_ID:
            return e_atom_mdat;
            
        case ATOM_MDHD_ID:
            return e_atom_mdhd;
            
        case ATOM_MVHD_ID:
            return e_atom_mvhd;
            
        case ATOM_MP4A_ID:
            return e_atom_mp4a;
            
        case ATOM_MP4V_ID:
            return e_atom_mp4v;
            
        case ATOM_MP4S_ID:
            return e_atom_mp4s;
            
        case ATOM_META_ID:
            return e_atom_meta;
            
        case ATOM_TRAK_ID:
            return e_atom_trak;
            
        case ATOM_TKHD_ID:
            return e_atom_tkhd;
            
        case ATOM_TREF_ID:
            return e_atom_tref;
            
        case ATOM_TRACK_ID:
            return e_atom_track;
            
        case ATOM_TEMPO_ID:
            return e_atom_tempo;
            
        case ATOM_STBL_ID:
            return e_atom_stbl;
            
        case ATOM_SMHD_ID:
            return e_atom_smhd;
            
        case ATOM_STSD_ID:
            return e_atom_stsd;
            
        case ATOM_STTS_ID:
            return e_atom_stts;
            
        case ATOM_STCO_ID:
            return e_atom_stco;
            
        case ATOM_STSC_ID:
            return e_atom_stsc;
            
        case ATOM_STSZ_ID:
            return e_atom_stsz;
            
        case ATOM_STZ2_ID:
            return e_atom_stz2;
            
        case ATOM_SKIP_ID:
            return e_atom_unknown;
            
        case ATOM_SINF_ID:
            return e_atom_sinf;
            
        case ATOM_SCHI_ID:
            return e_atom_schi;
            
        case ATOM_TITLE_ID:
            return e_atom_title;
            
        case ATOM_ARTIST_ID:
            return e_atom_artist;
            
        case ATOM_WRITER_ID:
            return e_atom_writer;
            
        case ATOM_ALBUM_ID:
            return e_atom_album;
            
        case ATOM_DATE_ID:
            return e_atom_date;
            
        case ATOM_TOOL_ID:
            return e_atom_tool;
            
        case ATOM_COMMENT_ID:
            return e_atom_comment;
            
        case ATOM_GENRE1_ID:
            return e_atom_genre1;
            
        case ATOM_EDTS_ID:
            return e_atom_edts;
            
        case ATOM_ESDS_ID:
            return e_atom_esds;
            
        case ATOM_FTYP_ID:
            return e_atom_ftyp;
            
        case ATOM_FREE_ID:
            return e_atom_unknown;
            
        case ATOM_HMHD_ID:
            return e_atom_hmhd;
            
        case ATOM_VMHD_ID:
            return e_atom_vmhd;
            
        case ATOM_UDTA_ID:
            return e_atom_udta;
            
        case ATOM_ILST_ID:
            return e_atom_ilst;
            
        case ATOM_NAME_ID:
            return e_atom_name;
            
        case ATOM_DATA_ID:
            return e_atom_data;
            
        case ATOM_DISC_ID:
            return e_atom_disc;
            
        case ATOM_GENRE2_ID:
            return e_atom_genre2;
            
        case ATOM_COVER_ID:
            return e_atom_cover;
            
        case ATOM_COMPILATION_ID:
            return e_atom_compilation;
            
        case ATOM_CTTS_ID:
            return e_atom_ctts;
            
        case ATOM_DRMS_ID:
            return e_atom_drms;
            
        case ATOM_FRMA_ID:
            return e_atom_frma;
            
        case ATOM_PRIV_ID:
            return e_atom_priv;
            
        case ATOM_IVIV_ID:
            return e_atom_iviv;
            
        case ATOM_USER_ID:
            return e_atom_user;
            
        case ATOM_KEY_ID:
            return e_atom_key;
        
        case ATOM_TEXT_ID:
            return e_atom_text;
        
        case ATOM_ALAC_ID:
            return e_atom_alac;

        default:
            break;
    }
    return e_atom_unknown;
}

//-------------------------------------------------------------------------------------------

bool Atom::readMP4File(File *in,bool metaOnly)
{
    AtomFile atomIn(in);
    return readMP4File(&atomIn,metaOnly);
}

//-------------------------------------------------------------------------------------------

bool Atom::readMP4File(AtomFile *in,bool metaOnly)
{
    bool res = true;
    
    if(in==0)
    {
        printError("readFile","No input file give to read");
        return false;
    }
    
    clear();
    
    try
    {
        Type type = e_atom_unknown;
        tint pos = 0,hdrSize = 0,atomSize = 0,size = in->length();
        
        atomSize = readHeader(in,type,hdrSize);
        if(type==e_atom_ftyp)
        {
            
            while(res && pos<size && atomSize)
            {
                pos += atomSize;
                if(pos < size)
                {
                    if(in->seek(pos,File::e_startPosition))
                    {
                        atomSize = readHeader(in,type,hdrSize);
                        
                        if(type==e_atom_moov)
                        {
                            m_moovRead = true;
                            m_moovOffset = pos;
                            m_moovSize = atomSize;
                        }
                        
                        if((pos + atomSize) <= size)
                        {
                            if(!(metaOnly && !isMeta(type)))
                            {
                                if(type < e_subatomic)
                                {
                                    res = readSubAtoms(in,atomSize - hdrSize,metaOnly);
                                }
                            }
                        }
                    }
                    else
                    {
                        res = false;
                    }
                }
            }
        }
        else
        {
            res = false;
        }
    }
    catch(AtomException& e)
    {
        common::BString errStr("Exception caught while reading input file. ");
        errStr += e.error();
        printError("readFile",static_cast<const tchar *>(errStr));
        res = false;
    }
    catch(...)
    {
        printError("readFile","General exception caught");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Atom::readSubAtoms(AtomFile *in,tint parentSize,bool metaOnly)
{
    Type type = e_atom_unknown;
    tint atomSize,offset,pos = 0,hdrSize = 0;
    bool res = true;
    
    offset = in->offset();
    
    while(res && pos<parentSize)
    {
        atomSize = readHeader(in,type,hdrSize);
        if(!atomSize)
        {
            printError("readSubAtoms","Header size zero bytes");
            return false;
        }
        pos += atomSize;
        
        if(pos<=parentSize)
        {
            if(type==e_atom_trak)
            {
                Track *t = new Track;
                m_tracks.append(t);
            }
        
            if(!(metaOnly && !isMeta(type)))
            {
                if(type < e_subatomic)
                {
                    res = readSubAtoms(in,atomSize - hdrSize,metaOnly);
                }
                else
                {
                    readAtom(in,type,atomSize);
                }            
            }
            
            if(!in->seek(offset + pos,File::e_startPosition))
            {
                printError("readSubAtoms","Failed to seek to beginning of next atom");
                res = false;
            }
        }
        else
        {
            printError("readSubAtoms","Child atom size bigger than expected");
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Atom::isMeta(Type t)
{
    bool res;
    
    switch(t)
    {
        case e_atom_edts:
        case e_atom_drms:
        case e_atom_sinf:
        case e_atom_schi:
        case e_atom_stts:
        case e_atom_stsz:
        case e_atom_stz2:
        case e_atom_stco:
        case e_atom_stsc:
        case e_atom_frma:
        case e_atom_iviv:
        case e_atom_priv:
            res = false;
            break;
            
        default:
            res = true;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint Atom::getSampleDuration(tint trackIdx,tint sample)
{
    tint i,co = 0;
    Track *t;
    
    t = m_tracks.at(trackIdx);
    
    for(i=0;i<t->m_sttsEntryCount;++i)
    {
        tint delta = t->m_sttsSampleCount[i];
        if(sample < (co + delta))
        {
            return t->m_sttsSampleDelta[i];
        }
        co += delta;
    }
    return -1;
}

//-------------------------------------------------------------------------------------------

void Atom::buildSampleTable(tint trackIdx)
{
    tint i,j,k;
    tuint32 offset = 0;
    tint totalSample = 0;
    Track *t;
    
    t = m_tracks.at(trackIdx);
    
    for(i=0;i<t->m_sttsEntryCount;++i)
    {
        totalSample += t->m_sttsSampleCount[i];
    }
    
    t->m_sampleLookupLength = totalSample;
    t->m_sampleLookup = new tuint32 [totalSample];
    
    for(i=0,j=0,k=0;i<totalSample;++i,++k)
    {        
        if(k >= t->m_sttsSampleCount[j])
        {
            k = 0;
            j++;
        }
        offset += t->m_sttsSampleDelta[j];
        t->m_sampleLookup[i] = offset;
    }
}

//-------------------------------------------------------------------------------------------

tint Atom::seekSamplePosition(tint trackIdx,common::TimeStamp& sT) const
{
    tint idx,lIdx,uIdx;
    tuint32 lSam,lSample,uSample;
    Track *t = m_tracks.at(trackIdx);
    
    lSam = static_cast<tuint32>(static_cast<tfloat64>(sT) * static_cast<tfloat64>(t->m_timeScale));
    if(lSam >= static_cast<tuint32>(t->m_duration))
    {
        return -1;
    }
    
    lIdx = 0;
    uIdx = t->m_sampleLookupLength - 1;
    
    while(lIdx <= uIdx)
    {
        idx = (lIdx + uIdx) >> 1;
        
        lSample = (idx) ? t->m_sampleLookup[idx - 1] : 0;
        uSample = t->m_sampleLookup[idx];
        
        if(lSample<=lSam && lSam<uSample)
        {
            sT = static_cast<tfloat64>(lSample) / static_cast<tfloat64>(t->m_timeScale);
            return idx;
        }
        else if(lSam < lSample)
        {
            uIdx = idx - 1;
        }
        else
        {
            lIdx = idx + 1;
        }
    }
    return -1;
}

//-------------------------------------------------------------------------------------------

tint Atom::getFrameSize(tint trackIdx,tint sample)
{
    Track *t = m_tracks.at(trackIdx);
    
    if(t->m_stszSampleSize)
    {
        return t->m_stszSampleSize;
    }
    else
    {
        return t->m_stszTable[sample];
    }
}

//-------------------------------------------------------------------------------------------

void Atom::chunkOfSample(tint trackIdx,tint sample,tint& chunkSample,tint& chunk)
{
    tint totalEntries = 0;
    tint chunk2Entry,chunk1,chunk2,chunk1Samples,rangeSamples,total = 0;
    Track *t = m_tracks.at(trackIdx);
    
    totalEntries = t->m_stscEntryCount;
    
    chunk1 = 1;
    chunk1Samples = 0;
    chunk2Entry = 0;
    
    do
    {
        chunk2 = t->m_stscFirstChunk[chunk2Entry];
        chunk = chunk2 - chunk1;
        rangeSamples = chunk * chunk1Samples;
        
        if(sample < (total + rangeSamples))
        {
            break;
        }
        
        chunk1Samples = t->m_stscSamplesPerChunk[chunk2Entry];
        chunk1 = chunk2;
        
        if(chunk2Entry < totalEntries)
        {
            chunk2Entry++;
            total += rangeSamples;
        }
    } while(chunk2Entry < totalEntries);
    
    if(chunk1Samples)
    {
        chunk = (sample - total) / chunk1Samples + chunk1;
    }
    else
    {
        chunk = 1;
    }
    chunkSample = total + (chunk - chunk1) * chunk1Samples;
}

//-------------------------------------------------------------------------------------------

tint Atom::chunkToOffset(tint trackIdx,tint chunk)
{
    Track *t = m_tracks.at(trackIdx);
    
    if(t->m_stcoEntryCount && (chunk > t->m_stcoEntryCount))
    {
        return t->m_stcoChunkOffset[t->m_stcoEntryCount - 1];
    }
    else if(t->m_stcoEntryCount)
    {
        return t->m_stcoChunkOffset[chunk - 1];
    }
    else
    {
        return 8;
    }
}

//-------------------------------------------------------------------------------------------

tint Atom::sampleRangeSize(tint trackIdx,tint chunkSample,tint sample)
{
    tint i,total;
    Track *t = m_tracks.at(trackIdx);
    
    if(t->m_stszSampleSize)
    {
        return (sample - chunkSample) * t->m_stszSampleSize;
    }
    else
    {
        if(sample >= t->m_stszSampleCount)
        {
            return 0;
        }
        for(i=chunkSample,total=0;i<sample;++i)
        {
            total += t->m_stszTable[i];
        }
    }
    return total;
}

//-------------------------------------------------------------------------------------------

tint Atom::sampleToOffset(tint trackIdx,tint sample)
{
    tint chunk,chunkSample,chunkOffset1,chunkOffset2;
    
    chunkOfSample(trackIdx,sample,chunkSample,chunk);
    chunkOffset1 = chunkToOffset(trackIdx,chunk);
    chunkOffset2 = chunkOffset1 + sampleRangeSize(trackIdx,chunkSample,sample);
    return chunkOffset2;
}

//-------------------------------------------------------------------------------------------

bool Atom::setSamplePosition(File *in,tint trackIdx,tint sample)
{
    AtomFile aFile(in);
    return setSamplePosition(&aFile,trackIdx,sample);
}

//-------------------------------------------------------------------------------------------

bool Atom::setSamplePosition(AtomFile *in,tint trackIdx,tint sample)
{
    bool res = false;
    
    try
    {
        tint offset;
        
        offset = sampleToOffset(trackIdx,sample);
        if(in->seek(offset,File::e_startPosition))
        {
            res = true;
        }
        else
        {
            printError("readFile","Failed to find position in file for given track and sample");
        }
    }
    catch(AtomException& e)
    {
        common::BString errStr("Exception caught while reading input file. ");
        errStr += e.error();
        printError("readFile",static_cast<const tchar *>(errStr));
        res = false;        
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Atom::readSample(File *in,tint trackIdx,tint sample,common::Array<tubyte,tubyte>& mem)
{
    AtomFile aFile(in);
    return readSample(&aFile,trackIdx,sample,mem);
}

//-------------------------------------------------------------------------------------------

bool Atom::readSample(AtomFile *in,tint trackIdx,tint sample,common::Array<tubyte,tubyte>& mem)
{
    bool res =  false;
    
    try
    {
        tint size;
        
        size = getFrameSize(trackIdx,sample);
        if(size>0)
        {
            if(setSamplePosition(in,trackIdx,sample))
            {
                mem.SetSize(size);
                in->read(mem.GetData(),size);
                res = true;
            }
            else
            {
                printError("readSample","Failed to locate sample data for given track");
            }
        }
        else
        {
            printError("readSample","No sample data exists for given track/sample pair");
        }
    }
    catch(AtomException& e)
    {
        common::BString errStr("Exception caught while reading input file. ");
        errStr += e.error();
        printError("readFile",static_cast<const tchar *>(errStr));
        res = false;        
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint Atom::noSamples(tint trackIdx)
{
    tint32 i,total = 0;
    Track *track;

    if(trackIdx>=0 && trackIdx<m_tracks.size())
    {
        track = m_tracks.at(trackIdx);

        for(i=0;i<track->m_sttsEntryCount;++i)
        {
            total += track->m_sttsSampleCount[i];
        }
    }
    return total;
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
