//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_TEST_HIDREMOTECONTROLLERTEST_H
#define __OMEGA_PLAYER_TEST_HIDREMOTECONTROLLERTEST_H
//-------------------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>

//-------------------------------------------------------------------------------------------

class HIDRemoteControllerTest : public QObject
{
    Q_OBJECT

    private slots:
        void onTimerAndPlayPressed();
        void onTimerAndVolumeUpPressed();
        void onTimerAndVolumeDownPressed();
        void onTimerAndFowardPressed();
        void onTimerAndBackPressed();

        void onVolumeUpIsPressed();
        void onVolumeUpIsReleased();
        void onVolumeUpHoldIsPressed();
        void onVolumeUpHoldIsReleased();
        void onVolumeDownIsPressed();
        void onVolumeDownIsReleased();
        void onVolumeDownHoldIsPressed();
        void onVolumeDownHoldIsReleased();

        void onPreviousIsPressed();
        void onPreviousIsReleased();
        void onSeekBackHoldIsPressed();
        void onSeekBackHoldIsReleased();
        void onNextIsPressed();
        void onNextIsReleased();
        void onSeekForwardHoldIsPressed();
        void onSeekForwardHoldIsReleased();

        void onPlayIsPressed();
        void onPlayIsReleased();

        void remoteEventButtonUpIsPressed();
        void remoteEventButtonUpIsReleased();
        void remoteEventButtonUpHoldIsPressed();
        void remoteEventButtonUpHoldIsReleased();
        void remoteEventButtonDownIsPressed();
        void remoteEventButtonDownIsReleased();
        void remoteEventButtonDownHoldIsPressed();
        void remoteEventButtonDownHoldIsReleased();
        void remoteEventButtonLeftIsPressed();
        void remoteEventButtonLeftIsReleased();
        void remoteEventButtonLeftHoldIsPressed();
        void remoteEventButtonLeftHoldIsReleased();
        void remoteEventButtonRightIsPressed();
        void remoteEventButtonRightIsReleased();
        void remoteEventButtonRightHoldIsPressed();
        void remoteEventButtonRightHoldIsReleased();
        void remoteEventButtonCenterIsPressed();
        void remoteEventButtonCenterIsReleased();
        void remoteEventButtonPlayIsPressed();
        void remoteEventButtonPlayIsReleased();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
