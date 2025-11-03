#include "gtest/gtest.h"

#include "audioio/inc/AudioSettings.h"

using namespace omega::audioio;

//-------------------------------------------------------------------------------------------

void removeAudioSettings(const QString& devName)
{
    QString groupName = "audio" + devName;
    QSettings settings;
    settings.remove(groupName);
}

//-------------------------------------------------------------------------------------------

TEST(AudioSettings, isExclusiveOffByDefault)
{
    QString devName = "test_audio_device";
    removeAudioSettings(devName);
    QSharedPointer<AudioSettings> pSettings = AudioSettings::instance(devName);
    ASSERT_FALSE(pSettings.isNull());
    EXPECT_FALSE(pSettings->isExclusive());
}

//-------------------------------------------------------------------------------------------

TEST(AudioSettings, isExclusiveOnWhenSet)
{
    QString devName = "test_audio_device";
    removeAudioSettings(devName);
    QSharedPointer<AudioSettings> pSettings = AudioSettings::instance(devName);
    ASSERT_FALSE(pSettings.isNull());
    EXPECT_FALSE(pSettings->isExclusive());
    pSettings->setExclusive(true);
    EXPECT_TRUE(pSettings->isExclusive());
}

//-------------------------------------------------------------------------------------------
