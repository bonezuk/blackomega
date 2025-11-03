#include <stdio.h>
#include <string.h>
#include "common/inc/BStringCollection.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

BStringCollection::BStringCollection() : firstItem(0),
    lastItem(0),
    total(0)
{}

//-------------------------------------------------------------------------------------------

BStringCollection::~BStringCollection()
{
    BSCItem *item;

    while(item=firstItem,item!=0)
    {
        firstItem=item->next;
        delete [] item->str;
        delete item;
    }
    firstItem=0;
    lastItem=0;
}

//-------------------------------------------------------------------------------------------

tchar *BStringCollection::AddString(const tchar *s,tint& len) const
{
    tchar *str;

    if(len==-1)
    {
        len = static_cast<tint>(::strlen(s));
    }
    str = new tchar [static_cast<tuint>(len) + 1];
    memcpy(str,s,static_cast<tuint>(len));
    str[len]='\0';
    return str;
}

//-------------------------------------------------------------------------------------------

void BStringCollection::Add(const tchar *s,tint len)
{
    BSCItem *item;

    try
    {
        item=new BSCItem;
        item->len=len;
        item->str=AddString(s,item->len);
        if(item->str==0)
        {
            delete item;
            return;
        }
        if(firstItem==0)
        {
            item->amount=0;
            lastItem=item;
        }
        else
        {
            item->amount=firstItem->len + firstItem->amount;
            firstItem->prev=item;
        }
        item->prev=0;
        item->next=firstItem;
        firstItem=item;
        total+=item->len;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

BStringCollection& operator << (BStringCollection& in,const BString& s)
{ //lint !e1929 parameter ensures reference exists prior to being called.
    in.Add(static_cast<const tchar *>(s),static_cast<tint>(s.StrLen()));
    return in;
}

//-------------------------------------------------------------------------------------------

BStringCollection& operator << (BStringCollection& in,const tchar *s)
{ //lint !e1929 parameter ensures reference exists prior to being called.
    in.Add(s,-1);
    return in;
}

//-------------------------------------------------------------------------------------------

BStringCollection& operator << (BStringCollection& in,const BStringCollection& s)
{ //lint !e1929 parameter ensures reference exists prior to being called.
    BStringCollection::BSCItem *item=s.lastItem;

    while(item!=0)
    {
        in.Add(item->str,item->len);
        item=item->prev;
    }
    return in;
}

//-------------------------------------------------------------------------------------------

BStringCollection& operator << (BStringCollection& in,const tchar s)
{
    tchar x[2] = {'\0','\0'};

    x[0] = s;
    in.Add(x,1);
    return in;
}

//-------------------------------------------------------------------------------------------

bool BStringCollection::Group(BString& str,bool reverse)
{
    str.InitializeVariables();

    if(!reverse)
    {
        BSCItem *item=lastItem;

        if(item!=0)
        {
            str.AllocateMemory(static_cast<tuint>(total)+4);
            while(item!=0)
            {
                str.CopyToBuffer(item->str,static_cast<tuint>(item->amount));
                item=item->prev;
            }
        }
    }
    else
    {
        tint amount=0;
        BSCItem *item=firstItem;

        if(item!=0)
        {
            str.AllocateMemory(static_cast<tuint>(total)+4);
            while(item!=0)
            {
                str.CopyToBuffer(item->str,static_cast<tuint>(amount));
                amount+=item->len;
                item=item->next;
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
