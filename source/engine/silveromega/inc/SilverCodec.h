//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_SILVEROMEGA_SILVERCODEC_H
#define __OMEGA_SILVEROMEGA_SILVERCODEC_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Mutex.h"
#include "engine/inc/Codec.h"
#include "engine/inc/File.h"
#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverContainer.h"
#include "engine/silveromega/inc/VSilverOgg.h"
#include "engine/silveromega/inc/VSilverWindow.h"
#include "engine/silveromega/inc/VSilverSeeker.h"
#include "engine/inc/OmegaDCT.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT SilverCodecInitialize
{
    public:
        SilverCodecInitialize();
        ~SilverCodecInitialize();
        
        static void start();
        static void end();

    protected:
    
        static SilverCodecInitialize *m_instance;
};

//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT SilverCodec : public engine::Codec
{
    public:
        Q_OBJECT
    
    public:
        SilverCodec(QObject *parent = 0);
        virtual ~SilverCodec();
        
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

        static void frameSynthesis(VSilverContainer *data,engine::Sequence *seq,VSilverWindow *window);
        
    protected:
        
        QString m_name;
        
        bool m_seekService;
        bool m_remoteService;
        
        common::BIOBufferedStream *m_cachedFile;
        engine::File *m_file;
        engine::Bitstream *m_stream;
        VSilverOgg *m_oggStream;
        VSilverContainer *m_container;
        VSilverWindow *m_window;
        
        int m_state;
        common::TimeStamp m_time;
        
        VSilverSeeker *m_seeker;
                
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------

extern void frameSynthesis(VSilverContainer *data,engine::Sequence *seq,VSilverWindow *window);

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
