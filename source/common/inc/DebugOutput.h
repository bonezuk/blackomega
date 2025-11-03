//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_DEBUGOUTPUT_H
#define __OMEGA_COMMON_DEBUGOUTPUT_H
//-------------------------------------------------------------------------------------------

#include <QMap>
#include <QVector>
#include <QString>
#include <QMutex>
#include <QStringList>

#include "common/inc/CommonTypes.h"
#include "common/inc/TimeStamp.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT DebugOutputItem
{
    public:
        DebugOutputItem();
        DebugOutputItem(const QString& name,tint lvl);
        DebugOutputItem(const DebugOutputItem& rhs);
        
        const QString& name() const;
        tint level() const;
        const common::TimeStamp& time() const;
        
        const DebugOutputItem& operator = (const DebugOutputItem& rhs);
        
        virtual QString print() const;
        
    protected:
                
        QString m_name;
        tint m_level;
        common::TimeStamp m_time;
        
        virtual void copy(const DebugOutputItem& rhs);
        
        QString printStamp() const;
        QString printTime(const TimeStamp& t) const;
};

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT DebugOutput
{
    public:
        ~DebugOutput();
        
        static DebugOutput& instance(tint debugLevel = 0);
        
        tint level() const;
        
        void append(const DebugOutputItem& item);
        void print();
        
    protected:
        
        static DebugOutput *m_instance;
        
        tint m_debugLevel;
        QMutex m_mutex;
        QStringList m_list;
        
        DebugOutput(tint debugLevel);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
