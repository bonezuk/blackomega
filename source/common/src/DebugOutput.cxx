#include "common/inc/DebugOutput.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// DebugOutputItem
//-------------------------------------------------------------------------------------------

DebugOutputItem::DebugOutputItem() : m_name("DebugOutputItem"),
    m_level(6),
    m_time()
{
    m_time = TimeStamp::now();
}

//-------------------------------------------------------------------------------------------

DebugOutputItem::DebugOutputItem(const QString& name,tint lvl) : m_name(name),
    m_level(lvl),
    m_time()
{
    m_time = TimeStamp::now();
}

//-------------------------------------------------------------------------------------------

DebugOutputItem::DebugOutputItem(const DebugOutputItem& rhs) : m_name(),
    m_level(6),
    m_time()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

void DebugOutputItem::copy(const DebugOutputItem& rhs)
{
    m_name = rhs.m_name;
    m_level = rhs.m_level;
    m_time = rhs.m_time;
}

//-------------------------------------------------------------------------------------------

const QString& DebugOutputItem::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

tint DebugOutputItem::level() const
{
    return m_level;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& DebugOutputItem::time() const
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

const DebugOutputItem& DebugOutputItem::operator = (const DebugOutputItem& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

QString DebugOutputItem::printStamp() const
{
    BString x;
    QString y;
    
    x  = m_name.toLatin1().constData();
    x += ": ";
    x += BString::Int(m_time.hour(),2,true);
    x += ":";
    x += BString::Int(m_time.minute(),2,true);
    x += ":";
    x += BString::Int(m_time.second(),2,true);
    x += ".";
    x += BString::Int(m_time.millisecond(),3,true);
    x += " - ";
    y = QString::fromLatin1(static_cast<const tchar *>(x));
    return y;
}

//-------------------------------------------------------------------------------------------

QString DebugOutputItem::printTime(const TimeStamp& t) const
{
    BString x;
    QString y;
    
    x  = common::BString::Int(t.minute(),2,true);
    x += ":";
    x += common::BString::Int(t.second(),2,true);
    x += ".";
    x += common::BString::Int(t.microsecond(),6,true);
    y = QString::fromLatin1(static_cast<const tchar *>(x));
    return y;
}

//-------------------------------------------------------------------------------------------

QString DebugOutputItem::print() const
{
    return printStamp();
}

//-------------------------------------------------------------------------------------------
// DebugOutput
//-------------------------------------------------------------------------------------------

DebugOutput *DebugOutput::m_instance = 0;

//-------------------------------------------------------------------------------------------

DebugOutput::DebugOutput(tint debugLevel) : m_debugLevel(debugLevel),
    m_mutex(),
    m_list()
{}

//-------------------------------------------------------------------------------------------

DebugOutput::~DebugOutput()
{
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

DebugOutput& DebugOutput::instance(tint debugLevel)
{
    if(m_instance==0)
    {
        m_instance = new DebugOutput(debugLevel);
    }
    return *m_instance;
}

//-------------------------------------------------------------------------------------------

void DebugOutput::append(const DebugOutputItem& item)
{
    if(item.level()>=m_debugLevel)
    {
        m_mutex.lock();
        m_list.append(item.print());
        m_mutex.unlock();
    }
}

//-------------------------------------------------------------------------------------------

void DebugOutput::print()
{
    QStringList::const_iterator ppI;
    
    m_mutex.lock();
    for(ppI=m_list.begin();ppI!=m_list.end();++ppI)
    {
        const QString& x = *ppI;
        Log::g_Log << x.toLatin1().constData() << c_endl;
    }
    m_list.clear();
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

tint DebugOutput::level() const
{
    return m_debugLevel;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
