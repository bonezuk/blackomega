//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_WASAPICOMINTERFACES_H
#define __OMEGA_AUDIOIO_WASAPICOMINTERFACES_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AudioIODLL.h"

#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <FunctionDiscoveryKeys_devpkey.h>
#include <Devicetopology.h>
#include <KsMedia.h>
#include <MMReg.h>
#include <guiddef.h>
#include <endpointvolume.h>
#include <audiopolicy.h>

#include <QSharedPointer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IMMDeviceEnumeratorIF
{
    public:
        IMMDeviceEnumeratorIF();
        IMMDeviceEnumeratorIF(IMMDeviceEnumerator *pInterface);
        virtual ~IMMDeviceEnumeratorIF();

        virtual HRESULT EnumAudioEndpoints(EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices);
        virtual HRESULT GetDefaultAudioEndpoint(EDataFlow dataFlow,ERole role,IMMDevice **ppDevice);
        virtual HRESULT GetDevice(LPCWSTR pwstrId,IMMDevice **ppDevice);
        virtual HRESULT RegisterEndpointNotificationCallback(IMMNotificationClient *pNotify);
        virtual HRESULT UnregisterEndpointNotificationCallback(IMMNotificationClient *pNotify);

    protected:

        IMMDeviceEnumerator *m_pInterface;
};

typedef QSharedPointer<IMMDeviceEnumeratorIF> IMMDeviceEnumeratorIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IMMDeviceCollectionIF
{
    public:
        IMMDeviceCollectionIF();
        IMMDeviceCollectionIF(IMMDeviceCollection *pInterface);
        virtual ~IMMDeviceCollectionIF();

        virtual HRESULT GetCount(UINT *pcDevices);
        virtual HRESULT Item(UINT nDevice,IMMDevice **ppDevice);

    protected:
        IMMDeviceCollection *m_pInterface;
};

typedef QSharedPointer<IMMDeviceCollectionIF> IMMDeviceCollectionIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IMMDeviceIF
{
    public:
        IMMDeviceIF();
        IMMDeviceIF(IMMDevice *pInterface);
        virtual ~IMMDeviceIF();

        virtual HRESULT Activate(REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface);
        virtual HRESULT GetId(LPWSTR *ppstrId);
        virtual HRESULT GetState(DWORD *pdwState);
        virtual HRESULT OpenPropertyStore(DWORD stgmAccess,IPropertyStore **ppProperties);

    protected:

        IMMDevice *m_pInterface;
};

typedef QSharedPointer<IMMDeviceIF> IMMDeviceIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IAudioClientIF
{
    public:
        IAudioClientIF();
        IAudioClientIF(IAudioClient *pInterface);
        virtual ~IAudioClientIF();

        virtual HRESULT Initialize(AUDCLNT_SHAREMODE shareMode,DWORD streamFlags,REFERENCE_TIME hnsBufferDuration,REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID audioSessionGuid);

        virtual HRESULT GetBufferSize(UINT32 *pNumBufferFrames);
        virtual HRESULT GetStreamLatency(REFERENCE_TIME *phnsLatency);
        virtual HRESULT GetCurrentPadding(UINT32 *pNumPaddingFrames);

        virtual HRESULT IsFormatSupported(AUDCLNT_SHAREMODE shareMode,const WAVEFORMATEX *pFormat,WAVEFORMATEX **ppClosestMatch);
        virtual HRESULT GetMixFormat(WAVEFORMATEX **ppDeviceFormat);

        virtual HRESULT GetDevicePeriod(REFERENCE_TIME *phnsDefaultDevicePeriod,REFERENCE_TIME *phnsMinimumDevicePeriod);

        virtual HRESULT Start();
        virtual HRESULT Stop();
        virtual HRESULT Reset();

        virtual HRESULT SetEventHandle(HANDLE eventHandle);

        virtual HRESULT GetService(REFIID riid,void **ppv);

    protected:

        IAudioClient *m_pInterface;
};

typedef QSharedPointer<IAudioClientIF> IAudioClientIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IPropertyStoreIF
{
    public:
        IPropertyStoreIF();
        IPropertyStoreIF(IPropertyStore *pInterface);
        virtual ~IPropertyStoreIF();

        virtual HRESULT Commit();
        virtual HRESULT GetAt(DWORD iProp,PROPERTYKEY *pkey);
        virtual HRESULT GetCount(DWORD *cProps);
        virtual HRESULT GetValue(REFPROPERTYKEY key,PROPVARIANT *pv);
        virtual HRESULT SetValue(REFPROPERTYKEY key,REFPROPVARIANT propvar);

    protected:

        IPropertyStore *m_pInterface;
};

typedef QSharedPointer<IPropertyStoreIF> IPropertyStoreIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IDeviceTopologyIF
{
    public:
        IDeviceTopologyIF();
        IDeviceTopologyIF(IDeviceTopology *pInterface);
        virtual ~IDeviceTopologyIF();

        virtual HRESULT GetConnector(UINT nIndex,IConnector **ppConnector);
        virtual HRESULT GetConnectorCount(UINT *pCount);
        virtual HRESULT GetDeviceId(LPWSTR *ppwstrDeviceId);
        virtual HRESULT GetPartById(UINT nId,IPart **ppPart);
        virtual HRESULT GetSignalPath(IPart *pIPartFrom,IPart *pIPartTo,BOOL bRejectMixedPaths,IPartsList **ppParts);
        virtual HRESULT GetSubunit(UINT nIndex,ISubunit **ppSubunit);
        virtual HRESULT GetSubunitCount(UINT *pCount);

    protected:

        IDeviceTopology *m_pInterface;
};

typedef QSharedPointer<IDeviceTopologyIF> IDeviceTopologyIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IConnectorIF
{
    public:
        IConnectorIF();
        IConnectorIF(IConnector *pInterface);
        virtual ~IConnectorIF();

        virtual HRESULT ConnectTo(IConnector *pConnectTo);
        virtual HRESULT Disconnect();
        virtual HRESULT GetConnectorIdConnectedTo(LPWSTR *ppwstrConnectorId);
        virtual HRESULT GetConnectedTo(IConnector **ppConTo);
        virtual HRESULT GetDataFlow(DataFlow *pFlow);
        virtual HRESULT GetDeviceIdConnectedTo(LPWSTR *ppwstrDeviceId);
        virtual HRESULT GetType(ConnectorType *pType);
        virtual HRESULT IsConnected(BOOL *pbConnected);
        virtual HRESULT QueryInterface(REFIID riid,void **ppvObject);

    protected:

        IConnector *m_pInterface;
};

typedef QSharedPointer<IConnectorIF> IConnectorIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IPartIF
{
    public:
        IPartIF();
        IPartIF(IPart *pInterface);
        virtual ~IPartIF();

        virtual HRESULT Activate(DWORD dwClsContext,REFIID refiid,void **ppvObject);
        virtual HRESULT EnumPartsIncoming(IPartsList **ppParts);
        virtual HRESULT EnumPartsOutgoing(IPartsList **ppParts);
        virtual HRESULT GetControlInterface(UINT nIndex,IControlInterface **ppFunction);
        virtual HRESULT GetControlInterfaceCount(UINT *pCount);
        virtual HRESULT GetGlobalId(LPWSTR *ppwstrGlobalId);
        virtual HRESULT GetLocalId(UINT *pnId);
        virtual HRESULT GetName(LPWSTR *ppwstrName);
        virtual HRESULT GetPartType(PartType *pPartType);
        virtual HRESULT GetSubType(GUID *pSubType);
        virtual HRESULT GetTopologyObject(IDeviceTopology **ppTopology);
        virtual HRESULT RegisterControlChangeCallback(REFGUID riid,IControlChangeNotify *pNotify);
        virtual HRESULT UnregisterControlChangeCallback(IControlChangeNotify *pNotify);
        virtual HRESULT QueryInterface(REFIID riid,void **ppvObject);

    protected:

        IPart *m_pInterface;
};

typedef QSharedPointer<IPartIF> IPartIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IPartsListIF
{
    public:
        IPartsListIF();
        IPartsListIF(IPartsList *pInterface);
        virtual ~IPartsListIF();

        virtual HRESULT GetCount(UINT *pCount);
        virtual HRESULT GetPart(UINT nIndex,IPart **ppPart);

    protected:

        IPartsList *m_pInterface;
};

typedef QSharedPointer<IPartsListIF> IPartsListIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IAudioChannelConfigIF
{
    public:
        IAudioChannelConfigIF();
        IAudioChannelConfigIF(IAudioChannelConfig *pInterface);
        virtual ~IAudioChannelConfigIF();

        virtual HRESULT GetChannelConfig(DWORD *pdwConfig);
        virtual HRESULT SetChannelConfig(DWORD dwConfig,LPCGUID pguidEventContext);

    protected:

        IAudioChannelConfig *m_pInterface;
};

typedef QSharedPointer<IAudioChannelConfigIF> IAudioChannelConfigIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IAudioRenderClientIF
{
    public:
        IAudioRenderClientIF();
        IAudioRenderClientIF(IAudioRenderClient *pInterface);
        virtual ~IAudioRenderClientIF();

        HRESULT GetBuffer(UINT32 NumFramesRequested,BYTE **ppData);
        HRESULT ReleaseBuffer(UINT32 NumFramesRequested,DWORD dwFlags);

    protected:

        IAudioRenderClient *m_pInterface;
};

typedef QSharedPointer<IAudioRenderClientIF> IAudioRenderClientIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IAudioClockIF
{
    public:
        IAudioClockIF();
        IAudioClockIF(IAudioClock *pInterface);
        virtual ~IAudioClockIF();

        HRESULT GetCharacteristics(DWORD *pdwCharacteristics);
        HRESULT GetFrequency(UINT64 *pu64Frequency);
        HRESULT GetPosition(UINT64 *pu64Position,UINT64 *pu64QPCPosition);

    protected:

        IAudioClock *m_pInterface;
};

typedef QSharedPointer<IAudioClockIF> IAudioClockIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IAudioEndpointVolumeIF
{
    public:
        IAudioEndpointVolumeIF();
        IAudioEndpointVolumeIF(IAudioEndpointVolume *pInterface);
        virtual ~IAudioEndpointVolumeIF();

        HRESULT GetChannelCount(UINT *pnChannelCount);
        HRESULT GetChannelVolumeLevel(UINT nChannel, float *pfLevelDB);
        HRESULT GetChannelVolumeLevelScalar(UINT nChannel, float *pfLevel);
        HRESULT GetMasterVolumeLevel(float *pfLevelDB);
        HRESULT GetMasterVolumeLevelScalar(float *pfLevel);
        HRESULT GetMute(BOOL *pbMute);
        HRESULT GetVolumeRange(float *pflVolumeMindB, float *pflVolumeMaxdB, float *pflVolumeIncrementdB);
        HRESULT GetVolumeStepInfo(UINT *pnStep, UINT *pnStepCount);
        HRESULT QueryHardwareSupport(DWORD *pdwHardwareSupportMask);
        HRESULT RegisterControlChangeNotify(IAudioEndpointVolumeCallback *pNotify);
        HRESULT SetChannelVolumeLevel(UINT nChannel, float fLevelDB, LPCGUID pguidEventContext);
        HRESULT SetChannelVolumeLevelScalar(UINT nChannel, float fLevel, LPCGUID pguidEventContext);
        HRESULT SetMasterVolumeLevel(float fLevelDB, LPCGUID pguidEventContext);
        HRESULT SetMasterVolumeLevelScalar(float fLevel, LPCGUID pguidEventContext);
        HRESULT SetMute(BOOL bMute, LPCGUID pguidEventContext);
        HRESULT UnregisterControlChangeNotify(IAudioEndpointVolumeCallback *pNotify);
        HRESULT VolumeStepDown(LPCGUID pguidEventContext);
        HRESULT VolumeStepUp(LPCGUID pguidEventContext);
    protected:

        IAudioEndpointVolume *m_pInterface;
};

typedef QSharedPointer<IAudioEndpointVolumeIF> IAudioEndpointVolumeIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT ISimpleAudioVolumeIF
{
    public:
        ISimpleAudioVolumeIF();
        ISimpleAudioVolumeIF(ISimpleAudioVolume *pInterface);
        virtual ~ISimpleAudioVolumeIF();

        HRESULT GetMasterVolume(float *pfLevel);
        HRESULT GetMute(BOOL *pbMute);
        HRESULT SetMasterVolume(float fLevel, LPCGUID EventContext);
        HRESULT SetMute(const BOOL bMute, LPCGUID EventContext);

    protected:

        ISimpleAudioVolume *m_pInterface;
};

typedef QSharedPointer<ISimpleAudioVolumeIF> ISimpleAudioVolumeIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT IAudioSessionControlIF
{
    public:
        IAudioSessionControlIF();
        IAudioSessionControlIF(IAudioSessionControl *pInterface);
        virtual ~IAudioSessionControlIF();

        HRESULT GetDisplayName(LPWSTR *pRetVal);
        HRESULT GetGroupingParam(GUID *pRetVal);
        HRESULT GetIconPath(LPWSTR *pRetVal);
        HRESULT GetState(AudioSessionState *pRetVal);
        HRESULT RegisterAudioSessionNotification(IAudioSessionEvents *NewNotifications);
        HRESULT SetDisplayName(LPCWSTR Value, LPCGUID EventContext);
        HRESULT SetGroupingParam(LPCGUID Override, LPCGUID EventContext);
        HRESULT SetIconPath(LPCWSTR Value, LPCGUID EventContext);
        HRESULT UnregisterAudioSessionNotification(IAudioSessionEvents *NewNotifications);

    protected:

        IAudioSessionControl* m_pInterface;
};

typedef QSharedPointer<IAudioSessionControlIF> IAudioSessionControlIFSPtr;

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
