#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/WasAPIIF.h"
#include "audioio/inc/AOQueryWasAPI.h"
#include "audioio/inc/WasAPICOMInterfaces.h"
#include "audioio/inc/WasAPILayerIF.h"

#include <QSettings>

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(Was,blankFormats)
{
    QSettings settings;
    settings.beginGroup("wasapi");
    
    WasAPIIFSPtr pWAS = WasAPIIF::instance("wasapi");
    
    ASSERT_FALSE(pWAS.isNull());

    AOQueryWasAPI audioQuery;
    
    ASSERT_TRUE(audioQuery.queryNames());
    
    for(int i=0;i<audioQuery.noDevices();i++)
    {
        QString key = "Formats_" + audioQuery.device(i).idConst() + "_E";
        settings.remove(key);
        key = "Formats_" + audioQuery.device(i).idConst() + "_M";
        settings.remove(key);
    }
    
    WasAPIIF::release();
    
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

QString deviceTopologyConnectorType(IConnector *pConnector)
{
    HRESULT hr;
    QString type;
    ConnectorType cType;
    
    hr = pConnector->GetType(&cType);
    if(hr==S_OK)
    {
        switch(cType)
        {
            case Physical_Internal:
                type = "Phyical Internal";
                break;
                
            case Physical_External:
                type = "Physical External";
                break;
            
            case Software_IO:
                type = "Software IO";
                break;
                
            case Software_Fixed:
                type = "Software Fixed";
                break;
                
            default:
                type = "Unknown";
                break;
        }
    }
    else
    {
        type = "Unknown";
    }
    return type;
}

void deviceTopologyPrintConnectorType(IConnector *pConnector)
{
    common::Log::g_Log.print("Connector Type - %s\n",deviceTopologyConnectorType(pConnector).toUtf8().constData());
}

QString deviceTopologyPartSubType(IPart *pPart)
{
    HRESULT hr;
    PartType partType;
    GUID subType;
    QString type = "Unknown";
    
    hr = pPart->GetPartType(&partType);
    if(hr==S_OK)
    {
        if(partType==Connector)
        {
            type = "Connector";
        }
        else
        {
            type = "Subunit";
        }
    }
    
    hr = pPart->GetSubType(&subType);
    if(hr==S_OK)
    {
        type += " ";
        if(subType==KSNODETYPE_3D_EFFECTS)
        {
            type += "KSNODETYPE_3D_EFFECTS";
        }
        else if(subType==KSNODETYPE_AGC)
        {
            type += "KSNODETYPE_AGC";
        }
        else if(subType==KSNODETYPE_DAC)
        {
            type += "KSNODETYPE_DAC";
        }
        else if(subType==KSNODETYPE_DEMUX)
        {
            type += "KSNODETYPE_DEMUX";
        }
        else if(subType==KSNODETYPE_DEV_SPECIFIC)
        {
            type += "KSNODETYPE_DEV_SPECIFIC";
        }
        else if(subType==KSNODETYPE_LOUDNESS)
        {
            type += "KSNODETYPE_LOUDNESS";
        }
        else if(subType==KSNODETYPE_MUTE)
        {
            type += "KSNODETYPE_MUTE";
        }
        else if(subType==KSNODETYPE_MUX)
        {
            type += "KSNODETYPE_MUX";
        }
        else if(subType==KSNODETYPE_PEAKMETER)
        {
            type += "KSNODETYPE_PEAKMETER";
        }
        else if(subType==KSNODETYPE_PROLOGIC_DECODER)
        {
            type += "KSNODETYPE_PROLOGIC_DECODER";
        }
        else if(subType==KSNODETYPE_TONE)
        {
            type += "KSNODETYPE_TONE";
        }
        else if(subType==KSNODETYPE_VOLUME)
        {
            type += "KSNODETYPE_VOLUME";
        }
    }
    return type;
}

void deviceTopologyParts(IPartsList *pParts)
{
    UINT count = 0;
    HRESULT hr;
    
    common::Log::g_Log.print("Scan Part List\n");
    hr = pParts->GetCount(&count);
    if(hr==S_OK && count>0)
    {
        for(UINT idx=0;idx<count;idx++)
        {
            IPart *pPart = 0;
            
            hr = pParts->GetPart(idx,&pPart);
            if(hr==S_OK && pPart!=0)
            {
                LPWSTR pPartName = 0;
                
                hr = pPart->GetName(&pPartName);
                if(hr==S_OK && pPartName!=0)
                {
                    QString n = QString::fromUtf16((const char16_t *)pPartName);
                    common::Log::g_Log.print("Part Name - %s\n",n.toUtf8().constData());
                    common::Log::g_Log.print("Part Type - %s\n",deviceTopologyPartSubType(pPart).toUtf8().constData());
                    CoTaskMemFree(pPartName);
                }
                pPart->Release();
            }
        }
    }
    common::Log::g_Log.print("Scan Part List - complete\n");
    
    pParts->Release();
}


void deviceTopologyScanPart(IPart *pPart)
{
    HRESULT hr;
    IPartsList *pParts = 0;
        
    common::Log::g_Log.print("Outgoing parts\n");
    hr = pPart->EnumPartsOutgoing(&pParts);
    if(hr==S_OK && pParts!=0)
    {
        deviceTopologyParts(pParts);
    }
    
    common::Log::g_Log.print("Incoming parts\n");
    hr = pPart->EnumPartsIncoming(&pParts);
    if(hr==S_OK && pParts!=0)
    {
        deviceTopologyParts(pParts);
    }
}

TEST(WasAPI,deviceTopologyA)
{
    WasAPIIFSPtr pWAS = WasAPIIF::instance("wasapi");
    ASSERT_FALSE(pWAS.isNull());

    QSharedPointer<WasAPILayerIF> pAPI = pWAS.dynamicCast<WasAPILayerIF>();
    ASSERT_FALSE(pAPI.isNull());

    AOQueryWasAPI audioQuery;    
    ASSERT_TRUE(audioQuery.queryNames());
    ASSERT_TRUE(audioQuery.noDevices() > 0);
    
    for(int i=0;i<audioQuery.noDevices();i++)
    {
        HRESULT hr;
        LPWSTR pDeviceName;
        IMMDevice *pDevice = 0;
        QString devID;
        
        devID = audioQuery.device(i).idConst();
        common::Log::g_Log.print("Querying Device Topology - %s\n",devID.toUtf8().constData());
        common::Log::g_Log.print("Name - %s\n",audioQuery.device(i).name().toUtf8().constData());
        
        pDeviceName = reinterpret_cast<LPWSTR>(CoTaskMemAlloc((devID.length() + 1) * sizeof(tuint16)));
        if(pDeviceName!=0)
        {
            memcpy(pDeviceName,devID.utf16(),devID.length() * sizeof(tuint16));
            pDeviceName[devID.length()] = 0;
            
            hr = pAPI->enumerator()->GetDevice(pDeviceName,&pDevice);
            if(hr==S_OK && pDevice!=0)
            {
                const IID IDOF_IDeviceTopology = __uuidof(IDeviceTopology);

                IDeviceTopology *pTopology = 0;
                
                hr = pDevice->Activate(IDOF_IDeviceTopology,CLSCTX_ALL,NULL,(void **)&pTopology);
                if(hr==S_OK && pTopology!=0)
                {
                    UINT count = 0;
                    
                    hr = pTopology->GetConnectorCount(&count);
                    if(hr==S_OK)
                    {
                        common::Log::g_Log.print("No. of Connectors - %d\n",count);
                        
                        for(UINT idx=0;idx<count;idx++)
                        {
                            IConnector *pConnector = 0;
                            
                            hr = pTopology->GetConnector(idx,&pConnector);
                            if(hr==S_OK && pConnector!=0)
                            {
                                common::Log::g_Log.print("Connector - %d\n",idx+1);
                                deviceTopologyPrintConnectorType(pConnector);
                                
                                BOOL connected;
                                hr = pConnector->IsConnected(&connected);
                                if(connected)
                                {
                                    IConnector *pConnectTo = 0;
                                    
                                    common::Log::g_Log.print("Is connected\n");
                                    hr = pConnector->GetConnectedTo(&pConnectTo);
                                    if(hr==S_OK && pConnectTo!=0)
                                    {
                                        const IID IDOF_IPart = __uuidof(IPart);
                                        IPart *pPartPrev = 0;
                                        
                                        common::Log::g_Log.print("Query connect part interface\n");
                                        
                                        hr = pConnectTo->QueryInterface(IDOF_IPart,(void **)&pPartPrev);
                                        if(hr==S_OK && pPartPrev!=0)
                                        {
                                            deviceTopologyScanPart(pPartPrev);
                                            pPartPrev->Release();    
                                        }
                                        
                                        pConnectTo->Release();
                                    }
                                }
                                else
                                {
                                    common::Log::g_Log.print("End of connection\n");
                                }
                                
                                pConnector->Release();
                            }
                        }
                    }
                    
                    pTopology->Release();
                }
                pDevice->Release();
            }
            CoTaskMemFree(pDeviceName);
        }
    }
    
    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

typedef void (*DeviceTopologyProcessPartFunc)(IPart *pPart,void *pValue);

QString deviceTopologyLevelSpacing(tint level)
{
    QString s;
    
    for(int i=0;i<level;i++)
    {
        s += "--";
    }
    s += " ";
    return s;
}

void deviceTopologyParseConnector(IConnector *pConnector,tint level,QVector<QPair<DeviceTopologyProcessPartFunc,void *> >& processList)
{
    HRESULT hr;
    BOOL connected = FALSE;
    QString s = deviceTopologyLevelSpacing(level);
    
    common::Log::g_Log.print("%sParsing Connector of Type - %s\n",s.toUtf8().constData(),deviceTopologyConnectorType(pConnector).toUtf8().constData());
    
    hr = pConnector->IsConnected(&connected);
    if(hr==S_OK && connected)
    {
        IConnector *pTerminal = 0;
        
        hr = pConnector->GetConnectedTo(&pTerminal);
        if(hr==S_OK && pTerminal!=0)
        {
            const IID IDOF_IPart = __uuidof(IPart);
            IPart *pPart = 0;
            
            common::Log::g_Log.print("%sConnected to Type - %s\n",s.toUtf8().constData(),deviceTopologyConnectorType(pTerminal).toUtf8().constData());
            
            hr = pTerminal->QueryInterface(IDOF_IPart,(void **)&pPart);
            if(hr==S_OK && pPart!=0)
            {
                IPartsList *pList = 0;
                bool loop = true;
                
                while(loop)
                {
                    IPart *pPartNext = 0;
                    
                    common::Log::g_Log.print("%sPart Type - %s\n",s.toUtf8().constData(),deviceTopologyPartSubType(pPart).toUtf8().constData());

                    for(QVector<QPair<DeviceTopologyProcessPartFunc,void *> >::iterator ppI=processList.begin();ppI!=processList.end();ppI++)
                    {
                        DeviceTopologyProcessPartFunc processFunc = (*ppI).first;
                        processFunc(pPart,(*ppI).second);
                    }
                    
                    hr = pPart->EnumPartsIncoming(&pList);
                    if(hr==S_OK && pList!=0)
                    {
                        hr = pList->GetPart(0,&pPartNext);
                        pList->Release();
                        
                        if(hr==S_OK && pPartNext!=0)
                        {
                            PartType partType;
                        
                            hr = pPartNext->GetPartType(&partType);
                            if(hr==S_OK)
                            {
                                if(partType!=Connector)
                                {
                                    pPart->Release();
                                    pPart = pPartNext;
                                }
                                else
                                {
                                    const IID IDOF_IConnector = __uuidof(IConnector);
                                    IConnector *pChildConnector = 0;
                                    
                                    hr = pPartNext->QueryInterface(IDOF_IConnector,(void **)&pChildConnector);
                                    if(hr==S_OK && pChildConnector!=0)
                                    {
                                        deviceTopologyParseConnector(pChildConnector,level+1,processList);
                                        pChildConnector->Release();
                                    }
                                    else
                                    {
                                        common::Log::g_Log.print("%sERROR - Failed to get next connector\n",s.toUtf8().constData());
                                    }
                                    loop = false;
                                }
                            }
                            else
                            {
                                common::Log::g_Log.print("%sERROR - Failed to get part type\n",s.toUtf8().constData());
                                loop = false;
                            }
                        }
                        else
                        {
                            common::Log::g_Log.print("%sERROR - Failed to get next enumerated part\n",s.toUtf8().constData());
                            loop = false;
                        }
                    }
                    else
                    {
                        common::Log::g_Log.print("%sERROR - Failed to enumerate incoming parts\n",s.toUtf8().constData());
                        loop = false;
                    }
                }
                
                pPart->Release();
            }
            else
            {
                common::Log::g_Log.print("%sERROR - Failed to get Part connection is made to\n",s.toUtf8().constData());
            }
            
            pTerminal->Release();
        }
        else
        {
            common::Log::g_Log.print("%sERROR - Failed to get connection\n",s.toUtf8().constData());
        }
    }
    else
    {
        ConnectorType type;
    
        hr = pConnector->GetType(&type);
        if(hr==S_OK && type==Software_IO)
        {
            common::Log::g_Log.print("%sTerminal Connector to Software\n",s.toUtf8().constData());
        }
        else
        {
            common::Log::g_Log.print("%sHas no connection\n",s.toUtf8().constData());
        }
    }
}

void deviceTopologyGetChannelMask(IPart *pPart,void *pValue)
{
    HRESULT hr;
    GUID subType;
    
    hr = pPart->GetSubType(&subType);
    if(hr==S_OK && (subType==KSNODETYPE_3D_EFFECTS || subType==KSNODETYPE_DAC || subType==KSNODETYPE_VOLUME || subType==KSNODETYPE_PROLOGIC_DECODER))
    {
        const IID IDOF_IAudioChannelConfig = __uuidof(IAudioChannelConfig);
        IAudioChannelConfig *pChannelConfig;    
    
        hr = pPart->Activate(CLSCTX_ALL,IDOF_IAudioChannelConfig,(void **)&pChannelConfig);
        if(hr==S_OK && pChannelConfig!=0)
        {
            DWORD config;
            
            hr = pChannelConfig->GetChannelConfig(&config);
            if(hr==S_OK)
            {
                int *pV = reinterpret_cast<int *>(pValue);
                *pV = static_cast<int>(config);
                common::Log::g_Log.print("++ Channel Mask : %4x\n",*pV);
            }
            pChannelConfig->Release();
        }
    }
}

TEST(WasAPI,deviceTopologyB)
{
    WasAPIIFSPtr pWAS = WasAPIIF::instance("wasapi");
    ASSERT_FALSE(pWAS.isNull());

    QSharedPointer<WasAPILayerIF> pAPI = pWAS.dynamicCast<WasAPILayerIF>();
    ASSERT_FALSE(pAPI.isNull());

    AOQueryWasAPI audioQuery;    
    ASSERT_TRUE(audioQuery.queryNames());
    ASSERT_TRUE(audioQuery.noDevices() > 0);
    
    for(int i=0;i<audioQuery.noDevices();i++)
    {
        HRESULT hr;
        LPWSTR pDeviceName;
        IMMDevice *pDevice = 0;
        QString devID;
        
        devID = audioQuery.device(i).idConst();
        common::Log::g_Log.print("Querying Device Topology - %s\n",devID.toUtf8().constData());
        common::Log::g_Log.print("Name - %s\n",audioQuery.device(i).name().toUtf8().constData());
        
        pDeviceName = reinterpret_cast<LPWSTR>(CoTaskMemAlloc((devID.length() + 1) * sizeof(tuint16)));
        if(pDeviceName!=0)
        {
            memcpy(pDeviceName,devID.utf16(),devID.length() * sizeof(tuint16));
            pDeviceName[devID.length()] = 0;
            
            hr = pAPI->enumerator()->GetDevice(pDeviceName,&pDevice);
            if(hr==S_OK && pDevice!=0)
            {
                const IID IDOF_IDeviceTopology = __uuidof(IDeviceTopology);
                IDeviceTopology *pTopology = 0;
                
                hr = pDevice->Activate(IDOF_IDeviceTopology,CLSCTX_ALL,NULL,(void **)&pTopology);
                if(hr==S_OK && pTopology!=0)
                {
                    IConnector *pRootConnector = 0;
                
                    hr = pTopology->GetConnector(0,&pRootConnector);
                    if(hr==S_OK && pRootConnector!=0)
                    {
                        DataFlow flow;
                        
                        hr = pRootConnector->GetDataFlow(&flow);
                        EXPECT_EQ(S_OK,hr);
                        EXPECT_EQ(In,hr);
                        
                        int channelMask = 0;
                        QVector<QPair<DeviceTopologyProcessPartFunc,void *> > processList;
                        processList.append(QPair<DeviceTopologyProcessPartFunc,void *>(deviceTopologyGetChannelMask,(void *)&channelMask));
                        
                        deviceTopologyParseConnector(pRootConnector,1,processList);
                        
                        pRootConnector->Release();
                    }
                    pTopology->Release();
                }
                pDevice->Release();
            }
            CoTaskMemFree(pDeviceName);
        }
        common::Log::g_Log.print("\n");
    }
    
    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(WasAPI,deviceDefaultMixedFormat)
{
    WasAPIIFSPtr pWAS = WasAPIIF::instance("wasapi");
    ASSERT_FALSE(pWAS.isNull());

    QSharedPointer<WasAPILayerIF> pAPI = pWAS.dynamicCast<WasAPILayerIF>();
    ASSERT_FALSE(pAPI.isNull());

    AOQueryWasAPI audioQuery;    
    ASSERT_TRUE(audioQuery.queryNames());
    ASSERT_TRUE(audioQuery.noDevices() > 0);
    
    for(int i=0;i<audioQuery.noDevices();i++)
    {
        HRESULT hr;
        LPWSTR pDeviceName;
        IMMDevice *pDevice = 0;
        QString devID;
        
        devID = audioQuery.device(i).idConst();
        common::Log::g_Log.print("Querying Default Format - %s\n",devID.toUtf8().constData());
        common::Log::g_Log.print("Name - %s\n",audioQuery.device(i).name().toUtf8().constData());
        
        pDeviceName = reinterpret_cast<LPWSTR>(CoTaskMemAlloc((devID.length() + 1) * sizeof(tuint16)));
        if(pDeviceName!=0)
        {
            memcpy(pDeviceName,devID.utf16(),devID.length() * sizeof(tuint16));
            pDeviceName[devID.length()] = 0;
            
            hr = pAPI->enumerator()->GetDevice(pDeviceName,&pDevice);
            if(hr==S_OK && pDevice!=0)
            {
                const IID IDOF_IAudioClient = __uuidof(IAudioClient);
                IAudioClient *pAudioClient;
        
                hr = pDevice->Activate(IDOF_IAudioClient,CLSCTX_ALL,0,reinterpret_cast<void **>(&pAudioClient));
                if(hr==S_OK && pAudioClient!=0)
                {
                    WAVEFORMATEX *pFormat = 0;
                    
                    hr = pAudioClient->GetMixFormat(&pFormat);
                    if(hr==S_OK && pFormat!=0)
                    {
                        common::Log::g_Log.print("Channels - %d, Sample Rate - %d, Bits - %d\n\n",pFormat->nChannels,pFormat->nSamplesPerSec,pFormat->wBitsPerSample);
                        CoTaskMemFree(pFormat);
                    }
                    
                    pAudioClient->Release();
                }
                pDevice->Release();
            }
            CoTaskMemFree(pDeviceName);
        }
        common::Log::g_Log.print("\n");
    }
    
    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(WasAPI, deviceLogCapabilities)
{
    WasAPIIFSPtr pWAS = WasAPIIF::instance("wasapi");
    ASSERT_FALSE(pWAS.isNull());

    QSharedPointer<WasAPILayerIF> pAPI = pWAS.dynamicCast<WasAPILayerIF>();
    ASSERT_FALSE(pAPI.isNull());

    AOQueryWasAPI audioQuery;    
    ASSERT_TRUE(audioQuery.queryNames());
    ASSERT_TRUE(audioQuery.noDevices() > 0);
    
    for(int i = 0; i < audioQuery.noDevices(); i++)
    {
        ASSERT_TRUE(audioQuery.queryDevice(i));
        const AOQueryWasAPI::DeviceWasAPI& dev = dynamic_cast<const AOQueryWasAPI::DeviceWasAPI&>(audioQuery.device(i));

        WasAPIDeviceSPtr pDev = dev.deviceInterface();
        QSharedPointer<WasAPIDeviceLayer> pDevice = pDev.dynamicCast<WasAPIDeviceLayer>();
        ASSERT_TRUE(!pDevice.isNull());

        common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;
        common::Log::g_Log << "-*** Audio Device ***------------------------------------" << common::c_endl;
        common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;
        common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;
        common::Log::g_Log << common::c_endl;
        common::Log::g_Log << "WasAPI Audio Device: " << dev.name() << common::c_endl;
        common::Log::g_Log << "\t GUID:" << dev.idConst() << common::c_endl;
        
        const QSet<int>& freqs = dev.frequencies();
        common::Log::g_Log << "Frequencies:" << common::c_endl;
        for(QSet<int>::const_iterator ppI = freqs.begin(); ppI != freqs.end(); ppI++)
        {
            common::Log::g_Log << "\t\t" << QString::number(*ppI) << common::c_endl;
        }
        common::Log::g_Log << common::c_endl;
                
        common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;


//        int chList[2] = { 1,7 };
//        for(tint chIdx = 0; chIdx < 2; chIdx++)
//        {
//            tint chs = chList[chIdx];
        for(tint chs = 0; chs < NUMBER_WASAPI_MAXCHANNELS; chs++)
        {
            for(tint bitIdx = 0; bitIdx < NUMBER_WASAPI_MAXBITS; bitIdx++)
            {
                for(tint freqIdx = 0; freqIdx < NUMBER_WASAPI_MAXFREQUENCIES; freqIdx++)
                //for(tint freqIdx = 7; freqIdx <= 8; freqIdx++)
                {
                    pDevice->printIndexedFormatSupport(bitIdx, chs, freqIdx);
                    common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;
                }
                common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;
            }
            common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;
        }
        common::Log::g_Log << "---------------------------------------------------------" << common::c_endl;        
    }
    
    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(WasAPI, deviceLogCapCSV)
{
    WasAPIIFSPtr pWAS = WasAPIIF::instance("wasapi");
    ASSERT_FALSE(pWAS.isNull());

    QSharedPointer<WasAPILayerIF> pAPI = pWAS.dynamicCast<WasAPILayerIF>();
    ASSERT_FALSE(pAPI.isNull());

    AOQueryWasAPI audioQuery;
    ASSERT_TRUE(audioQuery.queryNames());
    ASSERT_TRUE(audioQuery.noDevices() > 0);

    for (int i = 0; i < audioQuery.noDevices(); i++)
    {
        ASSERT_TRUE(audioQuery.queryDevice(i));
        const AOQueryWasAPI::DeviceWasAPI& dev = dynamic_cast<const AOQueryWasAPI::DeviceWasAPI&>(audioQuery.device(i));

        WasAPIDeviceSPtr pDev = dev.deviceInterface();
        QSharedPointer<WasAPIDeviceLayer> pDevice = pDev.dynamicCast<WasAPIDeviceLayer>();
        ASSERT_TRUE(!pDevice.isNull());

        common::Log::g_Log << dev.name() << "; ; ; ; ;\n";
        for(tint chs = 0; chs < NUMBER_WASAPI_MAXCHANNELS; chs++)
        {
            common::Log::g_Log << "Channels;" << pDevice->getNumberOfChannelsFromIndex(chs) << "; ; ; ;\n";
            common::Log::g_Log << "Bits/Freq;16;20;24;28;32;32-Float\n";
            for(tint isExclusive = 0; isExclusive < 2; isExclusive++)
            {
                for(tint freqIdx = 0; freqIdx < NUMBER_WASAPI_MAXFREQUENCIES; freqIdx++)
                {
                    common::Log::g_Log << pDevice->getFrequencyFromIndex(freqIdx) << "(" << ((!isExclusive) ? "S" : "E") << ");";
                    for (tint bitIdx = 0; bitIdx < NUMBER_WASAPI_MAXBITS; bitIdx++)
                    {
                        common::Log::g_Log << pDevice->capabilityCSVIndexed(bitIdx, chs, freqIdx, isExclusive);
                        common::Log::g_Log << ";";
                    }
                    common::Log::g_Log << pDevice->capabilityCSVIndexed(-1, chs, freqIdx, isExclusive);
                    common::Log::g_Log << "\n";
                }
            }
        }
        common::Log::g_Log << " ; ; ; ; ; ;\n";
    }

    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------
