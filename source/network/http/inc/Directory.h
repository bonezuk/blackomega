//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_HTTP_DIRECTORY_H
#define __OMEGA_NETWORK_HTTP_DIRECTORY_H
//-------------------------------------------------------------------------------------------

#include "network/http/inc/ResponseCodes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

template <class X> class Directory
{
    public:
        Directory(bool caseSenstive = false);
        ~Directory();

        void add(const tchar *path,X obj);
        void add(const QString& path,X obj);

        X get(const tchar *path,QString& tail);
        X get(const QString& path,QString& tail);

    protected:

        typedef struct s_DirectoryItem
        {
            struct s_DirectoryItem *parent;
            struct s_DirectoryItem *prev;
            struct s_DirectoryItem *next;
            struct s_DirectoryItem *firstChild;
            struct s_DirectoryItem *lastChild;
            QString name;
            X object;
        } DirectoryItem;

        DirectoryItem *m_root;

        bool m_caseSenstive;

        void newItem(DirectoryItem **ptrItem) const;
        void addItem(DirectoryItem *parent,DirectoryItem **ptrItem,const QString& name);
        void deleteItem(DirectoryItem *item);
        void formatPath(common::BString& path) const;
        bool findChildItem(const DirectoryItem *parent,DirectoryItem **ptrItem,const QString& name) const;
};

//-------------------------------------------------------------------------------------------

template <class X> Directory<X>::Directory(bool caseSenstive) : m_root(0),
    m_caseSenstive(caseSenstive)
{}

//-------------------------------------------------------------------------------------------

template <class X> Directory<X>::~Directory()
{
    try
    {
        deleteItem(m_root);
        m_root = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

template <class X> void Directory<X>::newItem(DirectoryItem **ptrItem) const
{
    DirectoryItem *item;

    if(ptrItem!=0)
    {
        item = new DirectoryItem;
        item->parent = 0;
        item->prev = 0;
        item->next = 0;
        item->firstChild = 0;
        item->lastChild = 0;
        item->object = 0;
        *ptrItem = item;
    }
}

//-------------------------------------------------------------------------------------------

template <class X> void Directory<X>::addItem(DirectoryItem *parent,DirectoryItem **ptrItem,const QString& name)
{
    DirectoryItem *item;

    if(ptrItem!=0)
    {
        newItem(&item);
        if(parent!=0)
        {
            if(parent->lastChild==0)
            {
                parent->firstChild = item;
            }
            else
            {
                parent->lastChild->next = item;
                item->prev = parent->lastChild;
            }
            parent->lastChild = item;
            item->parent = parent;
        }
        else
        {
            m_root = item;
        }
        item->name = name;
        *ptrItem = item;
    }
}

//-------------------------------------------------------------------------------------------

template <class X> void Directory<X>::deleteItem(DirectoryItem *item)
{
    DirectoryItem *pItem,*nItem;

    if(item!=0)
    {
        while(item->lastChild!=0)
        {
            deleteItem(item->lastChild);
        }

        pItem = item->prev;
        nItem = item->next;
        if(pItem!=0)
        {
            pItem->next = nItem;
        }
        else
        {
            if(item->parent!=0)
            {
                item->parent->firstChild = nItem;
            }
        }
        if(nItem!=0)
        {
            nItem->prev = pItem;
        }
        else
        {
            if(item->parent!=0)
            {
                item->parent->lastChild = pItem;
            }
        }
        if(item->parent==0)
        {
            m_root = 0;
        }
        delete item;
    }
}

//-------------------------------------------------------------------------------------------

template <class X> void Directory<X>::formatPath(common::BString& path) const
{
    tint i = 0;
    common::BString str(path);
    const tchar *s = static_cast<const tchar *>(str);

    if(!str.IsEmpty())
    {
        path = "/";
        while(i < str.GetLength())
        {
            if(s[i]=='/' || s[i]=='\\')
            {
                path += '/';
            }
            else
            {
                path += s[i];
            }
            i++;
        }
        path += "/";
    }
    else
    {
        path = "//";
    }
    if(!m_caseSenstive)
    {
        path.SelfLCase();
    }
}

//-------------------------------------------------------------------------------------------

template <class X> bool Directory<X>::findChildItem(const DirectoryItem *parent,DirectoryItem **ptrItem,const QString& name) const
{
    DirectoryItem *item;

    if(parent!=0 && ptrItem!=0)
    {
        item = parent->firstChild;
        while(item!=0)
        {
            if(item->name==name)
            {
                *ptrItem = item;
                return true;
            }
            item = item->next;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

template <class X> void Directory<X>::add(const tchar *path,X obj)
{
    tint i=1,j=0;
    const tchar *s;
    DirectoryItem *item,*parent;
    common::BString p(path),str;

    formatPath(p);

    if(m_root!=0)
    {
        item = m_root;
    }
    else
    {
        addItem(0,&item,QString::fromUtf8(static_cast<const tchar *>(str)));
    }

    s = static_cast<const tchar *>(p);
    while(i<p.GetLength())
    {
        if(s[i]=='/')
        {
            if((i-j) > 1)
            {
                parent = item;
                str = p.Sub(j+1,i-(j+1));
                if(!findChildItem(parent,&item,QString::fromUtf8(str.GetString())))
                {
                    addItem(parent,&item,QString::fromUtf8(static_cast<const tchar *>(str)));
                }
            }
            j = i;
        }
        i++;
    }
    item->object = obj;
}

//-------------------------------------------------------------------------------------------

template <class X> void Directory<X>::add(const QString& path,X obj)
{
    add(path.toUtf8().constData(),obj);
}

//-------------------------------------------------------------------------------------------

template <class X> X Directory<X>::get(const tchar *path,QString& tail)
{
    tint i=1,j=0,k=0;
    const tchar *s;
    DirectoryItem *oItem=0,*item,*parent;
    common::BString p(path),str;

    formatPath(p);

    if(m_root!=0)
    {
        item = m_root;
        if(item->object!=0)
        {
            oItem = item;
        }
    }
    else
    {
        return 0;
    }

    s = static_cast<const tchar *>(p);
    while(i<p.GetLength())
    {
        if(s[i]=='/')
        {
            if((i-j) > 1)
            {
                parent = item;
                str = p.Sub(j+1,i-(j+1));
                if(findChildItem(parent,&item,QString::fromUtf8(str.GetString())))
                {
                    if(item->object!=0)
                    {
                        oItem = item;
                        k = i;
                    }
                }
                else
                {
                    break;
                }
            }
            j = i;
        }
        i++;
    }

    if(oItem!=0)
    {
        if((k+1) < (p.GetLength()-1))
        {
            tail = QString::fromUtf8(p.Sub(k+1,(p.GetLength()-1) - (k+1)).GetString());
        }
        else
        {
            tail = "";
        }
        return oItem->object;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

template <class X> X Directory<X>::get(const QString& path,QString& tail)
{
    return get(path.toUtf8().constData(),tail);
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
