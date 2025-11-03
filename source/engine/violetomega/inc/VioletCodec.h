//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_VIOLETCODEC_H
#define __OMEGA_ENGINE_VIOLETOMEGA_VIOLETCODEC_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "engine/inc/RData.h"
#include "engine/inc/InterleavedCodec.h"
#include "engine/violetomega/inc/IFFFile.h"
#include "engine/violetomega/inc/IFFFormChunk.h"
#include "engine/violetomega/inc/IFFCommonChunk.h"
#include "engine/violetomega/inc/IFFSoundChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT VioletCodec : public InterleavedCodec
{
    public:
        Q_OBJECT

    public:
        VioletCodec(QObject *parent = 0);
        virtual ~VioletCodec();

        virtual bool open(const QString& name);
        virtual void close();

        virtual bool init();

        virtual bool isSeek() const;
        virtual bool seek(const common::TimeStamp& v);

        virtual bool isComplete() const;

        virtual bool isRemote() const;
        virtual bool isBuffered(tfloat32& percent);

        virtual tint bitrate() const;
        virtual tint frequency() const;
        virtual tint noChannels() const;
        virtual common::TimeStamp length() const;

        virtual CodecDataType dataTypesSupported() const;
        virtual bool setDataTypeFormat(CodecDataType type);

    protected:

        common::BIOBufferedStream *m_file;
        IFFFileSPtr m_iffFile;
        IFFCommonChunkSPtr m_pCommon;
        IFFSoundChunkSPtr m_pSound;

        sample_t *m_buffer;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual bool decodeNextPacket(int& outLen);
        virtual char *getPacketBuffer();
        virtual int bytesPerSample();
        virtual sample_t readSample(char *buffer);
};

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

