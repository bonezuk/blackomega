//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_CODEC_H
#define __OMEGA_ENGINE_CODEC_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Factory.h"
#include "common/inc/Mutex.h"
#include "engine/inc/EngineDLL.h"
#include "engine/inc/AData.h"
#include "engine/inc/RData.h"
#include "common/inc/BIOMemoryStream.h"
#include "engine/inc/IOFile.h"
#include "common/inc/BIOBufferedStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT CodecInitialize
{
    public:
        CodecInitialize();
        ~CodecInitialize();

        static void start();
        static void end();

    protected:

        static CodecInitialize *m_instance;
};

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Codec : public QObject
{
    public:
        Q_OBJECT

    public:
        
        typedef enum
        {
            e_codecMp3,
            e_codecMp4,
            e_codecWav,
            e_codecVorbis,
            e_codecFlac,
            e_codecWma,
            e_codecALAC,
            e_codecVorbisRTP,
            e_codecAIFF,
            e_codecMPC,
            e_codecAPE,
            e_codecWavPack,
            e_codecTone,
            e_codecUnknown
        } CodecType;
    
    public:
        Codec(CodecType type,QObject *parent = 0);
        virtual ~Codec();
        
        static bool isSupported(const QString& name);
        static bool isSupported(const QString& name, QString& key);
        
        static Codec *get(const QString& name);
        
        virtual Codec::CodecType type() const;
        
        virtual bool open(const QString& name) = 0;
        virtual void close() = 0;
        
        virtual bool isInitialized() const;
        virtual bool init() = 0;
        virtual bool next(AData& data) = 0;
        
        virtual bool isSeek() const = 0;
        // seek to first time window position before the given timestamp v.
        virtual bool seek(const common::TimeStamp& v) = 0;

        virtual bool isComplete() const = 0;
        
        virtual bool isRemote() const = 0;
        virtual bool isBuffered(tfloat32& percent) = 0;
        virtual void buffer(const common::TimeStamp& bLen);
        virtual void resync();
        
        virtual tint bitrate() const = 0;
        virtual tint frequency() const = 0;
        virtual tint noChannels() const = 0;
        virtual common::TimeStamp length() const = 0;
    
        static QString getFileExtension(const QString& name);
        static QString getFileExtension(const QString& name,QString& pro);

        virtual bool loadFileToMemory() const;

        static void setAudioThreadID();

        virtual tint noOutputChannels() const;
        virtual void setNoOutputChannels(tint noChs);
        
        virtual CodecDataType dataTypesSupported() const;
        virtual bool setDataTypeFormat(CodecDataType type);

    protected:
    
        static Qt::HANDLE m_audioThreadID;

        CodecType m_type;
        
        bool m_initFlag;
        common::TimeStamp m_bufferTimeLength;
        
        int m_noOutputChannels;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;

    signals:

        void onInit(void*);
};

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(ENGINE_EXPORT,CodecFactory,Codec)

//-------------------------------------------------------------------------------------------

inline tint16 *sampleInt16AtOffset(sample_t *buffer, tint idx)
{
    tint16 *bInt16 = reinterpret_cast<tint16 *>(buffer);
    return &bInt16[idx];
}

//-------------------------------------------------------------------------------------------

inline tint32 *sampleInt24AtOffset(sample_t *buffer, tint idx)
{
    tint32 *bInt24 = reinterpret_cast<tint32 *>(buffer);
    return &bInt24[idx];
}

//-------------------------------------------------------------------------------------------

inline tint32 *sampleInt32AtOffset(sample_t *buffer, tint idx)
{
    return sampleInt24AtOffset(buffer, idx);
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
