//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_KEYCODEEXCLUSION_H
#define __OMEGA_REMOTE_KEYCODEEXCLUSION_H
//-------------------------------------------------------------------------------------------

#include "remote/inc/KeyCodesContainer.h"

#include <QSharedPointer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyCodeExclusion
{
    public:
        virtual ~KeyCodeExclusion();

        static QSharedPointer<KeyCodeExclusion> instance();
        static void release();
        
        virtual void add(const KeyCode& k,const QString& description);
        
        virtual bool isAvailable(const KeyCode& k) const;
        virtual QString description(const KeyCode& k) const;
        
    protected:
    
        static QSharedPointer<KeyCodeExclusion> m_instance;
        
        QMap<KeyCode,QString> m_map;
        
        KeyCodeExclusion();
};

typedef QSharedPointer<KeyCodeExclusion> KeyCodeExclusionSPtr;

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
