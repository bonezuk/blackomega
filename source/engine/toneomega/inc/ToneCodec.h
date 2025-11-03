//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_TONEOMEGA_TONEOMEGA_H
#define __OMEGA_ENGINE_TONEOMEGA_TONEOMEGA_H
//-------------------------------------------------------------------------------------------

#include "engine/toneomega/inc/ToneOmegaDLL.h"
#include "engine/inc/Codec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace toneomega
{
//-------------------------------------------------------------------------------------------

class TONEOMEGA_EXPORT ToneCodec : public Codec
{
    public:
        Q_OBJECT
    public:
    
        typedef enum
        {
            e_toneSine,
            e_toneCosine,
            e_toneFixed,
            e_toneUnknown
        } ToneType;
    
    public:
        ToneCodec(QObject *parent = 0);
        virtual ~ToneCodec();
        
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
        
        ToneType m_toneType;
        tint m_toneFrequency;
        tint m_playbackFrequency;
        
        tint64 m_frameNumber;
        common::TimeStamp m_length;
        
        bool m_completeFlag;
        
        tfloat64 m_phase;
        tfloat64 m_phaseInc;
        
        tfloat64 m_fixedSample;

        tint m_intPhase;
        tint m_intPhaseInc;
        
        tint m_channelSelect;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual common::TimeStamp timeFramePosition(tint64 cFrame) const;
        
        virtual bool openFrequency(const QString& name);
        virtual bool openFixed(const QString& name);
};

//-------------------------------------------------------------------------------------------
} // namespace toneomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
