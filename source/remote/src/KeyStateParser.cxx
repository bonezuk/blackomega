#include "remote/inc/KeyStateParser.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------
// KeyStateParser::Item
//-------------------------------------------------------------------------------------------

KeyStateParser::Item::Item() : m_map(),
    m_value(0)
{}

//-------------------------------------------------------------------------------------------

KeyStateParser::Item::~Item()
{
    Item::freeMap(m_map);
}

//-------------------------------------------------------------------------------------------

bool KeyStateParser::Item::isNode() const
{
    return m_map.isEmpty();
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::Item::value() const
{
    return m_value;
}

//-------------------------------------------------------------------------------------------

void KeyStateParser::Item::setValue(int v)
{
    m_value = v;
}

//-------------------------------------------------------------------------------------------

const KeyStateParser::Item *KeyStateParser::Item::find(int entry) const
{
    const Item *cItem = 0;
    QMap<int,Item *>::const_iterator ppI = mapConst().find(entry);
    if(ppI!=mapConst().end())
    {
        cItem = ppI.value();
    }
    return cItem;
}

//-------------------------------------------------------------------------------------------

QMap<int,KeyStateParser::Item *>& KeyStateParser::Item::map()
{
    return m_map;
}

//-------------------------------------------------------------------------------------------

const QMap<int,KeyStateParser::Item *>& KeyStateParser::Item::mapConst() const
{
    return m_map;
}

//-------------------------------------------------------------------------------------------

void KeyStateParser::Item::freeMap(QMap<int,Item *>& cMap)
{
    Item *cItem;
    QMap<int,Item *>::iterator ppI;

    while(ppI=cMap.begin(),ppI!=cMap.end())
    {
        cItem = ppI.value();
        cMap.erase(ppI);
        deleteItem(cItem);
    }
}

//-------------------------------------------------------------------------------------------

void KeyStateParser::Item::deleteItem(Item *item)
{
    if(item!=0)
    {
        delete item;
    }
}

//-------------------------------------------------------------------------------------------
// KeyStateParser
//-------------------------------------------------------------------------------------------

KeyStateParser::KeyStateParser() : m_nextSequenceID(0),
    m_topItems(),
    m_sequenceLengthMap()
{}

//-------------------------------------------------------------------------------------------

KeyStateParser::~KeyStateParser()
{
    Item item;
    item.freeMap(m_topItems);
}

//-------------------------------------------------------------------------------------------

bool KeyStateParser::isEmpty() const
{
    return m_sequenceLengthMap.isEmpty();
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::size() const
{
    return m_sequenceLengthMap.size();
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::getNextSequenceID()
{
    m_nextSequenceID++;
    return m_nextSequenceID;
}

//-------------------------------------------------------------------------------------------

QMap<int,int>& KeyStateParser::getSequenceLengthMap()
{
    return m_sequenceLengthMap;
}

//-------------------------------------------------------------------------------------------

const QMap<int,int>& KeyStateParser::getSequenceLengthMapConst() const
{
    return m_sequenceLengthMap;
}

//-------------------------------------------------------------------------------------------

QMap<int,KeyStateParser::Item *>& KeyStateParser::getTopItems()
{
    return m_topItems;
}

//-------------------------------------------------------------------------------------------

const QMap<int,KeyStateParser::Item *>& KeyStateParser::getTopItemsConst() const
{
    return m_topItems;
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::sequence(const int *mem,int len)
{
    int seqID = 0;

    if(mem!=0)
    {
        Item *item = 0;

        for(int i=0;i<len;i++)
        {
            QMap<int,Item *>& cMap = (item!=0) ? item->map() : getTopItems();
            QMap<int,Item *>::iterator ppI = cMap.find(mem[i]);
            if(ppI!=cMap.end())
            {
                item = ppI.value();
                if(item->isNode())
                {
                    if(i==(len-1))
                    {
                        seqID = item->value();
                    }
                    return seqID;
                }
            }
            else
            {
                item = new Item;
                cMap.insert(mem[i],item);
                if(i==(len-1))
                {
                    seqID = getNextSequenceID();
                    item->setValue(seqID);
                }
            }
        }

        if(seqID > 0)
        {
            getSequenceLengthMap().insert(seqID,len);
        }
    }
    return seqID;
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::length(int sequenceID) const
{
    QMap<int,int>::const_iterator ppI = getSequenceLengthMapConst().find(sequenceID);
    return (ppI!=getSequenceLengthMapConst().end()) ? ppI.value() : 0;
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::find(const int *mem,int len,int& offset) const
{
    int seqID = 0;

    if(mem!=0)
    {
        while(!seqID && offset<len)
        {
            QMap<int,Item *>::const_iterator ppI = getTopItemsConst().find(mem[offset]);
            if(ppI!=getTopItemsConst().end())
            {
                const Item *item = ppI.value();
                if(!item->isNode())
                {
                    seqID = findRecursiveIter(mem,len,item,offset+1);
                    if(!seqID)
                    {
                        offset++;
                    }
                }
                else
                {
                    seqID = item->value();
                    if(!seqID)
                    {
                        offset++;
                    }
                }
            }
            else
            {
                offset++;
            }
        }
        if(seqID > 0)
        {
            QMap<int,int>::const_iterator ppI = getSequenceLengthMapConst().find(seqID);
            if(ppI!=getSequenceLengthMapConst().end())
            {
                offset += ppI.value();
            }
        }
    }
    return seqID;
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::findRecursiveIter(const int *mem,int len,const Item *parent,int index) const
{
    return findRecursive(mem,len,parent,index);
}

//-------------------------------------------------------------------------------------------

int KeyStateParser::findRecursive(const int *mem,int len,const Item *parent,int index) const
{
    int seqID = 0;

    if(index<len)
    {
        const Item *item = parent->find(mem[index]);
        if(item != 0)
        {
            if(!item->isNode())
            {
                seqID = findRecursiveIter(mem,len,item,index+1);
            }
            else
            {
                seqID = item->value();
            }
        }
    }
    return seqID;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
