#include "audioio/inc/ALSAStreamParser.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

ALSAStreamParser::ALSAStreamParser() : m_deviceName(),
	m_isDSDSpecial(false),
	m_isDSDOverPCM(false),
	m_isMSB(true)
{}

//-------------------------------------------------------------------------------------------

ALSAStreamParser::~ALSAStreamParser()
{}

//-------------------------------------------------------------------------------------------
/*
iFi (by AMR) iFi (by AMR) HD USB Audio at usb-0000:00:14.0-8.1.2, high speed : USB Audio

Playback:
  Status: Stop
  Interface 1
    Altset 1
    Format: S32_LE
    Channels: 2
    Endpoint: 0x01 (1 OUT) (ASYNC)
    Rates: 44100, 48000, 88200, 96000, 176400, 192000, 352800, 384000, 705600, 768000
    Data packet interval: 125 us
    Bits: 32
    Channel map: FL FR
    Sync Endpoint: 0x81 (1 IN)
    Sync EP Interface: 1
    Sync EP Altset: 1
    Implicit Feedback Mode: No
  Interface 1
    Altset 2
    Format: SPECIAL DSD_U32_BE
    Channels: 2
    Endpoint: 0x01 (1 OUT) (ASYNC)
    Rates: 44100, 48000, 88200, 96000, 176400, 192000, 352800, 384000, 705600, 768000
    Data packet interval: 125 us
    Bits: 32
    DSD raw: DOP=0, bitrev=0
    Channel map: FL FR
    Sync Endpoint: 0x81 (1 IN)
    Sync EP Interface: 1
    Sync EP Altset: 2
    Implicit Feedback Mode: No
*/
//-------------------------------------------------------------------------------------------

bool ALSAStreamParser::parseContent(const QString& content)
{
	if(content.isEmpty())
	{
		return false;
	}
	
	QStringList lines = content.split('\n', Qt::SkipEmptyParts);
	
	// 1. Extract device name from first line
	// Example: "Cambridge Audio Evo 150 at usb-0000:00:14.0-8.1.3, high speed : USB Audio"
	if (!lines.isEmpty())
	{
		QString firstLine = lines.first().trimmed();
		int atIndex = firstLine.indexOf(" at ");
		if (atIndex > 0)
		{
			m_deviceName = firstLine.left(atIndex).trimmed();
		}
		else
		{
			m_deviceName = firstLine;
		}
	}
	
	int noBits = 0, state = 0;
	for(const QString& line : lines)
	{
		QString t = line.trimmed();
		
		if(t.startsWith("Interface"))
		{
			state = 1;
		}
		else if(t.startsWith("Format:") && state == 1 && t.contains("SPECIAL"))
		{
			state = 2;
		}
		else if(t.startsWith("Bits:"))
		{
			noBits = t.section(QChar(':'), 1).trimmed().toInt();
		}
		else if(t.startsWith("DSD raw:") && state == 2)
		{
			m_isDSDSpecial = true;
		
			QRegularExpression rx("DOP=(\\d+),\\s*bitrev=(\\d+)");
			QRegularExpressionMatch match = rx.match(t);
			if(match.hasMatch())
			{
				m_isDSDOverPCM = (match.captured(1).toInt()) ? true : false;
				m_isMSB = (match.captured(2).toInt()) ? false : true;
			}
			state = 0;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool ALSAStreamParser::parse(const QString& fileName)
{
	bool res = false;
	
	if(!common::DiskOps::exist(fileName))
	{
		return false;
	}
	
	QFile file(fileName);
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QByteArray fArray = file.readAll();
		QString content = QString::fromUtf8(fArray.constData(), fArray.size());
		res = parseContent(content);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool ALSAStreamParser::isDSDSpecial() const
{
	return m_isDSDSpecial;
}

//-------------------------------------------------------------------------------------------

bool ALSAStreamParser::isDSDOverPCM() const
{
	return m_isDSDOverPCM;
}

//-------------------------------------------------------------------------------------------

bool ALSAStreamParser::isMSB() const
{
	return m_isMSB;
}

//-------------------------------------------------------------------------------------------

bool ALSAStreamParser::isLSB() const
{
	return (m_isMSB) ? false : true;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------

