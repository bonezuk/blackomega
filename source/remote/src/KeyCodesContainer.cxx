#include "remote/inc/KeyCodesContainer.h"

#if QT_VERSION >= 0x050000
#include <QSequentialIterable>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------
// KeyCode
//-------------------------------------------------------------------------------------------

KeyCode::KeyCode() : m_key(0),
    m_remote()
{}

//-------------------------------------------------------------------------------------------

KeyCode::KeyCode(tuint32 kCode) : m_key(kCode),
    m_remote()
{}

//-------------------------------------------------------------------------------------------

KeyCode::KeyCode(const QString& rString) : m_key(0),
    m_remote(rString)
{}

//-------------------------------------------------------------------------------------------

KeyCode::KeyCode(const QVariant& v) : m_key(0),
    m_remote()
{
    copy(v);
}

//-------------------------------------------------------------------------------------------

KeyCode::KeyCode(const KeyCode& rhs) : m_key(0),
    m_remote()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

KeyCode::~KeyCode()
{}

//-------------------------------------------------------------------------------------------

const KeyCode& KeyCode::operator = (const QVariant& v)
{
    copy(v);
    return *this;
}

//-------------------------------------------------------------------------------------------

const KeyCode& KeyCode::operator = (const KeyCode& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void KeyCode::copy(const QVariant& v)
{
    m_key = 0;
    m_remote = "";

    if(v.isValid() && !v.isNull() && v.canConvert<QString>())
    {
        QString s = v.toString();

        if(s.size() > 0)
        {
            if(s.at(0)==QChar('k'))
            {
                bool ok = true;
                tuint32 x = static_cast<tuint32>(s.mid(1).toULong(&ok)) & 0x1fffffff;
                if(ok)
                {
                    m_key = x;
                }
            }
            else if(s.at(0)==QChar('r'))
            {
                m_remote = s.mid(1);
            }
        }
    }
}


//-------------------------------------------------------------------------------------------

void KeyCode::copy(const KeyCode& rhs)
{
    m_key = rhs.m_key;
    m_remote = rhs.m_remote;
}

//-------------------------------------------------------------------------------------------

bool KeyCode::isEmpty() const
{
    return (m_key==0 && m_remote.isEmpty()) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool KeyCode::isKeyboard() const
{
    return (m_key > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool KeyCode::isRemote() const
{
    return (!m_remote.isEmpty()) ? true : false;
}

//-------------------------------------------------------------------------------------------

tuint32 KeyCode::keyCode() const
{
    return m_key;
}

//-------------------------------------------------------------------------------------------

void KeyCode::setKeyCode(tuint32 k)
{
    m_key = k & 0x1fffffff;
    m_remote = "";
}

//-------------------------------------------------------------------------------------------

const QString& KeyCode::remote() const
{
    return m_remote;
}

//-------------------------------------------------------------------------------------------

void KeyCode::setRemote(const QString& remote)
{
    m_key = 0;
    m_remote = remote;
}

//-------------------------------------------------------------------------------------------

QVariant KeyCode::variant() const
{
    QString s;
    if(isKeyboard())
    {
        s = "k" + QString::number(static_cast<ulong>(m_key));
    }
    else
    {
        s = "r" + m_remote;
    }
    return QVariant(s);
}

//-------------------------------------------------------------------------------------------
// -1 => a < b
//  0 => a = b
//  1 => a > b
//-------------------------------------------------------------------------------------------

tint KeyCode::compareTo(const KeyCode& b) const
{
    tint res;

    if(isEmpty())
    {
        if(b.isEmpty())
        {
            res = 0;
        }
        else
        {
            res = -1;
        }
    }
    else if(isKeyboard())
    {
        if(b.isEmpty())
        {
            res = 1;
        }
        else if(b.isKeyboard())
        {
            if(keyCode() < b.keyCode())
            {
                res = -1;
            }
            else if(keyCode() > b.keyCode())
            {
                res = 1;
            }
            else
            {
                res = 0;
            }
        }
        else
        {
            res = -1;
        }
    }
    else
    {
        if(b.isEmpty())
        {
            res = 1;
        }
        else if(b.isKeyboard())
        {
            res = 1;
        }
        else
        {
            if(remote() < b.remote())
            {
                res = -1;
            }
            else if(remote() > b.remote())
            {
                res = 1;
            }
            else
            {
                res = 0;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool operator == (const KeyCode& a,const KeyCode& b)
{
    return (a.compareTo(b)==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const KeyCode& a,const KeyCode& b)
{
    return (a.compareTo(b)!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <  (const KeyCode& a,const KeyCode& b)
{
    return (a.compareTo(b)<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const KeyCode& a,const KeyCode& b)
{
    return (a.compareTo(b)<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (const KeyCode& a,const KeyCode& b)
{
    return (a.compareTo(b)>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const KeyCode& a,const KeyCode& b)
{
    return (a.compareTo(b)>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// KeyCodesContainer
//-------------------------------------------------------------------------------------------

KeyCodesContainer::KeyCodesContainer() : m_list()
{}

//-------------------------------------------------------------------------------------------

KeyCodesContainer::KeyCodesContainer(const QVariant& v) : m_list()
{
    copy(v);
}

//-------------------------------------------------------------------------------------------

KeyCodesContainer::KeyCodesContainer(const KeyCodesContainer& rhs) : m_list()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const KeyCodesContainer& KeyCodesContainer::operator = (const QVariant& v)
{
    copy(v);
    return *this;
}

//-------------------------------------------------------------------------------------------

const KeyCodesContainer& KeyCodesContainer::operator = (const KeyCodesContainer& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void KeyCodesContainer::copy(const QVariant& v)
{
    m_list.clear();

    if(!v.isNull() && v.isValid() && v.canConvert<QVariantList>())
    {
#if QT_VERSION >= 0x050000
        QSequentialIterable iter = v.value<QSequentialIterable>();
        for(QSequentialIterable::const_iterator ppI=iter.begin();ppI!=iter.end();ppI++)
        {
            KeyCode k(*ppI);
            if(!k.isEmpty())
            {
                m_list.append(k);
            }
        }
#else
        QVariantList l = v.toList();
        for(QVariantList::const_iterator ppI=l.begin();ppI!=l.end();ppI++)
        {
            KeyCode k(*ppI);
            if(!k.isEmpty())
            {
                m_list.append(k);
            }
        }
#endif
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodesContainer::copy(const KeyCodesContainer& rhs)
{
    QList<KeyCode>::const_iterator ppI;

    m_list.clear();
    for(ppI=rhs.m_list.begin();ppI!=rhs.m_list.end();ppI++)
    {
        m_list.append(*ppI);
    }
}

//-------------------------------------------------------------------------------------------

bool KeyCodesContainer::isEmpty() const
{
    return m_list.isEmpty();
}

//-------------------------------------------------------------------------------------------

int KeyCodesContainer::size() const
{
    return m_list.size();
}

//-------------------------------------------------------------------------------------------

KeyCode& KeyCodesContainer::at(int index)
{
    return m_list[index];
}

//-------------------------------------------------------------------------------------------

const KeyCode& KeyCodesContainer::at(int index) const
{
    return m_list.at(index);
}

//-------------------------------------------------------------------------------------------

QList<KeyCode>& KeyCodesContainer::list()
{
    return m_list;
}

//-------------------------------------------------------------------------------------------

const QList<KeyCode>& KeyCodesContainer::list() const
{
    return m_list;
}

//-------------------------------------------------------------------------------------------

QVariant KeyCodesContainer::variant() const
{
    QList<QVariant> vList;
    QList<KeyCode>::const_iterator ppI;

    for(ppI=m_list.begin();ppI!=m_list.end();ppI++)
    {
        vList.append((*ppI).variant());
    }
    return QVariant(vList);
}

//-------------------------------------------------------------------------------------------

bool KeyCodesContainer::equalTo(const KeyCodesContainer& b) const
{
    bool res = true;

    if(m_list.size()==b.m_list.size())
    {
        for(int i=0;i<m_list.size() && res;i++)
        {
            res = (m_list.at(i) == b.m_list.at(i));
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool operator == (const KeyCodesContainer& a,const KeyCodesContainer& b)
{
    return (a.equalTo(b)) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const KeyCodesContainer& a,const KeyCodesContainer& b)
{
    return (!a.equalTo(b)) ? true : false;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
