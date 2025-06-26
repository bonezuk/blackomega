#include "remote/inc/KeyCodeExclusion.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

QSharedPointer<KeyCodeExclusion> KeyCodeExclusion::m_instance;

//-------------------------------------------------------------------------------------------

KeyCodeExclusion::KeyCodeExclusion() : m_map()
{}

//-------------------------------------------------------------------------------------------

KeyCodeExclusion::~KeyCodeExclusion()
{
    m_map.clear();
}

//-------------------------------------------------------------------------------------------

QSharedPointer<KeyCodeExclusion> KeyCodeExclusion::instance()
{
    if(m_instance.isNull())
    {
        QSharedPointer<KeyCodeExclusion> pInstance(new KeyCodeExclusion());
        m_instance = pInstance;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void KeyCodeExclusion::release()
{
    QSharedPointer<KeyCodeExclusion> pEmptyInstance;
    m_instance = pEmptyInstance;
}

//-------------------------------------------------------------------------------------------

void KeyCodeExclusion::add(const KeyCode& k,const QString& description)
{
    if(!k.isEmpty() && k.isKeyboard())
    {
        m_map.insert(k,description);
    }
}

//-------------------------------------------------------------------------------------------

bool KeyCodeExclusion::isAvailable(const KeyCode& k) const
{
    return (m_map.find(k)==m_map.end()) ? true : false;
}

//-------------------------------------------------------------------------------------------

QString KeyCodeExclusion::description(const KeyCode& k) const
{
    QString desc;
    QMap<KeyCode,QString>::const_iterator ppI = m_map.find(k);
    if(ppI!=m_map.end())
    {
        desc = ppI.value();
    }
    return desc;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
