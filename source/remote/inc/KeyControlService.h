//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_KEYCONTROLSERVICE_H
#define __OMEGA_REMOTE_KEYCONTROLSERVICE_H
//-------------------------------------------------------------------------------------------

#include "remote/inc/QKeyLineEdit.h"
#include "remote/inc/KeyAssignment.h"
#include "remote/inc/RemoteIF.h"
#include "remote/inc/KeyCodeStateEngine.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyControlService : public KeyCodeStateEngine
{
    public:
        Q_OBJECT

    public:
        KeyControlService(QObject *parent = 0);
        virtual ~KeyControlService();
        
        static QSharedPointer<KeyControlService> instance(QObject *parent);
        static QSharedPointer<KeyControlService> instance();
        static void release();

        virtual bool start();
        virtual void stop();
        
        virtual void remoteEvent(RemoteEvent *e);
        
        virtual bool onKeyPress(QKeyEvent *evt);
        virtual bool onKeyRelease(QKeyEvent *evt);

        virtual void startAssignment();
        virtual void endAssignment();
        
    protected:
    
        static QSharedPointer<KeyControlService> m_instance;
        
        QMap<KeyCode,int> m_keyRepeatMap;
        
        WinLIRCService *m_lircService;
        WinLIRCClient *m_lircClient;

        int m_assignmentState;
        
        virtual void printError(const tchar *strE,const tchar *strR) const;
    
        virtual bool loadKeyMappingOrSetupDefault();
        virtual bool loadKeyMappingFromXML(const QString& keyXMLFilename);
        virtual bool setupDefaultKeyMapping();
        virtual bool saveKeyboardMapping();
        virtual QString getKeyboardFilename() const;
        
        virtual QMap<KeyCode,int>& getKeyRepeatMap();
        virtual const QMap<KeyCode,int>& getKeyRepeatMapConst() const;
        
        virtual KeyCode getKeyCodeForEvent(QKeyEvent *evt) const;
        virtual void removeKeyboardMappings();
        virtual KeyAssignmentSPtr getKeyAssignment();
        virtual void removeKeyRepeatCount(const KeyCode& keyCode);

        virtual void onKeyProcess();
        virtual void onTimerImplementation();
};

typedef QSharedPointer<KeyControlService> KeyControlServiceSPtr;

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
