//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_GREENOMEGA_FLACMETAPICTURE_H
#define __OMEGA_GREENOMEGA_FLACMETAPICTURE_H
//-------------------------------------------------------------------------------------------

#include "engine/greenomega/inc/FLACMetaBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACMetaPicture : public FLACMetaBase
{
    public:
        
        typedef enum
        {
            e_Other = 0,
            e_Icon = 1,
            e_IconFile = 2,
            e_CoverFront = 3,
            e_CoverBack = 4,
            e_LeafletPage = 5,
            e_Media = 6,
            e_LeadArtist = 7,
            e_Artist = 8,
            e_Conductor = 9,
            e_Band = 10,
            e_Composer = 11,
            e_Lyricist = 12,
            e_RecordingLocation = 13,
            e_DuringRecording = 14,
            e_DuringPerformance = 15,
            e_Movie = 16,
            e_Fish = 17,
            e_Illustration = 18,
            e_BandLogo = 19,
            e_PublisherLogo = 20
        } PictureType;
        
        typedef enum
        {
            e_imageUnknown = 0,
            e_imageJPEG,
            e_imageGIF,
            e_imagePNG,
            e_imageBMP
        } PictureFormat;
        
    public:
        FLACMetaPicture();
        virtual ~FLACMetaPicture();
        
        virtual bool read(engine::Sequence *seq);
        
        const PictureType& imageType() const;
        const PictureFormat& imageFormat() const;
        const common::Array<tubyte,tubyte> *image() const;
        const QString& description() const;
        
    protected:
        
        PictureType m_imageType;
        PictureFormat m_imageFormat;
        common::Array<tubyte,tubyte> *m_image;
        QString m_description;
        
        QString format(const QString& mime);
        QString readString(engine::Sequence *seq) const;
};

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

