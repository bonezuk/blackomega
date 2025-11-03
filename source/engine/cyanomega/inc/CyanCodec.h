//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_CYANOMEGA_CYANCODEC_H
#define __OMEGA_ENGINE_CYANOMEGA_CYANCODEC_H
//-------------------------------------------------------------------------------------------

#include "engine/cyanomega/inc/CyanOmegaDLL.h"
#include "engine/cyanomega/inc/MusePackDecoder.h"
#include "engine/inc/InterleavedCodec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace cyanomega
{
//-------------------------------------------------------------------------------------------

class CYANOMEGA_EXPORT CyanCodec : public InterleavedCodec
{
    public:
        Q_OBJECT

    public:
        CyanCodec(QObject *parent = 0);
        virtual ~CyanCodec();

        virtual bool open(const QString& name);
        virtual void close();

        virtual bool isSeek() const;
        virtual bool seek(const common::TimeStamp& v);

        virtual bool isComplete() const;

        virtual bool isRemote() const;
        virtual bool isBuffered(tfloat32& percent);

        virtual tint bitrate() const;
        virtual tint frequency() const;
        virtual tint noChannels() const;
        virtual common::TimeStamp length() const;

    protected:

        MusePackDecoderSPtr m_pDecoder;

        QByteArray m_outputArray;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual bool decodeNextPacket(int& outLen);
        virtual char *getPacketBuffer();
        virtual int bytesPerSample();
        virtual sample_t readSample(char *buffer);
};

//-------------------------------------------------------------------------------------------
} // namespace cyanomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

