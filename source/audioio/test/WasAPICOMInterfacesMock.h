//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_WASAPICOMINTERFACESMOCK_H
#define __OMEGA_AUDIOIO_WASAPICOMINTERFACESMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/WasAPICOMInterfaces.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class IMMDeviceEnumeratorIFMock : public IMMDeviceEnumeratorIF
{
    public:
        IMMDeviceEnumeratorIFMock();
        virtual ~IMMDeviceEnumeratorIFMock();

        MOCK_METHOD3(EnumAudioEndpoints,HRESULT(EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices));
        MOCK_METHOD3(GetDefaultAudioEndpoint,HRESULT(EDataFlow dataFlow,ERole role,IMMDevice **ppDevice));
        MOCK_METHOD2(GetDevice,HRESULT(LPCWSTR pwstrId,IMMDevice **ppDevice));
        MOCK_METHOD1(RegisterEndpointNotificationCallback,HRESULT(IMMNotificationClient *pNotify));
        MOCK_METHOD1(UnregisterEndpointNotificationCallback,HRESULT(IMMNotificationClient *pNotify));
};

//-------------------------------------------------------------------------------------------

class IMMDeviceCollectionIFMock : public IMMDeviceCollectionIF
{
    public:
        IMMDeviceCollectionIFMock();
        virtual ~IMMDeviceCollectionIFMock();

        MOCK_METHOD1(GetCount,HRESULT(UINT *pcDevices));
        MOCK_METHOD2(Item,HRESULT(UINT nDevice,IMMDevice **ppDevice));
};

//-------------------------------------------------------------------------------------------

class IMMDeviceIFMock : public IMMDeviceIF
{
    public:
        IMMDeviceIFMock();
        virtual ~IMMDeviceIFMock();

        MOCK_METHOD4(Activate,HRESULT(REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface));
        MOCK_METHOD1(GetId,HRESULT(LPWSTR *ppstrId));
        MOCK_METHOD1(GetState,HRESULT(DWORD *pdwState));
        MOCK_METHOD2(OpenPropertyStore,HRESULT(DWORD stgmAccess,IPropertyStore **ppProperties));
};

//-------------------------------------------------------------------------------------------

class IAudioClientIFMock : public IAudioClientIF
{
    public:
        IAudioClientIFMock();
        virtual ~IAudioClientIFMock();

        MOCK_METHOD6(Initialize,HRESULT(AUDCLNT_SHAREMODE shareMode,DWORD streamFlags,REFERENCE_TIME hnsBufferDuration,REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID audioSessionGuid));

        MOCK_METHOD1(GetBufferSize,HRESULT(UINT32 *pNumBufferFrames));
        MOCK_METHOD1(GetStreamLatency,HRESULT(REFERENCE_TIME *phnsLatency));
        MOCK_METHOD1(GetCurrentPadding,HRESULT(UINT32 *pNumPaddingFrames));

        MOCK_METHOD3(IsFormatSupported,HRESULT(AUDCLNT_SHAREMODE shareMode,const WAVEFORMATEX *pFormat,WAVEFORMATEX **ppClosestMatch));
        MOCK_METHOD1(GetMixFormat,HRESULT(WAVEFORMATEX **ppDeviceFormat));

        MOCK_METHOD2(GetDevicePeriod,HRESULT(REFERENCE_TIME *phnsDefaultDevicePeriod,REFERENCE_TIME *phnsMinimumDevicePeriod));

        MOCK_METHOD0(Start,HRESULT());
        MOCK_METHOD0(Stop,HRESULT());
        MOCK_METHOD0(Reset,HRESULT());

        MOCK_METHOD1(SetEventHandle,HRESULT(HANDLE eventHandle));

        MOCK_METHOD2(GetService,HRESULT(REFIID riid,void **ppv));
};

//-------------------------------------------------------------------------------------------

class IPropertyStoreIFMock : public IPropertyStoreIF
{
    public:
        IPropertyStoreIFMock();
        virtual ~IPropertyStoreIFMock();

        MOCK_METHOD0(Commit,HRESULT());
        MOCK_METHOD2(GetAt,HRESULT(DWORD iProp,PROPERTYKEY *pkey));
        MOCK_METHOD1(GetCount,HRESULT(DWORD *cProps));
        MOCK_METHOD2(GetValue,HRESULT(REFPROPERTYKEY key,PROPVARIANT *pv));
        MOCK_METHOD2(SetValue,HRESULT(REFPROPERTYKEY key,REFPROPVARIANT propvar));
};

//-------------------------------------------------------------------------------------------

class IDeviceTopologyIFMock : public IDeviceTopologyIF
{
    public:
        IDeviceTopologyIFMock();
        virtual ~IDeviceTopologyIFMock();

        MOCK_METHOD2(GetConnector,HRESULT(UINT nIndex,IConnector **ppConnector));
        MOCK_METHOD1(GetConnectorCount,HRESULT(UINT *pCount));
        MOCK_METHOD1(GetDeviceId,HRESULT(LPWSTR *ppwstrDeviceId));
        MOCK_METHOD2(GetPartById,HRESULT(UINT nId,IPart **ppPart));
        MOCK_METHOD4(GetSignalPath,HRESULT(IPart *pIPartFrom,IPart *pIPartTo,BOOL bRejectMixedPaths,IPartsList **ppParts));
        MOCK_METHOD2(GetSubunit,HRESULT(UINT nIndex,ISubunit **ppSubunit));
        MOCK_METHOD1(GetSubunitCount,HRESULT(UINT *pCount));
};

//-------------------------------------------------------------------------------------------

class IConnectorIFMock : public IConnectorIF
{
    public:
        IConnectorIFMock();
        virtual ~IConnectorIFMock();

        MOCK_METHOD1(ConnectTo,HRESULT(IConnector *pConnectTo));
        MOCK_METHOD0(Disconnect,HRESULT());
        MOCK_METHOD1(GetConnectorIdConnectedTo,HRESULT(LPWSTR *ppwstrConnectorId));
        MOCK_METHOD1(GetConnectedTo,HRESULT(IConnector **ppConTo));
        MOCK_METHOD1(GetDataFlow,HRESULT(DataFlow *pFlow));
        MOCK_METHOD1(GetDeviceIdConnectedTo,HRESULT(LPWSTR *ppwstrDeviceId));
        MOCK_METHOD1(GetType,HRESULT(ConnectorType *pType));
        MOCK_METHOD1(IsConnected,HRESULT(BOOL *pbConnected));
        MOCK_METHOD2(QueryInterface,HRESULT(REFIID riid,void **ppvObject));
};

//-------------------------------------------------------------------------------------------

class IPartIFMock : public IPartIF
{
    public:
        IPartIFMock();
        virtual ~IPartIFMock();

        MOCK_METHOD3(Activate,HRESULT(DWORD dwClsContext,REFIID refiid,void **ppvObject));
        MOCK_METHOD1(EnumPartsIncoming,HRESULT(IPartsList **ppParts));
        MOCK_METHOD1(EnumPartsOutgoing,HRESULT(IPartsList **ppParts));
        MOCK_METHOD2(GetControlInterface,HRESULT(UINT nIndex,IControlInterface **ppFunction));
        MOCK_METHOD1(GetControlInterfaceCount,HRESULT(UINT *pCount));
        MOCK_METHOD1(GetGlobalId,HRESULT(LPWSTR *ppwstrGlobalId));
        MOCK_METHOD1(GetLocalId,HRESULT(UINT *pnId));
        MOCK_METHOD1(GetName,HRESULT(LPWSTR *ppwstrName));
        MOCK_METHOD1(GetPartType,HRESULT(PartType *pPartType));
        MOCK_METHOD1(GetSubType,HRESULT(GUID *pSubType));
        MOCK_METHOD1(GetTopologyObject,HRESULT(IDeviceTopology **ppTopology));
        MOCK_METHOD2(RegisterControlChangeCallback,HRESULT(REFGUID riid,IControlChangeNotify *pNotify));
        MOCK_METHOD1(UnregisterControlChangeCallback,HRESULT(IControlChangeNotify *pNotify));
        MOCK_METHOD2(QueryInterface,HRESULT(REFIID riid,void **ppvObject));
};

//-------------------------------------------------------------------------------------------

class IPartsListIFMock : public IPartsListIF
{
    public:
        IPartsListIFMock();
        virtual ~IPartsListIFMock();

        MOCK_METHOD1(GetCount,HRESULT(UINT *pCount));
        MOCK_METHOD2(GetPart,HRESULT(UINT nIndex,IPart **ppPart));
};

//-------------------------------------------------------------------------------------------

class IAudioChannelConfigIFMock : public IAudioChannelConfigIF
{
    public:
        IAudioChannelConfigIFMock();
        virtual ~IAudioChannelConfigIFMock();

        MOCK_METHOD1(GetChannelConfig,HRESULT(DWORD *pdwConfig));
        MOCK_METHOD2(SetChannelConfig,HRESULT(DWORD dwConfig,LPCGUID pguidEventContext));
};

//-------------------------------------------------------------------------------------------

class IAudioRenderClientIFMock : public IAudioRenderClientIF
{
    public:
        IAudioRenderClientIFMock();
        virtual ~IAudioRenderClientIFMock();

        MOCK_METHOD2(GetBuffer,HRESULT(UINT32 NumFramesRequested,BYTE **ppData));
        MOCK_METHOD2(ReleaseBuffer,HRESULT(UINT32 NumFramesRequested,DWORD dwFlags));
};

//-------------------------------------------------------------------------------------------

class IAudioClockIFMock : public IAudioClockIF
{
    public:
        IAudioClockIFMock();
        virtual ~IAudioClockIFMock();

        MOCK_METHOD1(GetCharacteristics,HRESULT(DWORD *pdwCharacteristics));
        MOCK_METHOD1(GetFrequency,HRESULT(UINT64 *pu64Frequency));
        MOCK_METHOD2(GetPosition,HRESULT(UINT64 *pu64Position,UINT64 *pu64QPCPosition));
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
