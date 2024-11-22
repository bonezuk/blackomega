//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AUDIOSETTINGS_H
#define __OMEGA_AUDIOIO_AUDIOSETTINGS_H
//-------------------------------------------------------------------------------------------

#include <QSettings>
#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "audioio/inc/AudioIODLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AudioSettings
{
	public:
		virtual ~AudioSettings();
		
		static QSharedPointer<AudioSettings> instance(const QString& devName);
		
		const QString& deviceName() const;
		
		bool isExclusive() const;
		void setExclusive(bool isExcl);
		
	private:
		static QMap<QString, QSharedPointer<AudioSettings> > m_instances;
		
		QString m_devName;
		
		AudioSettings(const QString& devName);
		QString groupName() const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
