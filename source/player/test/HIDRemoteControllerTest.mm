#include "player/inc/HIDRemoteController.h"
#include "player/test/HIDRemoteControllerTest.h"

using namespace omega::player;
using namespace testing;

//-------------------------------------------------------------------------------------------

class HIDRemoteControllerAccess : public HIDRemoteController
{
    public:
        HIDRemoteControllerAccess();
        virtual ~HIDRemoteControllerAccess();
        void testOnTimerImplementation();
        void setCurrentPressed(HIDRemoteController::Button button);
        
        HIDRemoteController::Button testRemoteVolumeUp(bool isPressed);
        HIDRemoteController::Button testRemoteVolumeUpHold(bool isPressed);
        HIDRemoteController::Button testRemoteVolumeDown(bool isPressed);
        HIDRemoteController::Button testRemoteVolumeDownHold(bool isPressed);
        HIDRemoteController::Button testRemotePreviousTrack(bool isPressed);
        HIDRemoteController::Button testRemoteSeekBackHold(bool isPressed);
        HIDRemoteController::Button testRemoteNextTrack(bool isPressed);
        HIDRemoteController::Button testRemoteSeekForwardHold(bool isPressed);
        HIDRemoteController::Button testRemotePlay(bool isPressed);
        
        MOCK_METHOD0(startTimer,void());
        MOCK_METHOD0(stopTimer,void());
};

//-------------------------------------------------------------------------------------------

HIDRemoteControllerAccess::HIDRemoteControllerAccess() : HIDRemoteController()
{}

//-------------------------------------------------------------------------------------------

HIDRemoteControllerAccess::~HIDRemoteControllerAccess()
{}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerAccess::testOnTimerImplementation()
{
    onTimerImplementation();
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerAccess::setCurrentPressed(HIDRemoteController::Button button)
{
    m_currentPressed = button;
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemoteVolumeUp(bool isPressed)
{
    return remoteVolumeUp(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemoteVolumeUpHold(bool isPressed)
{
    return remoteVolumeUpHold(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemoteVolumeDown(bool isPressed)
{
    return remoteVolumeDown(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemoteVolumeDownHold(bool isPressed)
{
    return remoteVolumeDownHold(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemotePreviousTrack(bool isPressed)
{
    return remotePreviousTrack(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemoteSeekBackHold(bool isPressed)
{
    return remoteSeekBackHold(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemoteNextTrack(bool isPressed)
{
    return remoteNextTrack(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemoteSeekForwardHold(bool isPressed)
{
    return remoteSeekForwardHold(isPressed);
}

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteControllerAccess::testRemotePlay(bool isPressed)
{
    return remotePlay(isPressed);
}

//-------------------------------------------------------------------------------------------
// Tests
//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onTimerAndPlayPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(playPauseClick()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemotePlayPause);
    rCtrl->testOnTimerImplementation();
    QCOMPARE(spy.count(),0);
    delete rCtrl;    
}
//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onTimerAndVolumeUpPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(volumeUpHold()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteVolumeUp);
    rCtrl->testOnTimerImplementation();
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onTimerAndVolumeDownPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(volumeDownHold()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteVolumeDown);
    rCtrl->testOnTimerImplementation();
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onTimerAndFowardPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(seekForward()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteForward);
    rCtrl->testOnTimerImplementation();
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onTimerAndBackPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(seekBack()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteBack);
    rCtrl->testOnTimerImplementation();
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeUpIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QVERIFY(rCtrl->testRemoteVolumeUp(true)==HIDRemoteController::e_RemoteVolumeUp);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeUpIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(volumeUpIncrement()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteVolumeUp);
    QVERIFY(rCtrl->testRemoteVolumeUp(false)==HIDRemoteController::e_RemoteNone);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeUpHoldIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(volumeUpHold()));
    EXPECT_CALL(*rCtrl,startTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteVolumeUpHold(true)==HIDRemoteController::e_RemoteVolumeUp);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeUpHoldIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    EXPECT_CALL(*rCtrl,stopTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteVolumeUpHold(false)==HIDRemoteController::e_RemoteNone);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeDownIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QVERIFY(rCtrl->testRemoteVolumeDown(true)==HIDRemoteController::e_RemoteVolumeDown);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeDownIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(volumeDownIncrement()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteVolumeDown);
    QVERIFY(rCtrl->testRemoteVolumeDown(false)==HIDRemoteController::e_RemoteNone);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeDownHoldIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(volumeDownHold()));
    EXPECT_CALL(*rCtrl,startTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteVolumeDownHold(true)==HIDRemoteController::e_RemoteVolumeDown);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onVolumeDownHoldIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    EXPECT_CALL(*rCtrl,stopTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteVolumeDownHold(false)==HIDRemoteController::e_RemoteNone);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onPreviousIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QVERIFY(rCtrl->testRemotePreviousTrack(true)==HIDRemoteController::e_RemoteBack);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onPreviousIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(previousTrackClick()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteBack);
    QVERIFY(rCtrl->testRemotePreviousTrack(false)==HIDRemoteController::e_RemoteNone);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onSeekBackHoldIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(seekBack()));
    EXPECT_CALL(*rCtrl,startTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteSeekBackHold(true)==HIDRemoteController::e_RemoteBack);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onSeekBackHoldIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    EXPECT_CALL(*rCtrl,stopTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteSeekBackHold(false)==HIDRemoteController::e_RemoteNone);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onNextIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QVERIFY(rCtrl->testRemoteNextTrack(true)==HIDRemoteController::e_RemoteForward);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onNextIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(nextTrackClick()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemoteForward);
    QVERIFY(rCtrl->testRemoteNextTrack(false)==HIDRemoteController::e_RemoteNone);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onSeekForwardHoldIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(seekForward()));
    EXPECT_CALL(*rCtrl,startTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteSeekForwardHold(true)==HIDRemoteController::e_RemoteForward);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onSeekForwardHoldIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    EXPECT_CALL(*rCtrl,stopTimer()).Times(1);
    QVERIFY(rCtrl->testRemoteSeekForwardHold(false)==HIDRemoteController::e_RemoteNone);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onPlayIsPressed()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QVERIFY(rCtrl->testRemotePlay(true)==HIDRemoteController::e_RemotePlayPause);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::onPlayIsReleased()
{
    HIDRemoteControllerAccess *rCtrl = new HIDRemoteControllerAccess;
    QSignalSpy spy(rCtrl,SIGNAL(playPauseClick()));
    rCtrl->setCurrentPressed(HIDRemoteController::e_RemotePlayPause);
    QVERIFY(rCtrl->testRemotePlay(false)==HIDRemoteController::e_RemoteNone);
    QCOMPARE(spy.count(),1);
    delete rCtrl;
}

//-------------------------------------------------------------------------------------------

class HIDRemoteEventButtonTest : public HIDRemoteController
{
    public:
        MOCK_METHOD1(remoteVolumeUp,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remoteVolumeUpHold,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remoteVolumeDown,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remoteVolumeDownHold,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remotePreviousTrack,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remoteSeekBackHold,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remoteNextTrack,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remoteSeekForwardHold,HIDRemoteController::Button(bool isPressed));
        MOCK_METHOD1(remotePlay,HIDRemoteController::Button(bool isPressed));
        
        HIDRemoteController::Button getCurrentPressed();
};

//-------------------------------------------------------------------------------------------

HIDRemoteController::Button HIDRemoteEventButtonTest::getCurrentPressed()
{
    return m_currentPressed;
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonUpIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeUp(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeUp));
    ctrl.remoteEvent(kHIDRemoteButtonCodeUp,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeUp);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonUpIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeUp(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeUp));
    ctrl.remoteEvent(kHIDRemoteButtonCodeUp,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeUp);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonUpHoldIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeUpHold(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeUp));
    ctrl.remoteEvent(kHIDRemoteButtonCodeUpHold,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeUp);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonUpHoldIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeUpHold(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeUp));
    ctrl.remoteEvent(kHIDRemoteButtonCodeUpHold,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeUp);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonDownIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeDown(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeDown));
    ctrl.remoteEvent(kHIDRemoteButtonCodeDown,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeDown);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonDownIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeDown(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeDown));
    ctrl.remoteEvent(kHIDRemoteButtonCodeDown,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeDown);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonDownHoldIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeDownHold(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeDown));
    ctrl.remoteEvent(kHIDRemoteButtonCodeDownHold,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeDown);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonDownHoldIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteVolumeDownHold(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteVolumeDown));
    ctrl.remoteEvent(kHIDRemoteButtonCodeDownHold,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteVolumeDown);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonLeftIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remotePreviousTrack(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteBack));
    ctrl.remoteEvent(kHIDRemoteButtonCodeLeft,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteBack);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonLeftIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remotePreviousTrack(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteBack));
    ctrl.remoteEvent(kHIDRemoteButtonCodeLeft,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteBack);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonLeftHoldIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteSeekBackHold(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteBack));
    ctrl.remoteEvent(kHIDRemoteButtonCodeLeftHold,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteBack);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonLeftHoldIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteSeekBackHold(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteBack));
    ctrl.remoteEvent(kHIDRemoteButtonCodeLeftHold,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteBack);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonRightIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteNextTrack(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteForward));
    ctrl.remoteEvent(kHIDRemoteButtonCodeRight,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteForward);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonRightIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteNextTrack(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteForward));
    ctrl.remoteEvent(kHIDRemoteButtonCodeRight,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteForward);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonRightHoldIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteSeekForwardHold(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteForward));
    ctrl.remoteEvent(kHIDRemoteButtonCodeRightHold,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteForward);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonRightHoldIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remoteSeekForwardHold(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemoteForward));
    ctrl.remoteEvent(kHIDRemoteButtonCodeRightHold,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemoteForward);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonCenterIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remotePlay(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemotePlayPause));
    ctrl.remoteEvent(kHIDRemoteButtonCodeCenter,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemotePlayPause);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonCenterIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remotePlay(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemotePlayPause));
    ctrl.remoteEvent(kHIDRemoteButtonCodeCenter,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemotePlayPause);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonPlayIsPressed()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remotePlay(true)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemotePlayPause));
    ctrl.remoteEvent(kHIDRemoteButtonCodePlay,true);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemotePlayPause);
}

//-------------------------------------------------------------------------------------------

void HIDRemoteControllerTest::remoteEventButtonPlayIsReleased()
{
    HIDRemoteEventButtonTest ctrl;
    EXPECT_CALL(ctrl,remotePlay(false)).Times(1)
        .WillOnce(Return(HIDRemoteController::e_RemotePlayPause));
    ctrl.remoteEvent(kHIDRemoteButtonCodePlay,false);
    QVERIFY(ctrl.getCurrentPressed()==HIDRemoteController::e_RemotePlayPause);
}

//-------------------------------------------------------------------------------------------

TEST(HIDRemoteController,runQt)
{
    HIDRemoteControllerTest remoteTest;
    EXPECT_TRUE(QTest::qExec(&remoteTest)==0);
}

//-------------------------------------------------------------------------------------------
