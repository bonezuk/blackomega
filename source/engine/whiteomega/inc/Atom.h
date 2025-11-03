//-------------------------------------------------------------------------------------------
#ifndef __ENGINE_WHITEOMEGA_ATOM_H
#define __ENGINE_WHITEOMEGA_ATOM_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Log.h"
#include "common/inc/Exception.h"
#include "common/inc/Array.h"
#include "common/inc/TimeStamp.h"
#include "engine/inc/File.h"
#include "engine/whiteomega/inc/WhiteOmegaDLL.h"

#include <QList>
#include <QMap>
#include <QString>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

#define ATOM_ID(a,b,c,d) (((static_cast<tuint32>(static_cast<tint32>(a)) << 24) & 0xff000000) | \
                          ((static_cast<tuint32>(static_cast<tint32>(b)) << 16) & 0x00ff0000) | \
                          ((static_cast<tuint32>(static_cast<tint32>(c)) <<  8) & 0x0000ff00) | \
                          ((static_cast<tuint32>(static_cast<tint32>(d))      ) & 0x000000ff))

#define ATOM_MOOV_ID        ATOM_ID('m','o','o','v')
#define ATOM_MINF_ID        ATOM_ID('m','i','n','f')
#define ATOM_MDIA_ID        ATOM_ID('m','d','i','a')
#define ATOM_MDAT_ID        ATOM_ID('m','d','a','t')
#define ATOM_MDHD_ID        ATOM_ID('m','d','h','d')
#define ATOM_MVHD_ID        ATOM_ID('m','v','h','d')
#define ATOM_MP4A_ID        ATOM_ID('m','p','4','a')
#define ATOM_MP4V_ID        ATOM_ID('m','p','4','v')
#define ATOM_MP4S_ID        ATOM_ID('m','p','4','s')
#define ATOM_TEXT_ID        ATOM_ID('t','e','x','t')
#define ATOM_META_ID        ATOM_ID('m','e','t','a')
#define ATOM_TRAK_ID        ATOM_ID('t','r','a','k')
#define ATOM_TKHD_ID        ATOM_ID('t','k','h','d')
#define ATOM_TREF_ID        ATOM_ID('t','r','e','f')
#define ATOM_TRACK_ID       ATOM_ID('t','r','k','n')
#define ATOM_TEMPO_ID       ATOM_ID('t','m','p','o')
#define ATOM_STBL_ID        ATOM_ID('s','t','b','l')
#define ATOM_SMHD_ID        ATOM_ID('s','m','h','d')
#define ATOM_STSD_ID        ATOM_ID('s','t','s','d')
#define ATOM_STTS_ID        ATOM_ID('s','t','t','s')
#define ATOM_STCO_ID        ATOM_ID('s','t','c','o')
#define ATOM_STSC_ID        ATOM_ID('s','t','s','c')
#define ATOM_STSZ_ID        ATOM_ID('s','t','s','z')
#define ATOM_STZ2_ID        ATOM_ID('s','t','z','2')
#define ATOM_SKIP_ID        ATOM_ID('s','k','i','p')
#define ATOM_SINF_ID        ATOM_ID('s','i','n','f')
#define ATOM_SCHI_ID        ATOM_ID('s','c','h','i')
#define ATOM_TITLE_ID       ATOM_ID(0xA9,'n','a','m')
#define ATOM_ARTIST_ID      ATOM_ID(0xA9,'A','R','T')
#define ATOM_WRITER_ID      ATOM_ID(0xA9,'w','r','t')
#define ATOM_ALBUM_ID       ATOM_ID(0xA9,'a','l','b')
#define ATOM_DATE_ID        ATOM_ID(0xA9,'d','a','y')
#define ATOM_TOOL_ID        ATOM_ID(0xA9,'t','o','o')
#define ATOM_COMMENT_ID     ATOM_ID(0xA9,'c','m','t')
#define ATOM_GENRE1_ID      ATOM_ID(0xA9,'g','e','n')
#define ATOM_EDTS_ID        ATOM_ID('e','d','t','s')
#define ATOM_ESDS_ID        ATOM_ID('e','s','d','s')
#define ATOM_FTYP_ID        ATOM_ID('f','t','y','p')
#define ATOM_FREE_ID        ATOM_ID('f','r','e','e')
#define ATOM_HMHD_ID        ATOM_ID('h','m','h','d')
#define ATOM_VMHD_ID        ATOM_ID('v','m','h','d')
#define ATOM_UDTA_ID        ATOM_ID('u','d','t','a')
#define ATOM_ILST_ID        ATOM_ID('i','l','s','t')
#define ATOM_NAME_ID        ATOM_ID('n','a','m','e')
#define ATOM_DATA_ID        ATOM_ID('d','a','t','a')
#define ATOM_DISC_ID        ATOM_ID('d','i','s','k')
#define ATOM_GENRE2_ID      ATOM_ID('g','n','r','e')
#define ATOM_COVER_ID       ATOM_ID('c','o','v','r')
#define ATOM_COMPILATION_ID ATOM_ID('c','p','i','l')
#define ATOM_CTTS_ID        ATOM_ID('c','t','t','s')
#define ATOM_DRMS_ID        ATOM_ID('d','r','m','s')
#define ATOM_FRMA_ID        ATOM_ID('f','r','m','a')
#define ATOM_PRIV_ID        ATOM_ID('p','r','i','v')
#define ATOM_IVIV_ID        ATOM_ID('i','v','i','v')
#define ATOM_USER_ID        ATOM_ID('u','s','e','r')
#define ATOM_KEY_ID         ATOM_ID('k','e','y',' ')
#define ATOM_ALAC_ID        ATOM_ID('a','l','a','c')

//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT AtomException : public common::Exception
{
    public:
        AtomException(const tchar *sFunc,const tchar *msg);
};

//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT AtomFile
{
    public:
        AtomFile(File *in);
        virtual ~AtomFile();
        
        virtual void read(tbyte *mem,tint len);
        virtual void read(tubyte *mem,tint len);
        
        virtual bool seek(tint offset,File::Position position);
        
        virtual tint length() const;
        virtual tint offset() const;
        
    protected:
    
        File *m_file;
};

//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT Atom
{
    public:
    
        typedef enum
        {
            // atoms with subatoms
            e_atom_moov        =  1,
            e_atom_trak        =  2,
            e_atom_edts        =  3,
            e_atom_mdia        =  4,
            e_atom_minf        =  5,
            e_atom_stbl        =  6,
            e_atom_udta        =  7,
            e_atom_ilst        =  8,
            e_atom_title       =  9,
            e_atom_artist      = 10,
            e_atom_writer      = 11,
            e_atom_album       = 12,
            e_atom_date        = 13,
            e_atom_tool        = 14,
            e_atom_comment     = 15,
            e_atom_genre1      = 16,
            e_atom_track       = 17,
            e_atom_disc        = 18,
            e_atom_compilation = 19,
            e_atom_genre2      = 20,
            e_atom_tempo       = 21,
            e_atom_cover       = 22,
            e_atom_drms        = 23,
            e_atom_sinf        = 24,
            e_atom_schi        = 25,
            
            // atoms without subatoms
            e_subatomic        = 128,
            e_atom_ftyp        = 129,
            e_atom_mdat        = 130,
            e_atom_mvhd        = 131,
            e_atom_tkhd        = 132,
            e_atom_tref        = 133,
            e_atom_mdhd        = 134,
            e_atom_vmhd        = 135,
            e_atom_smhd        = 136,
            e_atom_hmhd        = 137,
            e_atom_stsd        = 138,
            e_atom_stts        = 139,
            e_atom_stsz        = 140,
            e_atom_stz2        = 141,
            e_atom_stco        = 142,
            e_atom_stsc        = 143,
            e_atom_mp4a        = 144,
            e_atom_mp4v        = 145,
            e_atom_mp4s        = 146,
            e_atom_esds        = 147,
            e_atom_meta        = 148,
            e_atom_name        = 149,
            e_atom_data        = 150,
            e_atom_ctts        = 151,
            e_atom_frma        = 152,
            e_atom_iviv        = 153,
            e_atom_priv        = 154,
            e_atom_user        = 155,
            e_atom_key         = 156,
            e_atom_text        = 157,
            e_atom_alac        = 158,
            e_atom_unknown     = 255
        } Type;
        
        class Track
        {
            public:
            
                typedef enum
                {
                    e_track_audio   = 1,
                    e_track_video   = 2,
                    e_track_system  = 3,
                    e_track_text    = 4,
                    e_track_alac    = 5,
                    e_track_unknown = 0
                } Type;
            
            public:
                Track();
                ~Track();
            
                // General
                Type m_type;
                tint32 m_channelCount;
                tint32 m_sampleSize;
                tint32 m_sampleRate;
                tint32 m_audioType;
                
                tuint32 m_maxBitrate;
                tuint32 m_avgBitrate;
                tuint32 m_timeScale;
                tuint64 m_duration;
                
                // stsd
                tint32 m_stsdEntryCount;
                
                // stsz
                tint32 m_stszSampleSize;
                tint32 m_stszSampleCount;
                tint32 *m_stszTable;
                
                // stts
                tint32 m_sttsEntryCount;
                tint32 *m_sttsSampleCount;
                tint32 *m_sttsSampleDelta;
                
                // stsc
                tint32 m_stscEntryCount;
                tint32 *m_stscFirstChunk;
                tint32 *m_stscSamplesPerChunk;
                tint32 *m_stscSampleDescIndex;
                
                // stso
                tint32 m_stcoEntryCount;
                tint32 *m_stcoChunkOffset;
                
                // ctts
                tint32 m_cttsEntryCount;
                tint32 *m_cttsSampleCount;
                tint32 *m_cttsSampleOffset;
                
                // esde
                tint32 m_decoderConfigLen;
                tubyte *m_decoderConfig;
                
                // alac
                tint m_alacFrameLength;
                tint m_alacCompatibleVersion;
                tint m_alacBitDepth;
                tint m_alacPb;
                tint m_alacMb;
                tint m_alacKb;
                tint m_alacNumChannels;
                tint m_alacMaxRun;
                tint m_alacMaxFrameBytes;
                tint m_alacAvgBitRate;
                tint m_alacSampleRate;
                
                // sample lookup table
                tint m_sampleLookupLength;
                tuint32 *m_sampleLookup;
        };
        
    public:
        Atom();
        virtual ~Atom();
        
        bool readMP4File(File *in,bool metaOnly = false);
        bool readMP4File(AtomFile *in,bool metaOnly = false);
        
        // moov
        bool m_moovRead;
        tint m_moovOffset;
        tint m_moovSize;
        
        // mvhd
        tuint32 m_timeScale;
        tuint32 m_duration;
        
        QList<Track *> m_tracks;
        QMap<QString,QString> m_metadata;
        common::Array<tubyte,tubyte> *m_coverArtArray;
        
        virtual tint getSampleDuration(tint trackIdx,tint sample);
        virtual bool setSamplePosition(File *in,tint trackIdx,tint sample);
        virtual bool setSamplePosition(AtomFile *in,tint trackIdx,tint sample);
        virtual tint noSamples(tint trackIdx);
        virtual bool readSample(File *in,tint trackIdx,tint sample,common::Array<tubyte,tubyte>& mem);
        virtual bool readSample(AtomFile *in,tint trackIdx,tint sample,common::Array<tubyte,tubyte>& mem);
        
        void buildSampleTable(tint trackIdx);
        tint seekSamplePosition(tint trackIdx,common::TimeStamp& sT) const;
        
    protected:
    
        static const tchar *m_ID3Genres[];
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void clear();
        
        virtual tbyte readByte(AtomFile *in);
        virtual tubyte readUByte(AtomFile *in);
        virtual tint16 readInt16(AtomFile *in);
        virtual tuint16 readUInt16(AtomFile *in);
        virtual tint32 readInt24(AtomFile *in);
        virtual tuint32 readUInt24(AtomFile *in);
        virtual tint32 readInt32(AtomFile *in);
        virtual tuint32 readUInt32(AtomFile *in);
        virtual tint64 readInt64(AtomFile *in);
        virtual tuint64 readUInt64(AtomFile *in);
        virtual QString readString(AtomFile *in,tint len);
        
        virtual tint readHeader(AtomFile *in,Type& t);
        virtual tint readHeader(AtomFile *in,Type& t,tint& hdrSize);
        
        virtual Type getAtomType(tuint32 ID);
        
        virtual void readAtom(AtomFile *in,Type t,tint size);
        virtual void readSTSZ(AtomFile *in);
        virtual void readSTTS(AtomFile *in);
        virtual void readCTTS(AtomFile *in);
        virtual void readSTSC(AtomFile *in);
        virtual void readSTCO(AtomFile *in);
        virtual void readSTSD(AtomFile *in);
        virtual void readMVHD(AtomFile *in);
        virtual void readMDHD(AtomFile *in);
        virtual void readMETA(AtomFile *in,tint size);
        virtual void readMP4A(AtomFile *in);
        virtual void readESDS(AtomFile *in);
        virtual void readALAC(AtomFile *in);
        
        virtual QString getMetaTagName(Type t);
        virtual void parseMeta(AtomFile *in,tint size);
        virtual void parseMetaTag(AtomFile *in,Type parentType,tint size);
        
        virtual tint readMP4DescLength(AtomFile *in);
        
        virtual bool readSubAtoms(AtomFile *in,tint parentSize,bool metaOnly = false);
        virtual bool isMeta(Type t);
        
        virtual tint getFrameSize(tint trackIdx,tint sample);
        virtual void chunkOfSample(tint trackIdx,tint sample,tint& chunkSample,tint& chunk);
        virtual tint chunkToOffset(tint trackIdx,tint chunk);
        virtual tint sampleRangeSize(tint trackIdx,tint chunkSample,tint sample);
        virtual tint sampleToOffset(tint trackIdx,tint sample);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

