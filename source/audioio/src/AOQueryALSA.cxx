//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryALSA.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOQueryALSA::AOQueryALSA() : AOQueryDevice()
{
    LinuxALSAIF::instance("alsa");    
}

//-------------------------------------------------------------------------------------------

AOQueryALSA::~AOQueryALSA()
{}

//-------------------------------------------------------------------------------------------

void AOQueryALSA::printError(const tchar *strR,const tchar *strE,int rc) const
{
    common::Log::g_Log << "AOQueryALSA::" << strR << " - " << strE << "." << LinuxALSAIF::instance()->snd_strerror(rc) << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSA::queryNames()
{
    tint idx;
    bool res = false;
    QVector<tint> cards;
    
    cards = listOfCards();
    if(!cards.isEmpty())
    {
        for(idx=0;idx<cards.size();idx++)
        {
            int status;
            char *name = 0;
            int card = cards.at(idx);

            status = LinuxALSAIF::instance()->snd_card_get_longname(card,&name);
            if(!status && name!=0)
            {
                QString n = QString::fromUtf8(name);
                DeviceALSA *pDevice = new DeviceALSA();
                pDevice->id() = QString::number(card);
                pDevice->name() = n;
                m_devices.append(pDevice);
                res = true;
            }
            else
            {
                printError("queryNames","Failed to long name of sound card",status);
            }
        }
    }
    else
    {
        common::Log::g_Log << "No sound hardware found. Cannot initialise audio." << common::c_endl;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSA::queryDevice(int idx)
{
    bool res = false;
    
    if(idx>=0 && idx<m_devices.size())
    {
        DeviceALSA *pDevice = dynamic_cast<DeviceALSA *>(m_devices[idx]);
        if(!pDevice->isInitialized())
        {
            res = pDevice->queryDevice(pDevice->id().toInt());
        }
        else
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int AOQueryALSA::defaultDeviceIndex()
{
    // TODO - find if ALSA has default PCM device. For now return first device.
    return 0;
}

//-------------------------------------------------------------------------------------------

QVector<tint> AOQueryALSA::listOfCards() const
{
    int status,card;
    QVector<tint> cards;
    
    card = -1;
    do
    {
        status = LinuxALSAIF::instance()->snd_card_next(&card);
        if(status >= 0)
        {
            if(card >= 0)
            {
                cards.append(card);
            }
        }
        else
        {
            printError("listOfCards","Failed to query next sound card",status);
        }
    } while(status>=0 && card>=0);
    
    return cards;
}

//-------------------------------------------------------------------------------------------
// AOQueryALSA::Device
//-------------------------------------------------------------------------------------------

const int AOQueryALSA::DeviceALSA::c_alsaFormats[18] = {
    SND_PCM_FORMAT_S8, 
    SND_PCM_FORMAT_S16_LE, 
    SND_PCM_FORMAT_S16_BE, 
    SND_PCM_FORMAT_S24_LE, 
    SND_PCM_FORMAT_S24_BE, 
    SND_PCM_FORMAT_S32_LE, 
    SND_PCM_FORMAT_S32_BE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT_BE, 
    SND_PCM_FORMAT_FLOAT64_LE,
    SND_PCM_FORMAT_FLOAT64_BE,
    SND_PCM_FORMAT_S24_3LE,
    SND_PCM_FORMAT_S24_3BE,
    SND_PCM_FORMAT_S20_3LE,
    SND_PCM_FORMAT_S20_3BE,
    SND_PCM_FORMAT_S18_3LE,
    SND_PCM_FORMAT_S18_3BE,
    SND_PCM_FORMAT_UNKNOWN
};

//-------------------------------------------------------------------------------------------

AOQueryALSA::DeviceALSA::DeviceALSA() : AOQueryDevice::Device(),
    m_card(0),
    m_formats()
{
    m_type = AOQueryDevice::Device::e_deviceALSA;
}

//-------------------------------------------------------------------------------------------

AOQueryALSA::DeviceALSA::DeviceALSA(const DeviceALSA& rhs) : AOQueryDevice::Device(),
    m_card(0),
    m_formats()
{
    AOQueryALSA::DeviceALSA::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AOQueryALSA::DeviceALSA::~DeviceALSA()
{}

//-------------------------------------------------------------------------------------------

void AOQueryALSA::DeviceALSA::printError(const tchar *strE,const tchar *strR) const
{
    common::Log::g_Log << "AOQueryALSA::DeviceALSA::" << strE << " - " << strR << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AOQueryALSA::DeviceALSA::printErrorOS(const tchar *strE,const tchar *strR,int err) const
{
    QString str = strR;
    str += " " + QString::fromUtf8(LinuxALSAIF::instance()->snd_strerror(err));
    printError(strE,str.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSA::DeviceALSA::isSupported(const FormatDescription& desc) const
{
    return m_formats.isSupported(desc);
}

//-------------------------------------------------------------------------------------------

const FormatsSupported& AOQueryALSA::DeviceALSA::supported() const
{
    return m_formats;
}

//-------------------------------------------------------------------------------------------

void AOQueryALSA::DeviceALSA::querySupportedFormats(snd_pcm_t *handle)
{
    int formatIdx,channelIdx;
    QSet<tint> frequencies = FormatDescription::setOfFrequencies();

    m_formats.clear();
    for(formatIdx=0;c_alsaFormats[formatIdx]!=SND_PCM_FORMAT_UNKNOWN;formatIdx++)
    {
        for(channelIdx=0;channelIdx<8;channelIdx++)
        {
            tint noChannels = channelIdx + 1;
            for(QSet<tint>::iterator ppI=frequencies.begin();ppI!=frequencies.end();ppI++)
            {
                tint freq = *ppI;
                FormatDescription desc;
                
                if(descriptionFromFormat(c_alsaFormats[formatIdx],noChannels,freq,desc))
                {
                    if(hasFormat(handle,desc))
                    {
                        m_formats.add(desc);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSA::DeviceALSA::canSupportFormat(snd_pcm_t *handle,tint fType,tint noChannels,tint frequency) const
{
    int status;
    bool res = false;    
    snd_pcm_hw_params_t *params = 0;
    
    status = LinuxALSAIF::instance()->snd_pcm_hw_params_malloc(&params);
    if(!status && params!=0)
    {
        status = LinuxALSAIF::instance()->snd_pcm_hw_params_any(handle,params);
        if(!status)
        {
            status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_rate_resample(handle,params,0);
            if(!status)
            {
                status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_access(handle,params,SND_PCM_ACCESS_RW_INTERLEAVED);
                if(!status)
                {
                    status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_channels(handle,params,noChannels);
                    if(!status)
                    {
                        snd_pcm_format_t formatType = static_cast<snd_pcm_format_t>(fType);
                        
                        status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_format(handle,params,formatType);
                        if(!status)
                        {
                            status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_rate(handle,params,frequency,0);
                            if(!status)
                            {
                                status = LinuxALSAIF::instance()->snd_pcm_hw_params(handle,params);
                                if(!status)
                                {
                                    res = true;
                                }
                            }
                        }
                    }
                }
                else
                {
                       printErrorOS("hasFormat","Error restricting ALSA hardware parameters to PCM read/write access",status);
                }
            }
            else
            {
                printErrorOS("hasFormat","Failed to turn off ALSA software resampler",status);
            }
        }
        else
        {
               printErrorOS("hasFormat","Error getting ALSA hardware parameters for PCM",status);
        }
        LinuxALSAIF::instance()->snd_pcm_hw_params_free(params);
    }
    else
    {
         printErrorOS("hasFormat","Failed to allocate ALSA hardware parameters",status);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSA::DeviceALSA::hasFormat(snd_pcm_t *handle,const FormatDescription& desc) const
{
    int status;
    bool res = false;
    
    QVector<int> formatTypes = formatsFromDescription(desc);

    for(QVector<int>::iterator ppI = formatTypes.begin();ppI!=formatTypes.end() && !res;ppI++)
    {
        res = canSupportFormat(handle,*ppI,desc.channels(),desc.frequency());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QVector<int> AOQueryALSA::DeviceALSA::formatsFromDescription(const FormatDescription& desc) const
{
    QVector<int> formats;
    
    if(desc.typeOfData()==FormatDescription::e_DataFloatSingle)
    {
        formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_FLOAT_LE : SND_PCM_FORMAT_FLOAT_BE);
    }
    else if(desc.typeOfData()==FormatDescription::e_DataFloatDouble)
    {
        formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_FLOAT64_LE : SND_PCM_FORMAT_FLOAT64_BE);
    }
    else if(desc.typeOfData()==FormatDescription::e_DataSignedInteger)
    {
        if(desc.bits()==8)
        {
            formats.append(SND_PCM_FORMAT_S8);
        }
        else if(desc.bits()==16)
        {
            formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_S16_LE : SND_PCM_FORMAT_S16_BE);
        }
        else if(desc.bits()==18)
        {
            formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_S18_3LE : SND_PCM_FORMAT_S18_3BE);
        }
        else if(desc.bits()==20)
        {
            formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_S20_3LE : SND_PCM_FORMAT_S20_3BE);
        }
        else if(desc.bits()==24)
        {
            formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_S24_3LE : SND_PCM_FORMAT_S24_3BE);
            formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_S24_LE : SND_PCM_FORMAT_S24_BE);
        }
        else if(desc.bits()==32)
        {
            formats.append((desc.isLittleEndian()) ? SND_PCM_FORMAT_S32_LE : SND_PCM_FORMAT_S32_BE);
        }
    }
    return formats;
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSA::DeviceALSA::descriptionFromFormat(int alsaFormat,int noChannels,int frequency,FormatDescription& desc) const
{
    bool res = true;
    
    switch(alsaFormat)
    {
        case SND_PCM_FORMAT_S8:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(8);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_S16_LE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(16);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_S16_BE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(16);
            desc.setEndian(false);
            break;
        case SND_PCM_FORMAT_S24_LE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(24);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_S24_BE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(24);
            desc.setEndian(false);
            break;
        case SND_PCM_FORMAT_S32_LE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(32);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_S32_BE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(32);
            desc.setEndian(false);
            break;
        case SND_PCM_FORMAT_FLOAT_LE:
            desc.setTypeOfData(FormatDescription::e_DataFloatSingle);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_FLOAT_BE:
            desc.setTypeOfData(FormatDescription::e_DataFloatSingle);
            desc.setEndian(false);
            break;        
        case SND_PCM_FORMAT_FLOAT64_LE:
            desc.setTypeOfData(FormatDescription::e_DataFloatDouble);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_FLOAT64_BE:
            desc.setTypeOfData(FormatDescription::e_DataFloatDouble);
            desc.setEndian(false);
            break;
        case SND_PCM_FORMAT_S24_3LE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(24);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_S24_3BE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(24);
            desc.setEndian(false);
            break;
        case SND_PCM_FORMAT_S20_3LE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(20);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_S20_3BE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(20);
            desc.setEndian(false);
            break;
        case SND_PCM_FORMAT_S18_3LE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(18);
            desc.setEndian(true);
            break;
        case SND_PCM_FORMAT_S18_3BE:
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            desc.setNumberOfBits(18);
            desc.setEndian(false);
            break;
        default:
            res = false;
            break;
    }
    desc.setNumberOfChannels(noChannels);
    desc.setFrequency(frequency);
    return res;
}

//-------------------------------------------------------------------------------------------

void AOQueryALSA::DeviceALSA::populateFrequencyAndChannelSets()
{
    int totalChannels = 0;
    int formatIdx,channelIdx;
    QSet<tint> frequencies = FormatDescription::setOfFrequencies();
    
    for(formatIdx=0;c_alsaFormats[formatIdx]!=SND_PCM_FORMAT_UNKNOWN;formatIdx++)
    {
        for(channelIdx=0;channelIdx<8;channelIdx++)
        {
            tint noChannels = channelIdx + 1;

            for(QSet<tint>::iterator ppI=frequencies.begin();ppI!=frequencies.end();ppI++)
            {
                tint freq = *ppI;
                FormatDescription desc;
                
                if(descriptionFromFormat(c_alsaFormats[formatIdx],noChannels,freq,desc))
                {
                    if(isSupported(desc))
                    {
                        if(!isFrequencySupported(freq))
                        {
                            addFrequency(freq);
                        }
                        if(noChannels > totalChannels)
                        {
                            totalChannels = noChannels;
                        }
                    }
                }
            }
        }
    }
    setNoChannels(totalChannels);
    loadChannelMap();
}

//-------------------------------------------------------------------------------------------

QString AOQueryALSA::DeviceALSA::pcmDeviceName() const
{
    QString hw = "hw:" + QString::number(m_card);
    return hw;
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSA::DeviceALSA::queryDevice(int cardNo)
{
    int status;
    snd_pcm_t *handle;
    QString hwName;
    bool res = false;

    m_card = cardNo;
    hwName = pcmDeviceName().toLatin1();

    status = LinuxALSAIF::instance()->snd_pcm_open(&handle,hwName.toLatin1().data(),SND_PCM_STREAM_PLAYBACK,0);
    if(!status)
    {
        querySupportedFormats(handle);
        populateFrequencyAndChannelSets();
        setInitialized();
        res = true;
        
        status = LinuxALSAIF::instance()->snd_pcm_close(handle);
        if(status)
        {
            QString err = "Error closing PCM sound device " + hwName;
            printErrorOS("queryDevice",err.toUtf8().constData(),status);
        }
    }
    else
    {
        QString err = "Failed to open PCM sound device " + hwName;
        printErrorOS("queryDevice",err.toUtf8().constData(),status);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOQueryALSA::DeviceALSA::copy(const AOQueryDevice::Device& rhs)
{
    const AOQueryALSA::DeviceALSA& alsaDevice = dynamic_cast<const AOQueryALSA::DeviceALSA&>(rhs);
    AOQueryDevice::Device::copy(rhs);
    m_card = alsaDevice.m_card;
    m_formats = alsaDevice.m_formats;
}

//-------------------------------------------------------------------------------------------

void AOQueryALSA::DeviceALSA::print() const
{
    QSet<tint> frequencies = FormatDescription::setOfFrequencies();

    AOQueryDevice::Device::print();
    
    for(int channelIdx=0;channelIdx<8;channelIdx++)
    {
        tint noChannels = channelIdx + 1;
        
        for(QSet<int>::iterator ppI=frequencies.begin();ppI!=frequencies.end();ppI++)
        {
            for(int formatIdx=0;c_alsaFormats[formatIdx]!=SND_PCM_FORMAT_UNKNOWN;formatIdx++)
            {
                tint freq = *ppI;
                FormatDescription desc;
            
                if(descriptionFromFormat(c_alsaFormats[formatIdx],noChannels,freq,desc))
                {
                    if(isSupported(desc))
                    {
                        QString str = QString("%1Chs, %2Hz, %3").arg(noChannels).arg(freq).arg(formatToString(c_alsaFormats[formatIdx]));
                        common::Log::g_Log.print("%s\n",str.toUtf8().constData());
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

QString AOQueryALSA::DeviceALSA::formatToString(int alsaFormat)
{
    QString fmt;
    
    switch(alsaFormat)
    {
        case SND_PCM_FORMAT_S8:
            fmt = "Signed 8-Bits";
            break;
        case SND_PCM_FORMAT_S16_LE:
            fmt = "Signed 16-Bits Little Endian";
            break;
        case SND_PCM_FORMAT_S16_BE:
            fmt = "Signed 16-Bits Big Endian";
            break;
        case SND_PCM_FORMAT_S24_LE:
            fmt = "Signed 24-Bits (4-Bytes) Little Endian";
            break;
        case SND_PCM_FORMAT_S24_BE:
            fmt = "Signed 24-Bits (4-Bytes) Big Endian";
            break;
        case SND_PCM_FORMAT_S32_LE:
            fmt = "Signed 32-Bits Little Endian";
            break;
        case SND_PCM_FORMAT_S32_BE:
            fmt = "Signed 32-Bits Big Endian";
            break;
        case SND_PCM_FORMAT_FLOAT_LE:
            fmt = "Single Precision Float 32-Bits Little Endian";
            break;
        case SND_PCM_FORMAT_FLOAT_BE:
            fmt = "Single Precision Float 32-Bits Big Endian";
            break;        
        case SND_PCM_FORMAT_FLOAT64_LE:
            fmt = "Double Precision Float 64-Bits Little Endian";
            break;
        case SND_PCM_FORMAT_FLOAT64_BE:
            fmt = "Double Precision Float 64-Bits Big Endian";
            break;
        case SND_PCM_FORMAT_S24_3LE:
            fmt = "Signed 24-Bits (3-Bytes) Little Endian";
            break;
        case SND_PCM_FORMAT_S24_3BE:
            fmt = "Signed 24-Bits (3-Bytes) Big Endian";
            break;
        case SND_PCM_FORMAT_S20_3LE:
            fmt = "Signed 20-Bits Little Endian";
            break;
        case SND_PCM_FORMAT_S20_3BE:
            fmt = "Signed 20-Bits Big Endian";
            break;
        case SND_PCM_FORMAT_S18_3LE:
            fmt = "Signed 18-Bits Little Endian";
            break;
        case SND_PCM_FORMAT_S18_3BE:
            fmt = "Signed 18-Bits Big Endian";
            break;
        default:
            break;
    }
    return fmt;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
