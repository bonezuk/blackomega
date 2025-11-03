#include "network/inc/Resource.h"
#include "remote/inc/QKeyLineEdit.h"
#include "remote/inc/KeyControlService.h"
#include "remote/inc/KeyCodeExclusion.h"
#include "common/inc/DiskOps.h"

#include <QFileInfo>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

KeyControlServiceSPtr KeyControlService::m_instance;

//-------------------------------------------------------------------------------------------

KeyControlService::KeyControlService(QObject *parent) : KeyCodeStateEngine(parent),
    m_keyRepeatMap(),
    m_lircService(0),
    m_lircClient(0),
    m_assignmentState(0)
{}

//-------------------------------------------------------------------------------------------

KeyControlService::~KeyControlService()
{}

//-------------------------------------------------------------------------------------------

KeyControlServiceSPtr KeyControlService::instance(QObject *parent)
{
    if(m_instance.data()==0)
    {
        KeyControlServiceSPtr keyService(new KeyControlService(parent));
        if(keyService->start())
        {
            m_instance = keyService;
        }
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

KeyControlServiceSPtr KeyControlService::instance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void KeyControlService::printError(const tchar *strE,const tchar *strR) const
{
    common::Log::g_Log << "KeyControlService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void KeyControlService::release()
{
    if(m_instance.data()!=0)
    {
        KeyControlServiceSPtr emptyService;
        m_instance->stop();
        m_instance = emptyService;
    }
}

//-------------------------------------------------------------------------------------------

bool KeyControlService::start()
{
    bool res;

    m_keyAssignment = KeyAssignment::instance();
    if(m_keyAssignment.data()!=0)
    {
        if(loadKeyMappingOrSetupDefault())
        {
            if(setupTimer())
            {
                startTimer();
                res = true;
            }
            else
            {
                printError("start","Error setting up timer");
                res = false;
            }
        }
        else
        {
            printError("start","Error loading or setting default key mapping");
            res = false;
        }
    }
    else
    {
        printError("start","Failed to get key assignment class");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void KeyControlService::stop()
{
    stopTimer();
    freeTimer();
    if(m_keyAssignment.data()!=0)
    {
        saveKeyboardMapping();
    }
}

//-------------------------------------------------------------------------------------------

void KeyControlService::remoteEvent(RemoteEvent *e)
{}

//-------------------------------------------------------------------------------------------

bool KeyControlService::loadKeyMappingOrSetupDefault()
{
    bool res;
    QString keyXMLFilename = getKeyboardFilename();
    
    if(common::DiskOps::exist(keyXMLFilename))
    {
        res = loadKeyMappingFromXML(keyXMLFilename);
    }
    else
    {
        res = setupDefaultKeyMapping();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyControlService::loadKeyMappingFromXML(const QString& keyXMLFilename)
{
    bool res;
    
    if(m_keyAssignment->loadFromXML(keyXMLFilename))
    {
        res = true;
    }
    else
    {
        res = setupDefaultKeyMapping();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyControlService::setupDefaultKeyMapping()
{
    bool res;
    QStandardItemModel defaultKeyModel(5,1);
    
    KeyCodesContainer playKeys;
    playKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_Space)));
    playKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_P) | static_cast<tuint32>(Qt::ControlModifier)));
    
    KeyCodesContainer prevKeys;
    prevKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_Left) | static_cast<tuint32>(Qt::ControlModifier)));
    
    KeyCodesContainer nextKeys;
    nextKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_Right) | static_cast<tuint32>(Qt::ControlModifier)));
    
    KeyCodesContainer vDownKeys;
    vDownKeys.list().append(KeyCode(static_cast<tuint>(Qt::Key_Minus)));
    
    KeyCodesContainer vUpKeys;
    vUpKeys.list().append(KeyCode(static_cast<tuint>(Qt::Key_Equal)));

    defaultKeyModel.setData(defaultKeyModel.index(0,0,QModelIndex()),playKeys.variant(),Qt::EditRole);
    defaultKeyModel.setData(defaultKeyModel.index(1,0,QModelIndex()),prevKeys.variant(),Qt::EditRole);
    defaultKeyModel.setData(defaultKeyModel.index(2,0,QModelIndex()),nextKeys.variant(),Qt::EditRole);
    defaultKeyModel.setData(defaultKeyModel.index(3,0,QModelIndex()),vDownKeys.variant(),Qt::EditRole);
    defaultKeyModel.setData(defaultKeyModel.index(4,0,QModelIndex()),vUpKeys.variant(),Qt::EditRole);

    if(m_keyAssignment->loadFromModel(&defaultKeyModel))
    {
        res = true;
    }
    else
    {
        printError("setupDefaultKeyMapping","Failed to setup default key map");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyControlService::saveKeyboardMapping()
{
    bool res;
    QString keyXMLFilename = getKeyboardFilename();
    
    if(m_keyAssignment->saveToXML(keyXMLFilename))
    {
        res = true;
    }
    else
    {
        QString err;
        err = "Failed to save keyboard mapping setup to '" + keyXMLFilename + "'";
        printError("saveKeyboardMapping",err.toUtf8().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString KeyControlService::getKeyboardFilename() const
{
    QSettings iSettings(QSettings::IniFormat,QSettings::UserScope,QCoreApplication::organizationName(),QCoreApplication::applicationName());
    QFileInfo fInfo(iSettings.fileName());
    QString userDir = fInfo.absolutePath();
    if(userDir.at(userDir.length()-1)!='/' && userDir.at(userDir.length()-1)!='\\')
    {
        userDir += "/";
    }
    userDir = QDir::toNativeSeparators(userDir);
    omega::common::DiskOps::path(userDir,true);
    QString keyboardFilename = userDir + "keyboard.xml";
    return keyboardFilename;
}

//-------------------------------------------------------------------------------------------

KeyCode KeyControlService::getKeyCodeForEvent(QKeyEvent *evt) const
{
    tuint32 code = QKeyLineEdit::codeForKeyEvent(evt);
    KeyCode k(code);
    return k;
}

//-------------------------------------------------------------------------------------------

void KeyControlService::removeKeyboardMappings()
{
    QMap<KeyCode,QPair<int,common::TimeStamp> >& keyMap = getKeyStateMap();
    QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI;
    
    ppI = keyMap.begin();
    while(ppI!=keyMap.end())
    {
        if(ppI.key().isKeyboard())
        {
            removeKeyRepeatCount(ppI.key());
            ppI = keyMap.erase(ppI);
        }
        else
        {
            ppI++;
        }
    }
}

//-------------------------------------------------------------------------------------------

KeyAssignmentSPtr KeyControlService::getKeyAssignment()
{
    return m_keyAssignment;
}

//-------------------------------------------------------------------------------------------
// Policy is to remove other key press mappings and not to stack them
// as can happen with remote signals. As a key event for a release or
// press represents a change in the current key pressed configuration
// and the command for any given key code is given by those keys
// currently pressed down. So when new key is pressed the mapping and
// changes.
//-------------------------------------------------------------------------------------------

bool KeyControlService::onKeyPress(QKeyEvent *evt)
{
    bool res = false;

    if(evt!=0 && m_assignmentState==0)
    {
        KeyCode keyCode = getKeyCodeForEvent(evt);
        
        if(keyCode.isKeyboard())
        {
            QMap<KeyCode,QPair<int,common::TimeStamp> >& keyMap = getKeyStateMap();
            
#if defined(OMEGA_WIN32)
            if(getKeyAssignment()->assignedTo(keyCode)!=KeyAssignment::e_keyUnassigned || !KeyCodeExclusion::instance()->isAvailable(keyCode))
#else
            if(getKeyAssignment()->assignedTo(keyCode)!=KeyAssignment::e_keyUnassigned)
#endif
            {
                QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI = keyMap.find(keyCode);
                if(ppI==keyMap.end())
                {
                    removeKeyboardMappings();
                    keyMap.insert(keyCode,QPair<int,common::TimeStamp>(0,getReferenceTime()));
                }
                onKeyProcess();
                res = true;
            }
            else
            {
                removeKeyboardMappings();
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyControlService::onKeyRelease(QKeyEvent *evt)
{
    bool res = false;

    if(evt!=0 && m_assignmentState==0)
    {
        KeyCode keyCode = getKeyCodeForEvent(evt);
        
        if(keyCode.isKeyboard())
        {
            QMap<KeyCode,QPair<int,common::TimeStamp> >& keyMap = getKeyStateMap();
            
#if defined(OMEGA_WIN32)
            if(getKeyAssignment()->assignedTo(keyCode)!=KeyAssignment::e_keyUnassigned || !KeyCodeExclusion::instance()->isAvailable(keyCode))
#else
            if(getKeyAssignment()->assignedTo(keyCode)!=KeyAssignment::e_keyUnassigned)
#endif
            {
                QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI = keyMap.find(keyCode);
                if(ppI!=keyMap.end())
                {
                    processReleaseCommand(keyCode);
                    res = true;
                }
                removeKeyState(keyCode);
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QMap<KeyCode,int>& KeyControlService::getKeyRepeatMap()
{
    return m_keyRepeatMap;
}

//-------------------------------------------------------------------------------------------

const QMap<KeyCode,int>& KeyControlService::getKeyRepeatMapConst() const
{
    return m_keyRepeatMap;
}

//-------------------------------------------------------------------------------------------

void KeyControlService::removeKeyRepeatCount(const KeyCode& keyCode)
{
    QMap<KeyCode,int>& repeatMap = getKeyRepeatMap();
    QMap<KeyCode,int>::iterator ppI = repeatMap.find(keyCode);
    if(ppI!=repeatMap.end())
    {
        repeatMap.erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

void KeyControlService::onKeyProcess()
{
    QList<KeyCode> cmdList;
    QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI;
    QList<KeyCode>::iterator ppJ;
    
    for(ppI=getKeyStateMap().begin();ppI!=getKeyStateMap().end();ppI++)
    {
        if(ppI.key().isKeyboard())
        {
            cmdList.append(ppI.key());
        }
    }
    for(ppJ=cmdList.begin();ppJ!=cmdList.end();ppJ++)
    {
        processRemoteCommand(*ppJ,getKeyStateMap().find(*ppJ).value().first);
    }
}

//-------------------------------------------------------------------------------------------

void KeyControlService::onTimerImplementation()
{
    onProcessTimer();
}

//-------------------------------------------------------------------------------------------

void KeyControlService::startAssignment()
{
    m_assignmentState = 1;
}

//-------------------------------------------------------------------------------------------

void KeyControlService::endAssignment()
{
    m_assignmentState = 0;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
