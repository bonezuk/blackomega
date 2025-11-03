#include <QBuffer>
#include <QByteArray>

#include "engine/greenomega/inc/FLACMetaPicture.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

FLACMetaPicture::FLACMetaPicture() : FLACMetaBase(e_Picture),
    m_imageType(e_Other),
    m_imageFormat(e_imageUnknown),
    m_image(0),
    m_description()
{}

//-------------------------------------------------------------------------------------------

FLACMetaPicture::~FLACMetaPicture()
{
    if(m_image!=0)
    {
        delete m_image;
        m_image = 0;
    }
}

//-------------------------------------------------------------------------------------------

const FLACMetaPicture::PictureType& FLACMetaPicture::imageType() const
{
    return m_imageType;
}

//-------------------------------------------------------------------------------------------

const FLACMetaPicture::PictureFormat& FLACMetaPicture::imageFormat() const
{
    return m_imageFormat;
}

//-------------------------------------------------------------------------------------------

const common::Array<tubyte,tubyte> *FLACMetaPicture::image() const
{
    return m_image;
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaPicture::description() const
{
    return m_description;
}

//-------------------------------------------------------------------------------------------

bool FLACMetaPicture::read(engine::Sequence *seq)
{
    tint width,height,depth,noColors,len;
    QString f;

    len = seq->readBitsI(24) << 3;
    m_imageType = static_cast<PictureType>(seq->readBitsI(32));
    f = readString(seq);
    format(f);
    m_description = readString(seq);
    width = seq->readBitsI(32);
    height = seq->readBitsI(32);
    depth = seq->readBitsI(32);
    noColors = seq->readBitsI(32);
    len = seq->readBitsI(32);
    if(len>0)
    {
        tint i;
        tubyte *x;

        m_image = new common::Array<tubyte,tubyte>();
        m_image->SetSize(len);
        x = m_image->GetData();
        for(i=0;i<len;i++)
        {
            x[i] = static_cast<tubyte>(seq->readBitsI(8));
        }
    }
    return (m_image!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

QString FLACMetaPicture::format(const QString& mime)
{
    QString t;

    if(mime.contains("png",Qt::CaseInsensitive))
    {
        m_imageFormat = e_imagePNG;
        t = "PNG";
    }
    else if(mime.contains("bmp",Qt::CaseInsensitive))
    {
        m_imageFormat = e_imageBMP;
        t = "BMP";
    }
    else if(mime.contains("gif",Qt::CaseInsensitive))
    {
        m_imageFormat = e_imageGIF;
        t = "GIF";
    }
    else if(mime.contains("jpeg",Qt::CaseInsensitive) || mime.contains("jpg",Qt::CaseInsensitive))
    {
        m_imageFormat = e_imageJPEG;
        t = "JPG";
    }
    return t;
}

//-------------------------------------------------------------------------------------------

QString FLACMetaPicture::readString(engine::Sequence *seq) const
{
    tint i,len;
    tchar *t;
    QString s;

    len = seq->readBitsI(32);
    t = new tchar [len + 1];
    for(i=0;i<len;i++)
    {
        t[i] = seq->readBitsI(8);
    }
    t[i] = '\0';
    s = QString::fromUtf8(t,len);
    delete [] t;
    return s;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
