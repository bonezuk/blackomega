#if defined(OMEGA_WIN32)
#include <comdef.h>
#include <Wbemidl.h>
#include <windows.h>
#include <Iptypes.h>
#include <Iphlpapi.h>
#endif

#include <QProcess>
#include <QMap>
#include "common/inc/HardwareIdentification.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

HardwareIdentification *HardwareIdentification::m_instance = 0;

//-------------------------------------------------------------------------------------------

HardwareIdentification::HardwareIdentification() : m_idList()
{
    tint i;
    QMap<QString,HardwareType> idMap;
    QMap<QString,HardwareType>::iterator ppI;

#if defined(OMEGA_WIN32)
    getWDIInfomation(m_idList);
    getMACAddresses(m_idList);
#elif defined(OMEGA_MACOSX)
    getMacUUID(m_idList);
#endif

    for(i=0;i<m_idList.size();i++)
    {
        const QPair<HardwareType,QString>& p = m_idList.at(i);
        idMap.insert(p.second,p.first);
    }
    m_idList.clear();
    for(ppI=idMap.begin();ppI!=idMap.end();ppI++)
    {
        m_idList.append(QPair<HardwareType,QString>(ppI.value(),ppI.key()));
    }
}

//-------------------------------------------------------------------------------------------

HardwareIdentification::~HardwareIdentification()
{
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

HardwareIdentification& HardwareIdentification::instance()
{
    if(m_instance==0)
    {
        m_instance = new HardwareIdentification;
    }
    return *m_instance;
}

//-------------------------------------------------------------------------------------------

bool HardwareIdentification::getIdentity(QVector<QPair<HardwareType,QString> >& idList) const
{
    idList = m_idList;
    return (idList.size()>0);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool HardwareIdentification::getWDIInfomation(QVector<QPair<HardwareType,QString> >& idList)
{
    HRESULT hres;
    bool res = false;
    
    hres = CoInitializeEx(0,COINIT_MULTITHREADED);
    if(hres!=S_OK)
    {
        return false;
    }

    hres = ::CoInitializeSecurity(0,-1,0,0,RPC_C_AUTHN_LEVEL_DEFAULT,RPC_C_IMP_LEVEL_IMPERSONATE,0,EOAC_NONE,0);
    if(hres==S_OK)
    {
        IWbemLocator *pLoc = 0;
        hres = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,IID_IWbemLocator,(LPVOID *)&pLoc);
        if(hres==S_OK)
        {
            IWbemServices *pSvc = 0;
            hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),0,0,0,0,0,0,&pSvc);
            if(hres==S_OK)
            {
                hres = ::CoSetProxyBlanket(pSvc,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,0,RPC_C_AUTHN_LEVEL_CALL,RPC_C_IMP_LEVEL_IMPERSONATE,0,EOAC_NONE);
                if(hres==S_OK)
                {
                    IEnumWbemClassObject *pMExec = 0;
                    hres = pSvc->ExecQuery(bstr_t("WQL"),bstr_t("SELECT SerialNumber FROM Win32_BaseBoard"),WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,0,&pMExec);
                    if(hres==S_OK)
                    {
                        while(pMExec)
                        {
                            IWbemClassObject *pCls;
                            ULONG uReturn;
                            
                            pCls = 0;
                            uReturn = 0;
                            hres = pMExec->Next(WBEM_INFINITE,1,&pCls,&uReturn);
#pragma warning(suppress: 6102)
                            if(uReturn!=0 && pCls!=0)
                            {
                                VARIANT vtProp;
                                hres = pCls->Get(L"SerialNumber",0,&vtProp,0,0);
                                if(hres==S_OK)
                                {
                                    QString hID = QString::fromUtf16(reinterpret_cast<const char16_t*>(vtProp.bstrVal)).trimmed();
                                    if(!hID.isEmpty())
                                    {
                                        QPair<HardwareType,QString> hP(e_Motherboard,hID);
                                        idList.append(hP);
                                        res = true;
                                    }
                                }
                                pCls->Release();
                            }
                            else
                            {
                                break;
                            }
                        }
                        pMExec->Release();
                    }
                    
                    IEnumWbemClassObject *pDExec = 0;
                    hres = pSvc->ExecQuery(bstr_t("WQL"),bstr_t("SELECT SerialNumber FROM Win32_SystemEnclosure"),WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,0,&pDExec);
                    if(hres==S_OK)
                    {
                        IWbemClassObject *pCls;
                        ULONG uReturn;

                        while(pDExec)
                        {
                            pCls = 0;
                            uReturn = 0;
                            hres = pDExec->Next(WBEM_INFINITE,1,&pCls,&uReturn);
#pragma warning(suppress: 6102)
                            if(uReturn!=0)
                            {
                                VARIANT vtProp;

#pragma warning(suppress: 6102)
                                hres = pCls->Get(L"SerialNumber",0,&vtProp,0,0);
                                if(hres==S_OK)
                                {
                                    QString hID = QString::fromUtf16(reinterpret_cast<const char16_t*>(vtProp.bstrVal)).trimmed();
                                    if(!hID.isEmpty())
                                    {
                                        QPair<HardwareType,QString> hP(e_HardDisk,hID);
                                        idList.append(hP);
                                        res = true;
                                    }
                                }
                                pCls->Release();
                            }
                            else
                            {
                                break;
                            }
                        }
                        pDExec->Release();
                    }    
                }
                pSvc->Release();
            }
            pLoc->Release();
        }
    }
    CoUninitialize();
    return res;
}


//-------------------------------------------------------------------------------------------

bool HardwareIdentification::getMACAddresses(QVector<QPair<HardwareType,QString> >& idList)
{
    IP_ADAPTER_INFO adapterInfo[16];
    DWORD dwStatus,dwBufLen;
    bool res = false;
    
    dwBufLen = sizeof(adapterInfo);
    dwStatus = ::GetAdaptersInfo(adapterInfo,&dwBufLen);
    if(dwStatus==ERROR_SUCCESS)
    {
        PIP_ADAPTER_INFO pA = adapterInfo;

        do
        {
            QString mID = getMACString(pA->Address,pA->AddressLength);
            if(!mID.isEmpty())
            {
                QPair<HardwareType,QString> mP(e_NetworkMACAddress,mID);
                idList.append(mP);
                res = true;
            }
            pA = pA->Next;
        } while(pA);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString HardwareIdentification::getMACString(const BYTE *addr,int len)
{
    int i;
    QString n;
    
    for(i=0;i<len;i++)
    {
        tuint a,b;
        char t[3] = {'\0','\0','\0'};
        
        if(i)
        {
            n += ":";
        }
        a = (static_cast<tuint>(addr[i]) >> 4) & 0x0000000f;
        b = static_cast<tuint>(addr[i]) & 0x0000000f;
        if(a>=0 && a<10)
        {
            t[0] = static_cast<char>(a) + '0';
        }
        else
        {
            t[0] = static_cast<char>(a-10) + 'a';
        }
        if(b>=0 && b<10)
        {
            t[1] = static_cast<char>(b) + '0';
        }
        else
        {
            t[1] = static_cast<char>(b-10) + 'a';
        }
        n += t;
    }
    return n;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

bool HardwareIdentification::getMacUUID(QVector<QPair<HardwareType,QString> >& idList)
{
    QString uID;
    QProcess proc;
    QStringList args;
    
    args << "-c" << "ioreg -rd1 -c IOPlatformExpertDevice |  awk '/IOPlatformUUID/ { print $3; }'";
    proc.start("/bin/bash",args);
    proc.waitForFinished();
    uID = proc.readAll();
    if(!uID.isEmpty())
    {
        idList.append(QPair<HardwareType,QString>(e_Motherboard,uID));
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
