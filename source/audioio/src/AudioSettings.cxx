#include "audioio/inc/AudioSettings.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

QMap<QString, QSharedPointer<AudioSettings> > AudioSettings::m_instances;

//-------------------------------------------------------------------------------------------

AudioSettings::AudioSettings(const QString& devName) : m_devName(devName)
{}

//-------------------------------------------------------------------------------------------

AudioSettings::~AudioSettings()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<AudioSettings> AudioSettings::instance(const QString& devName)
{
    QMap<QString, QSharedPointer<AudioSettings> >::iterator ppI;
    QSharedPointer<AudioSettings> pSettings;

    ppI = m_instances.find(devName);
    if(ppI != m_instances.end())
    {
        pSettings = ppI.value();
    }
    else
    {
        QSharedPointer<AudioSettings> pNewS(new AudioSettings(devName));
        m_instances.insert(devName, pNewS);
        pSettings = pNewS;
    }
    return pSettings;
}

//-------------------------------------------------------------------------------------------

const QString& AudioSettings::deviceName() const
{
    return m_devName;
}

//-------------------------------------------------------------------------------------------

QString AudioSettings::groupName() const
{
    QString name = "audio" + m_devName;
    return name;
}

//-------------------------------------------------------------------------------------------

bool AudioSettings::isExclusive() const
{
    bool flag = false;
    QSettings settings;
    settings.beginGroup(groupName());
    if(settings.contains("exclusive"))
    {
        flag = settings.value("exclusive", QVariant(false)).toBool();
    }
    settings.endGroup();
    return flag;
}

//-------------------------------------------------------------------------------------------

void AudioSettings::setExclusive(bool isExcl)
{
    QSettings settings;
    settings.beginGroup(groupName());
    settings.setValue("exclusive", QVariant(isExcl));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
