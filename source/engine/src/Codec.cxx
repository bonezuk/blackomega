#include "engine/inc/Codec.h"
#include "engine/inc/OmegaDCT.h"
#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"

#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(CodecFactory,Codec)

//-------------------------------------------------------------------------------------------
// CodecInitialize
//-------------------------------------------------------------------------------------------

CodecInitialize *CodecInitialize::m_instance = 0;

//-------------------------------------------------------------------------------------------

CodecInitialize::CodecInitialize()
{
    Bitstream::start();
}

//-------------------------------------------------------------------------------------------

CodecInitialize::~CodecInitialize()
{
    try
    {
        OmegaDCT::stop();
        Bitstream::stop();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void CodecInitialize::start()
{
    if(m_instance==0)
    {
        m_instance = new CodecInitialize();
    }
}

//-------------------------------------------------------------------------------------------

void CodecInitialize::end()
{
    if(m_instance!=0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------
// Codec
//-------------------------------------------------------------------------------------------

Qt::HANDLE Codec::m_audioThreadID = 0;

//-------------------------------------------------------------------------------------------

Codec::Codec(CodecType type,QObject *parent) : QObject(parent),
    m_type(type),
    m_initFlag(false),
    m_bufferTimeLength(),
    m_noOutputChannels(2)
{}

//-------------------------------------------------------------------------------------------

Codec::~Codec()
{}

//-------------------------------------------------------------------------------------------

void Codec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Codec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

Codec::CodecType Codec::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

QString Codec::getFileExtension(const QString& name)
{
    QString pro;
    return getFileExtension(name,pro);
}

//-------------------------------------------------------------------------------------------

QString Codec::getFileExtension(const QString& name,QString& pro)
{
    int i;
    QString ext;

    pro = "";
    for(i=0;i<name.length() && pro.isEmpty();++i)
    {
        if(name.at(i)==QChar(':'))
        {
            pro = name.left(i).toLower();
        }
    }
    
    for(i=name.length()-2;i>=0 && ext.isEmpty();--i)
    {
        if(name.at(i)==QChar('.'))
        {
            ext = name.mid(i + 1).toLower();
        }
    }
    return ext;
}

//-------------------------------------------------------------------------------------------

bool Codec::isSupported(const QString& name)
{
    QString k;
    return isSupported(name, k);
}

//-------------------------------------------------------------------------------------------

bool Codec::isSupported(const QString& name, QString& key)
{
    QString ext, pro;
    bool res = true;
    
    ext = getFileExtension(name,pro);
    if(ext == "wav" || ext == "wave") 
    {
        // Waveform Audio File Format: .wav .wave,  https://en.wikipedia.org/wiki/WAV (Blueomega)
        key = "wav";
    }
    else if(ext == "mp3")
    {
        // MPEG-1 Audio Layer III or MPEG-2 Audio Layer III .mp3 (Blackomega) (Never seen a .bit file in all my years)
        key = "mp3";
    }
    else if(ext == "ogg")
    {
        // Vorbis Ogg: .ogg (Silveromega) (.oga is used for other Vorbis encapsulated codecs such as FLAC or Speex)
        key = "ogg";
    }
    else if(ext == "m4a" || ext == "m4b")
    {
        // MPEG-4 Audio AAC: .m4a, .m4b (Whiteomega) (Atom container can be MPEG-4 or Apple Lossless both of which whiteomega support)
        key = "m4a";
    }
    else if(ext == "caf")
    {
        // Apple Lossless CAF: .caf (Redomega) (Apple Lossless using CAF container,)
        key = "caf";
    }
    else if(ext == "mpc" || ext == "mp+" || ext == "mpp")
    {
        // Musepack: .mpc, .mp+, .mpp (Cyanomega) (Musepack https://en.wikipedia.org/wiki/Musepack)
        key = "mpc";
    }
    else if(ext == "flac")
    {
        // FLAC: .flac (Greenomega) (https://en.wikipedia.org/wiki/FLAC)
        key = "flac";
    }
    else if(ext == "aiff" || ext == "aif" || ext == "aifc")
    {
        // Audio Interchange File Format: .aiff, .aif, .aifc (Violetomega) (https://en.wikipedia.org/wiki/Audio_Interchange_File_Format)
        key = "aiff";
    }
    else if(ext == "wv")
    {
        // Wavpack: .wv (Wavpackomega) (.wvc is the correction file which must be paired with .wv file)
        key = "wv";
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

Codec *Codec::get(const QString& name)
{
    Codec *c = 0;
    QString ext, pro, key;
    
    ext = getFileExtension(name,pro);
    key = ext;
    
    if(pro=="rtp" && ext=="ogg")
    {
        ext += ":" + pro;
        c = CodecFactory::createUnmanaged(ext);
    }
    else if(isSupported(name, key) || ext=="tone")
    {
        c = CodecFactory::createUnmanaged(key);
    }

    if(c!=0)
    {
        if(!c->open(name))
        {
            delete c;
            c = 0;
        }
    }
    return c;
}

//-------------------------------------------------------------------------------------------

void Codec::buffer(const common::TimeStamp& bLen)
{
    m_bufferTimeLength = bLen;
}

//-------------------------------------------------------------------------------------------

void Codec::resync()
{}

//-------------------------------------------------------------------------------------------

bool Codec::isInitialized() const
{
    return m_initFlag;
}

//-------------------------------------------------------------------------------------------

bool Codec::loadFileToMemory() const
{
    bool flag;
    if(QThread::currentThreadId()==m_audioThreadID)
    {
        QSettings settings;
        settings.beginGroup("audio");
        if(settings.contains("loadFileToMemory"))
        {
            flag = settings.value("loadFileToMemory").toBool();
        }
        else
        {
            flag = false;
        }
        settings.endGroup();
    }
    else
    {
        flag = false;
    }
    return flag;
}

//-------------------------------------------------------------------------------------------

void Codec::setAudioThreadID()
{
    m_audioThreadID = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

tint Codec::noOutputChannels() const
{
    return m_noOutputChannels;
}

//-------------------------------------------------------------------------------------------

void Codec::setNoOutputChannels(tint noChs)
{
    if(noChs<=0)
    {
        noChs = 1;
    }
    else if(noChs>8)
    {
        noChs = 8;
    }
    m_noOutputChannels = noChs;
}

//-------------------------------------------------------------------------------------------

CodecDataType Codec::dataTypesSupported() const
{
    return e_SampleFloat;
}

//-------------------------------------------------------------------------------------------

bool Codec::setDataTypeFormat(CodecDataType type)
{
    return (!(type & ~e_SampleFloat) && type == e_SampleFloat) ? true : false;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
