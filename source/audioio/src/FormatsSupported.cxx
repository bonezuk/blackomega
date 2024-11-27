#include "audioio/inc/FormatsSupported.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

FormatsSupported::FormatsSupported() : m_formats()
{}

//-------------------------------------------------------------------------------------------

FormatsSupported::FormatsSupported(const FormatsSupported& rhs) : m_formats()
{
	copy(rhs);
}

//-------------------------------------------------------------------------------------------

const FormatsSupported& FormatsSupported::operator = (const FormatsSupported& rhs)
{
	if(this!=&rhs)
	{
		copy(rhs);
	}
	return *this;
}

//-------------------------------------------------------------------------------------------

void FormatsSupported::copy(const FormatsSupported& rhs)
{
	m_formats.clear();
	m_formats = rhs.m_formats;
}

//-------------------------------------------------------------------------------------------

bool FormatsSupported::isSupported(const FormatDescription& desc) const
{
	tuint32 key = toKey(desc);
	return (m_formats.find(key)!=m_formats.end());
}

//-------------------------------------------------------------------------------------------

void FormatsSupported::add(const FormatDescription& desc)
{
	tuint32 key = toKey(desc);
	if(m_formats.find(key)==m_formats.end())
	{
		m_formats.insert(key);
	}
}

//-------------------------------------------------------------------------------------------

void FormatsSupported::clear()
{
	m_formats.clear();
}

//-------------------------------------------------------------------------------------------

bool FormatsSupported::isEmpty() const
{
	return m_formats.isEmpty();
}

//-------------------------------------------------------------------------------------------
// Key format (e=isBigEndian, d=dataType, b=bits, c=channels, f=frequency, x=reserved)
// 32        24        16        8         0
// |xxxx|xxxx|xxxe|dddd|bbbb|bbbc|cccf|ffff|
//
// Design notes
// dataType - current (1 - 4) 2 bits (4 bits 0-16)
// bits - current (0 - 33) 6 bits (7 bits 0-63)
// channels - current (0 - 7) 3 - bits (4 bits 0-16)
// frequency - (0 - 17) 5 bits (5 bits 0-32)
// isBigEndian - (0 - 1) 1 bit (1 bit)
// total = 18 bits (5+4+7+4 = 20 bits / future = 12 bits)
//
//  0000 - 0  1000 - 8
//  0001 - 1  1001 - 9
//  0010 - 2  1010 - A
//  0011 - 3  1011 - B
//  0100 - 4  1100 - C
//  0101 - 5  1101 - D
//  0110 - 6  1110 - E
//  0111 - 7  1111 - F
//-------------------------------------------------------------------------------------------

tuint32 FormatsSupported::toKey(const FormatDescription& desc) const
{
	tuint32 key = (desc.isBigEndian()) ? 0x00100000 : 0;
	key |= ((static_cast<tuint32>(desc.typeOfData()) - 1) << 16) & 0x000f0000;
	key |= (static_cast<tuint32>(desc.bitsIndex()) << 9) & 0x0000fe00;
	key |= (static_cast<tuint32>(desc.channelsIndex()) << 5) & 0x000001e0;
	key |= (static_cast<tuint32>(desc.frequencyIndex())) & 0x0000001f;
	return key;
}

//-------------------------------------------------------------------------------------------

FormatDescription FormatsSupported::fromKey(tuint32 key) const
{
	FormatDescription format;
	format.setTypeOfData(static_cast<FormatDescription::DataType>(((key >> 16) & 0x0000000f) + 1));
	format.setBitsIndex(static_cast<tint>((key >> 9) & 0x0000007f));
	format.setChannelsIndex(static_cast<tint>((key >> 5) & 0x0000000f));
	format.setFrequencyIndex(static_cast<tint>(key & 0x0000001f));
	format.setEndian(((key >> 20) & 0x00000001) ? false : true);
	return format;
}

//-------------------------------------------------------------------------------------------

void FormatsSupported::print() const
{
	for(QSet<tuint32>::const_iterator ppI = m_formats.begin(); ppI != m_formats.end(); ppI++)
	{
		tuint32 key = *ppI;
		FormatDescription d = fromKey(key);
		common::Log::g_Log << d.description() << common::c_endl;
	}
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
