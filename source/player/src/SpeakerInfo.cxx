#include "player/inc/SpeakerInfo.h"

#include <QFile>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(track::info::InfoFactory,track::info::Info, \
                            SpeakerInfoFactory,SpeakerInfo, \
                            "speaker",false)

//-------------------------------------------------------------------------------------------

SpeakerInfo::SpeakerInfo(QObject *parent) : track::info::Info(parent),
    m_speakerType(audioio::e_UnknownChannel),
    m_imageResource()
{}

//-------------------------------------------------------------------------------------------

SpeakerInfo::~SpeakerInfo()
{}

//-------------------------------------------------------------------------------------------

void SpeakerInfo::setFilename(const QString& name)
{
    track::info::Info::setFilename(name);
    setSpeakerType();
}

//-------------------------------------------------------------------------------------------

void SpeakerInfo::setSpeakerType()
{
    QString fileName = filename();

    if(fileName=="(01)-center.wav")
    {
         m_speakerType = audioio::e_Center;
         m_Title = "Center Speaker";
         m_imageResource = ":/speakers/Resources/speakers/centre.png";
    }
    else if(fileName=="(02)-left_front.wav")
    {
         m_speakerType = audioio::e_FrontLeft;
         m_Title = "Front Left Speaker";
         m_imageResource = ":/speakers/Resources/speakers/frontLeft.png";
    }
    else if(fileName=="(03)-right_front.wav")
    {
         m_speakerType = audioio::e_FrontRight;
         m_Title = "Front Right Speaker";
         m_imageResource = ":/speakers/Resources/speakers/frontRight.png";
    }
    else if(fileName=="(04)-left_surround.wav")
    {
         m_speakerType = audioio::e_SurroundLeft;
         m_Title = "Surround Left Speaker";
         m_imageResource = ":/speakers/Resources/speakers/surroundLeft.png";
    }
    else if(fileName=="(05)-right_surround.wav")
    {
         m_speakerType = audioio::e_SurroundRight;
         m_Title = "Surround Right Speaker";
         m_imageResource = ":/speakers/Resources/speakers/surroundRight.png";
    }
    else if(fileName=="(06)-left_rear.wav")
    {
         m_speakerType = audioio::e_RearLeft;
         m_Title = "Rear Left Speaker";
         m_imageResource = ":/speakers/Resources/speakers/surroundLeft.png";
    }
    else if(fileName=="(07)-right_rear.wav")
    {
         m_speakerType = audioio::e_RearRight;
         m_Title = "Rear Right Speaker";
         m_imageResource = ":/speakers/Resources/speakers/surroundRight.png";
    }
    else if(fileName=="(08)-subwoofer.wav")
    {
         m_speakerType = audioio::e_LFE;
         m_Title = "Subwoofer";
         m_imageResource = ":/speakers/Resources/speakers/subwoofer.png";
    }
    else
    {
         m_speakerType = audioio::e_UnknownChannel;
    }

    {
        QString fName = ":/surround/Resources/surround/" + fileName;
        engine::Codec *codec = engine::Codec::get(fName);

        if(codec!=0)
        {
            setLength(codec->length());
            delete codec;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool SpeakerInfo::isImage() const
{
    if(m_imageResource.isEmpty())
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-------------------------------------------------------------------------------------------

track::info::ImageInfoArray *SpeakerInfo::getImageData(track::info::Info::ImageFormat& format) const
{
    track::info::ImageInfoArray *iArray = 0;

    if(!m_imageResource.isEmpty())
    {
        QFile iFile(m_imageResource);
        if(iFile.open(QIODevice::ReadOnly))
        {
            iArray = new track::info::ImageInfoArray;
            iArray->SetSize(iFile.size());
            iFile.read(reinterpret_cast<char *>(iArray->GetData()),iFile.size());
            format = track::info::Info::e_imagePNG;
        }
    }
    return iArray;
}

//-------------------------------------------------------------------------------------------

track::info::ImageInfoArray *SpeakerInfo::getImageData(track::info::IDTagImageType type,track::info::Info::ImageFormat& format) const
{
    return getImageData(format);
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
