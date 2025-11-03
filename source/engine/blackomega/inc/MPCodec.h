//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_MPCODEC_H
#define __OMEGA_ENGINE_BLACKOMEGA_MPCODEC_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Codec.h"
#include "engine/blackomega/inc/BSequence.h"
#include "engine/blackomega/inc/BandIndex.h"
#include "engine/blackomega/inc/MPHeader.h"
#include "engine/blackomega/inc/ScaleFactor.h"
#include "engine/blackomega/inc/Dequantize.h"
#include "engine/blackomega/inc/DequantizeStereo.h"
#include "engine/blackomega/inc/Stereo.h"
#include "engine/blackomega/inc/AntiAlias.h"
#include "engine/blackomega/inc/Reorder.h"
#include "engine/blackomega/inc/Hybrid.h"
#include "engine/blackomega/inc/Subband.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT MPCodecInitialize
{
    public:
        MPCodecInitialize();
        ~MPCodecInitialize();

        static void start();
        static void end();

    protected:

        static MPCodecInitialize *m_instance;
};

//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT MPCodec : public engine::Codec
{
    public:
        Q_OBJECT

    public:
        MPCodec(QObject *parent=0);
        virtual ~MPCodec();

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

    protected:

        common::Allocation m_alloc;

        common::BIOBufferedStream *m_cachedFile;
        engine::File *m_file;

        BBitstream *m_bs;
        tint m_state;
        common::TimeStamp m_time;

        MPHeader *m_header;
        ScaleFactor *m_scale[2][2];
        Dequantize *m_dequantize[2];
        DequantizeStereo *m_dequantizeStereo[2];
        Stereo *m_stereo[2];
        Reorder *m_reorder[2][2];
        AntiAlias *m_antiAlias[2][2];
        Hybrid *m_hybrid[2];
        Subband *m_subband[2];

        sample_t m_hybridOut[SBLIMIT][SSLIMIT];
        sample_t *m_polyPhaseIn;

        tint m_grMax;

        tint m_outOffset;
        sample_t m_out[SBLIMIT * SSLIMIT * 4];

        tint m_frameCount;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        void initStructure();
        void freeStructure();

        void link(HeaderItem *item);

        bool decode();
};

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

