//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_ALSASTREAMPARSER_H
#define __OMEGA_AUDIOIO_ALSASTREAMPARSER_H
//-------------------------------------------------------------------------------------------

#include <QFile>
#include <QString>
#include <QRegularExpression>

#include "audioio/inc/AudioIODLL.h"
#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT ALSAStreamParser
{
	public:
		ALSAStreamParser();
		virtual ~ALSAStreamParser();
		
		bool parse(const QString& fileName);
		
		const QString& deviceName() const;
		bool isDSDSpecial() const;
		bool isDSDOverPCM() const;
		bool isMSB() const;
		bool isLSB() const;
	
	private:
		QString m_deviceName;
		bool m_isDSDSpecial;
		bool m_isDSDOverPCM;
		bool m_isMSB;
		
		bool ALSAStreamParser::parseContent(const QString& content);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
