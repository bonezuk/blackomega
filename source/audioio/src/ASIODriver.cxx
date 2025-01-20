#include "audioio/inc/ASIODriver.h"
#include "common/inc/Log.h"
#include "common/inc/DiskOps.h"

//-------------------------------------------------------------------------------------------
// ASIO definitions
//-------------------------------------------------------------------------------------------

#define MAXPATHLEN 512
#define MAXDRVNAMELEN 128

#define ASIODRV_DESC "description"
#define INPROC_SERVER "InprocServer32"
#define ASIO_PATH "software\\asio"
#define COM_CLSID "clsid"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------
// ASIODriverService::DriverInfo
//-------------------------------------------------------------------------------------------

ASIODriverService::DriverInfo::DriverInfo() : m_id(0),
	m_path(),
	m_name()
{
	::memset(&m_clsid,0,sizeof(CLSID));
}

//-------------------------------------------------------------------------------------------

ASIODriverService::DriverInfo::DriverInfo(const DriverInfo& rhs) : m_id(0),
	m_path(),
	m_name()
{
	copy(rhs);
}

//-------------------------------------------------------------------------------------------

const ASIODriverService::DriverInfo& ASIODriverService::DriverInfo::operator = (const DriverInfo& rhs)
{
	if(this!=&rhs)
	{
		copy(rhs);
	}
	return *this;
}

//-------------------------------------------------------------------------------------------

void ASIODriverService::DriverInfo::copy(const DriverInfo& rhs)
{
	m_id = rhs.m_id;
	m_clsid = rhs.m_clsid;
	m_path = rhs.m_path;
	m_name = rhs.m_name;
}

//-------------------------------------------------------------------------------------------

tint& ASIODriverService::DriverInfo::id()
{
	return m_id;
}

//-------------------------------------------------------------------------------------------

const tint& ASIODriverService::DriverInfo::id() const
{
	return m_id;
}

//-------------------------------------------------------------------------------------------

CLSID& ASIODriverService::DriverInfo::clsid()
{
	return m_clsid;
}

//-------------------------------------------------------------------------------------------

const CLSID& ASIODriverService::DriverInfo::clsid() const
{
	return m_clsid;
}

//-------------------------------------------------------------------------------------------

QString& ASIODriverService::DriverInfo::path()
{
	return m_path;
}

//-------------------------------------------------------------------------------------------

const QString& ASIODriverService::DriverInfo::path() const
{
	return m_path;
}

//-------------------------------------------------------------------------------------------

QString& ASIODriverService::DriverInfo::name()
{
	return m_name;
}

//-------------------------------------------------------------------------------------------

const QString& ASIODriverService::DriverInfo::name() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------
// ASIODriverService
//-------------------------------------------------------------------------------------------

ASIODriverService *ASIODriverService::m_instance = 0;

//-------------------------------------------------------------------------------------------

ASIODriverService::ASIODriverService() : m_comSystem(S_OK),
	m_driverMap(),
	m_asioMap()
{}

//-------------------------------------------------------------------------------------------

ASIODriverService::~ASIODriverService()
{
	shutdown();
	m_instance = 0;
}

//-------------------------------------------------------------------------------------------

void ASIODriverService::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "ASIODriverService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

ASIODriverService& ASIODriverService::instance()
{
	return *m_instance;
}

//-------------------------------------------------------------------------------------------

bool ASIODriverService::start()
{
	if(m_instance==0)
	{
		m_instance = new ASIODriverService;
		if(!m_instance->init())
		{
			delete m_instance;
			m_instance = 0;
		}
	}
	return (m_instance!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

void ASIODriverService::stop()
{
	if(m_instance!=0)
	{
		delete m_instance;
		m_instance = 0;
	}
}

//-------------------------------------------------------------------------------------------

bool ASIODriverService::init()
{
	m_driverMap.clear();
	m_comSystem = ::CoInitialize(0);
	if(m_comSystem==S_OK || m_comSystem==S_FALSE)
	{
		int dIdx = 0;
		DWORD index = 0;
		HKEY hkEnum;
		LONG cr;
		char keyname[MAXDRVNAMELEN];
		
		hkEnum = 0;
		cr = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "software\\asio", 0, KEY_READ, &hkEnum);
		if(cr == ERROR_SUCCESS)
		{
			while(cr == ERROR_SUCCESS)
			{
				DriverInfo info;

				cr = ::RegEnumKeyA(hkEnum, index, (LPSTR)keyname, MAXDRVNAMELEN);
				if(cr == ERROR_SUCCESS)
				{
					info.id() = index;
					info.name() = QString::fromLatin1(keyname);
					if(getDriverInfo(hkEnum, info))
					{
						if(canOpenDevice(info))
						{
							m_driverMap.insert(dIdx, info);
							dIdx++;
						}
					}
				}
				index++;
			}
			if(hkEnum == 0)
			{
				::RegCloseKey(hkEnum);
			}
		}
	}
	else
	{
		printError("init","Error starting up COM system");
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool ASIODriverService::canOpenDevice(DriverInfo& info)
{
	IASIO *iDriver;
	bool res = false;
	
	if(::CoCreateInstance(info.clsid(),0,CLSCTX_INPROC_SERVER,info.clsid(),(LPVOID *)&iDriver)==S_OK)
	{
		ASIODriverInfo dInfo;
		ASIODriver driver(this,iDriver);
		
		if(driver.ASIOInit(&dInfo)==ASE_OK)
		{
			res = true;
		}
		driver.ASIOExit();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void ASIODriverService::shutdown()
{
	QMap<tint,ASIODriver *>::iterator ppI;
	
	while(ppI=m_asioMap.begin(),ppI!=m_asioMap.end())
	{
		ASIODriver *drv = ppI.value();
		m_asioMap.erase(ppI);
		drv->ASIOExit();
		delete drv;
	}
	if(m_comSystem==S_OK)
	{
		::CoUninitialize();
	}
}

//-------------------------------------------------------------------------------------------

bool ASIODriverService::getDriverInfo(HKEY hkey,DriverInfo& info)
{
	HKEY hksub;
	LONG cr;
	DWORD datatype,datasize;
	CLSID clsid;
	char databuf[256];
	WORD wData[100];
	bool res = false;
	
	cr = ::RegOpenKeyExA(hkey,(LPCSTR)info.name().toLatin1().constData(),0,KEY_READ,&hksub);
	if(cr==ERROR_SUCCESS)
	{
		datatype = REG_SZ;
		datasize = 256;
		cr = ::RegQueryValueExA(hksub,COM_CLSID,0,&datatype,(LPBYTE)databuf,&datasize);
		if(cr==ERROR_SUCCESS)
		{
			QString comID(QString::fromLatin1(databuf).toLower());
			
			if(findDriverPath(comID,info))
			{
				memset(wData, 0, 100 * sizeof(WORD));
				::MultiByteToWideChar(CP_ACP,0,(LPCSTR)databuf,-1,(LPWSTR)wData,100);
#pragma warning(suppress: 6102)
				cr = ::CLSIDFromString((LPOLESTR)wData,(LPCLSID)&clsid);
				if(cr==S_OK)
				{
					::memcpy(&(info.clsid()),&clsid,sizeof(CLSID));
				}
				
				datatype = REG_SZ;
				datasize = 256;
				cr = ::RegQueryValueExA(hksub,ASIODRV_DESC,0,&datatype,(LPBYTE)databuf,&datasize);
				if(cr==ERROR_SUCCESS)
				{
					info.name() = QString::fromLatin1(databuf);
				}
				res = true;
			}
		}
		::RegCloseKey(hksub);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool ASIODriverService::findDriverPath(const QString& comID,DriverInfo& info)
{
	DWORD datatype,datasize,index = 0;
	HKEY hkEnum,hkSub,hkPath;
	LONG cr;
	char tmpA[512],tmpB[512];
	bool loop = true,res = false;
	
	cr = ::RegOpenKeyExA(HKEY_CLASSES_ROOT,COM_CLSID,0,KEY_READ,&hkEnum);
	if(cr==ERROR_SUCCESS)
	{
		while(cr==ERROR_SUCCESS && loop)
		{
			cr = ::RegEnumKeyA(hkEnum,index,(LPSTR)tmpA,512);
			if(cr==ERROR_SUCCESS)
			{
				QString nID(QString::fromLatin1(tmpA).toLower());
				
				if(nID==comID)
				{
					cr = ::RegOpenKeyExA(hkEnum,(LPCSTR)tmpA,0,KEY_READ,&hkSub);
					if(cr==ERROR_SUCCESS)
					{
						cr = ::RegOpenKeyExA(hkSub,(LPCSTR)INPROC_SERVER,0,KEY_READ,&hkPath);
						if(cr==ERROR_SUCCESS)
						{
							datatype = REG_SZ;
							datasize = 512;
							
							cr = ::RegQueryValueExA(hkPath,0,0,&datatype,(LPBYTE)tmpB,&datasize);
							if(cr==ERROR_SUCCESS)
							{
								info.path() = tmpB;
								res = true;
							}
							::RegCloseKey(hkPath);
						}
						::RegCloseKey(hkSub);
					}
					loop = false;
				}
			}
			index++;
		}
		::RegCloseKey(hkEnum);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tint ASIODriverService::noDrivers() const
{
	return m_driverMap.size();
}

//-------------------------------------------------------------------------------------------

const ASIODriverService::DriverInfo& ASIODriverService::driverInfo(tint index) const
{
	QMap<tint,DriverInfo>::const_iterator ppI;
	Q_ASSERT(index>=0 && index<m_driverMap.size());
	ppI = m_driverMap.find(index);
	Q_ASSERT(ppI!=m_driverMap.end());
	return ppI.value();
}

//-------------------------------------------------------------------------------------------

bool ASIODriverService::open(tint index)
{
	QMap<tint,DriverInfo>::iterator ppI;
	QMap<tint,ASIODriver *>::iterator ppJ;
	bool res = false;
	
	ppI = m_driverMap.find(index);
	if(ppI!=m_driverMap.end())
	{
		ppJ = m_asioMap.find(index);
		if(ppJ==m_asioMap.end())
		{
			IASIO *iDriver;
			DriverInfo& info = ppI.value();
			
			if(::CoCreateInstance(info.clsid(),0,CLSCTX_INPROC_SERVER,info.clsid(),(LPVOID *)&iDriver)==S_OK)
			{
				ASIODriver *driver = new ASIODriver(this,iDriver);
				m_asioMap.insert(index,driver);
				res = true;
			}
			else
			{
				QString err;
				err = "Failed to create ASIO interface to audio device " + info.name();
				printError("open",err.toLatin1().constData());
			}
		}
		else
		{
			res = true;
		}
	}
	else
	{
		printError("open","Audio device index out of range");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void ASIODriverService::close(tint index)
{
	ASIODriver *driver;
	QMap<tint,ASIODriver *>::iterator ppI;
	
	ppI = m_asioMap.find(index);
	if(ppI!=m_asioMap.end())
	{
		driver = ppI.value();
		m_asioMap.erase(ppI);
		driver->ASIOExit();
		delete driver;
	}
}

//-------------------------------------------------------------------------------------------

ASIODriver& ASIODriverService::driver(tint index)
{
	ASIODriver *driver;
	QMap<tint,ASIODriver *>::iterator ppI;
	
	ppI = m_asioMap.find(index);
	Q_ASSERT(ppI!=m_asioMap.end());
	driver = ppI.value();
	return *driver;
}

//-------------------------------------------------------------------------------------------

ASIODriver *ASIODriverService::driverPtr(tint index)
{
	ASIODriver *driver;
	QMap<tint,ASIODriver *>::iterator ppI;
	
	ppI = m_asioMap.find(index);
	if(ppI!=m_asioMap.end())
	{
		driver = ppI.value();
	}
	else
	{
		driver = 0;
	}
	return driver;
}

//-------------------------------------------------------------------------------------------

tint ASIODriverService::getSampleSize(ASIOSampleType t)
{
	switch(t)
	{
		case ASIOSTInt16LSB:   // 2
		case ASIOSTInt16MSB:   // 2
			return 2;
		
		case ASIOSTInt24LSB:   // 3
		case ASIOSTInt24MSB:   // 3
			return 3;
		
		case ASIOSTInt32LSB:   // 4
		case ASIOSTInt32MSB:   // 4
		case ASIOSTFloat32MSB: // 4
		case ASIOSTFloat32LSB: // 4
		case ASIOSTInt32LSB16: // 4
		case ASIOSTInt32MSB16: // 4
		case ASIOSTInt32LSB18: // 4
		case ASIOSTInt32MSB18: // 4
		case ASIOSTInt32LSB20: // 4
		case ASIOSTInt32MSB20: // 4
		case ASIOSTInt32LSB24: // 4
		case ASIOSTInt32MSB24: // 4
			return 4;
		
		case ASIOSTFloat64LSB: // 8
		case ASIOSTFloat64MSB: // 8
			return 8;
	}
	return 0;	
}

//-------------------------------------------------------------------------------------------
// ASIODriver
//-------------------------------------------------------------------------------------------

ASIODriver::ASIODriver(ASIODriverService *srv,IASIO *drv) : m_service(srv),
	m_driver(drv)
{}

//-------------------------------------------------------------------------------------------

ASIODriver::~ASIODriver()
{
	ASIOExit();
}

//-------------------------------------------------------------------------------------------

ASIODriverService *ASIODriver::service()
{
	return m_service;
}

//-------------------------------------------------------------------------------------------

IASIO *ASIODriver::driver()
{
	return m_driver;
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOInit(ASIODriverInfo *info)
{
	info->driverVersion = 0;
	strcpy(info->name,"No ASIO Driver");
	if(m_driver!=0)
	{
		if(m_driver->init(info->sysRef))
		{
			strcpy(info->errorMessage,"No ASIO Driver Error");
			m_driver->getDriverName(info->name);
			info->driverVersion = m_driver->getDriverVersion();
			return ASE_OK;
		}
		else
		{
			m_driver->getErrorMessage(info->errorMessage);
			// Set driver to NULL on error message as unhandled exception that cannot be caught thrown by foo's DSD ASIO plugin when Release called.
			m_driver = 0;
		}
	}
	return ASE_NotPresent;
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOExit()
{
	if(m_driver!=0)
	{
		m_driver->Release();
		m_driver = 0;
	}
	return ASE_OK;
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOStart()
{
	if(m_driver!=0)
	{
		return m_driver->start();
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOStop()
{
	if(m_driver!=0)
	{
		return m_driver->stop();
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOGetChannels(long *numInputChannels, long *numOutputChannels)
{
	if(m_driver!=0)
	{
		return m_driver->getChannels(numInputChannels,numOutputChannels);
	}
	else
	{
		*numInputChannels = 0;
		*numOutputChannels = 0;
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOGetLatencies(long *inputLatency, long *outputLatency)
{
	if(m_driver!=0)
	{
		return m_driver->getLatencies(inputLatency,outputLatency);
	}
	else
	{
		*inputLatency = 0;
		*outputLatency = 0;
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOGetBufferSize(long *minSize, long *maxSize, long *preferredSize, long *granularity)
{
	if(m_driver!=0)
	{
		return m_driver->getBufferSize(minSize,maxSize,preferredSize,granularity);
	}
	else
	{
		*minSize = 0;
		*maxSize = 0;
		*preferredSize = 0;
		*granularity = 0;
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOCanSampleRate(ASIOSampleRate sampleRate)
{
	if(m_driver!=0)
	{
		return m_driver->canSampleRate(sampleRate);
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOGetSampleRate(ASIOSampleRate *currentRate)
{
	if(m_driver!=0)
	{
		return m_driver->getSampleRate(currentRate);
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOSetSampleRate(ASIOSampleRate sampleRate)
{
	if(m_driver!=0)
	{
		return m_driver->setSampleRate(sampleRate);
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOGetClockSources(ASIOClockSource *clocks, long *numSources)
{
	if(m_driver!=0)
	{
		return m_driver->getClockSources(clocks,numSources);
	}
	else
	{
		*numSources = 0;
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOSetClockSource(long reference)
{
	if(m_driver!=0)
	{
		return m_driver->setClockSource(reference);
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOGetSamplePosition (ASIOSamples *sPos, ASIOTimeStamp *tStamp)
{
	if(m_driver!=0)
	{
		return m_driver->getSamplePosition(sPos,tStamp);
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOGetChannelInfo(ASIOChannelInfo *info)
{
	if(m_driver!=0)
	{
		return m_driver->getChannelInfo(info);
	}
	else
	{
		info->channelGroup = -1;
		info->type = ASIOSTInt16MSB;
		strcpy(info->name,"None");
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOCreateBuffers(ASIOBufferInfo *bufferInfos, long numChannels,long bufferSize, ASIOCallbacks *callbacks)
{
	if(m_driver!=0)
	{
		return m_driver->createBuffers(bufferInfos,numChannels,bufferSize,callbacks);
	}
	else
	{
		tint i;
		ASIOBufferInfo *info = bufferInfos;
		
		for(i=0;i<numChannels;i++,info++)
		{
			info->buffers[0] = info->buffers[1] = 0;
		}
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIODisposeBuffers()
{
	if(m_driver!=0)
	{
		return m_driver->disposeBuffers();
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOControlPanel()
{
	if(m_driver!=0)
	{
		return m_driver->controlPanel();
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOFuture(long selector, void *params)
{
	if(m_driver!=0)
	{
		return m_driver->future(selector,params);
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------

ASIOError ASIODriver::ASIOOutputReady()
{
	if(m_driver!=0)
	{
		return m_driver->outputReady();
	}
	else
	{
		return ASE_NotPresent;
	}
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
