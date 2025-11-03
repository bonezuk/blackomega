//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_KEYSTATEPARSER_H
#define __OMEGA_REMOTE_KEYSTATEPARSER_H
//-------------------------------------------------------------------------------------------

#include "remote/inc/RemoteDLL.h"

#include <QMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyStateParser
{
    public:

        class Item;

    public:
        KeyStateParser();
        virtual ~KeyStateParser();

        virtual bool isEmpty() const;
        virtual int size() const;

        virtual int sequence(const int *mem,int len);
        virtual int length(int sequenceID) const;

        virtual int find(const int *mem,int len,int& offset) const;

    protected:

        int m_nextSequenceID;
        QMap<int,KeyStateParser::Item *> m_topItems;
        QMap<int,int> m_sequenceLengthMap;

        virtual QMap<int,KeyStateParser::Item *>& getTopItems();
        virtual const QMap<int,KeyStateParser::Item *>& getTopItemsConst() const;
        virtual int getNextSequenceID();
        virtual QMap<int,int>& getSequenceLengthMap();
        virtual const QMap<int,int>& getSequenceLengthMapConst() const;

        virtual int findRecursive(const int *mem,int len,const Item *parent,int index) const;
        virtual int findRecursiveIter(const int *mem,int len,const Item *parent,int index) const;
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyStateParser::Item
{
    public:
        friend class KeyStateParser;

    public:
        Item();
        virtual ~Item();

        virtual bool isNode() const;
        virtual int value() const;
        virtual void setValue(int v);
        virtual const Item *find(int entry) const;

        virtual QMap<int,Item *>& map();
        virtual const QMap<int,Item *>& mapConst() const;

    protected:
        QMap<int,Item *> m_map;
        int m_value;

        virtual void freeMap(QMap<int,Item *>& cMap);
        virtual void deleteItem(Item *item);
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

