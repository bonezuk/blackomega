//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_WASAPIVOLUMEEVENTS_H
#define __OMEGA_AUDIOIO_WASAPIVOLUMEEVENTS_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/WasAPICOMInterfaces.h"

//-------------------------------------------------------------------------------------------
// GUID to volume controls identifying the volume source change
//-------------------------------------------------------------------------------------------

DEFINE_GUIDSTRUCT("241a7a58-6c86-41f6-ba70-da2bbba7378a", GUID_OMEGA_VOLUME_EVENTS);
#define GUID_OMEGA_VOLUME_EVENTS DEFINE_GUIDNAMED(GUID_OMEGA_VOLUME_EVENTS)

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

typedef void(*VolumeChangeNotifier)(LPVOID, sample_t);

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT WasAPISharedVolumeEvents : public IAudioSessionEvents
{
    public:
        WasAPISharedVolumeEvents(VolumeChangeNotifier pNotifier, LPVOID pVInstance);
        virtual ~WasAPISharedVolumeEvents();
        
        // IUnknown methods.
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
        STDMETHODIMP QueryInterface(REFIID iid, VOID **ppvInterface);
        
        // IAudioSessionEvents methods
        STDMETHODIMP OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext);
        STDMETHODIMP OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext);
        STDMETHODIMP OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext);
        STDMETHODIMP OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext);
        STDMETHODIMP OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext);
        STDMETHODIMP OnStateChanged(AudioSessionState NewState);
        STDMETHODIMP OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason);

    private:
        LONG m_cRef;
        VolumeChangeNotifier m_pNotifier;
        LPVOID m_pVInstance;
        
        virtual void logEvent(const char *strR, const char *eventStr) const;
        virtual void logEvent(const char *strR, const QString& eventStr) const;
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT WasAPIExclusiveVolumeEvents : public IAudioEndpointVolumeCallback
{
    public:
        WasAPIExclusiveVolumeEvents(VolumeChangeNotifier pNotifier, LPVOID pVInstance);
        virtual ~WasAPIExclusiveVolumeEvents();

        // IUnknown methods.
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
        STDMETHODIMP QueryInterface(REFIID iid, VOID **ppvInterface);

        // IAudioEndpointVolumeCallback method
        STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
        
    private:
        LONG m_cRef;
        VolumeChangeNotifier m_pNotifier;
        LPVOID m_pVInstance;
        
        virtual void logEvent(const char *strR, const QString& eventStr) const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
