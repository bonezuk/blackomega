#include "audioio/inc/WasAPICOMInterfaces.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------
// IMMDeviceEnumeratorIF
//-------------------------------------------------------------------------------------------

IMMDeviceEnumeratorIF::IMMDeviceEnumeratorIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IMMDeviceEnumeratorIF::IMMDeviceEnumeratorIF(IMMDeviceEnumerator *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IMMDeviceEnumeratorIF::~IMMDeviceEnumeratorIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceEnumeratorIF::EnumAudioEndpoints(EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->EnumAudioEndpoints(dataFlow,dwStateMask,ppDevices);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceEnumeratorIF::GetDefaultAudioEndpoint(EDataFlow dataFlow,ERole role,IMMDevice **ppDevice)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetDefaultAudioEndpoint(dataFlow,role,ppDevice);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceEnumeratorIF::GetDevice(LPCWSTR pwstrId,IMMDevice **ppDevice)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetDevice(pwstrId,ppDevice);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceEnumeratorIF::RegisterEndpointNotificationCallback(IMMNotificationClient *pNotify)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->RegisterEndpointNotificationCallback(pNotify);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceEnumeratorIF::UnregisterEndpointNotificationCallback(IMMNotificationClient *pNotify)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->UnregisterEndpointNotificationCallback(pNotify);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IMMDeviceIF
//-------------------------------------------------------------------------------------------

IMMDeviceIF::IMMDeviceIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IMMDeviceIF::IMMDeviceIF(IMMDevice *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IMMDeviceIF::~IMMDeviceIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceIF::Activate(REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Activate(iid,dwClsCtx,pActivationParams,ppInterface);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceIF::GetId(LPWSTR *ppstrId)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetId(ppstrId);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceIF::GetState(DWORD *pdwState)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetState(pdwState);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceIF::OpenPropertyStore(DWORD stgmAccess,IPropertyStore **ppProperties)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->OpenPropertyStore(stgmAccess,ppProperties);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IAudioClientIF
//-------------------------------------------------------------------------------------------
    
IAudioClientIF::IAudioClientIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IAudioClientIF::IAudioClientIF(IAudioClient *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IAudioClientIF::~IAudioClientIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::Initialize(AUDCLNT_SHAREMODE shareMode,DWORD streamFlags,REFERENCE_TIME hnsBufferDuration,REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID audioSessionGuid)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Initialize(shareMode,streamFlags,hnsBufferDuration,hnsPeriodicity,pFormat,audioSessionGuid);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::GetBufferSize(UINT32 *pNumBufferFrames)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetBufferSize(pNumBufferFrames);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::GetStreamLatency(REFERENCE_TIME *phnsLatency)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetStreamLatency(phnsLatency);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::GetCurrentPadding(UINT32 *pNumPaddingFrames)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetCurrentPadding(pNumPaddingFrames);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::IsFormatSupported(AUDCLNT_SHAREMODE shareMode,const WAVEFORMATEX *pFormat,WAVEFORMATEX **ppClosestMatch)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->IsFormatSupported(shareMode,pFormat,ppClosestMatch);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::GetMixFormat(WAVEFORMATEX **ppDeviceFormat)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetMixFormat(ppDeviceFormat);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::GetDevicePeriod(REFERENCE_TIME *phnsDefaultDevicePeriod,REFERENCE_TIME *phnsMinimumDevicePeriod)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetDevicePeriod(phnsDefaultDevicePeriod,phnsMinimumDevicePeriod);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::Start()
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Start();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::Stop()
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Stop();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::Reset()
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Reset();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::SetEventHandle(HANDLE eventHandle)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetEventHandle(eventHandle);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClientIF::GetService(REFIID riid,void **ppv)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetService(riid,ppv);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IMMDeviceCollectionIF
//-------------------------------------------------------------------------------------------

IMMDeviceCollectionIF::IMMDeviceCollectionIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IMMDeviceCollectionIF::IMMDeviceCollectionIF(IMMDeviceCollection *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IMMDeviceCollectionIF::~IMMDeviceCollectionIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }    
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceCollectionIF::GetCount(UINT *pcDevices)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetCount(pcDevices);
    }
    return res;    
}

//-------------------------------------------------------------------------------------------

HRESULT IMMDeviceCollectionIF::Item(UINT nDevice,IMMDevice **ppDevice)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Item(nDevice,ppDevice);
    }
    return res;    
}

//-------------------------------------------------------------------------------------------
// IPropertyStoreIF
//-------------------------------------------------------------------------------------------

IPropertyStoreIF::IPropertyStoreIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IPropertyStoreIF::IPropertyStoreIF(IPropertyStore *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IPropertyStoreIF::~IPropertyStoreIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IPropertyStoreIF::Commit()
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Commit();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPropertyStoreIF::GetAt(DWORD iProp,PROPERTYKEY *pkey)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetAt(iProp,pkey);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPropertyStoreIF::GetCount(DWORD *cProps)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetCount(cProps);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPropertyStoreIF::GetValue(REFPROPERTYKEY key,PROPVARIANT *pv)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetValue(key,pv);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPropertyStoreIF::SetValue(REFPROPERTYKEY key,REFPROPVARIANT propvar)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetValue(key,propvar);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IDeviceTopologyIF
//-------------------------------------------------------------------------------------------

IDeviceTopologyIF::IDeviceTopologyIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IDeviceTopologyIF::IDeviceTopologyIF(IDeviceTopology *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IDeviceTopologyIF::~IDeviceTopologyIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IDeviceTopologyIF::GetConnector(UINT nIndex,IConnector **ppConnector)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetConnector(nIndex,ppConnector);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IDeviceTopologyIF::GetConnectorCount(UINT *pCount)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetConnectorCount(pCount);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IDeviceTopologyIF::GetDeviceId(LPWSTR *ppwstrDeviceId)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetDeviceId(ppwstrDeviceId);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IDeviceTopologyIF::GetPartById(UINT nId,IPart **ppPart)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetPartById(nId,ppPart);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IDeviceTopologyIF::GetSignalPath(IPart *pIPartFrom,IPart *pIPartTo,BOOL bRejectMixedPaths,IPartsList **ppParts)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetSignalPath(pIPartFrom,pIPartTo,bRejectMixedPaths,ppParts);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IDeviceTopologyIF::GetSubunit(UINT nIndex,ISubunit **ppSubunit)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetSubunit(nIndex,ppSubunit);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IDeviceTopologyIF::GetSubunitCount(UINT *pCount)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetSubunitCount(pCount);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IConnectorIF
//-------------------------------------------------------------------------------------------

IConnectorIF::IConnectorIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IConnectorIF::IConnectorIF(IConnector *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IConnectorIF::~IConnectorIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::ConnectTo(IConnector *pConnectTo)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->ConnectTo(pConnectTo);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::Disconnect()
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Disconnect();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::GetConnectorIdConnectedTo(LPWSTR *ppwstrConnectorId)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetConnectorIdConnectedTo(ppwstrConnectorId);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::GetConnectedTo(IConnector **ppConTo)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetConnectedTo(ppConTo);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::GetDataFlow(DataFlow *pFlow)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetDataFlow(pFlow);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::GetDeviceIdConnectedTo(LPWSTR *ppwstrDeviceId)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetDeviceIdConnectedTo(ppwstrDeviceId);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::GetType(ConnectorType *pType)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetType(pType);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::IsConnected(BOOL *pbConnected)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->IsConnected(pbConnected);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IConnectorIF::QueryInterface(REFIID riid,void **ppvObject)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->QueryInterface(riid,ppvObject);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IPartIF
//-------------------------------------------------------------------------------------------

IPartIF::IPartIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IPartIF::IPartIF(IPart *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IPartIF::~IPartIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::Activate(DWORD dwClsContext,REFIID refiid,void **ppvObject)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->Activate(dwClsContext,refiid,ppvObject);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::EnumPartsIncoming(IPartsList **ppParts)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->EnumPartsIncoming(ppParts);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::EnumPartsOutgoing(IPartsList **ppParts)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->EnumPartsOutgoing(ppParts);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetControlInterface(UINT nIndex,IControlInterface **ppFunction)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetControlInterface(nIndex,ppFunction);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetControlInterfaceCount(UINT *pCount)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetControlInterfaceCount(pCount);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetGlobalId(LPWSTR *ppwstrGlobalId)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetGlobalId(ppwstrGlobalId);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetLocalId(UINT *pnId)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetLocalId(pnId);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetName(LPWSTR *ppwstrName)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetName(ppwstrName);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetPartType(PartType *pPartType)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetPartType(pPartType);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetSubType(GUID *pSubType)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetSubType(pSubType);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::GetTopologyObject(IDeviceTopology **ppTopology)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetTopologyObject(ppTopology);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::RegisterControlChangeCallback(REFGUID riid,IControlChangeNotify *pNotify)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->RegisterControlChangeCallback(riid,pNotify);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::UnregisterControlChangeCallback(IControlChangeNotify *pNotify)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->UnregisterControlChangeCallback(pNotify);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartIF::QueryInterface(REFIID riid,void **ppvObject)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->QueryInterface(riid,ppvObject);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IPartsListIF
//-------------------------------------------------------------------------------------------

IPartsListIF::IPartsListIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IPartsListIF::IPartsListIF(IPartsList *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IPartsListIF::~IPartsListIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IPartsListIF::GetCount(UINT *pCount)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetCount(pCount);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IPartsListIF::GetPart(UINT nIndex,IPart **ppPart)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetPart(nIndex,ppPart);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IAudioChannelConfigIF
//-------------------------------------------------------------------------------------------

IAudioChannelConfigIF::IAudioChannelConfigIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IAudioChannelConfigIF::IAudioChannelConfigIF(IAudioChannelConfig *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IAudioChannelConfigIF::~IAudioChannelConfigIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioChannelConfigIF::GetChannelConfig(DWORD *pdwConfig)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetChannelConfig(pdwConfig);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioChannelConfigIF::SetChannelConfig(DWORD dwConfig,LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetChannelConfig(dwConfig,pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IAudioRenderClientIF
//-------------------------------------------------------------------------------------------

IAudioRenderClientIF::IAudioRenderClientIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IAudioRenderClientIF::IAudioRenderClientIF(IAudioRenderClient *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IAudioRenderClientIF::~IAudioRenderClientIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioRenderClientIF::GetBuffer(UINT32 NumFramesRequested,BYTE **ppData)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetBuffer(NumFramesRequested,ppData);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioRenderClientIF::ReleaseBuffer(UINT32 NumFramesRequested,DWORD dwFlags)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->ReleaseBuffer(NumFramesRequested,dwFlags);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IAudioClockIF
//-------------------------------------------------------------------------------------------

IAudioClockIF::IAudioClockIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IAudioClockIF::IAudioClockIF(IAudioClock *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IAudioClockIF::~IAudioClockIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClockIF::GetCharacteristics(DWORD *pdwCharacteristics)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetCharacteristics(pdwCharacteristics);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClockIF::GetFrequency(UINT64 *pu64Frequency)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetFrequency(pu64Frequency);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioClockIF::GetPosition(UINT64 *pu64Position,UINT64 *pu64QPCPosition)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetPosition(pu64Position,pu64QPCPosition);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IAudioEndpointVolumeIF
//-------------------------------------------------------------------------------------------

IAudioEndpointVolumeIF::IAudioEndpointVolumeIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IAudioEndpointVolumeIF::IAudioEndpointVolumeIF(IAudioEndpointVolume *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IAudioEndpointVolumeIF::~IAudioEndpointVolumeIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetChannelCount(UINT *pnChannelCount)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetChannelCount(pnChannelCount);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetChannelVolumeLevel(UINT nChannel, float *pfLevelDB)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetChannelVolumeLevel(nChannel, pfLevelDB);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetChannelVolumeLevelScalar(UINT nChannel, float *pfLevel)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetChannelVolumeLevelScalar(nChannel, pfLevel);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetMasterVolumeLevel(float *pfLevelDB)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetMasterVolumeLevel(pfLevelDB);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetMasterVolumeLevelScalar(float *pfLevel)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetMasterVolumeLevelScalar(pfLevel);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetMute(BOOL *pbMute)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetMute(pbMute);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetVolumeRange(float *pflVolumeMindB, float *pflVolumeMaxdB, float *pflVolumeIncrementdB)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetVolumeRange(pflVolumeMindB, pflVolumeMaxdB, pflVolumeIncrementdB);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::GetVolumeStepInfo(UINT *pnStep, UINT *pnStepCount)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetVolumeStepInfo(pnStep, pnStepCount);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::QueryHardwareSupport(DWORD *pdwHardwareSupportMask)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->QueryHardwareSupport(pdwHardwareSupportMask);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::RegisterControlChangeNotify(IAudioEndpointVolumeCallback *pNotify)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->RegisterControlChangeNotify(pNotify);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::SetChannelVolumeLevel(UINT nChannel, float fLevelDB, LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetChannelVolumeLevel(nChannel, fLevelDB, pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::SetChannelVolumeLevelScalar(UINT nChannel, float fLevel, LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetChannelVolumeLevelScalar(nChannel, fLevel, pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::SetMasterVolumeLevel(float fLevelDB, LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetMasterVolumeLevel(fLevelDB, pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::SetMasterVolumeLevelScalar(float fLevel, LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetMasterVolumeLevelScalar(fLevel, pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::SetMute(BOOL bMute, LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetMute(bMute, pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::UnregisterControlChangeNotify(IAudioEndpointVolumeCallback *pNotify)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->UnregisterControlChangeNotify(pNotify);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::VolumeStepDown(LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->VolumeStepDown(pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioEndpointVolumeIF::VolumeStepUp(LPCGUID pguidEventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface != 0)
    {
        res = m_pInterface->VolumeStepUp(pguidEventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// ISimpleAudioVolumeIF
//-------------------------------------------------------------------------------------------

ISimpleAudioVolumeIF::ISimpleAudioVolumeIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

ISimpleAudioVolumeIF::ISimpleAudioVolumeIF(ISimpleAudioVolume *pInterface) : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

ISimpleAudioVolumeIF::~ISimpleAudioVolumeIF()
{
    if(m_pInterface!=0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT ISimpleAudioVolumeIF::GetMasterVolume(float *pfLevel)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetMasterVolume(pfLevel);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT ISimpleAudioVolumeIF::GetMute(BOOL *pbMute)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetMute(pbMute);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT ISimpleAudioVolumeIF::SetMasterVolume(float fLevel, LPCGUID EventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetMasterVolume(fLevel, EventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT ISimpleAudioVolumeIF::SetMute(const BOOL bMute, LPCGUID EventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetMute(bMute, EventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// IAudioSessionControlIF
//-------------------------------------------------------------------------------------------

IAudioSessionControlIF::IAudioSessionControlIF() : m_pInterface(0)
{}

//-------------------------------------------------------------------------------------------

IAudioSessionControlIF::IAudioSessionControlIF(IAudioSessionControl *pInterface)  : m_pInterface(pInterface)
{}

//-------------------------------------------------------------------------------------------

IAudioSessionControlIF::~IAudioSessionControlIF()
{
    if(m_pInterface != 0)
    {
        m_pInterface->Release();
        m_pInterface = 0;
    }
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::GetDisplayName(LPWSTR *pRetVal)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetDisplayName(pRetVal);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::GetGroupingParam(GUID *pRetVal)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetGroupingParam(pRetVal);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::GetIconPath(LPWSTR *pRetVal)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetIconPath(pRetVal);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::GetState(AudioSessionState *pRetVal)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->GetState(pRetVal);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::RegisterAudioSessionNotification(IAudioSessionEvents *NewNotifications)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->RegisterAudioSessionNotification(NewNotifications);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::SetDisplayName(LPCWSTR Value, LPCGUID EventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetDisplayName(Value, EventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::SetGroupingParam(LPCGUID Override, LPCGUID EventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetGroupingParam(Override, EventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::SetIconPath(LPCWSTR Value, LPCGUID EventContext)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->SetIconPath(Value, EventContext);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

HRESULT IAudioSessionControlIF::UnregisterAudioSessionNotification(IAudioSessionEvents *NewNotifications)
{
    HRESULT res = S_FALSE;
    if(m_pInterface!=0)
    {
        res = m_pInterface->UnregisterAudioSessionNotification(NewNotifications);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
