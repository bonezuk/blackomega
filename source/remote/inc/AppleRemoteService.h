//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_APPLEREMOTESERVICE_H
#define __OMEGA_REMOTE_APPLEREMOTESERVICE_H
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

#include "remote/inc/RemoteIF.h"
#include "remote/inc/KeyStateParser.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFPlugIn.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/hid/IOHIDKeys.h>
#import <IOKit/hid/IOHIDLib.h>
#import <IOKit/hid/IOHIDManager.h>

#include <QSharedPointer>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

//-------------------------------------------------------------------------------------------

@interface AppleRemoteServiceProxy : NSObject
{}
- (void) remoteControlAvailable:(NSNotification *)notification;
- (void) processQueueCallbackWithResult:(IOReturn) result;
- (void) doTimer;
- (void) dealWithSecureEventInputChange;
@end

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT AppleRemoteEvent
{
    public:
        
        typedef enum
        {
            e_openEvent,
            e_endEvent,
            e_unknownEvent
        } Type;

    public:
        AppleRemoteEvent();
        AppleRemoteEvent(Type t);
        AppleRemoteEvent(const AppleRemoteEvent& rhs);
        virtual ~AppleRemoteEvent();
        
        const AppleRemoteEvent& operator = (const AppleRemoteEvent& rhs);
    
        virtual Type type() const;
        
    protected:
    
        Type m_type;
        
        virtual void copy(const AppleRemoteEvent& rhs);
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT AppleRemoteService : public QThread
{
    public:
    
        typedef enum
        {
            e_remoteButtonCentre = 1,
            e_remoteButtonCentreHold,
            e_remoteButtonPlusPress,
            e_remoteButtonPlusRelease,
            e_remoteButtonMinusPress,
            e_remoteButtonMinusRelease,
            e_remoteButtonLeftTouch,
            e_remoteButtonLeftPress,
            e_remoteButtonLeftRelease,
            e_remoteButtonRightTouch,
            e_remoteButtonRightPress,
            e_remoteButtonRightRelease,
            e_remoteButtonPlay,
            e_remoteButtonPlayHold,
            e_remoteButtonMenu,
            e_remoteButtonMenuHold,
            e_remoteButtonUnknown
        } Button;
    
    public:
        AppleRemoteService(QObject *parent = 0);
        virtual ~AppleRemoteService();
        
        static QSharedPointer<AppleRemoteService> instance(RemoteIF *pRemoteIF);
        static QSharedPointer<AppleRemoteService> instance();
        static void release();
        
        static bool isRemoteAvailable();
        
        virtual void remoteControlAvailable();
        virtual void processQueueCallback(IOReturn result);
        virtual void doTimer();
        virtual void dealWithSecureEventInputChange();
        
    protected:
        
        static QSharedPointer<AppleRemoteService> m_instance;
    
        AppleRemoteServiceProxy *m_proxy;
        IOHIDDeviceInterface **m_hidDeviceInterface;
        IOHIDQueueInterface **m_queue;
        QVector<IOHIDElementCookie> m_cookieArray;
        
        CFRunLoopSourceRef m_eventSource;
        
        QMap<int,Button> m_states;
        KeyStateParser m_parser;
        
        RemoteIF *m_pRemoteIF;
        
        QWaitCondition m_condition;
        QMutex m_mutex;
        
        CFRunLoopTimerRef m_timer;
        
        QList<AppleRemoteEvent> m_eventQueue;
    
        bool m_lastSecureEventInputState;
        io_object_t m_eventSecureInputNotification;
        IONotificationPortRef m_notifyPort;
    
        volatile bool m_serviceIsRunning;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual bool runService(RemoteIF *pRemoteIF);
        virtual void endService();
        virtual void run();
        
        virtual void getCondition();
        virtual void waitOnCondition();
        virtual void wakeCondition();
        
        virtual AppleRemoteEvent getEvent();
        virtual void queueEvent(const AppleRemoteEvent& e);
        
        virtual bool createTimer();
        virtual void freeTimer();
        
        virtual void processOpenDevice();
        virtual void processEnd();
        
        static io_object_t findRemoteAppleIRDevice();
        
        virtual void sendDistributedNotification(const QString& notificationName,const QString& targetIdentifier);
        virtual void sendFinishedNotificationForAppIdentifier(const QString& identifier);
        virtual void sendRequestForRemoteControlNotification();
        virtual void removeNotificationObserver();
        
        virtual bool openRemoteControlDevice();
        virtual void closeRemoteControlDevice(bool shallSendNotifications);
        virtual IOHIDDeviceInterface **createInterfaceForDevice(io_object_t hidDevice);
        virtual bool initializeCookies(QVector<IOHIDElementCookie>& cookieArray);
        virtual int openDevice();
        
        virtual void processQueue(const QVector<int>& cookieList,const QVector<int>& indexList);
        virtual void processButton(Button button,int indexState);
        virtual void printButton(Button button);
        
        virtual void defineIRParser();
        
        virtual bool openEventSecureInput();
        virtual void closeEventSecureInput();
        virtual bool retrieveSecureEventInputState();
        virtual bool isListeningToRemote();
        
        virtual const KeyStateParser& getKeyParser() const;
        virtual const QMap<int,Button>& getButtonStatesMap() const;
        virtual RemoteIF *getRemoteIF() const;
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT AppleIRRemoteServiceEvent : public RemoteEvent
{
    public:
        AppleIRRemoteServiceEvent(AppleRemoteService::Button button);
        virtual ~AppleIRRemoteServiceEvent();
        
        const AppleRemoteService::Button& button() const;
        
    protected:
        
        AppleRemoteService::Button m_button;
};


//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
