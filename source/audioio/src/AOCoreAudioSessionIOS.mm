//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------

#import <Foundation/Foundation.h>
#import <AVFAudio/AVAudioSession.h>

#include "audioio/inc/AOCoreAudioSessionIOS.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

QSharedPointer<AOCoreAudioSessionIOS> AOCoreAudioSessionIOS::m_instance;

//-------------------------------------------------------------------------------------------

AOCoreAudioSessionIOS::AOCoreAudioSessionIOS(QObject *parent) : QObject(parent)
{
    AOCoreAudioSessionIOS::init();
}

//-------------------------------------------------------------------------------------------

AOCoreAudioSessionIOS::~AOCoreAudioSessionIOS()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOCoreAudioSessionIOS> AOCoreAudioSessionIOS::audioInstance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioSessionIOS::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "AOCoreAudioSessionIOS::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioSessionIOS::printError(const char *strR, const char *strE, void *pE) const
{
    NSError *error = (NSError *)pE;

    if(error != nil)
    {
        QString errS;
        QString desc = QString::fromUtf8(error.localizedDescription.UTF8String);
        QString reason = QString::fromUtf8(error.localizedFailureReason.UTF8String);
        QString recovery = QString::fromUtf8(error.localizedRecoverySuggestion.UTF8String);
        errS  = strE;
        errS +=    common::c_endl;
        errS += "\tDescription - " + desc + common::c_endl;
        errS += "\tReason - " + reason + common::c_endl;
        errS += "\tSuggestion - " + recovery + common::c_endl;
        printError(strR, errS.toUtf8().constData());
    }
    else
    {
        printError(strR, strE);
    }
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioSessionIOS::init()
{
    bool res = false;
    AVAudioSession *aSession = [AVAudioSession sharedInstance];

    if(aSession != nil)
    {
        NSError *audioSessionError = nil;
        BOOL r = [aSession setCategory:AVAudioSessionCategoryPlayback error:&audioSessionError];
        if(r == YES)
        {
            r = [aSession setActive:YES error:&audioSessionError];
            if(r == YES)
            {
                res = true;
            }
            else
            {
                printError("init", "Error activating audio session", audioSessionError);
            }
        }
        else
        {
            printError("init", "Failed to set audio session to playback", audioSessionError);
        }
    }
    else
    {
        printError("init", "Failed to get audio session for app");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioSessionIOS::close()
{
    AVAudioSession *aSession = [AVAudioSession sharedInstance];

    if(aSession != nil)
    {
        NSError *audioSessionError = nil;
        BOOL r = [aSession setActive:NO error:&audioSessionError];
        if(r != YES)
        {
            printError("init", "Error deactivating audio session", audioSessionError);
        }
    }
    else
    {
        printError("close", "Failed to get audio session for app");
    }
}

//-------------------------------------------------------------------------------------------

QString getPortTypeName(AVAudioSessionPort portType)
{
    QString name;
    if([portType isEqualToString:AVAudioSessionPortBuiltInMic])
    {
        name = "AVAudioSessionPortBuiltInMic = An input from a device’s built-in microphone.";
    }
    else if([portType isEqualToString:AVAudioSessionPortHeadsetMic])
    {
        name = "AVAudioSessionPortHeadsetMic = An input from a wired headset’s built-in microphone.";
    }
    else if([portType isEqualToString:AVAudioSessionPortLineIn])
    {
        name = "AVAudioSessionPortLineIn = A line-level input from the dock connector.";
    }
    else if([portType isEqualToString:AVAudioSessionPortAirPlay])
    {
        name = "AVAudioSessionPortAirPlay = An output to an AirPlay device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortBluetoothA2DP])
    {
        name = "AVAudioSessionPortBluetoothA2DP = An output to a Bluetooth A2DP device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortBluetoothLE])
    {
        name = "AVAudioSessionPortBluetoothLE = An output to a Bluetooth Low Energy (LE) device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortBuiltInReceiver])
    {
        name = "AVAudioSessionPortBuiltInReceiver = An output to the speaker you hold to your ear when you’re on a phone call.";
    }
    else if([portType isEqualToString:AVAudioSessionPortBuiltInSpeaker])
    {
        name = "AVAudioSessionPortBuiltInSpeaker = An output to the device’s built-in speaker.";
    }
    else if([portType isEqualToString:AVAudioSessionPortHDMI])
    {
        name = "AVAudioSessionPortHDMI = An output to a High-Definition Multimedia Interface (HDMI) device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortHeadphones])
    {
        name = "AVAudioSessionPortHeadphones = An output to wired headphones.";
    }
    else if([portType isEqualToString:AVAudioSessionPortLineOut])
    {
        name = "AVAudioSessionPortLineOut = A line-level output to the dock connector.";
    }
    else if([portType isEqualToString:AVAudioSessionPortAVB])
    {
        name = "AVAudioSessionPortAVB = An I/O connection to an Audio Video Bridging (AVB) device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortBluetoothHFP])
    {
        name = "AVAudioSessionPortBluetoothHFP = An I/O connection to a Bluetooth Hands-Free Profile device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortDisplayPort])
    {
        name = "AVAudioSessionPortDisplayPort = An I/O connection to a DisplayPort device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortCarAudio])
    {
        name = "AVAudioSessionPortCarAudio = An I/O connection through Car Audio.";
    }
    else if([portType isEqualToString:AVAudioSessionPortFireWire])
    {
        name = "AVAudioSessionPortFireWire = An I/O connection to a FireWire device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortPCI])
    {
        name = "AVAudioSessionPortPCI = An I/O connection to a Peripheral Component Interconnect (PCI) device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortThunderbolt])
    {
        name = "AVAudioSessionPortThunderbolt = An I/O connection to a Thunderbolt device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortUSBAudio])
    {
        name = "AVAudioSessionPortUSBAudio = An I/O connection to a Universal Serial Bus (USB) device.";
    }
    else if([portType isEqualToString:AVAudioSessionPortVirtual])
    {
        name = "AVAudioSessionPortVirtual = An I/O connection that doesn’t correspond to physical audio hardware.";
    }
    return name;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioSessionIOS::logOutput()
{
    AVAudioSession *aSession = [AVAudioSession sharedInstance];
    AVAudioSessionRouteDescription *currentRoute = aSession.currentRoute;
    NSArray<AVAudioSessionPortDescription *> *outputs = currentRoute.outputs;
    common::Log::g_Log.print("Get audio description of current route\n");
    for(NSUInteger i = 0; i < outputs.count; i++)
    {
        AVAudioSessionPortDescription *port = [outputs objectAtIndex:i];
        QString name = QString::fromUtf8(port.portName.UTF8String);
        common::Log::g_Log.print("Output idx=%d, name=%s\n", i, name.toUtf8().constData());
        common::Log::g_Log.print("%s\n", getPortTypeName(port.portType).toUtf8().constData());

        NSArray<AVAudioSessionChannelDescription *> *channels = port.channels;
        for(NSUInteger chs = 0; chs < channels.count; chs++)
        {
            AVAudioSessionChannelDescription *chDesc = [channels objectAtIndex:chs];
            common::Log::g_Log.print("Channel idx=%d, number=%d\n", chs, chDesc.channelNumber);
            common::Log::g_Log.print("ch name=%s\n", chDesc.channelName.UTF8String);
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
