//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_ALSASTREAMPARSER_H
#define __OMEGA_AUDIOIO_ALSASTREAMPARSER_H
//-------------------------------------------------------------------------------------------

#include <QFile>
#include <QString>
#include <QList>
#include <QRegularExpression>
#include <QSharedPointer>

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
		int noBits() const;
		
		static QList<QSharedPointer<ALSAStreamParser> > parseProcForALSAStreams();
		
	private:
		QString m_deviceName;
		bool m_isDSDSpecial;
		bool m_isDSDOverPCM;
		bool m_isMSB;
		int m_noBits;
		
		bool parseContent(const QString& content);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
