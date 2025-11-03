//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_KEYCODESCONTAINER_H
#define __OMEGA_REMOTE_KEYCODESCONTAINER_H
//-------------------------------------------------------------------------------------------

#include <QVariant>
#include <QList>
#include <QVector>
#include <QString>

#include "remote/inc/RemoteDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyCode
{
    public:
        KeyCode();
        KeyCode(tuint32 kCode);
        KeyCode(const QString& rString);
        KeyCode(const QVariant& v);
        KeyCode(const KeyCode& rhs);
        virtual ~KeyCode();

        const KeyCode& operator = (const QVariant& v);
        const KeyCode& operator = (const KeyCode& rhs);

        virtual bool isEmpty() const;
        virtual bool isKeyboard() const;
        virtual bool isRemote() const;

        virtual tuint32 keyCode() const;
        virtual void setKeyCode(tuint32 k);

        virtual const QString& remote() const;
        virtual void setRemote(const QString& remote);

        virtual QVariant variant() const;

        friend REMOTE_EXPORT bool operator == (const KeyCode& a,const KeyCode& b);
        friend REMOTE_EXPORT bool operator != (const KeyCode& a,const KeyCode& b);
        friend REMOTE_EXPORT bool operator <  (const KeyCode& a,const KeyCode& b);
        friend REMOTE_EXPORT bool operator <= (const KeyCode& a,const KeyCode& b);
        friend REMOTE_EXPORT bool operator >  (const KeyCode& a,const KeyCode& b);
        friend REMOTE_EXPORT bool operator >= (const KeyCode& a,const KeyCode& b);

    protected:
        tuint32 m_key;
        QString m_remote;

        virtual void copy(const QVariant& v);
        virtual void copy(const KeyCode& rhs);

        virtual tint compareTo(const KeyCode& b) const;
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyCodesContainer
{
    public:
        KeyCodesContainer();
        KeyCodesContainer(const QVariant& v);
        KeyCodesContainer(const KeyCodesContainer& rhs);

        const KeyCodesContainer& operator = (const QVariant& v);
        const KeyCodesContainer& operator = (const KeyCodesContainer& rhs);

        bool isEmpty() const;
        int size() const;

        KeyCode& at(int index);
        const KeyCode& at(int index) const;

        QList<KeyCode>& list();
        const QList<KeyCode>& list() const;

        QVariant variant() const;

        friend REMOTE_EXPORT bool operator == (const KeyCodesContainer& a,const KeyCodesContainer& b);
        friend REMOTE_EXPORT bool operator != (const KeyCodesContainer& a,const KeyCodesContainer& b);

    protected:
        QList<KeyCode> m_list;

        void copy(const QVariant& v);
        void copy(const KeyCodesContainer& rhs);

        virtual bool equalTo(const KeyCodesContainer& b) const;
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
