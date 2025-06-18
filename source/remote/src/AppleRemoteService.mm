#include "remote/inc/AppleRemoteService.h"
#include "remote/inc/AppleIRRemoteController.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)

#ifndef kIOMainPortDefault
#define kIOMainPortDefault kIOMasterPortDefault
#endif

//-------------------------------------------------------------------------------------------
// AppleRemoteServiceProxy
//-------------------------------------------------------------------------------------------
@implementation AppleRemoteServiceProxy
//-------------------------------------------------------------------------------------------

- (id) init
{
    self = [super init];
    return self;
}

//-------------------------------------------------------------------------------------------

- (void) dealloc
{
    [super dealloc];
}

//-------------------------------------------------------------------------------------------

- (void) remoteControlAvailable:(NSNotification *)notification
{
    if(!omega::remote::AppleRemoteService::instance().isNull())
    {
        omega::remote::AppleRemoteService::instance()->remoteControlAvailable();
    }
}

//-------------------------------------------------------------------------------------------

- (void) processQueueCallbackWithResult:(IOReturn) result
{
    if(!omega::remote::AppleRemoteService::instance().isNull())
    {
        omega::remote::AppleRemoteService::instance()->processQueueCallback(result);
    }
}

//-------------------------------------------------------------------------------------------

- (void) doTimer
{
    if(!omega::remote::AppleRemoteService::instance().isNull())
    {
        omega::remote::AppleRemoteService::instance()->doTimer();
    }
}

//-------------------------------------------------------------------------------------------

- (void) dealWithSecureEventInputChange
{
    if(!omega::remote::AppleRemoteService::instance().isNull())
    {
        omega::remote::AppleRemoteService::instance()->dealWithSecureEventInputChange();
    }
}

//-------------------------------------------------------------------------------------------
@end
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

NSString *kRemoteControlDeviceName = @"RemoteControlDeviceName";
NSString *kApplicationIdentifier   = @"CFBundleIdentifier";
NSString *kTargetApplicationIdentifier = @"TargetBundleIdentifier";

NSString *REQUEST_FOR_REMOTE_CONTROL_NOTIFCATION     = @"mac.remotecontrols.RequestForRemoteControl";
NSString *FINISHED_USING_REMOTE_CONTROL_NOTIFICATION = @"mac.remotecontrols.FinishedUsingRemoteControl";

//-------------------------------------------------------------------------------------------

static void QueueCallbackFunction(void* target,  IOReturn result, void* refcon, void* sender)
{
    if (target!=0)
    {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        AppleRemoteServiceProxy *proxy = (AppleRemoteServiceProxy *)target;
        if(proxy!=nil)
        {
            [proxy processQueueCallbackWithResult:result];
        }
        [pool release];
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteServiceTimerCallback(CFRunLoopTimerRef timer, void *info)
{
    AppleRemoteServiceProxy *proxy = (AppleRemoteServiceProxy *)info;
    if(proxy!=nil)
    {
        [proxy doTimer];
    }
}

//-------------------------------------------------------------------------------------------

static void IOREInterestCallback(void *refcon,io_service_t service,uint32_t messageType, void *messageArgument)
{
    AppleRemoteServiceProxy *proxy = (AppleRemoteServiceProxy *)refcon;
    if(proxy!=nil)
    {
        [proxy dealWithSecureEventInputChange];
    }
}

//-------------------------------------------------------------------------------------------
// AppleRemoteEvent
//-------------------------------------------------------------------------------------------

AppleRemoteEvent::AppleRemoteEvent() : m_type(AppleRemoteEvent::e_unknownEvent)
{}

//-------------------------------------------------------------------------------------------

AppleRemoteEvent::AppleRemoteEvent(Type t) : m_type(t)
{}

//-------------------------------------------------------------------------------------------

AppleRemoteEvent::AppleRemoteEvent(const AppleRemoteEvent& rhs)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

AppleRemoteEvent::~AppleRemoteEvent()
{}

//-------------------------------------------------------------------------------------------

const AppleRemoteEvent& AppleRemoteEvent::operator = (const AppleRemoteEvent& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

AppleRemoteEvent::Type AppleRemoteEvent::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteEvent::copy(const AppleRemoteEvent& rhs)
{
    m_type = rhs.m_type;
}

//-------------------------------------------------------------------------------------------
// AppleRemoteService
//-------------------------------------------------------------------------------------------

QSharedPointer<AppleRemoteService> AppleRemoteService::m_instance;

//-------------------------------------------------------------------------------------------

AppleRemoteService::AppleRemoteService(QObject *parent) : QThread(parent),
    m_proxy(0),
    m_hidDeviceInterface(0),
    m_queue(0),
    m_cookieArray(),
    m_eventSource(0),
    m_states(),
    m_parser(),
    m_pRemoteIF(0),
    m_condition(),
    m_mutex(),
    m_timer(0),
    m_eventQueue(),
    m_lastSecureEventInputState(false),
    m_eventSecureInputNotification(MACH_PORT_NULL),
    m_notifyPort(0),
    m_serviceIsRunning(false)
{
    m_proxy = [[AppleRemoteServiceProxy alloc] init];
}

//-------------------------------------------------------------------------------------------

AppleRemoteService::~AppleRemoteService()
{
    [m_proxy dealloc];
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "AppleRemoteService::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AppleRemoteService> AppleRemoteService::instance(RemoteIF *pRemoteIF)
{
    if(m_instance.isNull())
    {
        QSharedPointer<AppleRemoteService> pService(new AppleRemoteService);
        if(pService->runService(pRemoteIF))
        {
            m_instance = pService;
        }
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AppleRemoteService> AppleRemoteService::instance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::release()
{
    if(!m_instance.isNull())
    {
        QSharedPointer<AppleRemoteService> pEmpty;
        m_instance->endService();
        m_instance = pEmpty;
    }
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::runService(RemoteIF *pRemoteIF)
{
    m_pRemoteIF = pRemoteIF;

    if(!m_serviceIsRunning)
    {
        defineIRParser();
        getCondition();
        start();
        waitOnCondition();
    }
    return m_serviceIsRunning;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::endService()
{
    if(isRunning())
    {
        AppleRemoteEvent e(AppleRemoteEvent::e_endEvent);
        queueEvent(e);
        wait();
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::getCondition()
{
    m_mutex.lock();
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::waitOnCondition()
{
    m_condition.wait(&m_mutex);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::wakeCondition()
{
    m_mutex.lock();
    m_condition.wakeAll();
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

AppleRemoteEvent AppleRemoteService::getEvent()
{
    AppleRemoteEvent e;

    m_mutex.lock();
    if(!m_eventQueue.isEmpty())
    {
        e = m_eventQueue.takeFirst();
    }
    m_mutex.unlock();
    return e;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::queueEvent(const AppleRemoteEvent& e)
{
    m_mutex.lock();
    m_eventQueue.append(e);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::run()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    if(openEventSecureInput())
    {
        if(createTimer())
        {
            if(openRemoteControlDevice())
            {
                m_serviceIsRunning = true;
                wakeCondition();
                CFRunLoopRun();
            }
            else
            {
                m_serviceIsRunning = false;
                wakeCondition();
            }
        }
        else
        {
            m_serviceIsRunning = false;
            wakeCondition();
        }
    }
    processEnd();
    m_serviceIsRunning = false;
    [pool release];
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::createTimer()
{
    CFRunLoopTimerContext ctx;
    ctx.version = 0;
    ctx.info = m_proxy;
    ctx.retain = 0;
    ctx.release = 0;
    ctx.copyDescription = 0;

    m_timer = CFRunLoopTimerCreate(kCFAllocatorDefault,
                                   CFAbsoluteTimeGetCurrent() + 0.1,
                                   0.1, 0, 0, AppleRemoteServiceTimerCallback, &ctx);
    if(m_timer!=0)
    {
        CFRunLoopAddTimer(CFRunLoopGetCurrent(),m_timer,kCFRunLoopDefaultMode);
    }
    return (m_timer!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::freeTimer()
{
    if(m_timer!=0)
    {
        CFRunLoopRemoveTimer(CFRunLoopGetCurrent(),m_timer,kCFRunLoopDefaultMode);
        CFRelease(m_timer);
        m_timer = 0;
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::doTimer()
{
    bool loop = true;

    while(loop)
    {
        AppleRemoteEvent e = getEvent();

        switch(e.type())
        {
            case AppleRemoteEvent::e_openEvent:
                processOpenDevice();
                break;

            case AppleRemoteEvent::e_endEvent:
                processEnd();
                break;

            case AppleRemoteEvent::e_unknownEvent:
            default:
                loop = false;
                break;
        }
    }
    AppleIRRemoteController::onTimer(m_pRemoteIF);
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::processOpenDevice()
{
    removeNotificationObserver();

    if(!openRemoteControlDevice())
    {
        removeNotificationObserver();
        processEnd();
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::processEnd()
{
    closeRemoteControlDevice(true);
    removeNotificationObserver();
    freeTimer();
    closeEventSecureInput();
}

//-------------------------------------------------------------------------------------------

io_object_t AppleRemoteService::findRemoteAppleIRDevice()
{
    const char *c_AppleIRDeviceName = "AppleIRController";

    io_object_t    hidDevice = 0;
    io_iterator_t hidObjectIterator = 0;
    IOReturn ioRes;
    CFMutableDictionaryRef hidMatchDictionary;

    hidMatchDictionary = IOServiceMatching(c_AppleIRDeviceName);
    ioRes = IOServiceGetMatchingServices(kIOMainPortDefault,hidMatchDictionary,&hidObjectIterator);
    if(ioRes==kIOReturnSuccess && hidObjectIterator!=0)
    {
        io_object_t matchingService = 0,foundService = 0;
        bool finalMatch = false;

        while(matchingService = IOIteratorNext(hidObjectIterator), matchingService!=0)
        {
            if(!finalMatch)
            {
                CFTypeRef className;

                if(!foundService)
                {
                    if(IOObjectRetain(matchingService)==kIOReturnSuccess)
                    {
                        foundService = matchingService;
                    }
                }

                className = IORegistryEntryCreateCFProperty((io_registry_entry_t)matchingService, CFSTR("IOClass"), kCFAllocatorDefault, 0);
                if (className!=0)
                {
                    NSString *cName = (NSString *)className;

                    if([cName isEqual:[NSString stringWithUTF8String:(c_AppleIRDeviceName)]])
                    {
                        if(foundService)
                        {
                            IOObjectRelease(foundService);
                            foundService = 0;
                        }
                        if(IOObjectRetain(matchingService)==kIOReturnSuccess)
                        {
                            foundService = matchingService;
                            finalMatch = true;
                        }
                    }
                    CFRelease(className);
                }
            }
            IOObjectRelease(matchingService);
        }
        hidDevice = foundService;
        IOObjectRelease(hidObjectIterator);
    }
    return hidDevice;
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::isRemoteAvailable()
{
    bool res;
    io_object_t hidDevice = findRemoteAppleIRDevice();

    if(hidDevice!=0)
    {
        IOObjectRelease(hidDevice);
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::sendDistributedNotification(const QString& notificationName,const QString& targetIdentifier)
{
    NSString *nName = nil;
    NSString *tName = nil;

    if(!notificationName.isEmpty())
    {
        nName = [NSString stringWithUTF8String:(notificationName.toUtf8().constData())];
    }
    if(!targetIdentifier.isEmpty())
    {
        tName = [NSString stringWithUTF8String:(targetIdentifier.toUtf8().constData())];
    }

    NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys: @"AppleIRController",
        kRemoteControlDeviceName, [[NSBundle mainBundle] bundleIdentifier],
        kApplicationIdentifier, tName,
        kTargetApplicationIdentifier, nil];

    [[NSDistributedNotificationCenter defaultCenter] postNotificationName:nName object:nil userInfo:userInfo deliverImmediately:YES];
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::sendFinishedNotificationForAppIdentifier(const QString& identifier)
{
    QString finishStr = QString::fromUtf8([FINISHED_USING_REMOTE_CONTROL_NOTIFICATION UTF8String]);
    sendDistributedNotification(finishStr,identifier);
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::sendRequestForRemoteControlNotification()
{
    QString requestStr = QString::fromUtf8([REQUEST_FOR_REMOTE_CONTROL_NOTIFCATION UTF8String]);
    sendDistributedNotification(requestStr,"");
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::removeNotificationObserver()
{
    NSDistributedNotificationCenter *defaultCenter = [NSDistributedNotificationCenter defaultCenter];
    [defaultCenter removeObserver:m_proxy name:FINISHED_USING_REMOTE_CONTROL_NOTIFICATION object:nil];
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::remoteControlAvailable()
{
    AppleRemoteEvent e(AppleRemoteEvent::e_openEvent);
    queueEvent(e);
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::openRemoteControlDevice()
{
    bool res = false;
    io_object_t hidDevice = findRemoteAppleIRDevice();

    if(hidDevice!=0)
    {
        m_hidDeviceInterface = createInterfaceForDevice(hidDevice);
        if(m_hidDeviceInterface!=0)
        {
            if(initializeCookies(m_cookieArray))
            {
                int openRes = openDevice();

                if(openRes>=0)
                {
                    if(openRes==0)
                    {
                        closeRemoteControlDevice(true);
                    }
                    res = true;
                }
                else
                {
                    printError("openRemoteControlDevice","Failed to open Apple IR remote device");
                }
            }
            else
            {
                printError("openRemoteControlDevice","Error initializing cookies for Apple IR remote");
            }
        }
        else
        {
            printError("openRemoteControlDevice","Failed to create interface for Apple IR remote hardware");
        }
        IOObjectRelease(hidDevice);
    }
    else
    {
        printError("openRemoteControlDevice","No Apple IR remote hardware found");
    }
    if(!res)
    {
        closeRemoteControlDevice(true);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::closeRemoteControlDevice(bool shallSendNotifications)
{
    bool sendNotification = false;

    if(m_eventSource!=0)
    {
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(),m_eventSource,kCFRunLoopDefaultMode);
        CFRelease(m_eventSource);
        m_eventSource = 0;
    }

    if(m_queue!=0)
    {
        (*m_queue)->stop(m_queue);
        (*m_queue)->dispose(m_queue);
        (*m_queue)->Release(m_queue);
        m_queue = 0;
        sendNotification = true;
    }

    m_cookieArray.clear();

    if(m_hidDeviceInterface!=0)
    {
        (*m_hidDeviceInterface)->close(m_hidDeviceInterface);
        (*m_hidDeviceInterface)->Release(m_hidDeviceInterface);
        m_hidDeviceInterface = 0;
    }

    if(shallSendNotifications && sendNotification)
    {
        sendFinishedNotificationForAppIdentifier("");
    }
}

//-------------------------------------------------------------------------------------------

IOHIDDeviceInterface **AppleRemoteService::createInterfaceForDevice(io_object_t hidDevice)
{
    io_name_t className;
    IOReturn res;
    IOHIDDeviceInterface **hidDeviceInterface = 0;

    res = IOObjectGetClass(hidDevice,className);
    if(res==kIOReturnSuccess)
    {
        IOCFPlugInInterface **plugInInterface = 0;
        SInt32 score = 0;

        res = IOCreatePlugInInterfaceForService(hidDevice, kIOHIDDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
        if(res==kIOReturnSuccess)
        {
            HRESULT plugInResult = S_OK;

            plugInResult = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID), (LPVOID *)&hidDeviceInterface);
            if(plugInResult!=S_OK)
            {
                printError("createInterfaceForDevice","Failed to query plugin interface for device");
                hidDeviceInterface = 0;
            }
        }
        else
        {
            printError("createInterfaceForDevice","Error getting interface for hardware service");
        }
    }
    else
    {
        printError("createInterfaceForDevice","Failed to get class for given hardware device");
    }
    return hidDeviceInterface;
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::initializeCookies(QVector<IOHIDElementCookie>& cookieArray)
{
    IOHIDDeviceInterface122 **handle = (IOHIDDeviceInterface122 **)m_hidDeviceInterface;
    bool res = false;

    cookieArray.clear();
    if(handle!=0 && (*handle)!=0)
    {
        CFArrayRef elements = nil;
        IOReturn success;

        success = (*handle)->copyMatchingElements(handle,0,&elements);
        if(success==kIOReturnSuccess && elements!=nil)
        {
            NSDictionary *element;
            NSEnumerator *elementsEnumerator = [(NSArray *)elements objectEnumerator];
            bool loop = true;

            while(loop)
            {
                element = [elementsEnumerator nextObject];
                if(element!=nil)
                {
                    IOHIDElementCookie cookie;
                    id object;

                    // Get cookie
                    object = [element valueForKey: @kIOHIDElementCookieKey ];
                    if(object!=nil && [object isKindOfClass:[NSNumber class]])
                    {
                        cookie = (IOHIDElementCookie)[object longValue];

                        // Get usage
                        object = [element valueForKey: @kIOHIDElementUsageKey ];
                        if(object!=nil && [object isKindOfClass:[NSNumber class]])
                        {
                            // Get usage page
                            object = [element valueForKey: @kIOHIDElementUsagePageKey ];
                            if(object!=nil && [object isKindOfClass:[NSNumber class]])
                            {
                                cookieArray.append(cookie);
                            }
                        }
                    }
                }
                else
                {
                    loop = false;
                }
            }

            CFRelease(elements);
            res = true;
        }
        else
        {
            printError("initializeCookies","Failed to get cookie information from interface");
        }
    }
    else
    {
        printError("initializeCookies","No hardware interface defined to get cookies from");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int AppleRemoteService::openDevice()
{
    IOReturn ioResult;
    IOHIDOptionsType openMode = kIOHIDOptionsTypeSeizeDevice;
    int res = -1;

    ioResult = (*m_hidDeviceInterface)->open(m_hidDeviceInterface,openMode);
    if(ioResult==KERN_SUCCESS)
    {
        m_queue = (*m_hidDeviceInterface)->allocQueue(m_hidDeviceInterface);
        if(m_queue!=0)
        {
            HRESULT result;

            result = (*m_queue)->create(m_queue,0,12);
            if(result==kIOReturnSuccess)
            {
                QVector<tuint32>::iterator ppI;

                for(ppI=m_cookieArray.begin();ppI!=m_cookieArray.end();ppI++)
                {
                    (*m_queue)->addElement(m_queue,*ppI,0);
                }

                ioResult = (*m_queue)->createAsyncEventSource(m_queue,&m_eventSource);
                if(ioResult==KERN_SUCCESS)
                {
                    ioResult = (*m_queue)->setEventCallout(m_queue,QueueCallbackFunction,m_proxy,0);
                    if(ioResult==KERN_SUCCESS)
                    {
                        CFRunLoopAddSource(CFRunLoopGetCurrent(), m_eventSource, kCFRunLoopDefaultMode);
                        (*m_queue)->start(m_queue);
                        res = 1;
                    }
                    else
                    {
                        printError("openDevice","Error when setting event callback");
                    }
                }
                else
                {
                    printError("openDevice","Error when creating async event source");
                }
            }
            else
            {
                printError("openDevice","Error when creating queue");
            }
        }
        else
        {
            printError("openDevice","Error when opening device");
        }
    }
    else if(ioResult==kIOReturnExclusiveAccess)
    {
        NSDistributedNotificationCenter *defaultCenter = [NSDistributedNotificationCenter defaultCenter];
        [defaultCenter addObserver:m_proxy selector:@selector(remoteControlAvailable:) name:FINISHED_USING_REMOTE_CONTROL_NOTIFICATION object:nil];

        sendRequestForRemoteControlNotification();

        res = 0;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::processQueueCallback(IOReturn result)
{
    IOHIDEventStruct event;
    AbsoluteTime zeroTime = {0,0};
    QVector<int> cookieList,indexList;

    while(result==kIOReturnSuccess)
    {
        result = (*m_queue)->getNextEvent(m_queue,&event,zeroTime,0);
        if(result==kIOReturnSuccess)
        {
            int cookie = (int)event.elementCookie;
            int index = (int)event.value;

            if(((int)event.elementCookie)!=5)
            {
                cookieList.append(cookie);
                indexList.append(index);
            }
        }
    }
    processQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::defineIRParser()
{
    const int c_remoteButtonCentre[]       = { 33, 21, 20,  3,  2, 33, 21, 20,  3, 2 }; // 10
    const int c_remoteButtonCentreHold[]   = { 33, 21, 20, 11,  2, 33, 21, 20, 11, 2 }; // 10
    const int c_remoteButtonPlus[]         = { 33, 31, 30, 21, 20,  2 }; // 6
    const int c_remoteButtonMinus[]        = { 33, 32, 30, 21, 20,  2 }; // 6
    const int c_remoteButtonLeftTouch[]    = { 33, 25, 21, 20,  2, 33, 25, 21, 20, 2 }; // 10
    const int c_remoteButtonLeft[]         = { 33, 21, 20, 13, 12,  2 }; // 6
    const int c_remoteButtonRightTouch[]   = { 33, 24, 21, 20,  2, 33, 24, 21, 20, 2 }; // 10
    const int c_remoteButtonRight[]        = { 33, 21, 20, 14, 12,  2 }; // 6
    const int c_remoteButtonPlay[]         = { 33, 21, 20,  8,  2, 33, 21, 20,  8, 2 }; // 10
    const int c_remoteButtonPlayHold[]     = { 37, 33, 21, 20,  2, 37, 33, 21, 20, 2 }; // 10
    const int c_remoteButtonMenu[]         = { 33, 22, 21, 20,  2, 33, 22, 21, 20, 2 }; // 10
    const int c_remoteButtonMenuHold[]     = { 33, 21, 20,  2, 33, 21, 20,  2 }; // 8
    const int c_remoteButtonPlaySecond[]   = { 33, 23, 21, 20,  2, 33, 23, 21, 20, 2 }; // 10
    const int c_remoteControlSwitched[]    = { 19 }; // 1

    m_states.clear();
    m_states.insert(m_parser.sequence(c_remoteButtonCentre,10),e_remoteButtonCentre);
    m_states.insert(m_parser.sequence(c_remoteButtonCentreHold,10),e_remoteButtonCentreHold);
    m_states.insert(m_parser.sequence(c_remoteButtonPlus,6),e_remoteButtonPlusPress);
    m_states.insert(m_parser.sequence(c_remoteButtonMinus,6),e_remoteButtonMinusPress);
    m_states.insert(m_parser.sequence(c_remoteButtonLeftTouch,10),e_remoteButtonLeftTouch);
    m_states.insert(m_parser.sequence(c_remoteButtonLeft,6),e_remoteButtonLeftPress);
    m_states.insert(m_parser.sequence(c_remoteButtonRightTouch,10),e_remoteButtonRightTouch);
    m_states.insert(m_parser.sequence(c_remoteButtonRight,6),e_remoteButtonRightPress);
    m_states.insert(m_parser.sequence(c_remoteButtonPlay,10),e_remoteButtonPlay);
    m_states.insert(m_parser.sequence(c_remoteButtonPlayHold,10),e_remoteButtonPlayHold);
    m_states.insert(m_parser.sequence(c_remoteButtonMenu,10),e_remoteButtonMenu);
    m_states.insert(m_parser.sequence(c_remoteButtonMenuHold,8),e_remoteButtonMenuHold);
    m_states.insert(m_parser.sequence(c_remoteButtonPlaySecond,10),e_remoteButtonPlay);
    m_states.insert(m_parser.sequence(c_remoteControlSwitched,1),e_remoteButtonUnknown);
}

//-------------------------------------------------------------------------------------------

const KeyStateParser& AppleRemoteService::getKeyParser() const
{
    return m_parser;
}

//-------------------------------------------------------------------------------------------

const QMap<int,AppleRemoteService::Button>& AppleRemoteService::getButtonStatesMap() const
{
    return m_states;
}

//-------------------------------------------------------------------------------------------

RemoteIF *AppleRemoteService::getRemoteIF() const
{
    return m_pRemoteIF;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::processQueue(const QVector<int>& cookieList,const QVector<int>& indexList)
{
    if(!cookieList.isEmpty() && cookieList.size()==indexList.size())
    {
        int i,offset,prev,seqID;
        int *cookieMem;

        cookieMem = new int [cookieList.size()];
        for(i=0;i<cookieList.size();i++)
        {
            cookieMem[i] = cookieList.at(i);
        }

        offset = 0;
        while(offset < cookieList.size())
        {
            prev = offset;
            seqID = getKeyParser().find(cookieMem,cookieList.size(),offset);
            if(seqID > 0)
            {
                QMap<int,Button>::const_iterator ppI = getButtonStatesMap().find(seqID);
                if(ppI!=getButtonStatesMap().end())
                {
                    int sum = 0;
                    prev = offset - getKeyParser().length(seqID);
                    if(prev < 0)
                    {
                        prev = 0;
                    }
                    for(i=prev;i<offset;i++)
                    {
                        sum += indexList.at(i);
                    }
                    processButton(ppI.value(),sum);
                }
            }
        }

        delete [] cookieMem;
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::processButton(Button button,int indexState)
{
    switch(button)
    {
        case e_remoteButtonPlusPress:
        case e_remoteButtonMinusPress:
        case e_remoteButtonLeftPress:
        case e_remoteButtonRightPress:
            if(indexState == 0)
            {
                button = static_cast<Button>(static_cast<int>(button) + 1);
            }
            break;

        default:
            break;
    }

    if(getRemoteIF()!=0)
    {
        AppleIRRemoteServiceEvent e(button);
        getRemoteIF()->remoteEvent(&e);
    }
    else
    {
        printButton(button);
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::printButton(Button button)
{
    QString name;

    switch(button)
    {
        case e_remoteButtonCentre:
            name = "Centre";
            break;
        case e_remoteButtonCentreHold:
            name = "Centre Hold";
            break;
        case e_remoteButtonPlusPress:
            name = "Plus Press";
            break;
        case e_remoteButtonPlusRelease:
            name = "Plus Release";
            break;
        case e_remoteButtonMinusPress:
            name = "Minus Press";
            break;
        case e_remoteButtonMinusRelease:
            name = "Minus Release";
            break;
        case e_remoteButtonLeftTouch:
            name = "Left Touch";
            break;
        case e_remoteButtonLeftPress:
            name = "Left Press";
            break;
        case e_remoteButtonLeftRelease:
            name = "Left Release";
            break;
        case e_remoteButtonRightTouch:
            name = "Right Touch";
            break;
        case e_remoteButtonRightPress:
            name = "Right Press";
            break;
        case e_remoteButtonRightRelease:
            name = "Right Release";
            break;
        case e_remoteButtonPlay:
            name = "Play";
            break;
        case e_remoteButtonPlayHold:
            name = "Play Hold";
            break;
        case e_remoteButtonMenu:
            name = "Menu";
            break;
        case e_remoteButtonMenuHold:
            name = "Menu Hold";
            break;
        case e_remoteButtonUnknown:
        default:
            name = "Unknown";
            break;
    }
    common::Log::g_Log.print("Button: %s\n",name.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::openEventSecureInput()
{
    bool res = false;
    io_registry_entry_t root = IORegistryGetRootEntry(kIOMainPortDefault);
    if(root!=MACH_PORT_NULL)
    {
        m_notifyPort = IONotificationPortCreate(kIOMainPortDefault);
        if(m_notifyPort!=0)
        {
            io_registry_entry_t entry;
            CFRunLoopSourceRef runLoopSource = IONotificationPortGetRunLoopSource(m_notifyPort);
            CFRunLoopAddSource(CFRunLoopGetCurrent(),runLoopSource,kCFRunLoopDefaultMode);

            entry = IORegistryEntryFromPath(kIOMainPortDefault,kIOServicePlane ":/");
            if(entry!=MACH_PORT_NULL)
            {
                kern_return_t kr;

                kr = IOServiceAddInterestNotification(m_notifyPort, entry, kIOBusyInterest, &IOREInterestCallback, m_proxy, &m_eventSecureInputNotification);
                if(kr==KERN_SUCCESS)
                {
                    res = true;
                }
                else
                {
                    printError("openEventSecureInput","Error when installing secure input event notification");
                    CFRunLoopRemoveSource(CFRunLoopGetCurrent(),runLoopSource,kCFRunLoopDefaultMode);
                    IONotificationPortDestroy(m_notifyPort);
                    m_notifyPort = 0;
                }
            }
            else
            {
                printError("openEventSecureInput","Error getting registry for secure input event notification");
            }
        }
        else
        {
            printError("openEventSecureInput","Failed to create I/O notification port");
        }
        IOObjectRelease(root);
    }
    else
    {
        printError("openEventSecureInput","Failed to get root I/O registry entry");
    }

    m_lastSecureEventInputState = retrieveSecureEventInputState();

    return res;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::closeEventSecureInput()
{
    if(m_notifyPort!=0)
    {
        CFRunLoopSourceRef runLoopSource = IONotificationPortGetRunLoopSource(m_notifyPort);
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(),runLoopSource,kCFRunLoopDefaultMode);
        IONotificationPortDestroy(m_notifyPort);
        m_notifyPort = 0;
    }
    if(m_eventSecureInputNotification!=MACH_PORT_NULL)
    {
        IOObjectRelease(m_eventSecureInputNotification);
        m_eventSecureInputNotification = MACH_PORT_NULL;
    }
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::retrieveSecureEventInputState()
{
    bool res = false;
    io_registry_entry_t root = IORegistryGetRootEntry(kIOMainPortDefault);
    if(root!=MACH_PORT_NULL)
    {
        CFArrayRef arrayRef = (CFArrayRef)IORegistryEntrySearchCFProperty(root,kIOServicePlane,CFSTR("IOConsoleUsers"),0,kIORegistryIterateRecursively);
        if(arrayRef!=0)
        {
            NSArray *array = (NSArray *)arrayRef;
            for(tuint32 i=0; i < [array count]; i++)
            {
                NSDictionary *dict = [array objectAtIndex:i];
                if([[dict objectForKey:@"kCGSSessionUserNameKey"] isEqual: NSUserName()])
                {
                    res = ([dict objectForKey:@"kCGSSessionSecureInputPID"] != nil) ? true : false;
                }
            }
            CFRelease(arrayRef);
        }
        IOObjectRelease(root);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AppleRemoteService::dealWithSecureEventInputChange()
{
    if(isListeningToRemote())
    {
        bool newState = retrieveSecureEventInputState();
        if(m_lastSecureEventInputState!=newState)
        {
            closeRemoteControlDevice(false);
            openRemoteControlDevice();
            m_lastSecureEventInputState = newState;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool AppleRemoteService::isListeningToRemote()
{
    return (m_hidDeviceInterface!=0 && !m_cookieArray.isEmpty() && m_queue!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// AppleIRRemoteServiceEvent
//-------------------------------------------------------------------------------------------

AppleIRRemoteServiceEvent::AppleIRRemoteServiceEvent(AppleRemoteService::Button button) : RemoteEvent(),
    m_button(button)
{}

//-------------------------------------------------------------------------------------------

AppleIRRemoteServiceEvent::~AppleIRRemoteServiceEvent()
{}

//-------------------------------------------------------------------------------------------

const AppleRemoteService::Button& AppleIRRemoteServiceEvent::button() const
{
    return m_button;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
