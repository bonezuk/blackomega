//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WAVPACKOMEGA_WAVPACKCODEC_H
#define __OMEGA_ENGINE_WAVPACKOMEGA_WAVPACKCODEC_H
//-------------------------------------------------------------------------------------------

#include "wavpack/wavpack.h"

#include "engine/inc/InterleavedCodec.h"
#include "engine/inc/WavChannelMask.h"
#include "engine/wavpackomega/inc/WavPackOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace wavpackomega
{
//-------------------------------------------------------------------------------------------

class WAVPACKOMEGA_EXPORT WavPackCodec : public InterleavedCodec
{
    public:
        Q_OBJECT

    public:
        static const tint c_BufferLength = 1024;

        typedef sample_t (*ReadSampleFunc)(char*);

    public:
        WavPackCodec(QObject *parent = 0);
        virtual ~WavPackCodec();

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

    protected:

        WavpackContext *m_context;

        int m_bitsPerSample;
        int m_channelMask;
        int m_noWavChannels;
        int m_sampleRate;

        WavChannelMask m_channelMap;

        sample_t *m_buffer;

        ReadSampleFunc m_readSample;

        common::BIOBufferedStream *m_file;
        common::BIOBufferedStream *m_fileCorrection;
        WavpackStreamReader *m_reader;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual QString correctionFileName(const QString& name);

        virtual void readDecodedData(sample_t *output,tint sampleOffset,tint amount);

        virtual bool decodeNextPacket(int& outLen);
        virtual char *getPacketBuffer();
        virtual int bytesPerSample();
        virtual sample_t readSample(char *buffer);

        static sample_t readSampleFloat(char *buffer);

        static sample_t readSampleInteger1BitLE(char *buffer);
        static sample_t readSampleInteger1BitBE(char *buffer);

        static sample_t readSampleInteger2BitsLE(char *buffer);
        static sample_t readSampleInteger2BitsBE(char *buffer);

        static sample_t readSampleInteger3BitsLE(char *buffer);
        static sample_t readSampleInteger3BitsBE(char *buffer);

        static sample_t readSampleInteger4BitsLE(char *buffer);
        static sample_t readSampleInteger4BitsBE(char *buffer);

        static sample_t readSampleInteger5BitsLE(char *buffer);
        static sample_t readSampleInteger5BitsBE(char *buffer);

        static sample_t readSampleInteger6BitsLE(char *buffer);
        static sample_t readSampleInteger6BitsBE(char *buffer);

        static sample_t readSampleInteger7BitsLE(char *buffer);
        static sample_t readSampleInteger7BitsBE(char *buffer);

        static sample_t readSampleInteger8BitsLE(char *buffer);
        static sample_t readSampleInteger8BitsBE(char *buffer);

        static sample_t readSampleInteger9BitsLE(char *buffer);
        static sample_t readSampleInteger9BitsBE(char *buffer);

        static sample_t readSampleInteger10BitsLE(char *buffer);
        static sample_t readSampleInteger10BitsBE(char *buffer);

        static sample_t readSampleInteger11BitsLE(char *buffer);
        static sample_t readSampleInteger11BitsBE(char *buffer);

        static sample_t readSampleInteger12BitsLE(char *buffer);
        static sample_t readSampleInteger12BitsBE(char *buffer);

        static sample_t readSampleInteger13BitsLE(char *buffer);
        static sample_t readSampleInteger13BitsBE(char *buffer);

        static sample_t readSampleInteger14BitsLE(char *buffer);
        static sample_t readSampleInteger14BitsBE(char *buffer);

        static sample_t readSampleInteger15BitsLE(char *buffer);
        static sample_t readSampleInteger15BitsBE(char *buffer);

        static sample_t readSampleInteger16BitsLE(char *buffer);
        static sample_t readSampleInteger16BitsBE(char *buffer);

        static sample_t readSampleInteger17BitsLE(char *buffer);
        static sample_t readSampleInteger17BitsBE(char *buffer);

        static sample_t readSampleInteger18BitsLE(char *buffer);
        static sample_t readSampleInteger18BitsBE(char *buffer);

        static sample_t readSampleInteger19BitsLE(char *buffer);
        static sample_t readSampleInteger19BitsBE(char *buffer);

        static sample_t readSampleInteger20BitsLE(char *buffer);
        static sample_t readSampleInteger20BitsBE(char *buffer);

        static sample_t readSampleInteger21BitsLE(char *buffer);
        static sample_t readSampleInteger21BitsBE(char *buffer);

        static sample_t readSampleInteger22BitsLE(char *buffer);
        static sample_t readSampleInteger22BitsBE(char *buffer);

        static sample_t readSampleInteger23BitsLE(char *buffer);
        static sample_t readSampleInteger23BitsBE(char *buffer);

        static sample_t readSampleInteger24BitsLE(char *buffer);
        static sample_t readSampleInteger24BitsBE(char *buffer);

        static sample_t readSampleInteger25BitsLE(char *buffer);
        static sample_t readSampleInteger25BitsBE(char *buffer);

        static sample_t readSampleInteger26BitsLE(char *buffer);
        static sample_t readSampleInteger26BitsBE(char *buffer);

        static sample_t readSampleInteger27BitsLE(char *buffer);
        static sample_t readSampleInteger27BitsBE(char *buffer);

        static sample_t readSampleInteger28BitsLE(char *buffer);
        static sample_t readSampleInteger28BitsBE(char *buffer);

        static sample_t readSampleInteger29BitsLE(char *buffer);
        static sample_t readSampleInteger29BitsBE(char *buffer);

        static sample_t readSampleInteger30BitsLE(char *buffer);
        static sample_t readSampleInteger30BitsBE(char *buffer);

        static sample_t readSampleInteger31BitsLE(char *buffer);
        static sample_t readSampleInteger31BitsBE(char *buffer);

        static sample_t readSampleInteger32BitsLE(char *buffer);
        static sample_t readSampleInteger32BitsBE(char *buffer);

        void setupReadSample();
        void setupReadSampleLE();
        void setupReadSampleBE();

        virtual void springClean();
};

//-------------------------------------------------------------------------------------------
} // namespace wavpackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
