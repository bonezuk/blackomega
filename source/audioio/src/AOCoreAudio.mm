#include "audioio/inc/AOCoreAudio.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOCoreAudio::AOCoreAudio(QObject *parent) : AOBase(parent)
{}

//-------------------------------------------------------------------------------------------

AOCoreAudio::~AOCoreAudio()
{}

//-------------------------------------------------------------------------------------------

int AOCoreAudio::getDeviceFrequency()
{
    int iFreq = m_frequency;
    int oFreq = -1;

    if(m_deviceIdx>=0 && m_deviceIdx<m_deviceInfo->noDevices())
    {
        const AOQueryDevice::Device& dev = m_deviceInfo->device(m_deviceIdx);

        if(!dev.isFrequencySupported(iFreq))
        {
            int diff;
            const QSet<int>& fSet = dev.frequencies();
            QSet<int>::const_iterator ppI;
            bool found = false;

            for(ppI=fSet.begin();ppI!=fSet.end();ppI++)
            {
                int cFreq = *ppI;
                if(cFreq > iFreq)
                {
                    diff = cFreq - iFreq;
                    if(found)
                    {
                        if(diff < (oFreq - iFreq))
                        {
                            oFreq = cFreq;
                        }
                    }
                    else
                    {
                        oFreq = cFreq;
                        found = true;
                    }
                }
            }
            if(!found)
            {
                for(ppI=fSet.begin();ppI!=fSet.end();ppI++)
                {
                    int cFreq = *ppI;
                    if(cFreq < iFreq)
                    {
                        diff = iFreq - cFreq;
                        if(found)
                        {
                            if(diff < (iFreq - oFreq))
                            {
                                oFreq = cFreq;
                            }
                        }
                        else
                        {
                            oFreq = cFreq;
                            found = true;
                        }
                    }
                }
            }

            if(found)
            {
                initResampler(iFreq,oFreq);
                m_frequency = oFreq;
            }
            else
            {
                oFreq = -1;
            }
        }
        else
        {
            oFreq = m_frequency;
        }
    }
    return oFreq;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
