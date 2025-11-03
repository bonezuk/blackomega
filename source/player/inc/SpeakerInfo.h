//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SPEAKERINFO_H
#define __OMEGA_PLAYER_SPEAKERINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"
#include "audioio/inc/AOBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class SpeakerInfo : public track::info::Info
{
    public:
        SpeakerInfo(QObject *parent = 0);
        virtual ~SpeakerInfo();
        
        virtual void setFilename(const QString& name);

        virtual bool isImage() const;
        virtual track::info::ImageInfoArray *getImageData(track::info::Info::ImageFormat& format) const;
        virtual track::info::ImageInfoArray *getImageData(track::info::IDTagImageType type,track::info::Info::ImageFormat& format) const;

    protected:
    
        audioio::ChannelType m_speakerType;
        QString m_imageResource;
        
        void setSpeakerType();
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

