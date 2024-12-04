#include "audioio/inc/AOQueryWin32.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOQueryWin32::AOQueryWin32() : AOQueryDevice(),
	m_devicesASIO(0),
	m_devicesWasAPI(0)
{
	m_devicesASIO = new AOQueryASIO;
	m_devicesWasAPI = new AOQueryWasAPI;
}

//-------------------------------------------------------------------------------------------

AOQueryWin32::~AOQueryWin32()
{
	if(m_devicesASIO!=0)
	{
		delete m_devicesASIO;
		m_devicesASIO = 0;
	}
	if(m_devicesWasAPI!=0)
	{
		delete m_devicesWasAPI;
		m_devicesWasAPI = 0;
	}
}

//-------------------------------------------------------------------------------------------

bool AOQueryWin32::queryNames()
{
	return (m_devicesASIO->queryNames() || m_devicesWasAPI->queryNames());
}

//-------------------------------------------------------------------------------------------

bool AOQueryWin32::queryDevice(int idx)
{
	bool res = false;
	
	if(idx>=0 && idx<m_devicesASIO->noDevices())
	{
		res = m_devicesASIO->queryDevice(idx);
	}
	else
	{
		idx -= m_devicesASIO->noDevices();
		
		if(idx>=0 && idx<m_devicesWasAPI->noDevices())
		{
			res = m_devicesWasAPI->queryDevice(idx);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int AOQueryWin32::defaultDeviceIndex()
{
	return m_devicesWasAPI->defaultDeviceIndex() + m_devicesASIO->noDevices();
}

//-------------------------------------------------------------------------------------------

int AOQueryWin32::noDevices() const
{
	return m_devicesASIO->noDevices() + m_devicesWasAPI->noDevices();
}

//-------------------------------------------------------------------------------------------

const AOQueryDevice::Device& AOQueryWin32::device(int idx) const
{
	if(idx>=0 && idx<m_devicesASIO->noDevices())
	{
		return m_devicesASIO->device(idx);
	}
	else
	{
		idx -= m_devicesASIO->noDevices();
		return m_devicesWasAPI->device(idx);
	}
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device* AOQueryWin32::deviceDirect(int idx)
{
	if(idx >= 0 && idx < m_devicesASIO->noDevices())
	{
		return m_devicesASIO->deviceDirect(idx);
	}
	else
	{
		idx -= m_devicesASIO->noDevices();
		return m_devicesWasAPI->deviceDirect(idx);
	}
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
