//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AOBASE_AOWIRELESSBRIDGE_H
#define __OMEGA_AOBASE_AOWIRELESSBRIDGE_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOWirelessBridge : public AOBase
{
    public:
        AOWirelessBridge(QObject *parent = 0);
        virtual ~AOWirelessBridge();

    protected:

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual engine::AData *allocateData(tint len,tint inChannel,tint outChannel);

        virtual bool openAudio();
        virtual void closeAudio();
        virtual bool startAudioDevice();
        virtual void stopAudioDevice();
        virtual bool isAudio() const;

        virtual void processComplete();
        virtual void processMessages();

        virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOWirelessBridge.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
