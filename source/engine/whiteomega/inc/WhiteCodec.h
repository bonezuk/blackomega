//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_WHITECODEC_H
#define __OMEGA_WHITEOMEGA_WHITECODEC_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Codec.h"
#include "engine/whiteomega/inc/WSequence.h"
#include "engine/whiteomega/inc/AACDecode.h"
#include "engine/whiteomega/inc/AACRoot.h"
#include "engine/whiteomega/inc/Atom.h"
#include "common/inc/BIOBufferedStream.h"
#include "engine/inc/IOFile.h"
#include "engine/whiteomega/inc/ALACAtomContainer.h"
#include "engine/redomega/inc/ALACDecoder.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT WhiteCodecInitialize
{
    public:
        WhiteCodecInitialize();
        ~WhiteCodecInitialize();

        static void start();
        static void end();

    protected:

        static WhiteCodecInitialize *m_instance;
};

//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT WhiteCodec : public engine::Codec
{
    public:
        Q_OBJECT

    public:
        WhiteCodec(QObject *parent = 0);
        virtual ~WhiteCodec();

        virtual bool open(const QString& name);
        virtual void close();

        virtual bool init();
        virtual bool next(AData& data);

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

        GAConfig m_gaConfig;

        common::BIOBufferedStream *m_fileStream;
        IOFile *m_file;
        Atom *m_atom;
        AACRoot *m_decoder;

        tint m_dSampleNo;
        tint m_dSampleLen;

        tint m_state;
        common::TimeStamp m_time;

        tint m_outOffset;
        tint m_outLen;
        sample_t *m_out;

        QSharedPointer<common::Array<tubyte,tubyte> > m_seqArray;
        WSequence *m_sequence;

        ALACAtomContainer *m_alacContainer;
        redomega::ALACDecoder *m_alacDecoder;
        redomega::ALACSequence *m_alacSequence;

        CodecDataType m_outputFormatType;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        template<class S> void sortChannels(S *buffer, tint amount, tint noChs);

        virtual void setPartDataType(RData::Part& part);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

