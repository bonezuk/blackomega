//-------------------------------------------------------------------------------------------
#ifndef __ENGINE_GREENOMEGA_FLACFRAMEWORK_H
#define __ENGINE_GREENOMEGA_FLACFRAMEWORK_H
//-------------------------------------------------------------------------------------------

#include "common/inc/TimeStamp.h"
#include "engine/inc/Sequence.h"
#include "engine/inc/BitStream.h"
#include "engine/greenomega/inc/FLACMetaBase.h"
#include "engine/greenomega/inc/FLACMetaComment.h"
#include "engine/greenomega/inc/FLACMetaStreamInfo.h"
#include "engine/greenomega/inc/FLACMetaSeekTable.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

#define FLAC_ID(a,b,c,d) (((static_cast<tuint32>(static_cast<tint32>(a)) << 24) & 0xff000000) | \
                          ((static_cast<tuint32>(static_cast<tint32>(b)) << 16) & 0x00ff0000) | \
                          ((static_cast<tuint32>(static_cast<tint32>(c)) <<  8) & 0x0000ff00) | \
                          ((static_cast<tuint32>(static_cast<tint32>(d))      ) & 0x000000ff))

#define FLAC_FILE_ID FLAC_ID('f','L','a','C')
#define FLAC_ID3_ID  FLAC_ID('I','D','3','.')

//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACFramework
{
    public:
        FLACFramework();
        virtual ~FLACFramework();

        bool init(const tchar *name);
        bool init(const QString& name);
        bool init(engine::File *f);

        FLACMetaStreamInfo *streamInfo();
        const FLACMetaStreamInfo *streamInfo() const;
        FLACMetaSeekTable *seekTable();
        const FLACMetaSeekTable *seekTable() const;
        const FLACMetaComment *comments() const;

        QVector<FLACMetaBase *>& metaData();
        const QVector<FLACMetaBase *>& metaData() const;

        Sequence *sequence();
        Bitstream *bitstream();
        tint streamOffset() const;

        tint bitrate() const;

    protected:

        engine::File *m_file;
        engine::Bitstream *m_bitstream;
        int m_sequenceNo;

        QVector<FLACMetaBase *> m_metadata;

        FLACMetaStreamInfo *m_streamInfo;
        FLACMetaSeekTable *m_seekTable;
        FLACMetaComment *m_comments;

        tint m_streamOffset;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        bool isValidFile();
        bool readMetaData();
};

//-------------------------------------------------------------------------------------------

inline FLACMetaStreamInfo *FLACFramework::streamInfo()
{
    return m_streamInfo;
}

//-------------------------------------------------------------------------------------------

inline const FLACMetaStreamInfo *FLACFramework::streamInfo() const
{
    return m_streamInfo;
}

//-------------------------------------------------------------------------------------------

inline FLACMetaSeekTable *FLACFramework::seekTable()
{
    return m_seekTable;
}

//-------------------------------------------------------------------------------------------

inline const FLACMetaSeekTable *FLACFramework::seekTable() const
{
    return m_seekTable;
}

//-------------------------------------------------------------------------------------------

inline const FLACMetaComment *FLACFramework::comments() const
{
    return m_comments;
}

//-------------------------------------------------------------------------------------------

inline Sequence *FLACFramework::sequence()
{
    return m_bitstream->getSequence(0);
}

//-------------------------------------------------------------------------------------------

inline Bitstream *FLACFramework::bitstream()
{
    return m_bitstream;
}

//-------------------------------------------------------------------------------------------

inline tint FLACFramework::streamOffset() const
{
    return m_streamOffset;
}

//-------------------------------------------------------------------------------------------

inline QVector<FLACMetaBase *>& FLACFramework::metaData()
{
    return m_metadata;
}

//-------------------------------------------------------------------------------------------

inline const QVector<FLACMetaBase *>& FLACFramework::metaData() const
{
    return m_metadata;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

