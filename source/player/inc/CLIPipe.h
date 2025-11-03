//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_CLIPIPE_H
#define __OMEGA_PLAYER_CLIPIPE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/RegistryScript.h"

#include <QPair>
#include <QStringList>
#include <QList>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

class CLIPipe
{
    public:
    
        typedef enum
        {
            eCmdAddFiles = 0,
            eCmdQueueFiles,
            eCmdNone
        } Command;
    
    public:
        virtual ~CLIPipe();
        
        static CLIPipe *instance();
        
        bool isServer() const;
        
        bool cli(const QStringList& cmd);

        bool process();
        
        Command next(QString& param);
        
    protected:
    
        static CLIPipe *m_instance;
        
        HANDLE m_mailSlot;
        bool m_serverMode;
        
        QList<QPair<Command,QString> > m_cmdList;
        
        CLIPipe();
        
        bool init();
        
        QString cliToXML(const QStringList& cmdList);
        void translateXML(const QString& cmd);
        QString getXMLAttribute(xmlNode *cNode,const QString& aName);
        
        
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

